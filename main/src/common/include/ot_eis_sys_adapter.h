#ifndef OT_EIS_SYS_ADAPTER_H
#define OT_EIS_SYS_ADAPTER_H

#include "ot_eis_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

ot_bool ot_eis_sys_ready(ot_void);
ot_s32 ot_eis_get_time_stamp(ot_void);
ot_s32 ot_eis_print_proc_title(ot_void *s, const ot_s8 *fmt, ...);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif