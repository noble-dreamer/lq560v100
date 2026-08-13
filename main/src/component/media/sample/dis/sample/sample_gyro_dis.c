/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */
#include "sample_gyro_dis.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "ot_common_motionsensor.h"
#include "ot_motionfusion_common.h"
#include "ot_motionsensor_chip_cmd.h"
#include "ot_motionfusion.h"
#include "sample_comm.h"
#include "sample_dis.h"

#include "ot_eis_vi_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define FOV_TO_LDCV2   0

#define NUMS_IN_BUF            4000  /* max numbers of gyro data or acc data in buffer */
#define GYRO_BUF_SIZE          (sizeof(ot_msensor_sample_data) * NUMS_IN_BUF)
#define ACC_BUF_SIZE           (sizeof(ot_msensor_sample_data) * NUMS_IN_BUF)

#define RAW_GYRO_PREC_BITS   15
#define DRIFT_GYRO_PREC_BITS 15
#define IMU_TEMP_RANGE_MIN   (-40)
#define IMU_TEMP_RANGE_MAX   85
#define IMU_TEMP_PREC_BITS   16
#define INT_MAX  0x7fffffff        /* max value for an int */

#define GYRO_DATA_RECORDER_RANGE   250
#define GYRO_DATA_DV_RANGE    1000
#define GYRO_DATA_ODR   800
#define ACC_DATA_RANGE  16
#define ACC_DATA_ODR    800

#define PIPE_NUM                2
#define TWO_PIPE_FRAME_4K_RATE  25
#define OT_MFUSION_MATRIX_NUM   9

const ot_u32 g_frame_rate = 30;
const ot_u32 g_input_frame_rate = 25;
const ot_s32 g_imu_rotation_matrix[OT_MFUSION_MATRIX_NUM] = {
    0, 32768, 0,
    32768, 0, 0,
    0, 0, -32768};

ot_double g_gyro_data_range = GYRO_DATA_DV_RANGE;
ot_s32 g_msensor_dev_fd = -1;
ot_bool g_gyro_started = OT_FALSE;
ot_msensor_buf_attr g_msensor_attr;
ot_bool g_get_query_point_en = OT_TRUE;


ot_s32 sample_motionsensor_init(ot_eis_dis_pdt_type pdt_type)
{
    ot_s32 ret;
    ot_u32 buf_size = GYRO_BUF_SIZE + ACC_BUF_SIZE;
    ot_msensor_param msensor_param_set;
    ot_smr_alloc_attr smr_alloc_attr = { 0 };

    g_msensor_dev_fd = open("/dev/motionsensor_chip", O_RDWR);
    if (g_msensor_dev_fd < 0) {
        sample_print("Error: cannot open MotionSensor device.may not load motionsensor driver !\n");
        return OT_FAILURE;
    }

    smr_alloc_attr.len = buf_size;
    strcpy(smr_alloc_attr.region_name , "");
    ret = ot_smr_alloc(&smr_alloc_attr, &g_msensor_attr.phys_addr, (ot_void **)&g_msensor_attr.virt_addr);
    if (ret != OT_SUCCESS) {
        sample_print("alloc smr for Motionsensor failed,ret:%x !\n", ret);
        ret =  OT_ERR_SMR_NO_MEM;
        goto close_fd;
    }

    (ot_void)memset_s((ot_void *)(ot_uintptr_t)g_msensor_attr.virt_addr, buf_size, 0, buf_size);

    g_msensor_attr.buf_len = buf_size;

    if (pdt_type == OT_EIS_DIS_PDT_TYPE_DV) {
        g_gyro_data_range = GYRO_DATA_DV_RANGE;
    } else if (pdt_type == OT_EIS_DIS_PDT_TYPE_RECORDER) {
        g_gyro_data_range = GYRO_DATA_RECORDER_RANGE;
    }

    /* set device work mode */
    msensor_param_set.attr.device_mask = OT_MSENSOR_DEVICE_GYRO | OT_MSENSOR_DEVICE_ACC;
    msensor_param_set.attr.temperature_mask = OT_MSENSOR_TEMP_GYRO | OT_MSENSOR_TEMP_ACC;
    /* set gyro samplerate and full scale range */
    msensor_param_set.config.gyro_config.odr = GYRO_DATA_ODR * OT_MSENSOR_GRADIENT;
    msensor_param_set.config.gyro_config.fsr = g_gyro_data_range * OT_MSENSOR_GRADIENT;
    /* set accel samplerate and full scale range */
    msensor_param_set.config.acc_config.odr = ACC_DATA_ODR * OT_MSENSOR_GRADIENT;
    msensor_param_set.config.acc_config.fsr = ACC_DATA_RANGE * OT_MSENSOR_GRADIENT;

    (ot_void)memcpy_s(&msensor_param_set.buf_attr, sizeof(ot_msensor_buf_attr),
                      &g_msensor_attr, sizeof(ot_msensor_buf_attr));

    ret = ioctl(g_msensor_dev_fd, MSENSOR_CMD_INIT, &msensor_param_set);
    if (ret) {
        sample_print("MSENSOR_CMD_INIT\n");
        ret = -1;
        goto smr_free;
    }
    return OT_SUCCESS;

smr_free:
    ot_smr_free((ot_phys_addr_t)g_msensor_attr.phys_addr);
    g_msensor_attr.phys_addr = 0;
    g_msensor_attr.virt_addr = (ot_u64)(ot_uintptr_t)NULL;
close_fd:
    close(g_msensor_dev_fd);
    g_msensor_dev_fd = -1;
    return ret;
}

