/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "stereo_camera.h"
#include "stereo_media.h"
#include "stereo_types.h"

static ot_bool g_pipeline_running = OT_FALSE;

static ot_s32 stereo_camera_init(void)
{
    ot_s32 ret;

    ret = stereo_media_sys_init();
    if (ret != OT_SUCCESS) {
        stereo_log_write("stereo_media_sys_init failed, ret:0x%x\n", ret);
        return OT_FAILURE;
    }

    stereo_log_write("stereo camera init done\n");
    return 0;
}

static ot_s32 stereo_camera_open(void)
{
    /* Nothing to open — no /dev/videoX UVC gadget needed */
    stereo_log_write("stereo camera open done\n");
    return 0;
}

static ot_s32 stereo_camera_close(void)
{
    stereo_media_sys_deinit();
    stereo_log_write("stereo camera close done\n");
    return 0;
}

static ot_s32 stereo_camera_run(void)
{
    ot_s32 ret;

    ret = stereo_media_startup();
    if (ret != OT_SUCCESS) {
        stereo_log_write("stereo_media_startup failed, ret:0x%x\n", ret);
        return OT_FAILURE;
    }

    g_pipeline_running = OT_TRUE;

    /* Wait for quit signal */
    while (g_pipeline_running == OT_TRUE) {
        if (stereo_apps_get_quit_flag() != 0) {
            g_pipeline_running = OT_FALSE;
            break;
        }
        stereo_media_print_fps();
        usleep(50 * 1000);
    }

    stereo_media_shutdown();

    return 0;
}

/* -------------------------------------------------------------------------- */
/* Ops table                                                                  */
/* -------------------------------------------------------------------------- */

static stereo_camera_t g_stereo_camera = {
    .init  = stereo_camera_init,
    .open  = stereo_camera_open,
    .close = stereo_camera_close,
    .run   = stereo_camera_run,
};

stereo_camera_t *get_stereo_camera(void)
{
    return &g_stereo_camera;
}

void release_stereo_camera(stereo_camera_t *camera)
{
    (void)camera;
}
