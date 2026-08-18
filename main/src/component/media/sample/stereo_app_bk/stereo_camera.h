/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef __STEREO_CAMERA_H__
#define __STEREO_CAMERA_H__

#include <signal.h>
#include "ot_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stereo_camera {
    ot_s32 (*init)(void);
    ot_s32 (*open)(void);
    ot_s32 (*close)(void);
    ot_s32 (*run)(void);
} stereo_camera_t;

stereo_camera_t *get_stereo_camera(void);
void release_stereo_camera(stereo_camera_t *camera);

sig_atomic_t stereo_apps_get_quit_flag(void);

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_CAMERA_H__ */
