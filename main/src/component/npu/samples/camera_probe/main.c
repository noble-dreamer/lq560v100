/*
 * camera_probe: 验证 sc132gs 双目媒体管线（不创建 UVC gadget）与检测通道参数。
 */
#include "sample_comm.h"
#include <stdio.h>
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
} probe_ctx;

static probe_scene_auto_ctx gs_scene_ctx;
static probe_ctx gs_ctx;

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

int main(void)
{
    if (probe_sys_init() != OT_SUCCESS) {
        printf("[probe] init fail\n");
        return -1;
    }
    probe_sys_exit();
    return 0;
}
