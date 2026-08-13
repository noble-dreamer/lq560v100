/*
  Copyright (c), 2021-2022, Oritek Tech. Co., Ltd.
 */

#ifndef OT_COMMON_SYS_BIND_H
#define OT_COMMON_SYS_BIND_H

#include "ot_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ot_u32 num;
    ot_omp_chn omp_chn[OT_MAX_BIND_DST_NUM];
} ot_omp_bind_dst;

#ifdef __cplusplus
}
#endif
#endif /* OT_COMMON_SYS_BIND_H */
