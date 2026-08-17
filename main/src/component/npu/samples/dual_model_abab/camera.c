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
#include <string.h>
#include <unistd.h>
#include "ot_scene.h"
#include "ot_scenecomm.h"

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
