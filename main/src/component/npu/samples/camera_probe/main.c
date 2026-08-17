/*
 * camera_probe: 验证 sc132gs 双目媒体管线（不创建 UVC gadget）与检测通道参数。
 */
#include "sample_comm.h"
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <unistd.h>
#include "ot_scene.h"
#include "ot_scenecomm.h"

#define PROBE_DET_W     416
#define PROBE_DET_H     312
#define PROBE_DET_CHN   2
#define PROBE_PIPE_NUM  4

static ot_bool        gs_sys_init = OT_FALSE;
static ot_eis_handle  gs_media_pipe_hdl = OT_NULL;

typedef struct {
    ot_eis_handle vi_pipe_hdl;
    ot_eis_handle vp_grp_hdl;
    ot_bool       run;
    pthread_t     tid;
} probe_scene_ctx;

typedef struct {
    ot_scene_param  scene_param;
    ot_u32          total_pipe;
    probe_scene_ctx norm_ctx[OT_SCENE_PIPE_MAX_NUM];
} probe_scene_auto_ctx;

typedef struct {
    sample_vi_cfg     vi_cfg[2];
    sample_vproc_attr vp_cfg[2];
    ot_bool           startup;
    ot_bool           get_run;
    pthread_t         get_tid;
} probe_ctx;

typedef struct {
    ot_u32           det_frames;
    ot_u32           lr_pairs;
    ot_u64           dpts_sum;
    ot_u64           dpts_min;
    ot_u64           dpts_max;
    ot_bool          det_info_printed;
    pthread_mutex_t  lock;
} probe_stats;

static probe_scene_auto_ctx gs_scene_ctx;
static probe_ctx gs_ctx;
static probe_stats gs_stats;

/* scene_auto 线程：先做静态 VI 参数设置与动态调优初始化，之后周期下发参数。
 * 与 uvc_app 的 sample_uvc_scene_norm_proc 保持一致。 */
static void *probe_scene_norm_proc(void *p)
{
    ot_s32 ret = OT_SUCCESS;
    probe_scene_ctx *ctx = (probe_scene_ctx *)p;

    ret = ot_scene_static_vi_set_param(ctx->vi_pipe_hdl, ctx->vp_grp_hdl);
    if (ret != OT_SUCCESS) {
        sample_print("probe scene static vi set param failed, ret:0x%x\n", ret);
        goto proc_exit;
    }

    ret = ot_scene_dynamic_normal_init(ctx->vi_pipe_hdl);
    if (ret != OT_SUCCESS) {
        sample_print("probe scene dynamic normal init failed, ret:0x%x\n", ret);
        goto proc_exit;
    }

    while (ctx->run == OT_TRUE) {
        ret = ot_scene_dynamic_normal_set_param(ctx->vi_pipe_hdl, ctx->vp_grp_hdl);
        usleep(100 * 1000);
        if (ret != OT_SUCCESS) {
            printf("probe scene dynamic set param failed, ret:0x%x\n", ret);
            continue;
        }
    }

    ot_scene_dynamic_normal_deinit(ctx->vi_pipe_hdl);

proc_exit:
    ctx->run = OT_FALSE;
    return OT_NULL;
}

