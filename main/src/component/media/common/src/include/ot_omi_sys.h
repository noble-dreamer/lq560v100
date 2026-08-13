/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef OT_OMI_SYS_H
#define OT_OMI_SYS_H

#include "ot_type.h"
#include "ot_common.h"
#include "ot_common_sys.h"
#include "ot_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

ot_s32 ot_omi_log_set_level_cfg(const ot_log_level_cfg *level_cfg);
ot_s32 ot_omi_log_get_level_cfg(ot_log_level_cfg *level_cfg);

ot_s32 ot_omi_log_set_wait_flag(ot_bool flag);
ot_s32 ot_omi_log_read(ot_char *buf, ot_u32 size);
ot_void ot_omi_log_close(ot_void);

ot_s32 ot_omi_sys_init(ot_void);
ot_s32 ot_omi_sys_exit(ot_void);

ot_s32 ot_omi_sys_set_cfg(const ot_omp_sys_cfg *sys_cfg);
ot_s32 ot_omi_sys_get_cfg(ot_omp_sys_cfg *sys_cfg);

ot_s32 ot_omi_sys_get_version(ot_omp_version *version);

ot_s32 ot_omi_sys_get_cur_pts(ot_u64 *cur_pts);
ot_s32 ot_omi_sys_init_pts_base(ot_u64 pts_base);
ot_s32 ot_omi_sys_sync_pts(ot_u64 pts_base);

ot_s32 ot_omi_sys_close_fd(ot_void);

ot_s32 ot_omi_sys_set_mem_cfg(const ot_omp_chn *omp_chn, const ot_char *smr_name);
ot_s32 ot_omi_sys_get_mem_cfg(const ot_omp_chn *omp_chn, ot_char *smr_name);

ot_s32 ot_omi_sys_set_scale_coef_level(const ot_scale_range *scale_range,
    const ot_scale_coef_level *scale_coef_level);
ot_s32 ot_omi_sys_get_scale_coef_level(const ot_scale_range *scale_range, ot_scale_coef_level *scale_coef_level);

ot_s32 ot_omi_sys_set_time_zone(ot_s32 time_zone);
ot_s32 ot_omi_sys_get_time_zone(ot_s32 *time_zone);

ot_s32 ot_omi_sys_set_gps_info(const ot_gps_info *gps_info);
ot_s32 ot_omi_sys_get_gps_info(ot_gps_info *gps_info);

ot_s32 ot_omi_sys_set_vi_vproc_mode(const ot_vi_vproc_mode *vi_vproc_mode);
ot_s32 ot_omi_sys_get_vi_vproc_mode(ot_vi_vproc_mode *vi_vproc_mode);

ot_s32 ot_omi_sys_set_vi_aiisp_mode(ot_vi_pipe vi_pipe, ot_vi_aiisp_mode aiisp_mode);
ot_s32 ot_omi_sys_get_vi_aiisp_mode(ot_vi_pipe vi_pipe, ot_vi_aiisp_mode *aiisp_mode);

ot_s32 ot_omi_sys_get_chip_id(ot_u32 *chip_id);
ot_s32 ot_omi_sys_get_custom_code(ot_u32 *custom_code);
ot_s32 ot_omi_sys_get_unique_id(ot_unique_id *unique_id);

ot_s32 ot_omi_sys_set_raw_frame_compress_param(const ot_raw_frame_compress_param *compress_param);
ot_s32 ot_omi_sys_get_raw_frame_compress_param(ot_raw_frame_compress_param *compress_param);

ot_s32 ot_omi_sys_set_tuning_connect(ot_s32 connect);
ot_s32 ot_omi_sys_get_tuning_connect(ot_s32 *connect);

ot_s32 ot_omi_sys_set_schedule_mode(const ot_schedule_mode *schedule_mode);
ot_s32 ot_omi_sys_get_schedule_mode(ot_schedule_mode *schedule_mode);

ot_s32 ot_omi_sys_set_fmu_attr(ot_fmu_id id, const ot_fmu_attr *fmu_attr);
ot_s32 ot_omi_sys_get_fmu_attr(ot_fmu_id id, ot_fmu_attr *fmu_attr);

ot_s32 ot_omi_sys_set_3dnr_pos(ot_3dnr_pos_type pos);
ot_s32 ot_omi_sys_get_3dnr_pos(ot_3dnr_pos_type *pos);

ot_s32 ot_omi_sys_start_sync_frame_rate_ctrl(const ot_omp_chn *src_chn, const ot_omp_chn *dst_chn);
ot_s32 ot_omi_sys_stop_sync_frame_rate_ctrl(const ot_omp_chn *src_chn, const ot_omp_chn *dst_chn);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_OMI_SYS_H */
