#include <stdio.h>
#include <string.h>
#include "ot_motionfusion.h"
#include "ot_omi_motionfusion.h"

#define motofusion_check_null(x)         \
    do {                                        \
        if (x == OT_NULL) {            \
            printf("param null!\n");      \
            return OT_ERR_MFUSION_NULL_PTR;      \
        }                                       \
    } while (0)

static ot_s32 motionfusion_errcode_trans(ot_s32 err)
{
    switch (err) {
        case OT_ERR_MOTIONFUSION_NOT_CONFIG:
            return OT_ERR_MFUSION_NOT_CONFIG;
        case OT_ERR_MOTIONFUSION_NOBUF:
            return OT_ERR_MFUSION_NOT_NOBUF;
        case OT_ERR_MOTIONFUSION_BUF_EMPTY:
            return OT_ERR_MFUSION_BUF_EMPTY;
        case OT_ERR_MOTIONFUSION_NULL_PTR:
            return OT_ERR_MFUSION_NULL_PTR;
        case OT_ERR_MOTIONFUSION_ILLEGAL_PARAM:
            return OT_ERR_MFUSION_ILLEGAL_PAARAM;
        case OT_ERR_MOTIONFUSION_BUF_FULL:
            return OT_ERR_MFUSION_BUF_FULL;
        case OT_ERR_MOTIONFUSION_SYS_NOTREADY:
            return OT_ERR_MFUSION_SYS_NOTREADY;
        case OT_ERR_MOTIONFUSION_NOT_SUPPORT:
            return OT_ERR_MFUSION_NOT_SUPPORT;
        case OT_ERR_MOTIONFUSION_NOT_PERMITTED:
            return OT_ERR_MFUSION_NOT_PERMITTED;
        case OT_ERR_MOTIONFUSION_BUSY:
            return OT_ERR_MFUSION_BUSY;
        case OT_ERR_MOTIONFUSION_INVALID_CHNID:
            return OT_ERR_MFUSION_INVALID_CHNID ;
        case OT_ERR_MOTIONFUSION_CHN_UNEXIST:
            return OT_ERR_MFUSION_CHN_UNEXIST;
        case OT_ERR_MOTIONFUSION_GYRO_NOTWORK:
            return OT_ERR_MFUSION_GYRO_NOTWORK;
        case OT_ERR_MOTIONFUSION_ACC_NOTWORK:
            return OT_ERR_MFUSION_ACC_NOTWORK;
        case OT_ERR_MOTIONFUSION_INVALID_MODE:
            return OT_ERR_MFUSION_INVALID_MODE;
        case OT_ERR_MOTIONFUSION_INVALID_USECASE:
            return OT_ERR_MFUSION_INVALID_USECASE;
        default:
            return OT_ERR_MFUSION_NOT_SUPPORT;
    }
    return OT_ERR_MFUSION_NOT_SUPPORT;
}

