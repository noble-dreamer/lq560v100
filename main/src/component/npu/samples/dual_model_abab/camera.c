/*
 * 相机源模块实现。
 *
 * 数据流：SC132GS 双目 1080x1280@30fps -> 左右 vproc 组中心裁剪
 * 960x1280@(60,0) + 270° 旋转 -> chn0 输出左右原生 1280x960 帧；左组
 * chn2 检测通道缩放为 416x312 YUV420SP，FRC 30->10。采集线程持续取流，
 * 单槽缓存最新检测帧（队列上界 1），推理侧按需拷贝。
 *
 * 模型输入是 416x416 YUV420SP（omg 把原始输入尺寸钉死在 ONNX 头尺寸，
 * 无法直接转出 640x480 输入），因此 416x312 内容上下各补 52 行灰边
 * （Y=UV=128），与离线转换时 letterbox 的标定域一致。
 */
#include "camera.h"
#include "sample_comm.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <time.h>
#include <unistd.h>
#include "ot_scene.h"
#include "ot_scenecomm.h"
#include "ot_smr.h"

#define CAMERA_SNS_TYPE   SMS_SC132GS_MIPI_1_5M_30FPS_RAW12_DIRECT_SLAVE
#define CAMERA_PIPE_NUM   4
#define CAMERA_DET_CHN    2
#define CAMERA_TOP_PAD    ((CAMERA_NPU_IN_H - CAMERA_DET_OUT_H) / 2)

typedef struct {
    ot_eis_handle vi_pipe_hdl;
    ot_eis_handle vp_grp_hdl;
    ot_bool       run;
    pthread_t     tid;
} camera_scene_ctx;

typedef struct {
    ot_scene_param   scene_param;
    ot_u32           total_pipe;
    camera_scene_ctx norm_ctx[OT_SCENE_PIPE_MAX_NUM];
} camera_scene_auto_ctx;

static camera_scene_auto_ctx gs_scene;

typedef struct {
    sample_vi_cfg     vi_cfg[2];
    sample_vproc_attr vp_cfg[2];
    ot_bool           started;
    ot_bool           get_run;
    pthread_t         get_tid;
    ot_eis_handle     det_chn;
    pthread_mutex_t   lock;
    pthread_cond_t    cond;
    ot_bool           slot_valid;
    ot_eis_img_frame  slot_frame;
    ot_u32            det_frames;
    ot_u32            lr_pairs;
    ot_u64            dpts_sum;
    ot_u64            dpts_min;
    ot_u64            dpts_max;
    ot_bool           det_info_printed;
} camera_ctx;

static camera_ctx gs_ctx;
static ot_bool gs_sys_init = OT_FALSE;
static ot_eis_handle gs_media_pipe_hdl = OT_NULL;

static void *camera_scene_norm_proc(void *p)
{
    ot_s32 ret = OT_SUCCESS;
    camera_scene_ctx *ctx = (camera_scene_ctx *)p;

    ret = ot_scene_static_vi_set_param(ctx->vi_pipe_hdl, ctx->vp_grp_hdl);
    if (ret != OT_SUCCESS) {
        sample_print("camera scene static vi set param failed, ret:0x%x\n", ret);
        goto proc_exit;
    }

    ret = ot_scene_dynamic_normal_init(ctx->vi_pipe_hdl);
    if (ret != OT_SUCCESS) {
        sample_print("camera scene dynamic normal init failed, ret:0x%x\n", ret);
        goto proc_exit;
    }

    while (ctx->run == OT_TRUE) {
        ret = ot_scene_dynamic_normal_set_param(ctx->vi_pipe_hdl, ctx->vp_grp_hdl);
        usleep(100 * 1000);
        if (ret != OT_SUCCESS) {
            printf("camera scene dynamic set param failed, ret:0x%x\n", ret);
            continue;
        }
    }

    ot_scene_dynamic_normal_deinit(ctx->vi_pipe_hdl);

proc_exit:
    ctx->run = OT_FALSE;
    return OT_NULL;
}

