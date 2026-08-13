/***********************************************************************************
*  Copyright (C), 2025 Shenzhen Oritek Semiconductors Co., Ltd. All right reserved.
*  ********************************************************************************
*  Description   : motionfusion api
*  Created       : 2025/07/01
*  Last Modified : 2025/07/01
*  Group         : SDK Team
*  ********************************************************************************/

#ifndef OT_MOTIONFUSION_H
#define OT_MOTIONFUSION_H

#include "ot_motionfusion_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MOTIONFUSION_DEV_ID_0 0
#define MOTIONFUSION_DEV_ID_1 1

ot_s32 ot_eis_mfusion_set_attr(const ot_u32 fusion_id, const ot_eis_mfusion_attr *mfusion_attr);
ot_s32 ot_eis_mfusion_get_attr(const ot_u32 fusion_id, ot_eis_mfusion_attr *mfusion_attr);

ot_s32 ot_eis_mfusion_set_gyro_drift(const ot_u32 fusion_id,
                                     const ot_eis_mfusion_drift *drift);
ot_s32 ot_eis_mfusion_get_gyro_drift(const ot_u32 fusion_id,
                                     ot_eis_mfusion_drift *drift);

ot_s32 ot_eis_mfusion_set_gyro_six_side_calibration(const ot_u32 fusion_id,
                                                    const ot_eis_mfusion_six_side_calibration *six_side_calibration);
ot_s32 ot_eis_mfusion_get_gyro_six_side_calibration(const ot_u32 fusion_id,
                                                    ot_eis_mfusion_six_side_calibration *six_side_calibration);

ot_s32 ot_eis_mfusion_set_gyro_temperature_drift(const ot_u32 fusion_id,
                                                 const ot_eis_mfusion_temperature_drift *temperature_drift);
ot_s32 ot_eis_mfusion_get_gyro_temperature_drift(const ot_u32 fusion_id,
                                                 ot_eis_mfusion_temperature_drift *temperature_drift);

ot_s32 ot_eis_mfusion_set_gyro_online_temperature_drift(const ot_u32 fusion_id,
                                                        const ot_eis_mfusion_temperature_drift *temperature_drift);
ot_s32 ot_eis_mfusion_get_gyro_online_temperature_drift(const ot_u32 fusion_id,
                                                        ot_eis_mfusion_temperature_drift *temperature_drift);

ot_s32 ot_eis_mfusion_set_gyro_online_drift(const ot_u32 fusion_id,
                                            const ot_eis_mfusion_drift *drift);
ot_s32 ot_eis_mfusion_get_gyro_online_drift(const ot_u32 fusion_id,
                                            ot_eis_mfusion_drift *drift);

ot_s32 ot_eis_mfusion_init_rotation_compensation(const ot_u32 fusion_id,
    const ot_eis_mfusion_rotation_cfg *cfg);
ot_s32 ot_eis_mfusion_deinit_rotation_compensation(const ot_u32 fusion_id);

ot_s32 ot_eis_mfusion_set_rotation_compensation(const ot_u32 fusion_id,
    const ot_eis_mfusion_rotation_compensation *compensation);
ot_s32 ot_eis_mfusion_get_rotation_compensation(const ot_u32 fusion_id,
    ot_eis_mfusion_rotation_compensation *compensation);

ot_s32 ot_eis_mfusion_send_quaternion(const ot_u32 fusion_id,
    ot_eis_mfusion_quaternion_buf *quaternion_buf);
ot_s32 ot_eis_mfusion_get_quaternion(const ot_u32 fusion_id, ot_u64 begin_pts, ot_u64 end_pts,
    ot_eis_mfusion_quaternion_buf *quaternion_buf);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OT_MOTIONFUSION_H */
