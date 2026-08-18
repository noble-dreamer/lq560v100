/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "stereo_media.h"
#include "stereo_camera.h"
#include "stereo_queue.h"
#include "stereo_cve.h"
#include "stereo_npu.h"
#include "stereo_yolo.h"
#include "stereo_sec.h"
#include "stereo_subpixel.h"
#include "stereo_venc.h"
#include "stereo_lz4.h"
#include "stereo_network.h"
#include "stereo_raw_capture.h"
#include "stereo_scene_auto.h"
#include "stereo_rectify.h"
#include "sample_comm.h"
#include "ot_eis_vi_api.h"
#include "ot_eis_vproc.h"
#include "ot_eis_vproc_grp.h"
#include "ot_buffer_pool.h"
#include "ot_smr.h"

/* -------------------------------------------------------------------------- */
/* Performance timing: stores NPU/SubPixel elapsed time to global variables     */
/* for network transmission (no stdout output).                                */
/* -------------------------------------------------------------------------- */
static volatile double g_perf_npu_ms = 0;
static volatile double g_perf_sub_ms = 0;
static volatile ot_u32 g_drop_count  = 0;  /* frames dropped at queue-full */

#define PERF_START(tag) \
    struct timeval _pf_##tag##_s; gettimeofday(&_pf_##tag##_s, NULL)
#define PERF_END(tag, var) do { \
    struct timeval _pf_##tag##_e; gettimeofday(&_pf_##tag##_e, NULL); \
    g_perf_##var = (_pf_##tag##_e.tv_sec - _pf_##tag##_s.tv_sec) * 1000.0 \
        + (_pf_##tag##_e.tv_usec - _pf_##tag##_s.tv_usec) / 1000.0; \
} while(0)

void stereo_media_get_perf(double *npu_ms, double *sub_ms)
{
    if (npu_ms) *npu_ms = g_perf_npu_ms;
    if (sub_ms)  *sub_ms  = g_perf_sub_ms;
}

/* -------------------------------------------------------------------------- */
/* Internal types                                                             */
/* -------------------------------------------------------------------------- */

typedef struct {
    /* VI config per sensor */
    sample_vi_cfg           vi_cfg[STEREO_SNS_NUM];
    /* VPROC config per sensor (only ch0 used for full-res output) */
    sample_vproc_attr       vp_cfg[STEREO_SNS_NUM];
    /* Sensor info */
    sample_sns_type         sns_type;
    ot_s32                  dev_id[STEREO_SNS_NUM];
} stereo_media_ctx_t;

typedef struct {
    ot_eis_handle chn_hdl[STEREO_SNS_NUM];    /* [0]=left ch0, [1]=right ch0 */
    ot_eis_handle grp_hdl[STEREO_SNS_NUM];
} stereo_vproc_ctx_t;

/* -------------------------------------------------------------------------- */
/* Global state                                                               */
/* -------------------------------------------------------------------------- */

static stereo_media_ctx_t        g_media_ctx;
static stereo_vproc_ctx_t        g_vproc_ctx;
static ot_eis_handle             g_media_pipe_hdl;
static ot_bool                   g_sys_inited      = OT_FALSE;
static ot_bool                   g_pipeline_started = OT_FALSE;
static ot_bool                   g_raw_only_mode   = OT_FALSE;

/* Pipeline control flags */
static volatile ot_bool          g_get_frm_run      = OT_FALSE;
static volatile ot_bool          g_cve_run           = OT_FALSE;
static volatile ot_bool          g_npu_run           = OT_FALSE;
/* tiny-yolov3 state: enabled when the model loads; the detection-channel
   producer (M4) sets g_det_frame_valid when a fresh frame is available. */
static volatile ot_bool          g_yolo_enabled      = OT_FALSE;
static volatile ot_bool          g_det_frame_valid   = OT_FALSE;
static stereo_yolo_box_t         g_yolo_boxes[STEREO_YOLO_MAX_BOX];
static volatile ot_bool          g_venc_run          = OT_FALSE;
static volatile ot_bool          g_net_run           = OT_FALSE;

/* Pipeline threads */
static pthread_t                 g_get_frm_tid;
static pthread_t                 g_cve_tid;
static pthread_t                 g_npu_tid;
static pthread_t                 g_venc_tid;
static pthread_t                 g_net_tid;

/* Stage queues */
static stereo_spsc_queue_t       g_q_vproc;     /* frame_pair_t -> cve */
static stereo_spsc_queue_t       g_q_npu;       /* npu_input_t -> npu */
static stereo_spsc_queue_t       g_q_venc;      /* infer_output_t -> venc */
static stereo_spsc_queue_t       g_q_net;       /* venc_output_t -> net */

/* FPS tracking */
static ot_u32                    g_total_frames = 0;
static struct timeval            g_start_time;

/* Per-stage frame counters (volatile for cross-thread visibility) */
static volatile ot_u32           g_fps_get_frm = 0;
static volatile ot_u32           g_fps_cve     = 0;
static volatile ot_u32           g_fps_npu     = 0;
static volatile ot_u32           g_fps_subpix  = 0;
static volatile ot_u32           g_fps_venc    = 0;
static volatile ot_u32           g_fps_net     = 0;

/* -------------------------------------------------------------------------- */
/* Public mode selection                                                       */
/* -------------------------------------------------------------------------- */

void stereo_media_set_raw_only(ot_bool enable)
{
    g_raw_only_mode = enable;
}

/* -------------------------------------------------------------------------- */
/* Forward declarations                                                       */
/* -------------------------------------------------------------------------- */

static void *stereo_get_frame_proc(void *p);
static void *stereo_cve_proc(void *p);
static void *stereo_npu_proc(void *p);
static void *stereo_venc_proc(void *p);
static void *stereo_net_proc(void *p);

/* -------------------------------------------------------------------------- */
/* Per-stage FPS reporting (called every ~1s from main loop)                    */
/* -------------------------------------------------------------------------- */

void stereo_media_print_fps(void)
{
    static struct timeval last_tv = {0, 0};
    static ot_u32 prev_get = 0, prev_cve = 0, prev_npu = 0, prev_subpix = 0, prev_venc = 0, prev_net = 0;
    struct timeval now;
    gettimeofday(&now, NULL);

    if (last_tv.tv_sec == 0 && last_tv.tv_usec == 0) {
        last_tv = now;
        prev_get    = g_fps_get_frm;
        prev_cve    = g_fps_cve;
        prev_npu    = g_fps_npu;
        prev_subpix = g_fps_subpix;
        prev_venc   = g_fps_venc;
        prev_net    = g_fps_net;
        return;
    }

    double elapsed = (double)(now.tv_sec - last_tv.tv_sec)
                   + (double)(now.tv_usec - last_tv.tv_usec) / 1000000.0;
    if (elapsed < 1.0) {
        return;
    }

    ot_u32 d_get    = g_fps_get_frm - prev_get;
    ot_u32 d_cve    = g_fps_cve     - prev_cve;
    ot_u32 d_npu    = g_fps_npu     - prev_npu;
    ot_u32 d_subpix = g_fps_subpix  - prev_subpix;
    ot_u32 d_venc   = g_fps_venc    - prev_venc;
    ot_u32 d_net    = g_fps_net     - prev_net;

    stereo_log_write("[stereo] FPS: get=%.1f cve=%.1f npu=%.1f subpix=%.1f venc=%.1f net=%.1f drops=%u\n",
                     d_get  / elapsed, d_cve  / elapsed,
                     d_npu  / elapsed, d_subpix / elapsed,
                     d_venc / elapsed, d_net  / elapsed, g_drop_count);

    prev_get    = g_fps_get_frm;
    prev_cve    = g_fps_cve;
    prev_npu    = g_fps_npu;
    prev_subpix = g_fps_subpix;
    prev_venc   = g_fps_venc;
    prev_net    = g_fps_net;
    last_tv     = now;
}