ot_s32 camera_scene_auto_start(const char *ini_path,
                               ot_eis_handle vi_pipe_hdl[OT_SCENE_PIPE_MAX_NUM],
                               ot_eis_handle vp_grp_hdl[OT_SCENE_PIPE_MAX_NUM],
                               ot_u32 max_hdl_num)
{
    ot_s32 ret = OT_SUCCESS;
    ot_u32 i = 0;
    camera_scene_auto_ctx *ctx = &gs_scene;

    memset(ctx, 0, sizeof(camera_scene_auto_ctx));

    ret = ot_scene_create_param(ini_path, &ctx->scene_param, NULL);
    if (ret != OT_SUCCESS) {
        sample_print("camera scene create param failed\n");
        return ret;
    }

    ret = ot_scene_init(&ctx->scene_param);
    if (ret != OT_SUCCESS) {
        sample_print("camera scene init failed, ret:0x%x\n", ret);
        return ret;
    }

    for (i = 0; i < max_hdl_num; i++) {
        ctx->norm_ctx[i].vi_pipe_hdl = vi_pipe_hdl[i];
        ctx->norm_ctx[i].vp_grp_hdl = vp_grp_hdl[i];
    }

    for (i = 0; i < max_hdl_num; i++) {
        ctx->norm_ctx[i].run = OT_TRUE;
        ret = pthread_create(&ctx->norm_ctx[i].tid, NULL, camera_scene_norm_proc,
                             (ot_void *)(ctx->norm_ctx + i));
        if (ret != 0) {
            sample_print("camera scene create thread failed, ret=%d\n", ret);
            goto start_norm_proc_failed;
        }
    }

    ctx->total_pipe = max_hdl_num;
    return OT_SUCCESS;

start_norm_proc_failed:
    for (ot_u32 k = 0; k < i; k++) {
        ctx->norm_ctx[k].run = OT_FALSE;
        pthread_join(ctx->norm_ctx[k].tid, NULL);
    }
    ot_scene_deinit();
    return ret;
}

void camera_scene_auto_stop(void)
{
    camera_scene_auto_ctx *ctx = &gs_scene;

    for (ot_u32 i = 0; i < ctx->total_pipe; i++) {
        ctx->norm_ctx[i].run = OT_FALSE;
        pthread_join(ctx->norm_ctx[i].tid, NULL);
    }
    ot_scene_deinit();
}

static ot_s32 camera_get_vrb_cfg(ot_u32 pipe_num, ot_u32 width, ot_u32 height,
                                 ot_video_buffer_attr *buffer_attr)
{
    ot_eis_buf_size_calc_cfg calc_cfg;
    ot_eis_img_attr img_attr;

    if (buffer_attr == OT_NULL) {
        return -1;
    }

    buffer_attr->cnt = 2;

    img_attr.width = width;
    img_attr.height = height;
    img_attr.bit_depth = OT_EIS_PIXEL_BIT_DEPTH_8;
    img_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_BAYER_12BPP;
    img_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    img_attr.layout = OT_EIS_IMAGE_LAYOUT_LINEAR;
    sample_common_get_pic_buf_cfg(&img_attr, &calc_cfg);
    buffer_attr->buf_blks[0].size = calc_cfg.blk_size;
    buffer_attr->buf_blks[0].cnt = pipe_num * 3;
    strcpy(buffer_attr->buf_blks[0].region_name, "anony");

    img_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
    sample_common_get_pic_buf_cfg(&img_attr, &calc_cfg);
    buffer_attr->buf_blks[1].size = calc_cfg.blk_size;
    buffer_attr->buf_blks[1].cnt = pipe_num * 3;
    strcpy(buffer_attr->buf_blks[1].region_name, "anony");

    return 0;
}