ot_s32 probe_scene_auto_start(const char *ini_path,
                              ot_eis_handle vi_pipe_hdl[OT_SCENE_PIPE_MAX_NUM],
                              ot_eis_handle vp_grp_hdl[OT_SCENE_PIPE_MAX_NUM],
                              ot_u32 max_hdl_num)
{
    ot_s32 ret = OT_SUCCESS;
    ot_u32 i = 0;
    probe_scene_auto_ctx *ctx = &gs_scene_ctx;

    memset(ctx, 0, sizeof(probe_scene_auto_ctx));

    ret = ot_scene_create_param(ini_path, &ctx->scene_param, NULL);
    if (ret != OT_SUCCESS) {
        sample_print("probe scene create param failed\n");
        return ret;
    }

    ret = ot_scene_init(&ctx->scene_param);
    if (ret != OT_SUCCESS) {
        sample_print("probe scene init failed, ret:0x%x\n", ret);
        return ret;
    }

    for (i = 0; i < max_hdl_num; i++) {
        ctx->norm_ctx[i].vi_pipe_hdl = vi_pipe_hdl[i];
        ctx->norm_ctx[i].vp_grp_hdl = vp_grp_hdl[i];
    }

    for (i = 0; i < max_hdl_num; i++) {
        ctx->norm_ctx[i].run = OT_TRUE;
        ret = pthread_create(&ctx->norm_ctx[i].tid, NULL, probe_scene_norm_proc,
                             (ot_void *)(ctx->norm_ctx + i));
        if (ret != 0) {
            sample_print("probe scene create thread failed, ret=%d\n", ret);
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

void probe_scene_auto_stop(void)
{
    probe_scene_auto_ctx *ctx = &gs_scene_ctx;

    for (ot_u32 i = 0; i < ctx->total_pipe; i++) {
        ctx->norm_ctx[i].run = OT_FALSE;
        pthread_join(ctx->norm_ctx[i].tid, NULL);
    }
    ot_scene_deinit();
}

/* USER 模式 vproc 通道需要用户缓冲池；与 uvc_app 的池配置一致。 */
ot_s32 probe_user_pool_create(ot_eis_img_attr *img_attr, ot_eis_handle *pool_hdl)
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
        printf("probe buffer pool create fail, ret=%d\n", ret);
        return ret;
    }
    *pool_hdl = tmp;
    return OT_SUCCESS;
}

/* 与 uvc_app 一致的双缓冲块 vrb 配置。 */
static ot_s32 probe_get_vrb_cfg(ot_u32 pipe_num, ot_u32 width, ot_u32 height,
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

static ot_s32 probe_sys_init(void)
{
    ot_s32 ret;
    ot_u32 max_w = 0;
    ot_u32 max_h = 0;
    ot_eis_img_size img_size = {0};
    ot_video_buffer_attr vrb_cfg;

    sample_comm_vi_get_size_by_sns_type(SMS_SC132GS_MIPI_1_5M_30FPS_RAW12_DIRECT_SLAVE,
                                        &img_size);
    max_w = img_size.width;
    max_h = img_size.height;

    probe_get_vrb_cfg(PROBE_PIPE_NUM, max_w, max_h, &vrb_cfg);
    ret = sample_comm_sys_init(&vrb_cfg);
    if (ret != OT_SUCCESS) {
        printf("[probe] sample_comm_sys_init fail, ret=%d\n", ret);
        return ret;
    }

    ret = sample_comm_sys_set_vi_vproc_init_cfg(OT_EIS_VI_OFFLINE_VPROC_OFFLINE);
    if (ret != OT_SUCCESS) {
        printf("[probe] set vi/vproc offline fail, ret=%d\n", ret);
        sample_comm_sys_exit();
        return ret;
    }

    ret = sample_comm_media_pipe_init(&gs_media_pipe_hdl);
    if (ret != OT_SUCCESS) {
        printf("[probe] media pipe init fail, ret=%d\n", ret);
        sample_comm_sys_exit();
        return ret;
    }

    ret = ot_buffer_pool_init();
    if (ret != OT_SUCCESS && ret != OT_ERR_BUFPOOL_ALREADY_INIT) {
        printf("[probe] buffer pool init fail, ret=%d\n", ret);
        sample_comm_media_pipe_stop(gs_media_pipe_hdl);
        sample_comm_sys_exit();
        return ret;
    }

    gs_sys_init = OT_TRUE;
    printf("[probe] media system init ok (sensor %ux%u)\n", max_w, max_h);
    return OT_SUCCESS;
}

static void probe_sys_exit(void)
{
    if (gs_sys_init != OT_TRUE) {
        return;
    }
    ot_buffer_pool_deinit();
    sample_comm_media_pipe_stop(gs_media_pipe_hdl);
    sample_comm_sys_exit();
    gs_sys_init = OT_FALSE;
    printf("[probe] media system exit ok\n");
}

/* 启动 sc132gs 双目管线：VI -> 左/右 vproc chn0（960x1280 裁剪+旋转后的原生
 * 帧），左组另加 chn2 检测通道（416x312 YUV420SP，FRC 30->10），最后接
 * scene_auto ISP 调优。全程不触碰 USB gadget。 */
static ot_s32 probe_startup(void)
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

    if (gs_sys_init != OT_TRUE) {
        return OT_FAILURE;
    }

    for (i = 0; i < 2; i++) {
        ot_s32 dev_id = (i == 0) ? 0 : 2;

        sample_comm_vi_get_default_vi_cfg_by_dev_id(
            SMS_SC132GS_MIPI_1_5M_30FPS_RAW12_DIRECT_SLAVE, &gs_ctx.vi_cfg[i], dev_id);
        for (ot_s32 j = 0; j < 2; j++) {
            gs_ctx.vi_cfg[i].pipe_info[0].pipe_id[j] = 2 * i + j;
        }
        ret = sample_comm_vi_start_vi(&gs_ctx.vi_cfg[i]);
        if (ret != OT_SUCCESS) {
            sample_print("probe vi start %d fail, ret:0x%x\n", i, ret);
            goto vi_fail;
        }
        gs_ctx.vi_cfg[i].media_pipe_hdl = gs_media_pipe_hdl;
        done_vi++;
    }

    for (i = 0; i < 2; i++) {
        sample_media_vproc_get_default_attr_by_snsor(
            SMS_SC132GS_MIPI_1_5M_30FPS_RAW12_DIRECT_SLAVE, &gs_ctx.vp_cfg[i]);
        gs_ctx.vp_cfg[i].chn_attr[0].mode = OT_EIS_VPROC_WORK_MODE_USER;
        gs_ctx.vp_cfg[i].chn_attr[0].frame_queue_depth = VPROC_FRAME_QUEUE_DEPTH;
        gs_ctx.vp_cfg[i].chn_attr[0].image_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
        gs_ctx.vp_cfg[i].chn_attr[0].image_attr.width = 960;
        gs_ctx.vp_cfg[i].chn_attr[0].image_attr.height = 1280;
    }
    gs_ctx.vp_cfg[0].chn_attr[PROBE_DET_CHN].mode = OT_EIS_VPROC_WORK_MODE_USER;
    gs_ctx.vp_cfg[0].chn_attr[PROBE_DET_CHN].frame_queue_depth = 2;
    gs_ctx.vp_cfg[0].chn_attr[PROBE_DET_CHN].image_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_YUV_420_SEMIPLANAR;
    /* 270° 旋转会交换输出宽高：按竖版 W=H_out/H=W_out 配置 */
    gs_ctx.vp_cfg[0].chn_attr[PROBE_DET_CHN].image_attr.width = PROBE_DET_H;
    gs_ctx.vp_cfg[0].chn_attr[PROBE_DET_CHN].image_attr.height = PROBE_DET_W;
    gs_ctx.vp_cfg[0].chn_attr[PROBE_DET_CHN].image_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    gs_ctx.vp_cfg[0].chn_attr[PROBE_DET_CHN].frc.src_frame_rate = 30;
    gs_ctx.vp_cfg[0].chn_attr[PROBE_DET_CHN].frc.dst_frame_rate = 10;

    for (i = 0; i < 2; i++) {
        ot_eis_handle pool_hdl = OT_NULL;

        ret = probe_user_pool_create(&gs_ctx.vp_cfg[i].chn_attr[0].image_attr, &pool_hdl);
        if (ret != OT_SUCCESS) {
            goto pool_fail;
        }
        gs_ctx.vp_cfg[i].chn_attr[0].pool_handle = pool_hdl;
        done_pool0++;
    }
    {
        ot_eis_handle pool_hdl = OT_NULL;

        ret = probe_user_pool_create(&gs_ctx.vp_cfg[0].chn_attr[PROBE_DET_CHN].image_attr,
                                     &pool_hdl);
        if (ret != OT_SUCCESS) {
            goto pool_fail;
        }
        gs_ctx.vp_cfg[0].chn_attr[PROBE_DET_CHN].pool_handle = pool_hdl;
        done_pool2++;
    }

    ret = sample_comm_start_vproc(&gs_ctx.vp_cfg[0], pipe_sw, chnl_sw_l);
    if (ret != OT_SUCCESS) {
        sample_print("probe vproc L start fail\n");
        goto vp_fail;
    }
    done_vp++;
    ret = sample_comm_start_vproc(&gs_ctx.vp_cfg[1], pipe_sw, chnl_sw_r);
    if (ret != OT_SUCCESS) {
        sample_print("probe vproc R start fail\n");
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
            ot_eis_vproc_chn_set_rotation(gs_ctx.vp_cfg[i].chn_hdl[PROBE_DET_CHN], &ro_param);
        }
    }

    for (i = 0; i < 2; i++) {
        ret = sample_comm_vi_bind_vproc(gs_ctx.vi_cfg[i].pipe_info[0].chn_info.chn_hdl,
                                        gs_ctx.vp_cfg[i].pipe_hdl[0], gs_media_pipe_hdl);
        if (ret != OT_SUCCESS) {
            sample_print("probe vi bind vproc %d fail, ret:0x%x\n", i, ret);
            goto bind_fail;
        }
        done_bind++;
    }

    for (i = 0; i < 2; i++) {
        vi_pipe_hdl[i] = gs_ctx.vi_cfg[i].pipe_info[0].pipe_hdl;
        vp_grp_hdl[i] = gs_ctx.vp_cfg[i].grp_hdl;
    }
    ret = probe_scene_auto_start("./param/sc132gs", vi_pipe_hdl, vp_grp_hdl, 2);
    if (ret != OT_SUCCESS) {
        sample_print("probe scene auto start fail, ret:0x%x\n", ret);
        goto bind_fail;
    }

    gs_ctx.startup = OT_TRUE;
    printf("[probe] pipeline up: chn0=960x1280 YVU420SP, chn2=%dx%d YUV420SP frc=30->10\n",
           PROBE_DET_W, PROBE_DET_H);
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
        ot_buffer_pool_destroy(gs_ctx.vp_cfg[0].chn_attr[PROBE_DET_CHN].pool_handle);
    }
