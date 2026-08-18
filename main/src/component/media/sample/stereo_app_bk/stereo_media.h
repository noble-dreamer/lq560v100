/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef __STEREO_MEDIA_H__
#define __STEREO_MEDIA_H__

#include "ot_common.h"
#include "stereo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Select raw-only mode before system init.
 */
void stereo_media_set_raw_only(ot_bool enable);

/**
 * @brief Initialize the media subsystem (VI, VPROC, NPU, CVE, VENC, buffer pools).
 */
ot_s32 stereo_media_sys_init(void);

/**
 * @brief De-initialize the media subsystem.
 */
void stereo_media_sys_deinit(void);

/**
 * @brief Start the media pipeline. Raw-only mode starts only VI + raw capture.
 */
ot_s32 stereo_media_startup(void);

/**
 * @brief Shutdown the pipeline: stop threads/raw capture, stop VPROC/VI.
 */
void stereo_media_shutdown(void);

/**
 * @brief Print per-stage FPS stats (call from main loop, prints once per second).
 */
void stereo_media_print_fps(void);

/**
 * @brief Get latest NPU/SubPixel timing (thread-safe read of volatile globals).
 */
void stereo_media_get_perf(double *npu_ms, double *sub_ms);

/**
 * @brief Single-camera operation flag. 2 sensors for stereo.
 */
#define STEREO_SNS_NUM  2

/**
 * @brief Left sensor dev_id=0, right sensor dev_id=2 (matching uvc_app convention).
 */
#define STEREO_DEV_ID_LEFT   0
#define STEREO_DEV_ID_RIGHT  2

/**
 * @brief VI pipe count per sensor.
 */
#define STEREO_VI_PIPE_CNT   2

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_MEDIA_H__ */