/* -------------------------------------------------------------------------- */
/* Debug logging helper                                                        */
/* -------------------------------------------------------------------------- */

#define STEREO_LOG(fmt, ...) \
    stereo_log_write("[stereo] " fmt, ##__VA_ARGS__)


/* -------------------------------------------------------------------------- */
/* Default media config (adapted from uvc_media.c)                             */
/* -------------------------------------------------------------------------- */

static void stereo_get_default_media_cfg(stereo_media_ctx_t *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->sns_type    = SMS_SC132GS_MIPI_1_5M_30FPS_RAW12_DIRECT_SLAVE;
    ctx->dev_id[0]   = STEREO_DEV_ID_LEFT;   /* 0 */
    ctx->dev_id[1]   = STEREO_DEV_ID_RIGHT;  /* 2 */
}

/* -------------------------------------------------------------------------- */
/* VRB config (adapted from sample_uvc_get_vrb_cfg)                           */
/* -------------------------------------------------------------------------- */

static ot_s32 stereo_get_vrb_cfg(ot_u32 pipe_num, ot_u32 width, ot_u32 height,
                                  ot_video_buffer_attr *buffer_attr)
{
    ot_eis_buf_size_calc_cfg calc_cfg;
    ot_eis_img_attr img_attr;

    if (buffer_attr == OT_NULL) {
        return OT_FAILURE;
    }

    memset(buffer_attr, 0, sizeof(*buffer_attr));
    buffer_attr->cnt = 2;

    /* Block 0: Bayer RAW 12bpp */
    img_attr.width         = width;
    img_attr.height        = height;
    img_attr.bit_depth     = OT_EIS_PIXEL_BIT_DEPTH_8;
    img_attr.pixel_fmt     = OT_EIS_IMAGE_FORMAT_BAYER_12BPP;
    img_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    img_attr.layout        = OT_EIS_IMAGE_LAYOUT_LINEAR;
    sample_common_get_pic_buf_cfg(&img_attr, &calc_cfg);
    buffer_attr->buf_blks[0].size = calc_cfg.blk_size;
    buffer_attr->buf_blks[0].cnt  = pipe_num * 3;
    strcpy(buffer_attr->buf_blks[0].region_name, "anony");

    /* Block 1: YVU 420 semiplanar */
    img_attr.pixel_fmt     = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
    img_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    img_attr.layout        = OT_EIS_IMAGE_LAYOUT_LINEAR;
    sample_common_get_pic_buf_cfg(&img_attr, &calc_cfg);
    buffer_attr->buf_blks[1].size = calc_cfg.blk_size;
    buffer_attr->buf_blks[1].cnt  = pipe_num * 3;
    strcpy(buffer_attr->buf_blks[1].region_name, "anony");

    return OT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* User-mode buffer pool creation (adapted from uvc_media.c)                   */
/* -------------------------------------------------------------------------- */

static ot_s32 stereo_user_buf_pool_create(ot_eis_img_attr *img_attr,
                                           ot_eis_handle *pool_hdl)
{
    ot_s32 ret;
    ot_u32 block_size;
    ot_video_buffer_attr pool_attr;
    ot_eis_handle tmp;

    memset(&pool_attr, 0, sizeof(pool_attr));
    img_attr->bit_depth = OT_EIS_PIXEL_BIT_DEPTH_8;
    sample_common_get_buffer_pool_cfg(img_attr, &block_size);
    pool_attr.cnt = 1;
    pool_attr.buf_blks[0].cnt = 10;
    pool_attr.buf_blks[0].size = block_size;
    strcpy(pool_attr.buf_blks[0].region_name, "anony");

    ret = ot_buffer_pool_create(&tmp, &pool_attr);
    if (ret != OT_SUCCESS) {
        STEREO_LOG("buffer_pool create fail! ret[%d]\n", ret);
        return ret;
    }
    *pool_hdl = tmp;
    STEREO_LOG("create pool hdl 0x%llx\n", (ot_u64)tmp);
    return OT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* System init (VI, VPROC, NPU, CVE, VENC, buffer pool, media pipe)           */
/* -------------------------------------------------------------------------- */

ot_s32 stereo_media_sys_init(void)
{
    ot_s32 ret;
    ot_u32 pipe_num;
    ot_eis_img_size img_size;
    ot_video_buffer_attr vrb_cfg;
    ot_eis_vi_vproc_mode_type vi_vp_mode = OT_EIS_VI_OFFLINE_VPROC_OFFLINE;

    stereo_get_default_media_cfg(&g_media_ctx);

    /* Create local debug dir early so init-stage logs/images can land on /data
       even if a later init step fails. (Logger already mkdir's in main, but
       keep this for robustness when sys_init is reused standalone.) */
    mkdir(STEREO_DEBUG_LOCAL_PATH, 0755);

    /* Determine image size from sensor type */
    sample_comm_vi_get_size_by_sns_type(g_media_ctx.sns_type, &img_size);
    pipe_num = STEREO_SNS_NUM * STEREO_VI_PIPE_CNT; /* 2 * 2 = 4 pipes */

    STEREO_LOG("Sensor: %dx%d, pipes=%d\n", img_size.width, img_size.height, pipe_num);

    /* VRB config */
    stereo_get_vrb_cfg(pipe_num, img_size.width, img_size.height, &vrb_cfg);

    /* System init */
    ret = sample_comm_sys_init(&vrb_cfg);
    if (ret != OT_SUCCESS) {
        STEREO_LOG("sample_comm_sys_init failed, ret:0x%x\n", ret);
        return ret;
    }

    ret = sample_comm_sys_set_vi_vproc_init_cfg(vi_vp_mode);
    if (ret != OT_SUCCESS) {
        STEREO_LOG("set vi vp mode failed, ret:0x%x\n", ret);
        goto sys_exit;
    }

    /* Media pipe */
    ret = sample_comm_media_pipe_init(&g_media_pipe_hdl);
    if (ret != OT_SUCCESS) {
        STEREO_LOG("media pipe init failed, ret:0x%x\n", ret);
        goto sys_exit;
    }

    /* Buffer pool */
    ret = ot_buffer_pool_init();
    if (ret != OT_SUCCESS && ret != OT_ERR_BUFPOOL_ALREADY_INIT) {
        STEREO_LOG("buffer_pool init fail! ret[%d]\n", ret);
        goto bf_init_failed;
    }

    if (g_raw_only_mode == OT_TRUE) {
        g_sys_inited = OT_TRUE;
        STEREO_LOG("system init done (raw-only: VI + raw capture only)\n");
        return OT_SUCCESS;
    }

    /* Device authorization check — must pass before loading model */
    ret = stereo_sec_check_device();
    if (ret != OT_SUCCESS) {
        STEREO_LOG("stereo_sec_check_device failed, ret:0x%x\n", ret);
        goto npu_init_failed;
    }

    /* NPU init and model load (decrypts model from .ortm.enc) */
    ret = stereo_npu_init();
    if (ret != OT_SUCCESS) {
        STEREO_LOG("stereo_npu_init failed, ret:0x%x\n", ret);
        goto npu_init_failed;
    }

    /* Second model is optional: a missing yolo model degrades to stereo-only */
    ret = stereo_yolo_init();
    if (ret != OT_SUCCESS) {
        STEREO_LOG("stereo_yolo_init failed (ret:0x%x), stereo-only mode\n", ret);
        g_yolo_enabled = OT_FALSE;
    } else {
        g_yolo_enabled = OT_TRUE;
    }

    /* CVE init */
    ret = stereo_cve_init();
    if (ret != OT_SUCCESS) {
        STEREO_LOG("stereo_cve_init failed, ret:0x%x\n", ret);
        goto cve_init_failed;
    }

    /* VENC init */
    ret = stereo_venc_init();
    if (ret != OT_SUCCESS) {
        STEREO_LOG("stereo_venc_init failed, ret:0x%x\n", ret);
        goto venc_init_failed;
    }

    /* Network init — board listens as TCP server, PC connects */
    ret = stereo_network_init();
    if (ret != OT_SUCCESS) {
        STEREO_LOG("stereo_network_init failed, ret:0x%x\n", ret);
        goto net_init_failed;
    }

    g_sys_inited = OT_TRUE;
    STEREO_LOG("system init done\n");
    return OT_SUCCESS;

net_init_failed:
    stereo_venc_deinit();
venc_init_failed:
    stereo_cve_deinit();
cve_init_failed:
    stereo_npu_deinit();
npu_init_failed:
    ot_buffer_pool_deinit();
bf_init_failed:
    sample_comm_media_pipe_stop(g_media_pipe_hdl);
sys_exit:
    sample_comm_sys_exit();
    return ret;
}

void stereo_media_sys_deinit(void)
{
    if (g_sys_inited != OT_TRUE) {
        return;
    }

    if (g_raw_only_mode == OT_TRUE) {
        ot_buffer_pool_deinit();
        sample_comm_media_pipe_stop(g_media_pipe_hdl);
        sample_comm_sys_exit();
        g_sys_inited = OT_FALSE;
        STEREO_LOG("system deinit done (raw-only)\n");
        return;
    }

    stereo_network_disconnect();
    stereo_venc_deinit();
    stereo_cve_deinit();
    stereo_yolo_deinit();
    stereo_npu_deinit();
    ot_buffer_pool_deinit();
    sample_comm_media_pipe_stop(g_media_pipe_hdl);
    sample_comm_sys_exit();

    g_sys_inited = OT_FALSE;
    STEREO_LOG("system deinit done\n");
}

static ot_s32 stereo_media_startup_raw_only(void)
{
    ot_s32 ret;
    ot_s32 started_vi = 0;
    stereo_media_ctx_t *ctx = &g_media_ctx;

    for (ot_s32 i = 0; i < STEREO_SNS_NUM; i++) {
        sample_comm_vi_get_default_vi_cfg_by_dev_id(ctx->sns_type,
            &ctx->vi_cfg[i], ctx->dev_id[i]);

        for (ot_s32 j = 0; j < STEREO_VI_PIPE_CNT; j++) {
            ctx->vi_cfg[i].pipe_info[0].pipe_id[j] = STEREO_VI_PIPE_CNT * i + j;
        }

        ret = sample_comm_vi_start_vi(&ctx->vi_cfg[i]);
        if (ret != OT_SUCCESS) {
            STEREO_LOG("raw-only vi_start[%d] failed, ret:0x%x\n", i, ret);
            goto raw_only_failed;
        }
        ctx->vi_cfg[i].media_pipe_hdl = g_media_pipe_hdl;
        started_vi++;
    }

    {
        ot_eis_handle vi_pipe_hdl[STEREO_SCENE_PIPE_MAX] = {0};
        ot_eis_handle vp_grp_hdl[STEREO_SCENE_PIPE_MAX] = {0};
        for (ot_s32 i = 0; i < STEREO_SNS_NUM; i++) {
            vi_pipe_hdl[i] = ctx->vi_cfg[i].pipe_info[0].pipe_hdl;
            vp_grp_hdl[i] = OT_NULL;
        }
        ret = stereo_scene_auto_start("./param/sc132gs", vi_pipe_hdl, vp_grp_hdl, STEREO_SNS_NUM);
        if (ret != OT_SUCCESS) {
            STEREO_LOG("raw-only scene_auto_start failed (ret:0x%x), skipping\n", ret);
        } else {
            STEREO_LOG("raw-only scene_auto started\n");
        }
    }

    ret = stereo_raw_capture_start(ctx->vi_cfg[0].pipe_info[0].chn_info.chn_hdl,
                                   ctx->vi_cfg[1].pipe_info[0].chn_info.chn_hdl);
    if (ret != OT_SUCCESS) {
        STEREO_LOG("raw-only raw capture start failed, ret=0x%x\n", ret);
        goto raw_only_failed;
    }

    g_pipeline_started = OT_TRUE;
    STEREO_LOG("raw-only started (VI + scene_auto + raw capture on port %d)\n", STEREO_RAW_CAPTURE_PORT);
    return OT_SUCCESS;

raw_only_failed:
    stereo_raw_capture_stop();
    stereo_scene_auto_stop();
    for (ot_s32 i = started_vi - 1; i >= 0; i--) {
        sample_comm_vi_stop_vi(&ctx->vi_cfg[i]);
    }
    return ret;
}

/* -------------------------------------------------------------------------- */
/* Pipeline startup                                                            */
/* -------------------------------------------------------------------------- */

ot_s32 stereo_media_startup(void)
{
    ot_s32 ret;
    ot_s32 i;
    ot_s32 started_vi = 0;
    ot_s32 started_vproc = 0;
    stereo_media_ctx_t *ctx = &g_media_ctx;

    if (g_raw_only_mode == OT_TRUE) {
        return stereo_media_startup_raw_only();
    }

    /* Pipeline queue init */
    stereo_queue_init(&g_q_vproc,    1);  /* Depth 1 + drop-if-full = minimal vproc_q latency */
    stereo_queue_init(&g_q_npu,      1);  /* Depth 1 for double-buffer safety: CVE must not overwrite NPU input buffer */
    stereo_queue_init(&g_q_venc,     STEREO_QUEUE_CAPACITY);
    stereo_queue_init(&g_q_net,      STEREO_QUEUE_CAPACITY);

    /* -- VI startup (2 sensors) -- */
    for (i = 0; i < STEREO_SNS_NUM; i++) {
        sample_comm_vi_get_default_vi_cfg_by_dev_id(ctx->sns_type,
            &ctx->vi_cfg[i], ctx->dev_id[i]);

        /* Align sensor output to 20fps to match NPU ~47ms throughput.
           image_mode.fps=20: sensor driver mode-match (20<=30 OK), AE targets 20fps,
           cmos_fps_set adjusts VMAX: 1408*30/20 = 2112 (0x840),
           slave sync vs_time: 800000*30/20 = 1200000 cycles (50ms@24MHz).

           IMPORTANT: Do NOT use VI frame_rate_ctrl hardware skip (src=30,dst=20).
           The two sensors (master/slave) are hardware-synced at exposure level,
           but frame_rate_ctrl skips frames independently per-channel, causing
           L/R temporal mismatch (alternating 0ms/33-50ms PTS difference).
           This desynchronization corrupts stereo disparity in motion scenes.
           With fps=20 only (native VMAX), both sensors produce frames at the
           same 20fps rate with matching PTS, preserving stereo sync. */
        ctx->vi_cfg[i].pipe_info[0].pipe_attr.image_mode.fps = 20;
        ctx->vi_cfg[i].pipe_info[0].pipe_attr.frame_rate_ctrl.src_frame_rate = 30;
        ctx->vi_cfg[i].pipe_info[0].pipe_attr.frame_rate_ctrl.dst_frame_rate = 30;

        /* Disable temporal 3DNR to eliminate 1-frame reference buffer latency.
           For stereo matching, the NPU model tolerates sensor noise;
           spatial-only NR or full disable saves ~33ms. */
        ctx->vi_cfg[i].pipe_info[0].nr_attr.enable  = OT_FALSE;

        /* Pipe ID assignment: left=0,1; right=2,3 */
        for (ot_s32 j = 0; j < STEREO_VI_PIPE_CNT; j++) {
            ctx->vi_cfg[i].pipe_info[0].pipe_id[j] = STEREO_VI_PIPE_CNT * i + j;
        }

        ret = sample_comm_vi_start_vi(&ctx->vi_cfg[i]);
        if (ret != OT_SUCCESS) {
            STEREO_LOG("sample_comm_vi_start_vi[%d] failed, ret:0x%x\n", i, ret);
            goto start_vi_failed;
        }
        ctx->vi_cfg[i].media_pipe_hdl = g_media_pipe_hdl;
        started_vi++;
    }

    /* -- VPROC config (2 groups, ch0 only, 270° rotation) --
       Sensor native: 1080x1280.  After 270° rotation the full-frame output is
       1280x1080, matching the current calibration image_size. */
    for (i = 0; i < STEREO_SNS_NUM; i++) {
        sample_media_vproc_get_default_attr_by_snsor(ctx->sns_type, &ctx->vp_cfg[i]);

        /* Ch0: pre-rotation internal format 1080x1280, user mode */
        ctx->vp_cfg[i].chn_attr[0].mode                 = OT_EIS_VPROC_WORK_MODE_USER;
        ctx->vp_cfg[i].chn_attr[0].frame_queue_depth    = STEREO_VPROC_QUEUE_DEPTH;
        ctx->vp_cfg[i].chn_attr[0].image_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
        ctx->vp_cfg[i].chn_attr[0].image_attr.width     = STEREO_SENSOR_HEIGHT;  /* 1080 */
        ctx->vp_cfg[i].chn_attr[0].image_attr.height    = STEREO_SENSOR_WIDTH;   /* 1280 */
    }

    /* -- Pre-load XY-LUT rectification (undistort + stereo rectify) -- */
    /* GDC/XY-LUT processes the FULL sensor frame (1080x1280) before crop+Rotation270.
       LUTs are generated from the 1280x1080 rotated calibration space and map back to native space. */
    {
        const ot_u32 sensor_native_w = 1080;
        const ot_u32 sensor_native_h = 1280;
        ret = stereo_rectify_load_xylut(sensor_native_w, sensor_native_h);
        if (ret != OT_SUCCESS) {
            STEREO_LOG("stereo_rectify_load_xylut failed (ret=0x%x), aborting\n", ret);
            goto start_vi_failed;
        }
    }

    for (i = 0; i < STEREO_SNS_NUM; i++) {
        ot_eis_vproc_gdc_cfg *gdc_cfg = stereo_rectify_get_gdc_cfg(i);
        if (gdc_cfg == OT_NULL) {
            STEREO_LOG("cam[%d] XY-LUT GDC config missing, aborting\n", i);
            ret = OT_FAILURE;
            goto start_vi_failed;
        }
        if (gdc_cfg->dst.width > ctx->vp_cfg[i].grp_attr.max_width) {
            ctx->vp_cfg[i].grp_attr.max_width = gdc_cfg->dst.width;
        }
        if (gdc_cfg->dst.height > ctx->vp_cfg[i].grp_attr.max_height) {
            ctx->vp_cfg[i].grp_attr.max_height = gdc_cfg->dst.height;
        }
        STEREO_LOG("cam[%d] XY-LUT dst=%ux%u grp_max=%ux%u\n", i,
                   gdc_cfg->dst.width, gdc_cfg->dst.height,
                   ctx->vp_cfg[i].grp_attr.max_width, ctx->vp_cfg[i].grp_attr.max_height);
    }

    /* -- User buffer pools for VPROC ch0 (per sensor) -- */
    for (i = 0; i < STEREO_SNS_NUM; i++) {
        ot_eis_handle pool_hdl = OT_NULL;
        ret = stereo_user_buf_pool_create(
            &ctx->vp_cfg[i].chn_attr[0].image_attr, &pool_hdl);
        if (ret != OT_SUCCESS) {
            STEREO_LOG("user buffer pool[%d] create failed, ret:0x%x\n", i, ret);
            goto create_pool_failed;
        }
        ctx->vp_cfg[i].chn_attr[0].pool_handle = pool_hdl;
    }

    /* -- VPROC group start -- */
    {
        ot_bool pipe_sw[OT_EIS_VPROC_GRP_PIPE_MAX_NUM] = {OT_TRUE, OT_FALSE, OT_FALSE, OT_FALSE};
        ot_bool chnl_sw[OT_EIS_VPROC_GRP_CHN_MAX_NUM]  = {OT_TRUE, OT_FALSE, OT_FALSE, OT_FALSE};

        for (i = 0; i < STEREO_SNS_NUM; i++) {
            ret = sample_comm_start_vproc(&ctx->vp_cfg[i], pipe_sw, chnl_sw);
            if (ret != OT_SUCCESS) {
                STEREO_LOG("sample_comm_start_vproc[%d] failed, ret:0x%x\n", i, ret);
                goto start_vp_failed;
            }

            STEREO_LOG("cam[%d] VPROC started (XY-LUT pending runtime apply)\n", i);

            /* Full-frame crop: keep native 1080×1280 after GDC, then rotate to 1280×1080.
               grp_set_crop executes AFTER GDC in the hardware pipeline. */
            {
                ot_eis_vproc_crop_param crop;
                memset(&crop, 0, sizeof(crop));
                crop.enable    = OT_TRUE;
                crop.crop_type = OT_EIS_COORD_ABS;
                crop.crop_rect.x      = 0;
                crop.crop_rect.y      = 0;
                crop.crop_rect.width  = STEREO_SENSOR_HEIGHT;  /* 1080 */
                crop.crop_rect.height = STEREO_SENSOR_WIDTH;   /* 1280 */
                ot_eis_vproc_grp_set_crop(ctx->vp_cfg[i].grp_hdl, &crop);
            }

            /* 270° rotation: portrait (1080x1280) → landscape (1280x1080) */
            {
                ot_eis_vproc_chn_rotation ro;
                memset(&ro, 0, sizeof(ro));
                ro.enable = OT_TRUE;
                ro.angle  = OT_EIS_RTT_270;
                ot_eis_vproc_chn_set_rotation(ctx->vp_cfg[i].chn_hdl[0], &ro);
            }

            g_vproc_ctx.chn_hdl[i] = ctx->vp_cfg[i].chn_hdl[0];
            g_vproc_ctx.grp_hdl[i] = ctx->vp_cfg[i].grp_hdl;
            started_vproc++;
        }
    }

    ret = stereo_rectify_apply_xylut(g_vproc_ctx.grp_hdl[0], g_vproc_ctx.grp_hdl[1]);
    if (ret != OT_SUCCESS) {
        STEREO_LOG("stereo_rectify_apply_xylut failed (ret=0x%x)\n", ret);
        goto start_vp_failed;
    }

    /* -- VI -> VPROC bind -- */
    for (i = 0; i < STEREO_SNS_NUM; i++) {
        ret = sample_comm_vi_bind_vproc(
            ctx->vi_cfg[i].pipe_info[0].chn_info.chn_hdl,
            ctx->vp_cfg[i].pipe_hdl[0],
            g_media_pipe_hdl);
        if (ret != OT_SUCCESS) {
            STEREO_LOG("vi bind vp[%d] failed, ret:0x%x\n", i, ret);
            goto vi_bind_vp_failed;
        }
    }

    /* -- Scene auto (ISP tuning) -- */
    {
        ot_eis_handle vi_pipe_hdl[STEREO_SCENE_PIPE_MAX];
        ot_eis_handle vp_grp_hdl[STEREO_SCENE_PIPE_MAX];
        for (i = 0; i < STEREO_SNS_NUM; i++) {
            vi_pipe_hdl[i] = ctx->vi_cfg[i].pipe_info[0].pipe_hdl;
            vp_grp_hdl[i]  = ctx->vp_cfg[i].grp_hdl;
        }
        ret = stereo_scene_auto_start("./param/sc132gs", vi_pipe_hdl, vp_grp_hdl, STEREO_SNS_NUM);
        if (ret != OT_SUCCESS) {
            STEREO_LOG("scene_auto_start failed (ret:0x%x), skipping\n", ret);
            /* non-fatal: scene_auto is optional AE/AWB tuning */
        }
    }

    /* -- Start independent raw full-frame capture control server -- */
    ret = stereo_raw_capture_start(ctx->vi_cfg[0].pipe_info[0].chn_info.chn_hdl,
                                   ctx->vi_cfg[1].pipe_info[0].chn_info.chn_hdl);
    if (ret != OT_SUCCESS) {
        STEREO_LOG("raw capture control server start failed (ret=0x%x), streaming continues\n", ret);
    }

    /* -- Spawn worker threads -- */
    g_get_frm_run = OT_TRUE;
    g_cve_run     = OT_TRUE;
    g_npu_run     = OT_TRUE;
    g_venc_run    = OT_TRUE;
    g_net_run     = OT_TRUE;

    pthread_create(&g_get_frm_tid, NULL, stereo_get_frame_proc,  NULL);
    pthread_create(&g_cve_tid,     NULL, stereo_cve_proc,       NULL);
    pthread_create(&g_npu_tid,     NULL, stereo_npu_proc,       NULL);
    pthread_create(&g_venc_tid,    NULL, stereo_venc_proc,      NULL);
    pthread_create(&g_net_tid,     NULL, stereo_net_proc,       NULL);

    gettimeofday(&g_start_time, NULL);
    g_pipeline_started = OT_TRUE;
    STEREO_LOG("pipeline started (5 threads, NPU+SubPixel serial)\n");
    return OT_SUCCESS;

vi_bind_vp_failed:
start_vp_failed:
    for (i = started_vproc - 1; i >= 0; i--) {
        sample_comm_stop_vproc(&ctx->vp_cfg[i]);
    }
create_pool_failed:
start_vi_failed:
    stereo_rectify_deinit();
    for (i = started_vi - 1; i >= 0; i--) {
        sample_comm_vi_stop_vi(&ctx->vi_cfg[i]);
    }
    stereo_queue_destroy(&g_q_vproc);
    stereo_queue_destroy(&g_q_npu);
    stereo_queue_destroy(&g_q_venc);
    stereo_queue_destroy(&g_q_net);
    return ret;
}

void stereo_media_shutdown(void)
{
    if (g_pipeline_started != OT_TRUE) {
        return;
    }

    STEREO_LOG("shutting down pipeline...\n");

    if (g_raw_only_mode == OT_TRUE) {
        stereo_raw_capture_stop();
        stereo_scene_auto_stop();
        stereo_media_ctx_t *ctx = &g_media_ctx;
        for (int i = 0; i < STEREO_SNS_NUM; i++) {
            sample_comm_vi_stop_vi(&ctx->vi_cfg[i]);
        }
        g_pipeline_started = OT_FALSE;
        STEREO_LOG("raw-only shutdown done\n");
        return;
    }

    /*
     * Shutdown order matters: stop producers FIRST, join them, then
     * stop consumers.  This prevents the get_frame thread from blocking
     * forever in stereo_queue_push() while cve has already drained the
     * queue and exited.
     *
     *  1. Stop get_frame   (producer → g_q_vproc)
     *  2. Stop cve          (consumer of g_q_vproc, producer → g_q_npu)
     *  3. Stop npu          (consumer of g_q_npu, serial SubPixel, producer → g_q_venc)
     *  4. Stop venc         (consumer of g_q_venc,  producer → g_q_net)
     *  5. Stop net          (consumer of g_q_net)
     */

    /* Phase 1: stop the head producer */
    g_get_frm_run = OT_FALSE;
    pthread_join(g_get_frm_tid, NULL);

    /* Phase 2: stop downstream consumers in order */
    g_cve_run  = OT_FALSE;
    pthread_join(g_cve_tid,  NULL);

    g_npu_run  = OT_FALSE;
    pthread_join(g_npu_tid,  NULL);

    g_venc_run = OT_FALSE;
    pthread_join(g_venc_tid, NULL);

    g_net_run  = OT_FALSE;
    pthread_join(g_net_tid,  NULL);

    /* Stop independent raw capture control before VI teardown */
    stereo_raw_capture_stop();

    /* Stop scene auto */
    stereo_scene_auto_stop();

    /* Deinit XY-LUT rectification */
    stereo_rectify_deinit();

    /* Stop VPROC and VI */
    stereo_media_ctx_t *ctx = &g_media_ctx;
    for (int i = 0; i < STEREO_SNS_NUM; i++) {
        sample_comm_stop_vproc(&ctx->vp_cfg[i]);
        sample_comm_vi_stop_vi(&ctx->vi_cfg[i]);
    }

    /* Destroy queues */
    stereo_queue_destroy(&g_q_vproc);
    stereo_queue_destroy(&g_q_npu);
    stereo_queue_destroy(&g_q_venc);
    stereo_queue_destroy(&g_q_net);

    g_pipeline_started = OT_FALSE;
    STEREO_LOG("pipeline shutdown done, total frames=%u\n", g_total_frames);

    /* Best-effort: copy debug files from local eMMC to NFS for host access.
       This happens AFTER all threads are joined — NFS hang here does not
       affect pipeline operation or Ctrl+C responsiveness. */
    {
        char cmd[512];
        snprintf(cmd, sizeof(cmd),
                 "mkdir -p %s/stereo_debug && cp -r %s/* %s/stereo_debug/ 2>/dev/null &",
                 STEREO_NFS_LOG_PATH, STEREO_DEBUG_LOCAL_PATH, STEREO_NFS_LOG_PATH);
        system(cmd);
        STEREO_LOG("debug files copied to %s/stereo_debug (best-effort)\n", STEREO_NFS_LOG_PATH);
    }
}

/* -------------------------------------------------------------------------- */
/* get_frame thread: acquire L+R VPROC ch0 frames                             */
/* -------------------------------------------------------------------------- */

static void *stereo_get_frame_proc(void *p)
{
    ot_s32 ret;
    ot_u32 frame_cnt = 0;
    ot_eis_handle chn_l = g_vproc_ctx.chn_hdl[0];
    ot_eis_handle chn_r = g_vproc_ctx.chn_hdl[1];

    (void)p;
    prctl(PR_SET_NAME, "st_get_frm", 0, 0, 0);

    /* Drain stale buffers */
    {
        ot_eis_img_frame tmp;
        while (ot_eis_vproc_chn_acquire_frame(chn_l, &tmp, 0) == OT_SUCCESS) {
            ot_eis_vproc_chn_release_frame(chn_l, &tmp);
        }
        while (ot_eis_vproc_chn_acquire_frame(chn_r, &tmp, 0) == OT_SUCCESS) {
            ot_eis_vproc_chn_release_frame(chn_r, &tmp);
        }
    }

    STEREO_LOG("get_frame started\n");

    while (g_get_frm_run == OT_TRUE) {
        stereo_frame_pair_t *pair = (stereo_frame_pair_t *)malloc(sizeof(stereo_frame_pair_t));
        if (!pair) { usleep(5000); continue; }
        memset(pair, 0, sizeof(*pair));

        /* Acquire left frame */
        ret = ot_eis_vproc_chn_acquire_frame(chn_l, &pair->left_full, 500);
        if (ret != OT_SUCCESS) {
            STEREO_LOG("acquire L failed 0x%x\n", ret);
            free(pair);
            usleep(10 * 1000);
            continue;
        }

        /* Acquire right frame */
        ret = ot_eis_vproc_chn_acquire_frame(chn_r, &pair->right_full, 500);
        if (ret != OT_SUCCESS) {
            STEREO_LOG("acquire R failed 0x%x\n", ret);
            ot_eis_vproc_chn_release_frame(chn_l, &pair->left_full);
            free(pair);
            usleep(10 * 1000);
            continue;
        }

        /* Drain VPROC output queue to the freshest synchronized pair.
           The VPROC channel buffers up to STEREO_VPROC_QUEUE_DEPTH frames and
           acquire is FIFO, so without draining we'd process a stale frame and
           add up to several frames of HIDDEN latency BEFORE the timestamp
           below (this latency is invisible to board_e2e).

           IMPORTANT: We verify L/R frame index match to guarantee stereo
           synchronization. If one channel has more frames queued than the
           other (e.g. due to independent frame_rate_ctrl skipping or VPROC
           timing jitter), we drop the excess frame and keep draining until
           both indices match or both queues are empty. */
        {
            ot_eis_img_frame nl, nr;
            while (g_get_frm_run == OT_TRUE) {
                ot_s32 got_l = ot_eis_vproc_chn_acquire_frame(chn_l, &nl, 0);
                ot_s32 got_r = ot_eis_vproc_chn_acquire_frame(chn_r, &nr, 0);

                if (got_l == OT_SUCCESS && got_r == OT_SUCCESS) {
                    /* Both available — check index match */
                    if (nl.index == nr.index) {
                        /* Synchronized newer pair — replace */
                        ot_eis_vproc_chn_release_frame(chn_l, &pair->left_full);
                        ot_eis_vproc_chn_release_frame(chn_r, &pair->right_full);
                        pair->left_full  = nl;
                        pair->right_full = nr;
                        g_drop_count++;
                    } else {
                        /* Index mismatch — advance both to newest available.
                           The channel that is behind will be caught up in next
                           iteration if a matching frame exists in its queue. */
                        ot_eis_vproc_chn_release_frame(chn_l, &pair->left_full);
                        ot_eis_vproc_chn_release_frame(chn_r, &pair->right_full);
                        pair->left_full  = nl;
                        pair->right_full = nr;
                        g_drop_count++;
                        continue;
                    }
                } else if (got_l == OT_SUCCESS) {
                    /* Only L available — check if it matches current R */
                    if (nl.index == pair->right_full.index) {
                        /* Matches current R — replace L only */
                        ot_eis_vproc_chn_release_frame(chn_l, &pair->left_full);
                        pair->left_full = nl;
                    } else if (nl.index > pair->right_full.index) {
                        /* L is ahead of R — drop old L, take new L (R will catch up) */
                        ot_eis_vproc_chn_release_frame(chn_l, &pair->left_full);
                        pair->left_full = nl;
                        break;
                    } else {
                        /* L is behind R — discard it, keep current pair */
                        ot_eis_vproc_chn_release_frame(chn_l, &nl);
                        break;
                    }
                } else if (got_r == OT_SUCCESS) {
                    /* Only R available — check if it matches current L */
                    if (nr.index == pair->left_full.index) {
                        /* Matches current L — replace R only */
                        ot_eis_vproc_chn_release_frame(chn_r, &pair->right_full);
                        pair->right_full = nr;
                    } else if (nr.index > pair->left_full.index) {
                        /* R is ahead of L — drop old R, take new R (L will catch up) */
                        ot_eis_vproc_chn_release_frame(chn_r, &pair->right_full);
                        pair->right_full = nr;
                        break;
                    } else {
                        /* R is behind L — discard it, keep current pair */
                        ot_eis_vproc_chn_release_frame(chn_r, &nr);
                        break;
                    }
                } else {
                    /* Both queues empty — current pair is the freshest */
                    break;
                }
            }

            /* Log L/R index and pts for synchronization diagnosis.
               If pts values match (within 1ms tolerance), frames are from the
               same sensor exposure even if index counters differ. */
            if (frame_cnt < 10 || (frame_cnt % 100) == 0) {
                ot_u64 pts_diff = (pair->left_full.pts > pair->right_full.pts) ?
                    (pair->left_full.pts - pair->right_full.pts) :
                    (pair->right_full.pts - pair->left_full.pts);
                STEREO_LOG("sync: L_idx=%u R_idx=%u L_pts=%llu R_pts=%llu diff=%llu us\n",
                           pair->left_full.index, pair->right_full.index,
                           (ot_u64)pair->left_full.pts, (ot_u64)pair->right_full.pts,
                           pts_diff);
            }
        }

        pair->frame_index   = frame_cnt;
        pair->left_chn_hdl  = chn_l;
        pair->right_chn_hdl = chn_r;
        {
            struct timeval tv; gettimeofday(&tv, NULL);
            pair->timestamp_us = (ot_u64)tv.tv_sec * 1000000 + tv.tv_usec;
        }

        /* Drop-and-push: if queue is full, discard this frame immediately
           rather than retrying. This prevents stale-frame accumulation and
           keeps vproc_q latency minimal. The VPROC frames are released back
           to the hardware pool so acquisition can continue. */
        if (g_get_frm_run == OT_FALSE) {
            ot_eis_vproc_chn_release_frame(chn_l, &pair->left_full);
            ot_eis_vproc_chn_release_frame(chn_r, &pair->right_full);
            free(pair);
            break;
        }
        if (!stereo_queue_try_push(&g_q_vproc, pair)) {
            /* Queue full — drop this frame to keep pipeline fresh */
            ot_eis_vproc_chn_release_frame(chn_l, &pair->left_full);
            ot_eis_vproc_chn_release_frame(chn_r, &pair->right_full);
            free(pair);
            g_drop_count++;
        } else {
            frame_cnt++;
            g_fps_get_frm++;
        }
    }

    STEREO_LOG("get_frame exit, got %u frames\n", frame_cnt);
    return OT_NULL;
}

/* -------------------------------------------------------------------------- */
/* cve_proc thread: downsample + center crop                                   */
/* -------------------------------------------------------------------------- */

static void *stereo_cve_proc(void *p)
{
    (void)p;
    prctl(PR_SET_NAME, "st_cve", 0, 0, 0);

    while (g_cve_run == OT_TRUE) {
        stereo_frame_pair_t *pair = (stereo_frame_pair_t *)stereo_queue_pop_timeout(
            &g_q_vproc, 100, &g_cve_run);
        if (!pair) {
            continue;
        }

        stereo_npu_input_t *npu_in = (stereo_npu_input_t *)malloc(sizeof(stereo_npu_input_t));
        if (!npu_in) {
            ot_eis_vproc_chn_release_frame(pair->left_chn_hdl,  &pair->left_full);
            ot_eis_vproc_chn_release_frame(pair->right_chn_hdl, &pair->right_full);
            free(pair);
            continue;
        }
        memset(npu_in, 0, sizeof(*npu_in));

        /* Copy full frames for later VENC encoding */
        npu_in->left_full     = pair->left_full;
        npu_in->right_full    = pair->right_full;
        npu_in->left_chn_hdl  = pair->left_chn_hdl;
        npu_in->right_chn_hdl = pair->right_chn_hdl;
        npu_in->frame_index   = pair->frame_index;
        npu_in->timestamp_us  = pair->timestamp_us;

        /* Run CVE preprocessing (zero-copy: mmap VPROC frames, CVE ops) */
        ot_s32 ret = stereo_cve_process(pair, &npu_in->left_crop, &npu_in->right_crop);

        free(pair); /* frame_pair is consumed */

        if (ret != OT_SUCCESS) {
            STEREO_LOG("cve_process failed, ret:0x%x, dropping frame\n", ret);
            ot_eis_vproc_chn_release_frame(npu_in->left_chn_hdl,  &npu_in->left_full);
            ot_eis_vproc_chn_release_frame(npu_in->right_chn_hdl, &npu_in->right_full);
            free(npu_in);
            continue;
        }

        /* Push to NPU queue — drop if full (NPU still busy) to avoid
           blocking CVE thread and causing vproc queue accumulation */
        if (!stereo_queue_try_push(&g_q_npu, npu_in)) {
            ot_eis_vproc_chn_release_frame(npu_in->left_chn_hdl,  &npu_in->left_full);
            ot_eis_vproc_chn_release_frame(npu_in->right_chn_hdl, &npu_in->right_full);
            free(npu_in);
            g_drop_count++;
        } else {
            g_fps_cve++;
        }
    }

    return OT_NULL;
}

/* -------------------------------------------------------------------------- */
/* npu_proc thread: NPU inference + SubPixel serial refinement                  */
/* -------------------------------------------------------------------------- */

static void *stereo_npu_proc(void *p)
{
    (void)p;
    prctl(PR_SET_NAME, "st_npu", 0, 0, 0);

    while (g_npu_run == OT_TRUE) {
        stereo_npu_input_t *npu_in = (stereo_npu_input_t *)stereo_queue_pop_timeout(
            &g_q_npu, 100, &g_npu_run);
        if (!npu_in) {
            continue;
        }

        /* Run NPU inference, then run SubPixel synchronously on the same output set. */
        void *cost_data = NULL;
        ot_u32 cost_size = 0;
        void *disp_data = NULL;
        ot_u32 disp_size = 0;
        ot_u32 buf_set_idx = 0;

        /* ABAB: trigger stereo, then yolo (only when a fresh detection frame
           is available), then wait stereo and finish with yolo. */
        ot_bool det_ready = OT_FALSE;

        PERF_START(npu);
        ot_s32 ret = stereo_npu_trigger(&npu_in->left_crop, &npu_in->right_crop);
        if (ret == OT_SUCCESS && g_yolo_enabled == OT_TRUE &&
            g_det_frame_valid == OT_TRUE) {
            det_ready = (stereo_yolo_trigger() == OT_SUCCESS);
        }
        if (ret == OT_SUCCESS) {
            ret = stereo_npu_wait(&cost_data, &cost_size,
                                  &disp_data, &disp_size,
                                  &buf_set_idx);
        }
        PERF_END(npu, npu_ms);

        if (ret != OT_SUCCESS || !disp_data) {
            if (det_ready) {
                stereo_yolo_wait();
                g_det_frame_valid = OT_FALSE;
            }
            STEREO_LOG("npu_infer failed, ret:0x%x, dropping frame\n", ret);
            ot_eis_vproc_chn_release_frame(npu_in->left_chn_hdl,  &npu_in->left_full);
            ot_eis_vproc_chn_release_frame(npu_in->right_chn_hdl, &npu_in->right_full);
            free(npu_in);
            continue;
        }

        stereo_infer_output_t *out = (stereo_infer_output_t *)malloc(sizeof(stereo_infer_output_t));
        if (!out) {
            stereo_npu_release_buf_set(buf_set_idx);
            ot_eis_vproc_chn_release_frame(npu_in->left_chn_hdl,  &npu_in->left_full);
            ot_eis_vproc_chn_release_frame(npu_in->right_chn_hdl, &npu_in->right_full);
            free(npu_in);
            continue;
        }
        memset(out, 0, sizeof(*out));

        out->left_full     = npu_in->left_full;
        out->right_full    = npu_in->right_full;
        out->left_chn_hdl  = npu_in->left_chn_hdl;
        out->right_chn_hdl = npu_in->right_chn_hdl;
        out->frame_index   = npu_in->frame_index;
        out->timestamp_us  = npu_in->timestamp_us;

        ot_u32 disp_w = STEREO_NPU_OUTPUT_W;
        ot_u32 disp_h = STEREO_NPU_OUTPUT_H;
        ot_u32 disp_pixels = disp_w * disp_h;
        out->disp_bytes = disp_pixels * sizeof(ot_u16);
        out->disparity = (ot_u16 *)malloc(out->disp_bytes);

        if (!out->disparity) {
            STEREO_LOG("disparity malloc failed, dropping frame\n");
            stereo_npu_release_buf_set(buf_set_idx);
            ot_eis_vproc_chn_release_frame(out->left_chn_hdl,  &out->left_full);
            ot_eis_vproc_chn_release_frame(out->right_chn_hdl, &out->right_full);
            free(out);
            free(npu_in);
            continue;
        }

        if (cost_data != NULL) {
            /* Dual-output model: run SubPixel refinement on cost volume + integer disparity */
            PERF_START(subpix);
            ret = stereo_subpixel_process(disp_w, disp_h, STEREO_NPU_COST_CHANNELS,
                                          (const ot_u8 *)cost_data,
                                          (const ot_u8 *)disp_data,
                                          out->disparity);
            PERF_END(subpix, sub_ms);
        } else {
            /* Single-output model: NPU outputs float32 disparity at half spatial
               resolution (224x320 for 448x640 input). The disparity values are also
               at half scale, so multiply by 2 to get full-resolution disparity.
               Then convert float32 -> uint16 Q5 (real_disp = value / 32). */
            PERF_START(subpix);
            const float *fp32_disp = (const float *)disp_data;
            for (ot_u32 i = 0; i < disp_pixels; i++) {
                float v = fp32_disp[i] * 2.0f;  /* half-res -> full-res disparity */
                if (v < 0.0f) v = 0.0f;
                ot_u32 q5 = (ot_u32)(v * 32.0f + 0.5f);
                if (q5 > 0xFFFF) q5 = 0xFFFF;
                out->disparity[i] = (ot_u16)q5;
            }
            PERF_END(subpix, sub_ms);
            ret = OT_SUCCESS;
        }

        stereo_npu_release_buf_set(buf_set_idx);
        free(npu_in);

        if (ret != OT_SUCCESS) {
            STEREO_LOG("subpixel_process failed, dropping frame\n");
            free(out->disparity);
            ot_eis_vproc_chn_release_frame(out->left_chn_hdl,  &out->left_full);
            ot_eis_vproc_chn_release_frame(out->right_chn_hdl, &out->right_full);
            free(out);
            continue;
        }

        /* Upsample disparity from NPU output (320x224) to display output (640x448)
           using nearest-neighbor interpolation. Each pixel is duplicated 2x2.
           This doubles the spatial resolution for display and measurement without
           changing disparity values (still in NPU input resolution). */
        {
            ot_u32 sub_w  = STEREO_NPU_OUTPUT_W;   /* 320 */
            ot_u32 sub_h  = STEREO_NPU_OUTPUT_H;   /* 224 */
            ot_u32 up_w   = STEREO_DISP_OUTPUT_W;  /* 640 */
            ot_u32 up_h   = STEREO_DISP_OUTPUT_H;  /* 448 */
            ot_u32 up_pixels = up_w * up_h;
            ot_u32 up_bytes  = up_pixels * sizeof(ot_u16);

            ot_u16 *upsampled = (ot_u16 *)malloc(up_bytes);
            if (upsampled) {
                for (ot_u32 y = 0; y < up_h; y++) {
                    ot_u32 sy = y / 2;
                    if (sy >= sub_h) sy = sub_h - 1;
                    const ot_u16 *src_row = out->disparity + sy * sub_w;
                    ot_u16 *dst_row = upsampled + y * up_w;
                    for (ot_u32 x = 0; x < up_w; x++) {
                        ot_u32 sx = x / 2;
                        if (sx >= sub_w) sx = sub_w - 1;
                        dst_row[x] = src_row[sx];
                    }
                }
                free(out->disparity);
                out->disparity = upsampled;
                out->disp_bytes = up_bytes;
            }
        }

        /* Finish the second model and decode boxes in 416x416 space; M5 maps
           them to the 1280x1080 left frame and draws the rectangles. */
        if (det_ready) {
            ot_s32 yret = stereo_yolo_wait();
            if (yret == OT_SUCCESS) {
                (void)stereo_yolo_decode(g_yolo_boxes, STEREO_YOLO_MAX_BOX);
            }
            g_det_frame_valid = OT_FALSE;
        }

        /* Push to VENC queue — drop if full to avoid blocking NPU thread */
        if (!stereo_queue_try_push(&g_q_venc, out)) {
            if (out->disparity) free(out->disparity);
            ot_eis_vproc_chn_release_frame(out->left_chn_hdl,  &out->left_full);
            ot_eis_vproc_chn_release_frame(out->right_chn_hdl, &out->right_full);
            free(out);
            g_drop_count++;
        } else {
            g_fps_npu++;
            g_fps_subpix++;
        }
    }

    return OT_NULL;
}

/* -------------------------------------------------------------------------- */
/* venc_proc thread: JPEG encoding                                             */
/* -------------------------------------------------------------------------- */

static void *stereo_venc_proc(void *p)
{
    (void)p;
    prctl(PR_SET_NAME, "st_venc", 0, 0, 0);

    while (g_venc_run == OT_TRUE) {
        stereo_infer_output_t *infer_out = (stereo_infer_output_t *)stereo_queue_pop_timeout(
            &g_q_venc, 100, &g_venc_run);
        if (!infer_out) {
            continue;
        }

        stereo_venc_output_t *venc_out = (stereo_venc_output_t *)malloc(sizeof(stereo_venc_output_t));
        if (!venc_out) {
            ot_eis_vproc_chn_release_frame(infer_out->left_chn_hdl,  &infer_out->left_full);
            ot_eis_vproc_chn_release_frame(infer_out->right_chn_hdl, &infer_out->right_full);
            free(infer_out);
            continue;
        }
        memset(venc_out, 0, sizeof(*venc_out));

        venc_out->frame_index  = infer_out->frame_index;
        venc_out->timestamp_us = infer_out->timestamp_us;

        /* Encode left/right JPEG + disparity LZ4 */
        ot_s32 ret = stereo_venc_encode(&infer_out->left_full,
                                         &infer_out->right_full,
                                         (const ot_u8 *)infer_out->disparity,
                                         infer_out->disp_bytes,
                                         venc_out);

        /* Release VPROC frames back to the pool */
        ot_eis_vproc_chn_release_frame(infer_out->left_chn_hdl,  &infer_out->left_full);
        ot_eis_vproc_chn_release_frame(infer_out->right_chn_hdl, &infer_out->right_full);
        if (infer_out->disparity) free(infer_out->disparity);
        free(infer_out);

        if (ret != OT_SUCCESS) {
            STEREO_LOG("venc_encode failed, ret:0x%x, dropping frame\n", ret);
            /* Free JPEG data if any was allocated */
            for (int i = 0; i < STEREO_VENC_CH_NUM; i++) {
                if (venc_out->packets[i].data) {
                    free(venc_out->packets[i].data);
                }
            }
            free(venc_out);
            continue;
        }

        /* Push to network queue — drop if full to avoid blocking VENC thread */
        if (!stereo_queue_try_push(&g_q_net, venc_out)) {
            for (int i = 0; i < STEREO_VENC_CH_NUM; i++) {
                if (venc_out->packets[i].data) free(venc_out->packets[i].data);
            }
            free(venc_out);
            g_drop_count++;
        } else {
            g_fps_venc++;
        }
    }

    return OT_NULL;
}

/* -------------------------------------------------------------------------- */
/* net_proc thread: TCP server — accept PC connections, send frames            */
/* -------------------------------------------------------------------------- */

static void *stereo_net_proc(void *p)
{
    (void)p;
    prctl(PR_SET_NAME, "st_net", 0, 0, 0);

    while (g_net_run == OT_TRUE) {

        /* Accept new client if none connected (non-blocking) */
        if (!stereo_network_is_connected()) {
            if (stereo_network_accept() == OT_SUCCESS) {
                /* Send calibration parameters once on new connection */
                stereo_network_send_calib();
            }

            /* Drain queued frames while no client — prevents backpressure */
            stereo_venc_output_t *drop;
            while ((drop = stereo_queue_try_pop(&g_q_net)) != NULL) {
                for (int i = 0; i < STEREO_VENC_CH_NUM; i++) {
                    if (drop->packets[i].data) free(drop->packets[i].data);
                }
                free(drop);
                g_total_frames++;
            }
            usleep(50 * 1000); /* 50ms poll when idle */
            continue;
        }

        /* Connected: wait for frame with timeout (allows quit check) */
        stereo_venc_output_t *venc_out = (stereo_venc_output_t *)stereo_queue_pop_timeout(
            &g_q_net, 100, &g_net_run);
        if (!venc_out) {
            continue;
        }

        /* Send, auto-drop client on error (perf JSON sent inside stereo_network_send) */
        stereo_network_send(venc_out);

        /* Log (periodic FPS/size summary) */
        stereo_network_write_log(venc_out->frame_index, venc_out->timestamp_us,
            venc_out->packets[0].size, venc_out->packets[1].size, venc_out->packets[2].size);

        /* Free */
        for (int i = 0; i < STEREO_VENC_CH_NUM; i++) {
            if (venc_out->packets[i].data) free(venc_out->packets[i].data);
        }
        free(venc_out);
        g_total_frames++;
        g_fps_net++;
    }

    return OT_NULL;
}