vi_fail:
    for (i = 0; i < done_vi; i++) {
        sample_comm_vi_stop_vi(&gs_ctx.vi_cfg[i]);
    }
    return ret;
}

static void probe_shutdown(void)
{
    if (gs_ctx.startup != OT_TRUE) {
        return;
    }
    probe_scene_auto_stop();
    for (ot_s32 i = 0; i < 2; i++) {
        sample_comm_vi_un_bind_vproc(gs_ctx.vi_cfg[i].pipe_info[0].chn_info.chn_hdl,
                                     gs_ctx.vp_cfg[i].pipe_hdl[0], gs_media_pipe_hdl);
    }
    for (ot_s32 i = 0; i < 2; i++) {
        sample_comm_stop_vproc(&gs_ctx.vp_cfg[i]);
        ot_buffer_pool_destroy(gs_ctx.vp_cfg[i].chn_attr[0].pool_handle);
    }
    ot_buffer_pool_destroy(gs_ctx.vp_cfg[0].chn_attr[PROBE_DET_CHN].pool_handle);
    for (ot_s32 i = 0; i < 2; i++) {
        sample_comm_vi_stop_vi(&gs_ctx.vi_cfg[i]);
    }
    gs_ctx.startup = OT_FALSE;
    printf("[probe] pipeline stopped\n");
}

