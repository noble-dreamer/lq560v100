/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#include "stereo_scene_auto.h"
#include "ot_scenecomm.h"
#include "ot_scene.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    ot_eis_handle vi_pipe_hdl;
    ot_eis_handle vp_grp_hdl;
    ot_bool       run;
    pthread_t     tid;
} StereoSceneContext;

typedef struct {
    ot_scene_param   scene_param;
    ot_u32           total_pipe;
    StereoSceneContext norm_ctx[STEREO_SCENE_PIPE_MAX];
} StereoSceneAutoContext;

static StereoSceneAutoContext gs_scene_ctx;
static ot_bool gs_scene_started = OT_FALSE;

static void *stereo_scene_norm_proc(void *p)
{
    ot_s32 ret;
    StereoSceneContext *ctx = (StereoSceneContext *)p;

    ret = ot_scene_static_vi_set_param(ctx->vi_pipe_hdl, ctx->vp_grp_hdl);
    if (ret != OT_SUCCESS) {
        printf("[stereo_scene] ot_scene_static_vi_set_param failed, ret:0x%x\n", ret);
        goto proc_exit;
    }

    ret = ot_scene_dynamic_normal_init(ctx->vi_pipe_hdl);
    if (ret != OT_SUCCESS) {
        printf("[stereo_scene] ot_scene_dynamic_normal_init failed, ret:0x%x\n", ret);
        goto proc_exit;
    }

    while (ctx->run == OT_TRUE) {
        ret = ot_scene_dynamic_normal_set_param(ctx->vi_pipe_hdl, ctx->vp_grp_hdl);
        usleep(100 * 1000); /* 100ms interval */
        if (ret != OT_SUCCESS) {
            printf("[stereo_scene] dynamic_normal_set_param failed, ret:0x%x\n", ret);
            continue;
        }
    }

    ot_scene_dynamic_normal_deinit(ctx->vi_pipe_hdl);

proc_exit:
    ctx->run = OT_FALSE;
    return OT_NULL;
}

ot_s32 stereo_scene_auto_start(const ot_s8 *ini_path,
                                ot_eis_handle vi_pipe_hdl[STEREO_SCENE_PIPE_MAX],
                                ot_eis_handle vp_grp_hdl[STEREO_SCENE_PIPE_MAX],
                                ot_u32 max_hdl_num)
{
    ot_s32 ret;
    ot_u32 i;
    StereoSceneAutoContext *ctx = &gs_scene_ctx;

    memset(ctx, 0, sizeof(*ctx));
    gs_scene_started = OT_FALSE;

    ret = ot_scene_create_param(ini_path, &ctx->scene_param, NULL);
    if (ret != OT_SUCCESS) {
        printf("[stereo_scene] ot_scene_create_param failed, ret:0x%x\n", ret);
        return ret;
    }

    ret = ot_scene_init(&ctx->scene_param);
    if (ret != OT_SUCCESS) {
        printf("[stereo_scene] ot_scene_init failed, ret:0x%x\n", ret);
        return ret;
    }

    for (i = 0; i < max_hdl_num; i++) {
        ctx->norm_ctx[i].vi_pipe_hdl = vi_pipe_hdl[i];
        ctx->norm_ctx[i].vp_grp_hdl  = vp_grp_hdl[i];
    }

    for (i = 0; i < max_hdl_num; i++) {
        ctx->norm_ctx[i].run = OT_TRUE;
        ret = pthread_create(&ctx->norm_ctx[i].tid, NULL,
                             stereo_scene_norm_proc,
                             (ot_void *)(ctx->norm_ctx + i));
        if (ret != 0) {
            printf("[stereo_scene] create normal thread[%d] failed, ret=%d\n", i, ret);
            goto start_proc_failed;
        }
    }

    ctx->total_pipe = max_hdl_num;
    gs_scene_started = OT_TRUE;
    printf("[stereo_scene] started %u ISP tuning threads\n", max_hdl_num);
    return OT_SUCCESS;

start_proc_failed:
    for (ot_u32 j = 0; j < i; j++) {
        ctx->norm_ctx[j].run = OT_FALSE;
        pthread_join(ctx->norm_ctx[j].tid, NULL);
    }
    ot_scene_deinit();
    return ret;
}

void stereo_scene_auto_stop(void)
{
    StereoSceneAutoContext *ctx = &gs_scene_ctx;
    if (gs_scene_started != OT_TRUE) {
        return;
    }
    for (ot_u32 i = 0; i < ctx->total_pipe; i++) {
        ctx->norm_ctx[i].run = OT_FALSE;
        pthread_join(ctx->norm_ctx[i].tid, NULL);
    }
    ot_scene_deinit();
    gs_scene_started = OT_FALSE;
    ctx->total_pipe = 0;
    printf("[stereo_scene] stopped\n");
}