int32_t camera_init(void)
{
    ot_s32 ret;
    ot_eis_img_size img_size = {0};
    ot_video_buffer_attr vrb_cfg;

    sample_comm_vi_get_size_by_sns_type(CAMERA_SNS_TYPE, &img_size);
    camera_get_vrb_cfg(CAMERA_PIPE_NUM, img_size.width, img_size.height, &vrb_cfg);
    ret = sample_comm_sys_init(&vrb_cfg);
    if (ret != OT_SUCCESS) {
        printf("[camera] sample_comm_sys_init fail, ret=%d\n", ret);
        return ret;
    }

    ret = sample_comm_sys_set_vi_vproc_init_cfg(OT_EIS_VI_OFFLINE_VPROC_OFFLINE);
    if (ret != OT_SUCCESS) {
        printf("[camera] set vi/vproc offline fail, ret=%d\n", ret);
        sample_comm_sys_exit();
        return ret;
    }

    ret = sample_comm_media_pipe_init(&gs_media_pipe_hdl);
    if (ret != OT_SUCCESS) {
        printf("[camera] media pipe init fail, ret=%d\n", ret);
        sample_comm_sys_exit();
        return ret;
    }

    ret = ot_buffer_pool_init();
    if (ret != OT_SUCCESS && ret != OT_ERR_BUFPOOL_ALREADY_INIT) {
        printf("[camera] buffer pool init fail, ret=%d\n", ret);
        sample_comm_media_pipe_stop(gs_media_pipe_hdl);
        sample_comm_sys_exit();
        return ret;
    }

    pthread_mutex_init(&gs_ctx.lock, NULL);
    pthread_cond_init(&gs_ctx.cond, NULL);
    gs_sys_init = OT_TRUE;
    printf("[camera] media system init ok\n");
    return OT_SUCCESS;
}

void camera_deinit(void)
{
    if (gs_sys_init != OT_TRUE) {
        return;
    }
    ot_buffer_pool_deinit();
    sample_comm_media_pipe_stop(gs_media_pipe_hdl);
    sample_comm_sys_exit();
    gs_sys_init = OT_FALSE;
    pthread_cond_destroy(&gs_ctx.cond);
    pthread_mutex_destroy(&gs_ctx.lock);
    printf("[camera] media system exit ok\n");
}

static void *camera_get_frame_proc(void *p);

static ot_s32 camera_user_pool_create(ot_eis_img_attr *img_attr, ot_eis_handle *pool_hdl)
{
    ot_s32 ret;
    ot_u32 block_size = 0;
    ot_video_buffer_attr pool_attr = {0};
    ot_eis_handle tmp = OT_NULL;

    img_attr->bit_depth = OT_EIS_PIXEL_BIT_DEPTH_8;
    sample_common_get_buffer_pool_cfg(img_attr, &block_size);
    pool_attr.cnt = 1;
    pool_attr.buf_blks[0].cnt = 10;
    pool_attr.buf_blks[0].size = block_size;
    ret = ot_buffer_pool_create(&tmp, &pool_attr);
    if (ret != OT_SUCCESS) {
        printf("[camera] buffer pool create fail, ret=%d\n", ret);
        return ret;
    }
    *pool_hdl = tmp;
    return OT_SUCCESS;
}

