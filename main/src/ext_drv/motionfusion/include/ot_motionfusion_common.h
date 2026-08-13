/***********************************************************************************
*  Copyright (C), 2025 Shenzhen Oritek Semiconductors Co., Ltd. All right reserved.
*  ********************************************************************************
*  Description   : motionfusion common define
*  Created       : 2025/07/01
*  Last Modified : 2025/07/01
*  Group         : SDK Team
*  ********************************************************************************/

#ifndef OT_MOTIONFUSION_COMMON_H
#define OT_MOTIONFUSION_COMMON_H

#include "ot_eis_type.h"
#include "ot_eis_errno.h"
#include "ot_eis_mod.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OT_MFUSION_NOT_CONFIG = 0x20,
    OT_MFUSION_NO_BUF,
    OT_MFUSION_NOT_READY,
    OT_MFUSION_NOT_PERMITTED,
    OT_MFUSION_INVALID_CHNID,
    OT_MFUSION_CHN_UNEXIST,
    OT_MFUSION_GYRO_NOTWORK,
    OT_MFUSION_ACC_NOTWORK,
    OT_MFUSION_INVALID_MODE,
    OT_MFUSION_INVALID_USECASE,
    OT_MFUSION_BUTT
} ot_motionfusion_err_code;

#define OT_ERR_MOTIONFUSION_DEFINE(errno)      OT_EIS_ERRNO_DEFINE(OT_EIS_MODULE_MOTIONFUSION, errno)

#define OT_ERR_MFUSION_NOT_CONFIG          OT_ERR_MOTIONFUSION_DEFINE(OT_MFUSION_NOT_CONFIG)
#define OT_ERR_MFUSION_NOT_NOBUF           OT_ERR_MOTIONFUSION_DEFINE(OT_MFUSION_NO_BUF)
#define OT_ERR_MFUSION_BUF_EMPTY           OT_ERR_MOTIONFUSION_DEFINE(OT_EIS_ERRNO_BUF_EMPTY)
#define OT_ERR_MFUSION_NULL_PTR            OT_ERR_MOTIONFUSION_DEFINE(OT_EIS_ERRNO_NULL_PTR)
#define OT_ERR_MFUSION_ILLEGAL_PAARAM      OT_ERR_MOTIONFUSION_DEFINE(OT_EIS_ERRNO_ILLEGAL_PARAM)
#define OT_ERR_MFUSION_BUF_FULL            OT_ERR_MOTIONFUSION_DEFINE(OT_EIS_ERRNO_BUF_FULL)
#define OT_ERR_MFUSION_SYS_NOTREADY        OT_ERR_MOTIONFUSION_DEFINE(OT_MFUSION_NOT_READY)
#define OT_ERR_MFUSION_NOT_SUPPORT         OT_ERR_MOTIONFUSION_DEFINE(OT_EIS_ERRNO_NO_SUPPORT)
#define OT_ERR_MFUSION_NOT_PERMITTED       OT_ERR_MOTIONFUSION_DEFINE(OT_MFUSION_NOT_PERMITTED)
#define OT_ERR_MFUSION_BUSY                OT_ERR_MOTIONFUSION_DEFINE(OT_EIS_ERRNO_BUSY)
#define OT_ERR_MFUSION_INVALID_CHNID       OT_ERR_MOTIONFUSION_DEFINE(OT_MFUSION_INVALID_CHNID)
#define OT_ERR_MFUSION_CHN_UNEXIST         OT_ERR_MOTIONFUSION_DEFINE(OT_MFUSION_CHN_UNEXIST)
#define OT_ERR_MFUSION_GYRO_NOTWORK        OT_ERR_MOTIONFUSION_DEFINE(OT_MFUSION_GYRO_NOTWORK)
#define OT_ERR_MFUSION_ACC_NOTWORK         OT_ERR_MOTIONFUSION_DEFINE(OT_MFUSION_ACC_NOTWORK)
#define OT_ERR_MFUSION_INVALID_MODE        OT_ERR_MOTIONFUSION_DEFINE(OT_MFUSION_INVALID_MODE)
#define OT_ERR_MFUSION_INVALID_USECASE     OT_ERR_MOTIONFUSION_DEFINE(OT_MFUSION_INVALID_USECASE)

