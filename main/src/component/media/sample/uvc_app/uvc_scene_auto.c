
/*
  Copyright (c), 2001-2022, Oritek Tech. Co., Ltd.
 */
#include "ot_scenecomm.h"
#include "ot_scene.h"
#include "uvc_scene_auto.h"
#include <unistd.h>


typedef struct {
    ot_eis_handle vi_pipe_hdl;
    ot_eis_handle vp_grp_hdl;
    ot_bool       run;
    pthread_t     tid;
} SceneContext;

typedef struct {
    ot_scene_param  scene_param;
    ot_u32          total_pipe;
    SceneContext    norm_ctx[OT_SCENE_PIPE_MAX_NUM];
} SceneAutoContex;

SceneAutoContex gs_scene_ctx;

static void *sample_uvc_scene_norm_proc(void *p)
{
    ot_s32 ret = OT_SUCCESS;

    SceneContext *ctx = (SceneContext *)p;

    ret = ot_scene_static_vi_set_param(ctx->vi_pipe_hdl, ctx->vp_grp_hdl);
    if (ret != OT_SUCCESS) {
        sample_print("ot_scene_static_vi_set_param failed, ret:0x%x\n", ret);
        goto proc_exit;
    }

    ret = ot_scene_dynamic_normal_init(ctx->vi_pipe_hdl);
    if (ret != OT_SUCCESS) {
        sample_print("ot_scene_dynamic_normal_init failed, ret:0x%x\n", ret);
        goto proc_exit;
    }

    while(ctx->run == OT_TRUE) {
        ret = ot_scene_dynamic_normal_set_param(ctx->vi_pipe_hdl, ctx->vp_grp_hdl);
        usleep(100 * 1000);
        if (ret != OT_SUCCESS) {
            printf("ot_scene_dynamic_normal_set_param failed, vi_pipe_hdl:0x%p, vp_grp_hdl:0x%p, ret:0x%x", ctx->vi_pipe_hdl, ctx->vp_grp_hdl, ret);
            continue;
        }
    }

    ot_scene_dynamic_normal_deinit(ctx->vi_pipe_hdl);

proc_exit:
    ctx->run = OT_FALSE;
    return OT_NULL;
}

ot_s32 sample_uvc_scene_auto_start(const char *ini_path, ot_eis_handle vi_pipe_hdl[OT_SCENE_PIPE_MAX_NUM], ot_eis_handle vp_grp_hdl[OT_SCENE_PIPE_MAX_NUM], ot_u32 max_hdl_num)
{
    ot_s32 ret = OT_SUCCESS;
    ot_u32 i1 = 0;
    SceneAutoContex *ctx = &gs_scene_ctx;

    memset(ctx, 0, sizeof(SceneAutoContex));

    ret = ot_scene_create_param(ini_path, &ctx->scene_param, NULL);
    if (ret != OT_SUCCESS) {
        sample_print("ot_scene_create_param failed\n");
        return ret;
    }

    ret = ot_scene_init(&ctx->scene_param);
    if (ret != OT_SUCCESS) {
        sample_print("ot_scene_init failed, ret:0x%x\n", ret);
        return ret;
    }

    for (ot_u32 i = 0; i < max_hdl_num; i++) {
        ctx->norm_ctx[i].vi_pipe_hdl = vi_pipe_hdl[i];
        ctx->norm_ctx[i].vp_grp_hdl  = vp_grp_hdl[i];
    }

    for (i1 = 0; i1 < max_hdl_num; i1++) {
        ctx->norm_ctx[i1].run = OT_TRUE;
        ret = pthread_create(&ctx->norm_ctx[i1].tid, NULL, sample_uvc_scene_norm_proc, (ot_void *)(ctx->norm_ctx + i1));
        if (ret != 0) {
            sample_print("create normal thread failed, ret=%d\n", ret);
            goto start_norm_proc_failed;
        }
    }

    ctx->total_pipe = max_hdl_num;

    return OT_SUCCESS;

start_norm_proc_failed:
    for (ot_u32 i = 0; i < i1; i++) {
        ctx->norm_ctx[i].run = OT_FALSE;
        pthread_join(ctx->norm_ctx[i].tid, NULL);
    }

    ot_scene_deinit();

    return ret;
}

void sample_uvc_scene_auto_stop()
{
    SceneAutoContex *ctx = &gs_scene_ctx;
    for (ot_u32 i = 0; i < ctx->total_pipe; i++) {
        ctx->norm_ctx[i].run = OT_FALSE;
        pthread_join(ctx->norm_ctx[i].tid, NULL);
    }

    ot_scene_deinit();
}
