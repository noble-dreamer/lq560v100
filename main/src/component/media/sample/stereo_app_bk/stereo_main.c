/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <time.h>
#include "stereo_camera.h"
#include "stereo_media.h"
#include "stereo_types.h"

volatile sig_atomic_t g_need_quit_flag = 0;

/* -------------------------------------------------------------------------- */
/* Unified thread-safe run logger: stdout + /data/stereo_debug/stereo_run.log  */
/* -------------------------------------------------------------------------- */
static pthread_mutex_t g_log_lock = PTHREAD_MUTEX_INITIALIZER;
static FILE           *g_log_fp   = NULL;

void stereo_log_init(void)
{
    /* Ensure the debug directory exists before opening the log file. */
    mkdir(STEREO_DEBUG_LOCAL_PATH, 0755);
    pthread_mutex_lock(&g_log_lock);
    if (g_log_fp == NULL) {
        g_log_fp = fopen(STEREO_RUN_LOG_FILE, "a");
    }
    pthread_mutex_unlock(&g_log_lock);
}

void stereo_log_deinit(void)
{
    pthread_mutex_lock(&g_log_lock);
    if (g_log_fp != NULL) {
        fflush(g_log_fp);
        fclose(g_log_fp);
        g_log_fp = NULL;
    }
    pthread_mutex_unlock(&g_log_lock);
}

void stereo_log_write(const char *fmt, ...)
{
    /* Runtime logging disabled — no stdout, no file I/O, no mutex.
       PERF timing (g_perf_npu_ms / g_perf_sub_ms) still works
       because it stores to global variables, not via this function. */
    (void)fmt;
}

sig_atomic_t stereo_apps_get_quit_flag(void)
{
    return g_need_quit_flag;
}

static void stereo_apps_handle_signal(int signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        g_need_quit_flag = 1;
    }
}

static void stereo_apps_usage(const char *prog_name)
{
    printf("Usage: %s [-h] [--raw-only]\n", prog_name);
    printf("\t-h, --help    print help information\n");
    printf("\t--raw-only    start only VI + raw capture server on port %d\n", STEREO_RAW_CAPTURE_PORT);
}

ot_s32 main(ot_s32 argc, ot_s8 *argv[])
{
    ot_s32 ret;
    ot_bool raw_only = OT_FALSE;

    for (ot_s32 i = 1; i < argc; i++) {
        if (strcmp((const char *)argv[i], "-h") == 0 || strcmp((const char *)argv[i], "--help") == 0) {
            stereo_apps_usage((const char *)argv[0]);
            return 0;
        }
        if (strcmp((const char *)argv[i], "--raw-only") == 0) {
            raw_only = OT_TRUE;
            continue;
        }
        printf("Unknown argument: %s\n", argv[i]);
        stereo_apps_usage((const char *)argv[0]);
        return -1;
    }

    /* Register signal handlers */
    struct sigaction act = {0};
    act.sa_handler = stereo_apps_handle_signal;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);

    /* Bring up the unified run logger as early as possible so that any
       failure during init is captured into /data/stereo_debug/stereo_run.log */
    stereo_log_init();

    stereo_media_set_raw_only(raw_only);

    stereo_log_write("=== Stereo Deep Learning App ===\n");
    if (raw_only == OT_TRUE) {
        stereo_log_write("Mode: raw-only, VI dump + TCP %s:%d\n", STEREO_NET_IP, STEREO_RAW_CAPTURE_PORT);
    } else {
        stereo_log_write("Model: %s\n", "/data/model/stereo_match.ortm");
        stereo_log_write("Output: TCP %s:%d\n", STEREO_NET_IP, STEREO_NET_PORT);
        stereo_log_write("Pipeline: 1280x1080 -> CVE(640x540) -> Crop(640x448) -> NPU(dual) -> SubPixel 320x224 Q5\n");
    }

    ret = get_stereo_camera()->init();
    if (ret != 0) {
        stereo_log_write("stereo camera init failed, ret:0x%x\n", ret);
        goto err;
    }

    ret = get_stereo_camera()->open();
    if (ret != 0) {
        stereo_log_write("stereo camera open failed, ret:0x%x\n", ret);
        goto err;
    }

    ret = get_stereo_camera()->run();
    if (ret != 0) {
        stereo_log_write("stereo camera run failed, ret:0x%x\n", ret);
        goto err;
    }

    stereo_log_write("Stereo app exit!\n");

err:
    get_stereo_camera()->close();
    stereo_log_write("exit app normally\n");
    stereo_log_deinit();
    return 0;
}