int32_t camera_start(uint32_t camera_fps)
{
    ot_s32 ret = OT_SUCCESS;
    ot_s32 i = 0;
    ot_s32 done_vi = 0;
    ot_s32 done_pool0 = 0;
    ot_s32 done_pool2 = 0;
    ot_s32 done_vp = 0;
    ot_s32 done_bind = 0;
    ot_bool pipe_sw[OT_EIS_VPROC_GRP_PIPE_MAX_NUM] = {OT_TRUE, OT_FALSE, OT_FALSE, OT_FALSE};
    ot_bool chnl_sw_l[OT_EIS_VPROC_GRP_CHN_MAX_NUM] = {OT_TRUE, OT_FALSE, OT_TRUE, OT_FALSE};
    ot_bool chnl_sw_r[OT_EIS_VPROC_GRP_CHN_MAX_NUM] = {OT_TRUE, OT_FALSE, OT_FALSE, OT_FALSE};
    ot_eis_handle vi_pipe_hdl[OT_EIS_VI_MAX_PIPE_NUM] = {0};
    ot_eis_handle vp_grp_hdl[OT_EIS_VI_MAX_PIPE_NUM] = {0};
    uint32_t fps = camera_fps;

    if (gs_sys_init != OT_TRUE || gs_ctx.started == OT_TRUE) {
        return OT_FAILURE;
    }
    if (fps == 0) {
        fps = CAMERA_FPS_DEFAULT;
    }
    if (fps > CAMERA_FPS_MAX) {
        fps = CAMERA_FPS_MAX;
    }

    for (i = 0; i < 2; i++) {
        ot_s32 dev_id = (i == 0) ? 0 : 2;

        sample_comm_vi_get_default_vi_cfg_by_dev_id(CAMERA_SNS_TYPE, &gs_ctx.vi_cfg[i], dev_id);
        for (ot_s32 j = 0; j < 2; j++) {
            gs_ctx.vi_cfg[i].pipe_info[0].pipe_id[j] = 2 * i + j;
        }
        ret = sample_comm_vi_start_vi(&gs_ctx.vi_cfg[i]);
        if (ret != OT_SUCCESS) {
            sample_print("camera vi start %d fail, ret:0x%x\n", i, ret);
            goto vi_fail;
        }
        gs_ctx.vi_cfg[i].media_pipe_hdl = gs_media_pipe_hdl;
        done_vi++;
    }
    for (i = 0; i < 2; i++) {
        sample_media_vproc_get_default_attr_by_snsor(CAMERA_SNS_TYPE, &gs_ctx.vp_cfg[i]);
        gs_ctx.vp_cfg[i].chn_attr[0].mode = OT_EIS_VPROC_WORK_MODE_USER;
        gs_ctx.vp_cfg[i].chn_attr[0].frame_queue_depth = VPROC_FRAME_QUEUE_DEPTH;
        gs_ctx.vp_cfg[i].chn_attr[0].image_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
        gs_ctx.vp_cfg[i].chn_attr[0].image_attr.width = 960;
        gs_ctx.vp_cfg[i].chn_attr[0].image_attr.height = 1280;
    }
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].mode = OT_EIS_VPROC_WORK_MODE_USER;
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].frame_queue_depth = 2;
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].image_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_YUV_420_SEMIPLANAR;
    /* 270° 旋转交换输出宽高：按竖版 W=H_out/H=W_out 配置 */
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].image_attr.width = CAMERA_DET_OUT_H;
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].image_attr.height = CAMERA_DET_OUT_W;
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].image_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].frc.src_frame_rate = 30;
    gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].frc.dst_frame_rate = (ot_s32)fps;

    for (i = 0; i < 2; i++) {
        ot_eis_handle pool_hdl = OT_NULL;

        ret = camera_user_pool_create(&gs_ctx.vp_cfg[i].chn_attr[0].image_attr, &pool_hdl);
        if (ret != OT_SUCCESS) {
            goto pool_fail;
        }
        gs_ctx.vp_cfg[i].chn_attr[0].pool_handle = pool_hdl;
        done_pool0++;
    }
    {
        ot_eis_handle pool_hdl = OT_NULL;

        ret = camera_user_pool_create(&gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].image_attr,
                                      &pool_hdl);
        if (ret != OT_SUCCESS) {
            goto pool_fail;
        }
        gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].pool_handle = pool_hdl;
        done_pool2++;
    }
    ret = sample_comm_start_vproc(&gs_ctx.vp_cfg[0], pipe_sw, chnl_sw_l);
    if (ret != OT_SUCCESS) {
        sample_print("camera vproc L start fail\n");
        goto vp_fail;
    }
    done_vp++;
    gs_ctx.det_chn = gs_ctx.vp_cfg[0].chn_hdl[CAMERA_DET_CHN];
    ret = sample_comm_start_vproc(&gs_ctx.vp_cfg[1], pipe_sw, chnl_sw_r);
    if (ret != OT_SUCCESS) {
        sample_print("camera vproc R start fail\n");
        goto vp_fail;
    }
    done_vp++;

    for (i = 0; i < 2; i++) {
        ot_eis_vproc_chn_rotation ro_param = {0};

        gs_ctx.vp_cfg[i].set_attr.crop_param.enable = OT_TRUE;
        gs_ctx.vp_cfg[i].set_attr.crop_param.crop_type = OT_EIS_COORD_ABS;
        gs_ctx.vp_cfg[i].set_attr.crop_param.crop_rect.x = 60;
        gs_ctx.vp_cfg[i].set_attr.crop_param.crop_rect.y = 0;
        gs_ctx.vp_cfg[i].set_attr.crop_param.crop_rect.width = 960;
        gs_ctx.vp_cfg[i].set_attr.crop_param.crop_rect.height = 1280;
        ot_eis_vproc_grp_set_crop(gs_ctx.vp_cfg[i].grp_hdl,
                                  &gs_ctx.vp_cfg[i].set_attr.crop_param);
        ro_param.enable = OT_TRUE;
        ro_param.angle = OT_EIS_RTT_270;
        ot_eis_vproc_chn_set_rotation(gs_ctx.vp_cfg[i].chn_hdl[0], &ro_param);
        if (i == 0) {
            ot_eis_vproc_chn_set_rotation(gs_ctx.vp_cfg[i].chn_hdl[CAMERA_DET_CHN], &ro_param);
        }
    }
    for (i = 0; i < 2; i++) {
        ret = sample_comm_vi_bind_vproc(gs_ctx.vi_cfg[i].pipe_info[0].chn_info.chn_hdl,
                                        gs_ctx.vp_cfg[i].pipe_hdl[0], gs_media_pipe_hdl);
        if (ret != OT_SUCCESS) {
            sample_print("camera vi bind vproc %d fail, ret:0x%x\n", i, ret);
            goto bind_fail;
        }
        done_bind++;
    }
    for (i = 0; i < 2; i++) {
        vi_pipe_hdl[i] = gs_ctx.vi_cfg[i].pipe_info[0].pipe_hdl;
        vp_grp_hdl[i] = gs_ctx.vp_cfg[i].grp_hdl;
    }
    ret = camera_scene_auto_start("./param/sc132gs", vi_pipe_hdl, vp_grp_hdl, 2);
    if (ret != OT_SUCCESS) {
        sample_print("camera scene auto start fail, ret:0x%x\n", ret);
        goto bind_fail;
    }
    gs_ctx.get_run = OT_TRUE;
    if (pthread_create(&gs_ctx.get_tid, NULL, camera_get_frame_proc, &gs_ctx) != 0) {
        sample_print("camera get-frame thread create fail\n");
        gs_ctx.get_run = OT_FALSE;
        camera_scene_auto_stop();
        goto bind_fail;
    }
    gs_ctx.started = OT_TRUE;
    printf("[camera] pipeline up: chn0=960x1280 YVU420SP, chn2=%dx%d YUV420SP frc=30->%u\n",
           CAMERA_DET_OUT_W, CAMERA_DET_OUT_H, fps);
    return OT_SUCCESS;