ot_void sample_motionsensor_deinit(void)
{
    ot_s32 ret;
    if (g_msensor_dev_fd < 0) {
        return;
    }

    ret = ioctl(g_msensor_dev_fd, MSENSOR_CMD_DEINIT, NULL);
    if (ret != OT_SUCCESS) {
        sample_print("motionsensor deinit failed , ret:0x%x !\n", ret);
    }

    ret = ot_smr_free((ot_phys_addr_t)g_msensor_attr.phys_addr);
    if (ret != OT_SUCCESS) {
        sample_print("motionsensor smr free failed, ret:0x%x !\n", ret);
    }

    g_msensor_attr.phys_addr = 0;
    g_msensor_attr.virt_addr = (ot_u64)(ot_uintptr_t)NULL;

    close(g_msensor_dev_fd);
    g_msensor_dev_fd = -1;

    return;
}

ot_s32 sample_motionsensor_start()
{
    ot_s32 ret;
    ret =  ioctl(g_msensor_dev_fd, MSENSOR_CMD_START, NULL);
    if (ret) {
        perror("IOCTL_CMD_START_MPU");
        return -1;
    }

    g_gyro_started = OT_TRUE;
    return ret;
}

ot_s32 sample_motionsensor_stop(void)
{
    ot_s32 ret;
    ret = ioctl(g_msensor_dev_fd, MSENSOR_CMD_STOP, NULL);
    if (ret != OT_SUCCESS) {
        sample_print("stop motionsensor failed!\n");
    }

    return ret;
}