#define OT_EIS_MFUSION_MAX_CHN_NUM 1

#define OT_EIS_MFUSION_AXIS_NUM         3
#define OT_EIS_MFUSION_MATRIX_NUM       9
#define OT_EIS_MFUSION_MATRIX_TEMPERATURE_NUM  9
#define OT_EIS_MFUSION_TEMPERATURE_LUT_SAMPLES 30

#define OT_EIS_MFUSION_TEMPERATURE_GYRO 0x1
#define OT_EIS_MFUSION_TEMPERATURE_ACC  0x2
#define OT_EIS_MFUSION_TEMPERATURE_MAGN 0x4
#define OT_EIS_MFUSION_TEMPERATURE_ALL  0x7

#define OT_EIS_MFUSION_DEVICE_GYRO 0x1
#define OT_EIS_MFUSION_DEVICE_ACC  0x2
#define OT_EIS_MFUSION_DEVICE_MAGN 0x4
#define OT_EIS_MFUSION_DEVICE_ALL  0x7

#define OT_EIS_MFUSION_SIXSIDE_MATRIX_GRADINT 15
#define OT_EIS_MFUSION_ZERO_OFFSET_GRADINT    15
#define OT_EIS_MFUSION_TEMPERATURE_OFFSET_GRADINT    15

#define OT_EIS_MFUSION_COMMON_BUF_SIZE 128
#define OT_EIS_MFUSION_LUT_STATUS_NUM   2

#define OT_EIS_MFUSION_QUATERNION_MAX_NUM 128

typedef struct {
    /*
     * RW; continues steady time (in sec)
     * threshold for steady detection
     * range: [0, (1<<16-1]
     */
    ot_u32 steady_time_thr;
    /*
     * RW; max gyro ZRO tolerance presented in datasheet,
     * with (ADC word length - 1) decimal bits
     * range: [0, 100 * (1<<15)]
     */
    ot_s32 gyro_offset;
    /*
     * RW; max acc ZRO tolerance presented in datasheet,
     * with (ADC word length - 1) decimal bits
     * range: [0, 0.5 * (1<<15)]
     */
    ot_s32 acc_offset;
    /*
     * RW; gyro rms noise of under the current filter BW,
     * with (ADC Word Length - 1) decimal bits
     * range: [0, 0.5 * (1<<15)]
     */
    ot_s32 gyro_rms;
    /*
     * RW; acc rms noise of under the current filter BW
     * with (acc word length - 1) decimal bits
     * range: [0, 0.005 * (1<<15)]
     */
    ot_s32 acc_rms;
    /*
     * RW; scale factor of gyro offset for steady detection,
     * larger -> higher recall, but less the precision
     * range: [0, 1000 * (1<<4)]
     */
    ot_s32 gyro_offset_factor;
    /*
     * RW; scale factor of acc offset for steady detection,
     * larger -> higher recall, but less the precision
     * range: [0, 1000 * (1<<4)]
     */
    ot_s32 acc_offset_factor;
    /*
     * RW; scale factor of gyro rms for steady detection,
     * larger -> higher recall, but less the precision
     * range: [0, 1000 * (1<<4)]
     */
    ot_s32 gyro_rms_factor;
    /*
     * RW; scale factor of acc rms for steady detection,
     * larger -> higher recall, but less the precision
     * range: [0, 1000 * (1<<4)]
     */
    ot_s32 acc_rms_factor;
} ot_eis_mfusion_steady_detect_attr;

typedef struct {
    ot_u32 device_mask;      /* device mask: gyro,acc or magn */
    ot_u32 temperature_mask; /* temperature mask: gyro temperature ,acc temperatureor magn temperature */
    ot_eis_mfusion_steady_detect_attr steady_detect_attr;
} ot_eis_mfusion_attr;