ot_s32 ot_eis_mfusion_set_attr(const ot_u32 fusion_id, const ot_eis_mfusion_attr *mfusion_attr)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_attr mfusion_attr_inner = {0};

    motofusion_check_null(mfusion_attr);

    memcpy(&mfusion_attr_inner, mfusion_attr, sizeof(ot_eis_mfusion_attr));

    ret = ot_omi_mfusion_set_attr(fusion_id, &mfusion_attr_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }

    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_get_attr(const ot_u32 fusion_id, ot_eis_mfusion_attr *mfusion_attr)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_attr mfusion_attr_inner = {0};

    motofusion_check_null(mfusion_attr);

    ret = ot_omi_mfusion_get_attr(fusion_id, &mfusion_attr_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }
    memcpy(mfusion_attr, &mfusion_attr_inner, sizeof(ot_eis_mfusion_attr));

    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_set_gyro_drift(const ot_u32 fusion_id, const ot_eis_mfusion_drift *drift)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_drift drift_attr_inner = {0};

    motofusion_check_null(drift);

    memcpy(&drift_attr_inner, drift, sizeof(ot_eis_mfusion_drift));
    ret = ot_omi_mfusion_set_gyro_drift(fusion_id, &drift_attr_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }

    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_get_gyro_drift(const ot_u32 fusion_id, ot_eis_mfusion_drift *drift)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_drift drift_attr_inner = {0};

    motofusion_check_null(drift);

    ret = ot_omi_mfusion_get_gyro_drift(fusion_id, &drift_attr_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }
    memcpy(drift, &drift_attr_inner, sizeof(ot_eis_mfusion_drift));

    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_set_gyro_six_side_calibration(const ot_u32 fusion_id, const ot_eis_mfusion_six_side_calibration *six_side_calibration)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_six_side_calibration six_side_calibration_inner = {0};

    motofusion_check_null(six_side_calibration);

    memcpy(&six_side_calibration_inner, six_side_calibration, sizeof(ot_eis_mfusion_six_side_calibration));
    ret = ot_omi_mfusion_set_gyro_six_side_calibration(fusion_id, &six_side_calibration_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }

    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_get_gyro_six_side_calibration(const ot_u32 fusion_id, ot_eis_mfusion_six_side_calibration *six_side_calibration)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_six_side_calibration six_side_calibration_inner = {0};

    motofusion_check_null(six_side_calibration);

    ret = ot_omi_mfusion_get_gyro_six_side_calibration(fusion_id, &six_side_calibration_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }
    memcpy(six_side_calibration, &six_side_calibration_inner, sizeof(ot_eis_mfusion_six_side_calibration));

    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_set_gyro_temperature_drift(const ot_u32 fusion_id, const ot_eis_mfusion_temperature_drift *temperature_drift)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_temperature_drift temperature_drift_inner = {0};

    motofusion_check_null(temperature_drift);

    memcpy(&temperature_drift_inner, temperature_drift, sizeof(ot_eis_mfusion_temperature_drift));
    ret = ot_omi_mfusion_set_gyro_temperature_drift(fusion_id, &temperature_drift_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }

    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_get_gyro_temperature_drift(const ot_u32 fusion_id, ot_eis_mfusion_temperature_drift *temperature_drift)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_temperature_drift temperature_drift_inner = {0};

    motofusion_check_null(temperature_drift);

    ret = ot_omi_mfusion_get_gyro_temperature_drift(fusion_id, &temperature_drift_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }

    memcpy(temperature_drift, &temperature_drift_inner, sizeof(ot_eis_mfusion_temperature_drift));
    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_set_gyro_online_temperature_drift(const ot_u32 fusion_id, const ot_eis_mfusion_temperature_drift *temperature_drift)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_temperature_drift temperature_drift_inner = {0};

    motofusion_check_null(temperature_drift);

    memcpy(&temperature_drift_inner, temperature_drift, sizeof(ot_eis_mfusion_temperature_drift));
    ret = ot_omi_mfusion_set_gyro_online_temperature_drift(fusion_id, &temperature_drift_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }

    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_get_gyro_online_temperature_drift(const ot_u32 fusion_id, ot_eis_mfusion_temperature_drift *temperature_drift)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_temperature_drift temperature_drift_inner = {0};

    motofusion_check_null(temperature_drift);

    ret = ot_omi_mfusion_get_gyro_online_temperature_drift(fusion_id, &temperature_drift_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }

    memcpy(temperature_drift, &temperature_drift_inner, sizeof(ot_eis_mfusion_temperature_drift));
    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_set_gyro_online_drift(const ot_u32 fusion_id, const ot_eis_mfusion_drift *drift)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_drift drift_inner = {0};

    motofusion_check_null(drift);

    memcpy(&drift_inner, drift, sizeof(ot_eis_mfusion_drift));
    ret = ot_omi_mfusion_set_gyro_online_drift(fusion_id, &drift_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }

    return OT_SUCCESS;

}
ot_s32 ot_eis_mfusion_get_gyro_online_drift(const ot_u32 fusion_id, ot_eis_mfusion_drift *drift)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_drift drift_inner = {0};

    motofusion_check_null(drift);

    ret = ot_omi_mfusion_get_gyro_online_drift(fusion_id, &drift_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }
    memcpy(drift, &drift_inner, sizeof(ot_eis_mfusion_drift));
    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_init_rotation_compensation(const ot_u32 fusion_id, const ot_eis_mfusion_rotation_cfg *cfg)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_rotation_cfg cfg_inner = {0};

    motofusion_check_null(cfg);

    memcpy(&cfg_inner, cfg, sizeof(ot_eis_mfusion_rotation_cfg));
    ret = ot_omi_mfusion_init_rotation_compensation(fusion_id, &cfg_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }

    return OT_SUCCESS;
}
ot_s32 ot_eis_mfusion_deinit_rotation_compensation(const ot_u32 fusion_id)
{
    ot_s32 ret = OT_FAILURE;

    ret = ot_omi_mfusion_deinit_rotation_compensation(fusion_id);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }

    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_set_rotation_compensation(const ot_u32 fusion_id, const ot_eis_mfusion_rotation_compensation *compensation)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_rotation_compensation compensation_inner = {0};

    motofusion_check_null(compensation);

    memcpy(&compensation_inner, compensation, sizeof(ot_eis_mfusion_rotation_compensation));
    ret = ot_omi_mfusion_set_rotation_compensation(fusion_id, &compensation_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }

    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_get_rotation_compensation(const ot_u32 fusion_id, ot_eis_mfusion_rotation_compensation *compensation)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_rotation_compensation compensation_inner = {0};

    motofusion_check_null(compensation);

    ret = ot_omi_mfusion_get_rotation_compensation(fusion_id, &compensation_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }
    memcpy(compensation, &compensation_inner, sizeof(ot_eis_mfusion_rotation_compensation));
    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_send_quaternion(const ot_u32 fusion_id, ot_eis_mfusion_quaternion_buf *quaternion_buf)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_quaternion_buf quaternion_buf_inner = {0};

    motofusion_check_null(quaternion_buf);

    memcpy(&quaternion_buf_inner, quaternion_buf, sizeof(ot_eis_mfusion_quaternion_buf));
    ret = ot_omi_mfusion_send_quaternion(fusion_id, &quaternion_buf_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }

    return OT_SUCCESS;
}

ot_s32 ot_eis_mfusion_get_quaternion(const ot_u32 fusion_id, ot_u64 begin_pts, ot_u64 end_pts, ot_eis_mfusion_quaternion_buf *quaternion_buf)
{
    ot_s32 ret = OT_FAILURE;
    ot_mfusion_quaternion_buf quaternion_buf_inner = {0};

    motofusion_check_null(quaternion_buf);

    ret = ot_omi_mfusion_get_quaternion(fusion_id, begin_pts, end_pts, &quaternion_buf_inner);
    if(ret != OT_SUCCESS) {
        return motionfusion_errcode_trans(ret);
    }
    memcpy(quaternion_buf, &quaternion_buf_inner, sizeof(ot_eis_mfusion_quaternion_buf));

    return OT_SUCCESS;
}
