/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */
#ifndef OT_OMI_MOTIONFUSION_H
#define OT_OMI_MOTIONFUSION_H

//#include "ot_type.h"
#include "ot_common_motionfusion.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define MOTIONFUSION_DEVICE_ID_0 0
#define MOTIONFUSION_DEVICE_ID_1 1

ot_s32 ot_omi_mfusion_set_attr(const ot_u32 fusion_id, const ot_mfusion_attr *mfusion_attr);
ot_s32 ot_omi_mfusion_get_attr(const ot_u32 fusion_id, ot_mfusion_attr *mfusion_attr);

ot_s32 ot_omi_mfusion_set_gyro_drift(const ot_u32 fusion_id,
                                     const ot_mfusion_drift *drift);
ot_s32 ot_omi_mfusion_get_gyro_drift(const ot_u32 fusion_id,
                                     ot_mfusion_drift *drift);

ot_s32 ot_omi_mfusion_set_gyro_six_side_calibration(const ot_u32 fusion_id,
                                                    const ot_mfusion_six_side_calibration *six_side_calibration);
ot_s32 ot_omi_mfusion_get_gyro_six_side_calibration(const ot_u32 fusion_id,
                                                    ot_mfusion_six_side_calibration *six_side_calibration);

ot_s32 ot_omi_mfusion_set_gyro_temperature_drift(const ot_u32 fusion_id,
                                                 const ot_mfusion_temperature_drift *temperature_drift);
ot_s32 ot_omi_mfusion_get_gyro_temperature_drift(const ot_u32 fusion_id,
                                                 ot_mfusion_temperature_drift *temperature_drift);

ot_s32 ot_omi_mfusion_set_gyro_online_temperature_drift(const ot_u32 fusion_id,
                                                        const ot_mfusion_temperature_drift *temperature_drift);
ot_s32 ot_omi_mfusion_get_gyro_online_temperature_drift(const ot_u32 fusion_id,
                                                        ot_mfusion_temperature_drift *temperature_drift);

ot_s32 ot_omi_mfusion_set_gyro_online_drift(const ot_u32 fusion_id,
                                            const ot_mfusion_drift *drift);
ot_s32 ot_omi_mfusion_get_gyro_online_drift(const ot_u32 fusion_id,
                                            ot_mfusion_drift *drift);

ot_s32 ot_omi_mfusion_init_rotation_compensation(const ot_u32 fusion_id,
    const ot_mfusion_rotation_cfg *cfg);
ot_s32 ot_omi_mfusion_deinit_rotation_compensation(const ot_u32 fusion_id);

ot_s32 ot_omi_mfusion_set_rotation_compensation(const ot_u32 fusion_id,
    const ot_mfusion_rotation_compensation *compensation);
ot_s32 ot_omi_mfusion_get_rotation_compensation(const ot_u32 fusion_id,
    ot_mfusion_rotation_compensation *compensation);

ot_s32 ot_omi_mfusion_send_quaternion(const ot_u32 fusion_id,
    ot_mfusion_quaternion_buf *quaternion_buf);
ot_s32 ot_omi_mfusion_get_quaternion(const ot_u32 fusion_id, ot_u64 begin_pts, ot_u64 end_pts,
    ot_mfusion_quaternion_buf *quaternion_buf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef OT_OMI_MONTIONFUSION_H */
