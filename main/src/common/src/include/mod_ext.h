/*
  Copyright (c), 2021-2023, Oritek Tech. Co., Ltd.
 */

#ifndef MOD_EXT_H
#define MOD_EXT_H

#include "ot_osal.h"

#include "ot_type.h"
#include "ot_common.h"

#define MAX_OMP_MODULES OT_ID_BUTT

#define VERSION_MAGIC 20210823

#define MAX_MOD_NAME    16

typedef enum {
    MOD_NOTICE_STOP = 0x11,
} mod_notice_id;

typedef enum {
    MOD_STATE_FREE = 0x11,
    MOD_STATE_BUSY = 0X22,
} mod_state;

typedef ot_s32 fn_mod_init(ot_void *);
typedef ot_void fn_mod_exit(ot_void);
typedef ot_void fn_mod_notify(mod_notice_id notice_id);
typedef ot_void fn_mod_query_state(mod_state *state);
typedef ot_u32 fn_mod_ver_checker(ot_void);

typedef struct {
    struct osal_list_head list;

    ot_char mod_name[MAX_MOD_NAME];
    ot_mod_id mod_id;

    fn_mod_init *pfn_init;
    fn_mod_exit *pfn_exit;
    fn_mod_query_state *pfn_query_state;
    fn_mod_notify *pfn_notify;
    fn_mod_ver_checker *pfn_ver_checker;

    ot_bool inited;

    ot_void *export_funcs;
    ot_void *data;

    ot_char *version;
} umap_module;

ot_char *comi_get_module_name(ot_mod_id mod_id);
ot_void *comi_get_module_func_by_id(ot_mod_id mod_id);

ot_void comi_stop_modules(ot_void);
ot_s32 comi_query_modules(ot_void);
ot_s32 comi_init_modules(ot_void);
ot_void comi_exit_modules(ot_void);

ot_void comi_stop_modules_for_audio(ot_void);
ot_s32 comi_query_modules_for_audio(ot_void);
ot_s32 comi_init_modules_for_audio(ot_void);
ot_void comi_exit_modules_for_audio(ot_void);

ot_s32 comi_register_module(umap_module *module);
ot_void comi_unregister_module(ot_mod_id mod_id);

#define func_entry(type, id) ((type *)comi_get_module_func_by_id(id))
#define check_func_entry(id) (comi_get_module_func_by_id(id) != NULL)
#define func_entry_null(id) (!check_func_entry(id))

#endif /*  MOD_EXT_H */