bind_fail:
    for (i = 0; i < done_bind; i++) {
        sample_comm_vi_un_bind_vproc(gs_ctx.vi_cfg[i].pipe_info[0].chn_info.chn_hdl,
                                     gs_ctx.vp_cfg[i].pipe_hdl[0], gs_media_pipe_hdl);
    }
vp_fail:
    for (i = 0; i < done_vp; i++) {
        sample_comm_stop_vproc(&gs_ctx.vp_cfg[i]);
    }
pool_fail:
    for (i = 0; i < done_pool0; i++) {
        ot_buffer_pool_destroy(gs_ctx.vp_cfg[i].chn_attr[0].pool_handle);
    }
    if (done_pool2 != 0) {
        ot_buffer_pool_destroy(gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].pool_handle);
    }
vi_fail:
    for (i = 0; i < done_vi; i++) {
        sample_comm_vi_stop_vi(&gs_ctx.vi_cfg[i]);
    }
    return ret;
}

void camera_stop(void)
{
    if (gs_ctx.started != OT_TRUE) {
        return;
    }
    if (gs_ctx.get_run == OT_TRUE) {
        gs_ctx.get_run = OT_FALSE;
        pthread_join(gs_ctx.get_tid, NULL);
    }
    camera_scene_auto_stop();
    for (ot_s32 i = 0; i < 2; i++) {
        sample_comm_vi_un_bind_vproc(gs_ctx.vi_cfg[i].pipe_info[0].chn_info.chn_hdl,
                                     gs_ctx.vp_cfg[i].pipe_hdl[0], gs_media_pipe_hdl);
    }
    for (ot_s32 i = 0; i < 2; i++) {
        sample_comm_stop_vproc(&gs_ctx.vp_cfg[i]);
        ot_buffer_pool_destroy(gs_ctx.vp_cfg[i].chn_attr[0].pool_handle);
    }
    ot_buffer_pool_destroy(gs_ctx.vp_cfg[0].chn_attr[CAMERA_DET_CHN].pool_handle);
    for (ot_s32 i = 0; i < 2; i++) {
        sample_comm_vi_stop_vi(&gs_ctx.vi_cfg[i]);
    }
    gs_ctx.started = OT_FALSE;
    printf("[camera] pipeline stopped\n");
}

