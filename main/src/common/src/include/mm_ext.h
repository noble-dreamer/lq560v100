/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef MM_EXT_H
#define MM_EXT_H

#include "ot_osal.h"
#include "ot_math.h"
#include "ot_common.h"
#include "smr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const ot_char *smr_name;
    const ot_char *buf_name;
    ot_ulong size;
    ot_bool kernel_only;
} mm_malloc_param;

ot_phys_addr_t comi_smr_malloc(const mm_malloc_param *param);
ot_void comi_smr_free(ot_phys_addr_t phys_addr, ot_void *virt_addr);
ot_s32 comi_smr_malloc_nocache(const mm_malloc_param *param, ot_phys_addr_t *phys_addr, ot_void **virt_addr);
ot_s32 comi_smr_malloc_cached(const mm_malloc_param *param, ot_phys_addr_t *phys_addr, ot_void **virt_addr);
ot_void *comi_remap_cached(ot_phys_addr_t phys_addr, ot_ulong size);
ot_void *comi_remap_nocache(ot_phys_addr_t phys_addr, ot_ulong size);
ot_void comi_unmap(ot_void *virt_addr);
ot_s32 comi_check_smr_phy_addr(ot_phys_addr_t phys_addr, ot_ulong len);
ot_void comi_dcache_region_wb(ot_void *virt_addr, ot_phys_addr_t phys_addr, ot_ulong len);
ot_s32 comi_invalid_cache_byaddr(ot_void *virt_addr, ot_phys_addr_t phys_addr, ot_ulong len);

ot_void *comi_smr_phys_to_handle(ot_phys_addr_t phys_addr);

static inline ot_bool read_user_linear_space_valid(ot_u8 *addr_start, ot_u32 len)
{
    ot_u8 check;
    ot_u8 *addr_end = OT_NULL;

    if (len == 0) {
        return OT_FALSE;
    }

    if (!osal_access_ok(OSAL_VERIFY_READ, addr_start, len)) {
        return OT_FALSE;
    }

    addr_end = addr_start + len - 1;
    if (osal_copy_from_user(&check, addr_end, 1)) {
        return OT_FALSE;
    }

    return OT_TRUE;
}

static inline ot_bool write_user_linear_space_valid(ot_u8 *addr_start, ot_u32 len)
{
    ot_u8 check = 0;
    ot_u8 *addr_end = OT_NULL;

    if (len == 0) {
        return OT_FALSE;
    }

    if (!osal_access_ok(OSAL_VERIFY_WRITE, addr_start, len)) {
        return OT_FALSE;
    }

    addr_end = addr_start + len - 1;
    if (osal_copy_to_user(addr_end, &check, 1)) {
        return OT_FALSE;
    }

    return OT_TRUE;
}

#ifdef __cplusplus
}
#endif

#endif