/* 左右原生帧按 PTS 配对并统计对齐误差；检测通道非阻塞取流、丢弃积压。 */
static void probe_record_pair(ot_u64 pts_l, ot_u64 pts_r)
{
    ot_u64 dpts = (pts_l > pts_r) ? (pts_l - pts_r) : (pts_r - pts_l);

    pthread_mutex_lock(&gs_stats.lock);
    gs_stats.lr_pairs++;
    gs_stats.dpts_sum += dpts;
    if (gs_stats.lr_pairs == 1 || dpts < gs_stats.dpts_min) {
        gs_stats.dpts_min = dpts;
    }
    if (dpts > gs_stats.dpts_max) {
        gs_stats.dpts_max = dpts;
    }
    pthread_mutex_unlock(&gs_stats.lock);
}

static void *probe_get_frame_proc(void *p)
{
    probe_ctx *ctx = (probe_ctx *)p;
    ot_eis_handle chn_l = ctx->vp_cfg[0].chn_hdl[0];
    ot_eis_handle chn_r = ctx->vp_cfg[1].chn_hdl[0];
    ot_eis_handle chn_d = ctx->vp_cfg[0].chn_hdl[PROBE_DET_CHN];
    ot_eis_img_frame img_l = {0};
    ot_eis_img_frame img_r = {0};
    ot_eis_img_frame img_d = {0};

    prctl(PR_SET_NAME, "probe-get-frame", 0, 0, 0);

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

        probe_record_pair(img_l.pts, img_r.pts);
        ot_eis_vproc_chn_release_frame(chn_l, &img_l);
        ot_eis_vproc_chn_release_frame(chn_r, &img_r);

        while (ot_eis_vproc_chn_acquire_frame(chn_d, &img_d, 0) == OT_SUCCESS) {
            pthread_mutex_lock(&gs_stats.lock);
            if (gs_stats.det_info_printed == OT_FALSE) {
                printf("[probe] det frame %ux%u fmt=%d compress=%d stride=%u/%u pts=%llu\n",
                       img_d.attr.width, img_d.attr.height, img_d.attr.pixel_fmt,
                       img_d.attr.compress_mode, img_d.buff.stride[0],
                       img_d.buff.stride[1], (unsigned long long)img_d.pts);
                gs_stats.det_info_printed = OT_TRUE;
            }
            gs_stats.det_frames++;
            pthread_mutex_unlock(&gs_stats.lock);
            ot_eis_vproc_chn_release_frame(chn_d, &img_d);
        }
    }
    return OT_NULL;
}