static void camera_record_pair(ot_u64 pts_l, ot_u64 pts_r)
{
    ot_u64 dpts = (pts_l > pts_r) ? (pts_l - pts_r) : (pts_r - pts_l);

    pthread_mutex_lock(&gs_ctx.lock);
    gs_ctx.lr_pairs++;
    gs_ctx.dpts_sum += dpts;
    if (gs_ctx.lr_pairs == 1 || dpts < gs_ctx.dpts_min) {
        gs_ctx.dpts_min = dpts;
    }
    if (dpts > gs_ctx.dpts_max) {
        gs_ctx.dpts_max = dpts;
    }
    pthread_mutex_unlock(&gs_ctx.lock);
}

/* 采集线程：左右原生帧按 PTS 配对统计；检测通道非阻塞取流，单槽缓存
 * 最新一帧并丢弃积压，推理侧需要时再拷贝。 */
static void *camera_get_frame_proc(void *p)
{
    camera_ctx *ctx = (camera_ctx *)p;
    ot_eis_handle chn_l = ctx->vp_cfg[0].chn_hdl[0];
    ot_eis_handle chn_r = ctx->vp_cfg[1].chn_hdl[0];
    ot_eis_handle chn_d = ctx->vp_cfg[0].chn_hdl[CAMERA_DET_CHN];
    ot_eis_img_frame img_l = {0};
    ot_eis_img_frame img_r = {0};
    ot_eis_img_frame img_d = {0};

    prctl(PR_SET_NAME, "camera-get-frame", 0, 0, 0);

    while (ot_eis_vproc_chn_acquire_frame(chn_l, &img_l, 0) == OT_SUCCESS) {
        ot_eis_vproc_chn_release_frame(chn_l, &img_l);
    }
    while (ot_eis_vproc_chn_acquire_frame(chn_r, &img_r, 0) == OT_SUCCESS) {
        ot_eis_vproc_chn_release_frame(chn_r, &img_r);
    }

    while (ctx->get_run == OT_TRUE) {
        ot_s32 ret = ot_eis_vproc_chn_acquire_frame(chn_l, &img_l, 500);

        if (ret != OT_SUCCESS) {
            usleep(100 * 1000);
            continue;
        }
        ret = ot_eis_vproc_chn_acquire_frame(chn_r, &img_r, 500);
        if (ret != OT_SUCCESS) {
            ot_eis_vproc_chn_release_frame(chn_l, &img_l);
            continue;
        }

        camera_record_pair(img_l.pts, img_r.pts);
        ot_eis_vproc_chn_release_frame(chn_l, &img_l);
        ot_eis_vproc_chn_release_frame(chn_r, &img_r);

        while (ot_eis_vproc_chn_acquire_frame(chn_d, &img_d, 0) == OT_SUCCESS) {
            pthread_mutex_lock(&ctx->lock);
            if (ctx->slot_valid == OT_TRUE) {
                ot_eis_vproc_chn_release_frame(chn_d, &ctx->slot_frame);
            }
            if (ctx->det_info_printed == OT_FALSE) {
                printf("[camera] det frame %ux%u fmt=%d compress=%d stride=%u/%u\n",
                       img_d.attr.width, img_d.attr.height, img_d.attr.pixel_fmt,
                       img_d.attr.compress_mode, img_d.buff.stride[0],
                       img_d.buff.stride[1]);
                ctx->det_info_printed = OT_TRUE;
            }
            ctx->slot_frame = img_d;
            ctx->slot_valid = OT_TRUE;
            ctx->det_frames++;
            pthread_mutex_unlock(&ctx->lock);
            pthread_cond_broadcast(&ctx->cond);
        }
    }

    pthread_mutex_lock(&ctx->lock);
    if (ctx->slot_valid == OT_TRUE) {
        ot_eis_vproc_chn_release_frame(chn_d, &ctx->slot_frame);
        ctx->slot_valid = OT_FALSE;
    }
    pthread_mutex_unlock(&ctx->lock);
    return OT_NULL;
}

