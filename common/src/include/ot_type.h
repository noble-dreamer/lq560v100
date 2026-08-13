/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef OT_TYPE_H
#define OT_TYPE_H

#ifdef __KERNEL__

#include <linux/types.h>
#else

#include <stdint.h>
#endif
#include "autoconf.h"

#ifndef OT_NULL
#define OT_NULL                 0L
#endif

#include "ot_eis_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#ifndef NULL
    #define NULL                0L
#endif


typedef unsigned char           ot_uchar;

#ifndef OT_SUCCESS
#define OT_SUCCESS              0
#endif

#ifndef OT_FAILURE
#define OT_FAILURE              (-1)
#endif

#ifdef CONFIG_PHYS_ADDR_BIT_WIDTH_64
    typedef unsigned long long  ot_phys_addr_t;
#else
    typedef unsigned int        ot_phys_addr_t;
#endif

typedef ot_u32                  ot_handle;
typedef uintptr_t               ot_uintptr_t;
typedef unsigned int            ot_fr32;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* OT_TYPE_H */