static ot_s32 sample_dis_set_mfusion_attr(ot_u32 fusion_id, ot_eis_dis_pdt_type mode)
{
    ot_eis_mfusion_attr mfusion_attr = { 0 };

    mfusion_attr.steady_detect_attr.steady_time_thr = 3; /* 3: default value for IPC */
    mfusion_attr.steady_detect_attr.gyro_offset = (ot_s32)(10 * (1 << 15)); /* 10, 15: default value */
    mfusion_attr.steady_detect_attr.acc_offset = (ot_s32)(0.1 * (1 << 15)); /* 0.1, 15: default value */
    mfusion_attr.steady_detect_attr.gyro_rms = (ot_s32)(0.054 * (1 << 15)); /* 0.054, 15: default value */
    mfusion_attr.steady_detect_attr.acc_rms =  (ot_s32)(1.3565 * (1 << 15) / 1000); /* 1.3565, 15, 1000: default value */
    mfusion_attr.steady_detect_attr.gyro_offset_factor = (ot_s32)(2 * (1 << 4)); /* 2, 4: default value */
    mfusion_attr.steady_detect_attr.acc_offset_factor = (ot_s32)(2 * (1 << 4)); /* 2, 4: default value */
    mfusion_attr.steady_detect_attr.gyro_rms_factor = (ot_s32)(8 * (1 << 4)); /* 8, 4: default value */
    mfusion_attr.steady_detect_attr.acc_rms_factor = (ot_s32)(10 * (1 << 4)); /* 10, 4: default value */

    mfusion_attr.device_mask      = OT_EIS_MFUSION_DEVICE_GYRO | OT_EIS_MFUSION_DEVICE_ACC;
    mfusion_attr.temperature_mask = OT_EIS_MFUSION_TEMPERATURE_GYRO | OT_EIS_MFUSION_TEMPERATURE_ACC;

    if (mode == OT_EIS_DIS_PDT_TYPE_RECORDER) {
        mfusion_attr.steady_detect_attr.steady_time_thr = 3; /* 3: default value for IPC */
        mfusion_attr.steady_detect_attr.gyro_rms_factor = (ot_s32)(8 * (1 << 4)); /* 8, 4: default value for IPC */
        mfusion_attr.steady_detect_attr.acc_rms_factor = (ot_s32)(10 * (1 << 4)); /* 10, 4: default value for IPC */
    } else if (mode == OT_EIS_DIS_PDT_TYPE_DV) {
        mfusion_attr.steady_detect_attr.steady_time_thr = 1; /* 1: default value for DV */
        mfusion_attr.steady_detect_attr.gyro_rms_factor = (ot_s32)(12.5 * (1 << 4)); /* 12.5,4:default value for DV */
        mfusion_attr.steady_detect_attr.acc_rms_factor = (ot_s32)(100 * (1 << 4)); /* 100, 4: default value for DV */
    }

    return ot_eis_mfusion_set_attr(fusion_id, &mfusion_attr);
}

static ot_s32 sample_dis_set_temperature_drift(ot_u32 fusion_id)
{
    ot_eis_mfusion_temperature_drift temperature_drift = { 0 };
    ot_u32 i;
    ot_s32 ret;

    temperature_drift.enable = OT_TRUE;

    temperature_drift.mode = OT_EIS_IMU_TEMPERATURE_DRIFT_LUT;
    temperature_drift.temperature_lut.range_min = 20 * 1024; /* 1024: 2^10, 10 bit precision; 20 degree */
    temperature_drift.temperature_lut.range_max = 78 * 1024; /* 1024: 2^10, 10 bit precision; 78 degree */
    temperature_drift.temperature_lut.step = 2 * 1024; /* 1024: 2^10, 10 bit precision; 2 step */

    for (i = 0; i < OT_EIS_MFUSION_TEMPERATURE_LUT_SAMPLES; i++) {
        temperature_drift.temperature_lut.gyro_lut_status[i][0] = INT_MAX;
        temperature_drift.temperature_lut.gyro_lut_status[i][1] = INT_MAX;
    }

    (ot_void)memset_s(temperature_drift.temperature_lut.imu_lut,
        OT_EIS_MFUSION_TEMPERATURE_LUT_SAMPLES * OT_EIS_MFUSION_AXIS_NUM * sizeof(ot_s32),
        0, OT_EIS_MFUSION_TEMPERATURE_LUT_SAMPLES * OT_EIS_MFUSION_AXIS_NUM * sizeof(ot_s32));

    ret = ot_eis_mfusion_set_gyro_online_temperature_drift(fusion_id, &temperature_drift);
    if (ret != OT_SUCCESS) {
        sample_print("motionfusion_set_gyro_online_temp_drift failed!\n");
    }

    return ret;
}