static void camera_fill_rows(uint8_t *dst, uint32_t dst_stride, uint32_t rows,
                             const uint8_t *src, uint32_t src_stride, uint8_t fill)
{
    for (uint32_t r = 0; r < rows; r++) {
        if (src != NULL) {
            memcpy(dst + (size_t)r * dst_stride, src + (size_t)r * src_stride,
                   dst_stride);
        } else {
            memset(dst + (size_t)r * dst_stride, fill, dst_stride);
        }
    }
}

int32_t camera_copy_latest_to_input(uint8_t *dst, uint32_t dst_len)
{
    const uint8_t *y = NULL;
    const uint8_t *uv = NULL;
    ot_void *map = NULL;
    uint32_t map_size = 0;
    ot_u64 c_off;
    uint32_t top_pad = CAMERA_TOP_PAD;
    uint32_t y_size = CAMERA_NPU_IN_W * CAMERA_NPU_IN_H;
    struct timespec ts;
    int32_t ret = -1;

    if (dst == NULL || dst_len < CAMERA_NPU_IN_LEN) {
        return -1;
    }

    pthread_mutex_lock(&gs_ctx.lock);
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;
    while (gs_ctx.slot_valid != OT_TRUE) {
        if (pthread_cond_timedwait(&gs_ctx.cond, &gs_ctx.lock, &ts) != 0) {
            goto unlock_out;
        }
    }
    if (gs_ctx.slot_valid != OT_TRUE ||
        gs_ctx.slot_frame.attr.width != CAMERA_DET_OUT_W ||
        gs_ctx.slot_frame.attr.height != CAMERA_DET_OUT_H) {
        goto unlock_out;
    }
    map_size = gs_ctx.slot_frame.buff.stride[0] * gs_ctx.slot_frame.attr.height +
               gs_ctx.slot_frame.buff.stride[1] * gs_ctx.slot_frame.attr.height / 2;
    c_off = gs_ctx.slot_frame.buff.phys_addr[1] - gs_ctx.slot_frame.buff.phys_addr[0];
    if (c_off >= map_size) {
        goto unlock_out;
    }
    /* USER 模式帧的 virt_addr 不可靠，按 uvc_app 的方式映射物理地址 */
    if (ot_smr_mmap(gs_ctx.slot_frame.buff.phys_addr[0], map_size, OT_TRUE, &map) !=
        OT_SUCCESS) {
        goto unlock_out;
    }
    y = (const uint8_t *)map;
    uv = y + c_off;

    camera_fill_rows(dst, CAMERA_NPU_IN_W, top_pad, NULL, 0, 128);
    camera_fill_rows(dst + (size_t)top_pad * CAMERA_NPU_IN_W, CAMERA_NPU_IN_W,
                     CAMERA_DET_OUT_H, y, gs_ctx.slot_frame.buff.stride[0], 128);
    camera_fill_rows(dst + (size_t)(top_pad + CAMERA_DET_OUT_H) * CAMERA_NPU_IN_W,
                     CAMERA_NPU_IN_W, top_pad, NULL, 0, 128);

    camera_fill_rows(dst + y_size, CAMERA_NPU_IN_W, top_pad / 2, NULL, 0, 128);
    camera_fill_rows(dst + y_size + (size_t)(top_pad / 2) * CAMERA_NPU_IN_W,
                     CAMERA_NPU_IN_W, CAMERA_DET_OUT_H / 2, uv,
                     gs_ctx.slot_frame.buff.stride[1], 128);
    camera_fill_rows(dst + y_size + (size_t)(top_pad / 2 + CAMERA_DET_OUT_H / 2) *
                     CAMERA_NPU_IN_W, CAMERA_NPU_IN_W, top_pad / 2, NULL, 0, 128);
    if (map != NULL) {
        ot_smr_munmap(map, map_size);
    }
    /* 拷贝即消费：归还当前帧并清空单槽，下一次拷贝等待 10fps 新帧 */
    ot_eis_vproc_chn_release_frame(gs_ctx.det_chn, &gs_ctx.slot_frame);
    gs_ctx.slot_valid = OT_FALSE;
    ret = 0;

unlock_out:
    pthread_mutex_unlock(&gs_ctx.lock);
    return ret;
}

