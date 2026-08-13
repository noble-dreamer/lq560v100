/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <signal.h>
#include <stdint.h>

typedef struct ot_camera {
    int (*init)();
    int (*open)();
    int (*close)();
    int (*run)();
} ot_camera;

int camera_set_uvc_device_cnt(uint32_t dev_cnt);
uint32_t camera_get_uvc_device_cnt(void);

ot_camera *get_ot_camera(void);
void release_ot_camera(ot_camera *camera);

void camera_register_uvc_mpi_ops(void);
void camera_register_uac_mpi_ops(void);

sig_atomic_t uvc_apps_get_quit_flag(void);

unsigned int get_g_uac_val(void);
#endif // __CAMERA_H__