ot_s32 sample_motionfusion_init_param(ot_eis_dis_pdt_type mode)
{
    ot_s32 ret;
    const ot_u32 fusion_id = 0;
    ot_s32 gyro_drift[OT_EIS_MFUSION_AXIS_NUM] = {0, 0, 0};
    ot_eis_mfusion_six_side_calibration six_side_calibration = { 0 };
    ot_eis_mfusion_drift drift = { 0 };

    six_side_calibration.enable = OT_TRUE;
    (ot_void)memcpy_s(six_side_calibration.matrix, OT_MFUSION_MATRIX_NUM * sizeof(ot_s32),
                      g_imu_rotation_matrix, OT_MFUSION_MATRIX_NUM * sizeof(ot_s32));

    drift.enable = OT_TRUE;
    (ot_void)memcpy_s(drift.drift, OT_EIS_MFUSION_AXIS_NUM * sizeof(ot_s32),
                      gyro_drift, OT_EIS_MFUSION_AXIS_NUM * sizeof(ot_s32));

    ret = sample_dis_set_mfusion_attr(fusion_id, mode);
    if (ret != OT_SUCCESS) {
        goto end;
    }

    ret = ot_eis_mfusion_set_gyro_six_side_calibration(fusion_id, &six_side_calibration);
    if (ret != OT_SUCCESS) {
        sample_print("motionfusion_set_gyro_six_side_cal failed!\n");
        goto end;
    }

    if (mode == OT_EIS_DIS_PDT_TYPE_RECORDER) {
        ret = ot_eis_mfusion_set_gyro_online_drift(fusion_id, &drift);
        if (ret != OT_SUCCESS) {
            sample_print("motionfusion_set_gyro_online_drift failed!\n");
            goto end;
        }
    } else if (mode == OT_EIS_DIS_PDT_TYPE_DV) {
        ret = sample_dis_set_temperature_drift(fusion_id);
        if (ret != OT_SUCCESS) {
            goto end;
        }
    }

end:
    return ret;
}

ot_s32 sample_motionfusion_deinit_param()
{
    const ot_u32 fusion_id = 0;
    ot_s32 ret;
    ot_s32 gyro_drift[OT_EIS_MFUSION_AXIS_NUM] = { 0 };
    ot_eis_mfusion_drift drift = { 0 };
    ot_eis_mfusion_temperature_drift temperature_drift = { 0 };
    drift.enable = OT_TRUE;
    (ot_void)memcpy_s(drift.drift, OT_EIS_MFUSION_AXIS_NUM * sizeof(ot_s32), gyro_drift, OT_EIS_MFUSION_AXIS_NUM * sizeof(ot_s32));

    ret = ot_eis_mfusion_get_gyro_online_drift(fusion_id, &drift);
    if (ret != OT_SUCCESS) {
        sample_print("mfusion_get_gyro_online_drift failed!\n");
        goto end;
    }

    drift.enable = OT_FALSE;

    ret = ot_eis_mfusion_set_gyro_online_drift(fusion_id, &drift);
    if (ret != OT_SUCCESS) {
        sample_print("ot_eis_mfusion_set_gyro_online_drift failed!\n");
        goto end;
    }

    temperature_drift.enable = OT_TRUE;
    ret = ot_eis_mfusion_get_gyro_online_temperature_drift(fusion_id, &temperature_drift);
    if (ret != OT_SUCCESS) {
        sample_print("mfusion_get_gyro_online_temperature_drift failed!\n");
        goto end;
    }

    temperature_drift.enable = OT_FALSE;
    ret = ot_eis_mfusion_set_gyro_online_temperature_drift(fusion_id, &temperature_drift);
    if (ret != OT_SUCCESS) {
        sample_print("mfusion_set_gyro_online_temperature_drift failed!\n");
        goto end;
    }

    sleep(1);
end:
    return ret;
}