int32_t camera_get_stats(camera_stats *stats)
{
    if (stats == NULL) {
        return -1;
    }
    pthread_mutex_lock(&gs_ctx.lock);
    stats->det_frames = gs_ctx.det_frames;
    stats->lr_pairs = gs_ctx.lr_pairs;
    stats->dpts_min_us = gs_ctx.dpts_min;
    stats->dpts_avg_us = (gs_ctx.lr_pairs != 0) ? gs_ctx.dpts_sum / gs_ctx.lr_pairs : 0;
    stats->dpts_max_us = gs_ctx.dpts_max;
    pthread_mutex_unlock(&gs_ctx.lock);
    return 0;
}

int32_t camera_dump_latest(const char *path)
{
    uint8_t *buf;
    FILE *fp;
    int32_t ret;

    if (path == NULL) {
        return -1;
    }
    buf = (uint8_t *)malloc(CAMERA_NPU_IN_LEN);
    if (buf == NULL) {
        return -1;
    }
    ret = camera_copy_latest_to_input(buf, CAMERA_NPU_IN_LEN);
    if (ret == 0) {
        fp = fopen(path, "wb");
        if (fp == NULL) {
            ret = -1;
        } else {
            if (fwrite(buf, 1, CAMERA_NPU_IN_LEN, fp) != CAMERA_NPU_IN_LEN) {
                ret = -1;
            }
            fclose(fp);
        }
    }
    free(buf);
    return ret;
}