/* angle data per sample */
typedef struct {
    ot_s32 x;
    ot_s32 y;
    ot_s32 z;
    ot_s32 temperature;
    ot_u64 pts;
} ot_eis_mfusion_sample_data;

typedef struct {
    ot_eis_mfusion_sample_data gyro_data[OT_EIS_MFUSION_COMMON_BUF_SIZE];
    ot_u32 buf_data_num; /* RW; data length of the occupied buffer */
    ot_u32 buf_rep_num;  /* RW; data start position of invalid, in case of pts overlap */
} ot_eis_mfusion_gyro_buf;

typedef struct {
    ot_eis_mfusion_sample_data acc_data[OT_EIS_MFUSION_COMMON_BUF_SIZE];
    ot_u32 buf_data_num; /* RW; data length of the occupied buffer */
    ot_u32 buf_rep_num;  /* RW; data start position of invalid, in case of pts overlap */
} ot_eis_mfusion_acc_buf;

typedef enum {
    OT_EIS_IMU_TEMPERATURE_DRIFT_CURV = 0, /* polynomial mode */
    OT_EIS_IMU_TEMPERATURE_DRIFT_LUT,      /* lookup table mode */
    OT_EIS_IMU_TEMPERATURE_DRIFT_BUTT
} ot_eis_mfusion_temperature_drift_mode;

typedef struct {
    /*
     * RW;temperature drift lookup table
     * the 1st col is the time (in sec) for which the sample has not been updated.
     * the 2nd col is the nearest temperature sample during update
     */
    ot_s32 imu_lut[OT_EIS_MFUSION_TEMPERATURE_LUT_SAMPLES][OT_EIS_MFUSION_AXIS_NUM];
    ot_s32 gyro_lut_status[OT_EIS_MFUSION_TEMPERATURE_LUT_SAMPLES][OT_EIS_MFUSION_LUT_STATUS_NUM];
    ot_s32 range_min; /* RW;temperature range minimum  */
    ot_s32 range_max; /* RW;temperature range maximum  */
    ot_u32 step;
} ot_eis_mfusion_temperature_drift_lut;

typedef struct {
    ot_bool enable;
    ot_eis_mfusion_temperature_drift_mode mode; /* RW;temperature drift mode  */
    union {
        /*
         * RW;temperature drift polynomial matrix data
         * AUTO:ot_mfusion_temperature_drift_mode:OT_IMU_TEMPERATURE_DRIFT_CURV
         */
        ot_s32 temperature_matrix[OT_EIS_MFUSION_MATRIX_TEMPERATURE_NUM];
        /*
         * RW;temperature drift lookup table data
         * AUTO:ot_mfusion_temperature_drift_mode:OT_IMU_TEMPERATURE_DRIFT_LUT
         */
        ot_eis_mfusion_temperature_drift_lut temperature_lut;
    };
} ot_eis_mfusion_temperature_drift;

typedef struct {
    ot_bool enable;
    ot_s32 drift[OT_EIS_MFUSION_AXIS_NUM];
} ot_eis_mfusion_drift;

typedef struct {
    ot_bool enable;
    ot_s32 matrix[OT_EIS_MFUSION_MATRIX_NUM];
} ot_eis_mfusion_six_side_calibration;

typedef struct {
    ot_bool enable;
    ot_u32 crop_ratio;       /* RW; Range:[20,100]; Crop ratio of output image. */
} ot_eis_mfusion_rotation_compensation;

typedef struct {
    ot_u32 buf_size;
} ot_eis_mfusion_rotation_cfg;

typedef struct {
    ot_s32 w;
    ot_s32 x;
    ot_s32 y;
    ot_s32 z;
    ot_u64 pts;
} ot_eis_mfusion_quaternion_data;

typedef struct {
    ot_u32 num;
    ot_eis_mfusion_quaternion_data data[OT_EIS_MFUSION_QUATERNION_MAX_NUM];
} ot_eis_mfusion_quaternion_buf;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*OT_MOTIONFUSION_COMMON_H*/