static void probe_print_stats(ot_u32 elapsed_s, ot_bool final)
{
    ot_u32 det = 0;
    ot_u32 pairs = 0;
    ot_u64 sum = 0;
    ot_u64 min = 0;
    ot_u64 max = 0;

    pthread_mutex_lock(&gs_stats.lock);
    det = gs_stats.det_frames;
    pairs = gs_stats.lr_pairs;
    sum = gs_stats.dpts_sum;
    min = gs_stats.dpts_min;
    max = gs_stats.dpts_max;
    pthread_mutex_unlock(&gs_stats.lock);

    if (final) {
        printf("[probe] final: det=%u frames %.1f fps, lr_pairs=%u, "
               "dpts min/avg/max = %llu/%llu/%llu us\n",
               det, (elapsed_s != 0) ? (double)det / (double)elapsed_s : 0.0,
               pairs, (unsigned long long)min,
               (pairs != 0) ? (unsigned long long)(sum / pairs) : 0,
               (unsigned long long)max);
    } else {
        printf("[probe] t=%us det=%u lr_pairs=%u\n", elapsed_s, det, pairs);
    }
}

static void probe_udc_snapshot(const char *tag)
{
    DIR *dir = opendir("/sys/class/udc");
    struct dirent *ent;

    if (dir == OT_NULL) {
        printf("[probe] %s udc: open fail\n", tag);
        return;
    }
    while ((ent = readdir(dir)) != OT_NULL) {
        char path[320];
        char buf[32] = {0};
        int fd;
        ssize_t n;

        if (ent->d_name[0] == '.') {
            continue;
        }
        snprintf(path, sizeof(path), "/sys/class/udc/%s/state", ent->d_name);
        fd = open(path, O_RDONLY);
        if (fd >= 0) {
            n = read(fd, buf, sizeof(buf) - 1);
            close(fd);
            if (n > 0 && buf[n - 1] == '\n') {
                buf[n - 1] = 0;
            }
            printf("[probe] %s udc %s state=%s\n", tag, ent->d_name, buf);
        }
    }
    closedir(dir);
}

int main(int argc, char **argv)
{
    ot_u32 run_s = (argc > 1) ? (ot_u32)atoi(argv[1]) : 30;
    ot_u32 elapsed = 0;

    if (run_s == 0) {
        run_s = 1;
    }
    if (run_s > 300) {
        run_s = 300;
    }

    if (probe_sys_init() != OT_SUCCESS) {
        printf("[probe] init fail\n");
        return -1;
    }
    probe_udc_snapshot("before");
    if (probe_startup() != OT_SUCCESS) {
        printf("[probe] startup fail\n");
        probe_sys_exit();
        return -1;
    }

    pthread_mutex_init(&gs_stats.lock, OT_NULL);
    gs_ctx.get_run = OT_TRUE;
    if (pthread_create(&gs_ctx.get_tid, OT_NULL, probe_get_frame_proc, &gs_ctx) != 0) {
        printf("[probe] create get-frame thread fail\n");
        gs_ctx.get_run = OT_FALSE;
        probe_shutdown();
        probe_sys_exit();
        return -1;
    }

    while (elapsed < run_s) {
        sleep(1);
        elapsed++;
        if ((elapsed % 5) == 0) {
            probe_print_stats(elapsed, OT_FALSE);
        }
    }

    gs_ctx.get_run = OT_FALSE;
    pthread_join(gs_ctx.get_tid, OT_NULL);
    probe_print_stats(run_s, OT_TRUE);
    probe_udc_snapshot("after");

    probe_shutdown();
    probe_sys_exit();
    pthread_mutex_destroy(&gs_stats.lock);
    return 0;
}