ot_s32 sample_dis_start_gyro(ot_eis_dis_pdt_type pdt_type)
{
    ot_s32 ret;

    ret = sample_motionsensor_init(pdt_type);
    if (ret != OT_SUCCESS) {
        sample_print("init gyro fail.ret:0x%x !\n", ret);
        return ret;
    }

    ret = sample_motionsensor_start();
    if (ret != OT_SUCCESS) {
        sample_print("start gyro fail.ret:0x%x !\n", ret);
        goto motionsensor_init_fail;
    }

    ret = sample_motionfusion_init_param(pdt_type);
    if (ret != OT_SUCCESS) {
        sample_print("motionfusion set param fail.ret:0x%x !\n", ret);
        goto motionsensor_start_fail;
    }

    return OT_SUCCESS;

motionsensor_start_fail:
    sample_motionsensor_stop();
motionsensor_init_fail:
    sample_motionsensor_deinit();
    return ret;
}

ot_void sample_dis_stop_gyro()
{
    sample_motionfusion_deinit_param();
    sample_motionsensor_stop();
    sample_motionsensor_deinit();
}

ot_s32 sample_dis_start_gyro_sample(sample_vi_cfg *vi_cfg, sample_vproc_attr *vproc_attr, sample_vo_attr *vo_attr,
                                    ot_eis_img_size *img_size, ot_eis_dis_pdt_type pdt_type)
{
    ot_s32 ret;

    ret = sample_dis_start_sample(vi_cfg, vproc_attr, vo_attr, img_size);
    if (ret != OT_SUCCESS) {
        return OT_FAILURE;
    }
    printf("====== please use PQtools to reduce frame_rate ======\n");
    getchar();

    ret = sample_dis_start_gyro(pdt_type);
    if (ret != OT_SUCCESS) {
        sample_dis_stop_sample(vi_cfg, vproc_attr, vo_attr);
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

ot_void sample_dis_stop_gyro_sample(sample_vi_cfg *vi_cfg, sample_vproc_attr *vproc_attr, sample_vo_attr *vo_attr)
{
    sample_dis_stop_sample_without_sys_exit(vi_cfg, vproc_attr, vo_attr);
    sample_dis_stop_gyro();
    sample_comm_sys_exit();
}

ot_s32 sample_dis_get_dis_ldc_attr_from_fov(const ot_eis_img_size *size, ot_eis_dis_ldc_attr *dis_ldc_attr)
{
#if FOV_TO_LDCV2
    sample_fov_attr fov_attr;
    ot_s32 ret;

    fov_attr.width  = size->width;
    fov_attr.height = size->height;
    fov_attr.type   = OT_FOV_TYPE_DIAGONAL;
    fov_attr.fov    = 90 * (1 << FOV_PREC_BITS); /* 90 degree */

    ret = ot_sample_fov_to_ldcv2(&fov_attr, dis_ldc_attr);
    if (ret == OT_SUCCESS) {
        return OT_SUCCESS;
    }

    sample_print("sample_fov_to_ldcv2 failed.ret:0x%x !\n", ret);
#endif
    return OT_FAILURE;
}

/* ldc v2 attr can get with len calibration tool, see more in PQTools */
ot_void sample_dis_get_dis_ldc_attr(const ot_eis_img_size *size, ot_eis_dis_ldc_attr *dis_ldc_attr)
{
    if (sample_dis_get_dis_ldc_attr_from_fov(size, dis_ldc_attr) == OT_SUCCESS) {
        return;
    }
    /* no LDC */
    dis_ldc_attr->focal_len_x = 412202 * size->width / 3840; /* 412202: from PQTools, according to 3840 */
    dis_ldc_attr->focal_len_y = 411850 * size->height / 2160; /* 411850: from PQTools, according to 2160 */
    dis_ldc_attr->coord_shift_x = 164627 * size->width / 3840; /* 164627: from PQTools, according to 3840 */
    dis_ldc_attr->coord_shift_y = 101025 * size->height / 2160; /* 101025: from PQTools, according to 2160 */

    dis_ldc_attr->src_calibration_ratio[0] = 100000;  /* 100000: fixed value */
    dis_ldc_attr->src_calibration_ratio[1] = 0; /* index 1 */
    dis_ldc_attr->src_calibration_ratio[2] = 0; /* index 2 */
    dis_ldc_attr->src_calibration_ratio[3] = 0; /* index 3 */
    dis_ldc_attr->src_calibration_ratio[4] = 0; /* index 4 */
    dis_ldc_attr->src_calibration_ratio[5] = 0; /* index 5 */
    dis_ldc_attr->src_calibration_ratio[6] = 0; /* index 6 */
    dis_ldc_attr->src_calibration_ratio[7] = 0; /* index 7 */
    dis_ldc_attr->src_calibration_ratio[8] = 800000; /* 800000: get from PQTools */

    dis_ldc_attr->dst_calibration_ratio[0] = 100000;  /* 100000: fixed value */
    dis_ldc_attr->dst_calibration_ratio[1] = 0; /* index 1 */
    dis_ldc_attr->dst_calibration_ratio[2] = 0; /* index 2 */
    dis_ldc_attr->dst_calibration_ratio[3] = 0; /* index 3 */
    dis_ldc_attr->dst_calibration_ratio[4] = 0; /* index 4 */
    dis_ldc_attr->dst_calibration_ratio[5] = 0; /* index 5 */
    dis_ldc_attr->dst_calibration_ratio[6] = 0; /* index 6 */
    dis_ldc_attr->dst_calibration_ratio[7] = 0; /* index 7 */
    dis_ldc_attr->dst_calibration_ratio[8] = 0; /* index 8 */
    dis_ldc_attr->dst_calibration_ratio[9] = 0; /* index 9 */
    dis_ldc_attr->dst_calibration_ratio[10] = 0; /* index 10 */
    dis_ldc_attr->dst_calibration_ratio[11] = 0; /* index 11 */
    dis_ldc_attr->dst_calibration_ratio[12] = 800000; /* index 12; 800000: get from PQTools */
    dis_ldc_attr->dst_calibration_ratio[13] = 800000; /* index 13; 800000: get from PQTools */
    dis_ldc_attr->max_du = (ot_s32)(16 * (1 << 16));  /* 16: max value */
}

static ot_void sample_dis_get_gyro_dis_cfg(const ot_eis_img_size *size, ot_eis_dis_pdt_type pdt_type,
    ot_eis_dis_cfg *dis_cfg, ot_eis_dis_attr *dis_attr)
{
    const ot_s32 imu_filter_delay_time = 2900; /* according to imu datesheet */
    const ot_s32 read_out_delay_time = 92;     /* according to sensor datesheet */
    dis_cfg->mode = OT_EIS_DIS_MODE_GYRO;
    dis_cfg->motion_level = OT_EIS_DIS_MOTION_LEVEL_NORM;
    dis_cfg->crop_ratio = 80; /* 80: tipical crop ratio value */
    dis_cfg->buf_num = 5; /* 5: tipical buffer num value */
    dis_cfg->scale = OT_TRUE;
    dis_cfg->frame_rate = g_frame_rate;
    dis_cfg->pdt_type = pdt_type;
    dis_cfg->camera_steady = pdt_type == OT_EIS_DIS_PDT_TYPE_RECORDER ? OT_TRUE : OT_FALSE;

    dis_attr->enable = OT_TRUE;
    dis_attr->moving_subject_level = 0;
    dis_attr->rolling_shutter_coef = 0;
    dis_attr->still_crop = OT_FALSE;
    dis_attr->hor_limit = 512; /* 512: tipical hor_limit value when camera_steady is false */
    dis_attr->ver_limit = 512; /* 512: tipical ver_limit value when camera_steady is false */
    dis_attr->strength = 1024; /* 1024: max strength */

    dis_attr->timelag = imu_filter_delay_time - read_out_delay_time;
    dis_attr->dis_ldc_en = OT_FALSE;
    sample_dis_get_dis_ldc_attr(size, &dis_attr->dis_ldc_attr);
}

ot_s32 sample_dis_start_gyro_dis(ot_vi_pipe_handle pipe_hdl, ot_vi_chn_handle chn_hdl, ot_eis_dis_cfg *dis_cfg, ot_eis_dis_attr *dis_attr)
{
    ot_s32 ret;

    ret = ot_eis_vi_set_chn_dis_cfg(pipe_hdl, chn_hdl, dis_cfg);
    if (ret != OT_SUCCESS) {
        sample_print("set dis config failed.ret:0x%x !\n", ret);
        return ret;
    }

    ret = ot_eis_vi_set_chn_dis_attr(pipe_hdl, chn_hdl, dis_attr);
    if (ret != OT_SUCCESS) {
        sample_print("set dis attr failed.ret:0x%x !\n", ret);
        return ret;
    }

    return OT_SUCCESS;
}

ot_s32 sample_dis_gyro_switch_disldc(ot_vi_pipe_handle pipe_hdl, ot_vi_chn_handle chn_hdl, ot_eis_dis_attr *dis_attr)
{
    ot_s32 ret;

    sample_dis_pause("switch to DIS_LDC!");
    dis_attr->still_crop = OT_TRUE;
    ret = ot_eis_vi_set_chn_dis_attr(pipe_hdl, chn_hdl, dis_attr);
    if (ret != OT_SUCCESS) {
        sample_print("set dis attr failed.ret:0x%x !\n", ret);
        return ret;
    }

    sample_dis_pause("DIS_LDC and GYRO_DIS!");
    dis_attr->still_crop = OT_FALSE;
    ret = ot_eis_vi_set_chn_dis_attr(pipe_hdl, chn_hdl, dis_attr);
    if (ret != OT_SUCCESS) {
        sample_print("set dis attr failed.ret:0x%x !\n", ret);
        return ret;
    }

    return OT_SUCCESS;
}

ot_s32 sample_dis_gyro_ldc_switch(ot_u32 vo_intf_type)
{
    ot_s32 ret;
    ot_eis_img_size size;
    sample_vi_cfg vi_cfg = { 0 };
    sample_vo_attr *vo_attr = OT_NULL;
    sample_vproc_attr vproc_attr = { 0 };
    sample_sns_type sns_type = SONY_IMX623_MIPI_3M_60FPS_16BIT;
    ot_eis_dis_cfg dis_cfg = { 0 };
    ot_eis_dis_attr dis_attr = { 0 };
    const ot_vi_pipe_id vi_pipe_id = 0;
    ot_vi_pipe_handle vi_pipe_hdl;
    ot_vi_chn_handle vi_chn_hdl;

    sample_dis_set_save_stream(OT_TRUE);

    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);
    vi_cfg.pipe_info[vi_pipe_id].chn_info.chn_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    vi_cfg.pipe_info[vi_pipe_id].nr_attr.enable = OT_FALSE;
    vi_pipe_hdl = vi_cfg.pipe_info[vi_pipe_id].pipe_hdl;
    vi_chn_hdl  = vi_cfg.pipe_info[vi_pipe_id].chn_info.chn_hdl;

    sample_comm_vi_get_size_by_sns_type(sns_type, &size);

    printf("input size:%dx%d, frame rate:%d\n", size.width, size.height, g_frame_rate);

    sample_media_vproc_get_default_attr_by_snsor(sns_type, &vproc_attr);

    vo_attr = sample_comm_args_get_vo_attr();

    ret = sample_dis_start_gyro_sample(&vi_cfg, &vproc_attr, vo_attr, &size, OT_EIS_DIS_PDT_TYPE_DV);
    if (ret != OT_SUCCESS) {
        return OT_FAILURE;
    }

    sample_dis_get_gyro_dis_cfg(&size, OT_EIS_DIS_PDT_TYPE_RECORDER, &dis_cfg, &dis_attr);
    dis_attr.dis_ldc_en = OT_TRUE;
    ret = sample_dis_start_gyro_dis(vi_pipe_hdl, vi_chn_hdl, &dis_cfg, &dis_attr);
    if (ret != OT_SUCCESS) {
        sample_dis_stop_gyro_sample(&vi_cfg, &vproc_attr, vo_attr);
        return ret;
    }

    ret = sample_dis_gyro_switch_disldc(vi_pipe_hdl, vi_chn_hdl, &dis_attr);

    sample_dis_pause("exit");

    sample_dis_stop_gyro_sample(&vi_cfg, &vproc_attr, vo_attr);
    return ret;
}

ot_eis_img_size g_venc_size = {1936, 1552};
ot_void sample_dis_set_venc_chn_size(ot_eis_img_size img_size)
{
    g_venc_size = img_size;
}

ot_s32 sample_dis_start_gyro_runbe_sample(sample_vi_cfg *vi_cfg, sample_vo_attr *vo_attr,
                                          sample_vproc_attr *vproc_attr, ot_eis_img_size *img_size)
{
    ot_s32 ret;

    ret = sample_dis_start_sample(vi_cfg, vo_attr, vproc_attr, img_size);
    if (ret != OT_SUCCESS) {
        printf("sample_dis_start_sample failed!\n");
        return OT_FAILURE;
    }
    return OT_SUCCESS;
}

static ot_s32 sample_dis_send_data(ot_eis_img_size* input_size)
{
    ot_s32 ret;
    ot_eis_img_size size = { 0 };
    sample_vi_cfg vi_cfg = { 0 };
    sample_vproc_attr vproc_attr = { 0 };
    sample_vo_attr *vo_attr = OT_NULL;
    const ot_vi_pipe_id vi_pipe = 3;  //yuv
    const ot_vi_chn_id vi_chn = 0;
    sample_dis_set_save_stream(OT_TRUE);
    size.width = input_size->width;
    size.height = input_size->height;
    sample_sns_type sns_type = SONY_IMX623_MIPI_3M_60FPS_16BIT;

    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);
    vi_cfg.sns_info.bus_id = -1;
    vi_cfg.dev_info.dev_attr.in_size.width = size.width;
    vi_cfg.dev_info.dev_attr.in_size.height = size.height;
    vi_cfg.pipe_info[0].chn_info.chn_attr.size.width = size.width;
    vi_cfg.pipe_info[0].chn_info.chn_attr.size.height = size.height;
    vi_cfg.pipe_info[0].pipe_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    vi_cfg.pipe_info[0].pipe_attr.pixel_format = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
    vi_cfg.pipe_info[0].chn_info.chn_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    vi_cfg.pipe_info[0].nr_attr.enable = OT_FALSE;
    vi_cfg.pipe_info[0].pipe_id[0] = vi_pipe;
    vi_cfg.pipe_info[0].pipe_attr.frame_source = OT_EIS_VI_PIPE_FRAME_SOURCE_USER;
    vi_cfg.pipe_info[0].pipe_attr.isp_bypass = OT_TRUE;

    sample_dis_set_venc_chn_size(size);

    printf("input size:%dx%d, frame rate:%d\n", size.width, size.height, g_input_frame_rate);

    vo_attr = sample_comm_args_get_vo_attr();
    sample_media_vproc_get_default_attr_by_snsor(sns_type, &vproc_attr);

    sample_dis_set_send_data_statue(OT_TRUE);
    ret = sample_dis_start_gyro_runbe_sample(&vi_cfg, &vproc_attr, vo_attr, &size);
    if (ret != OT_SUCCESS) {
        printf("sample_dis_start_gyro_runbe_sample failed!!!\n");
        return OT_FAILURE;
    }

    sample_dis_pause("waiting for send data");
    sample_dis_stop_sample(&vi_cfg, &vproc_attr, vo_attr);
    return ret;
}

ot_s32 sample_dis_send(ot_eis_img_size* input_size)
{
    return sample_dis_send_data(input_size);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of __cplusplus */

