/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */


#ifndef MOTIONSENSOR_CHIP_PROC_H
#define MOTIONSENSOR_CHIP_PROC_H

#include "motionsensor_ext.h"
#include "ot_common_motionsensor.h"
#include "ot_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_LEN 32

ot_s32 mpu_proc_init(ot_void);
void mpu_proc_exit(ot_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif
