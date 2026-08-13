/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <limits.h>
#include <sys/select.h>
#include <math.h>

#include "ot_type.h"
#include "ot_common_video.h"
#include "ot_buffer.h"
#include "ot_common_motionsensor.h"
#include "ot_motionsensor_chip_cmd.h"
#include "ot_motionsensor_mng_cmd.h"
#include "ot_motionfusion.h"
#include "securec.h"
#include "ini.h"
#include "ot_eis_video.h"
#include "ot_eis_media_sys_common.h"
#include "ot_eis_vi.h"
#include "ot_eis_vi_api.h"
#include "sample_comm.h"

ot_s32 g_vi_send_pipe = 3;
ot_s32 g_hmax = 18518;
ot_s32 g_vmax = 2160;
ot_bool g_first_time_to_read = OT_TRUE;
ot_bool g_read_frame_flag = OT_FALSE;
ot_s32 g_dis_msensor_chip_dev = -1;
ot_s32 g_dis_msensor_mng_dev = -1;
ot_bool signal_false = OT_FALSE;
ot_dynamic_range g_dynamic_range_dis = OT_DYNAMIC_RANGE_SDR8;
ini_dictionary *ldc_dir = OT_NULL;
ini_dictionary *mes_dir = OT_NULL;
const ot_u32 g_msensor_line_count = OT_EIS_MFUSION_LUT_STATUS_NUM + OT_EIS_MFUSION_AXIS_NUM + 1; /* 1:step */
const ot_u32 g_msensor_lut_count = OT_EIS_MFUSION_TEMPERATURE_LUT_SAMPLES * g_msensor_line_count;

FILE *g_dis_send_srs_fd = OT_NULL;
FILE *g_send_pts_fd = OT_NULL;
FILE *g_send_gyrodata_fd = OT_NULL;
FILE *g_send_acc_fd = OT_NULL;

#define DIS_FILE_LENGTH 128
#define DIS_MAX_VPROC_GRP OT_VI_MAX_DEV_NUM
#define DIS_SOURE_FILE_LENGTH 256
#define DIS_SRS_GYRO_FILE_LENGTH 256
#define DIS_MAX_DIGIT_LEN           6
#define DIS_SEND_ARG_BASE       10
#define DIS_IPC_MODE            0
#define DIS_DV_MODE            1
#define MIN_FRAME_COUNT         1
#define DIS_NUMS_IN_BUF 4000 /* max numbers of gyro data or acc data in buffer */
#define DIS_X_BUF_LEN (100000)
#define DIS_GYRO_BUF_LEN ((4 * 3 * DIS_X_BUF_LEN) + 8 * DIS_X_BUF_LEN)
#define DIS_GYRO_BUF_SIZE DIS_GYRO_BUF_LEN
#define DIS_ACC_BUF_SIZE (sizeof(ot_msensor_sample_data) * DIS_NUMS_IN_BUF)
#define DIS_GYRO_DATA_RECORDER_RANGE 250
#define DIS_GYRO_DATA_DV_RANGE 1000
#define DIS_GYRO_DATA_ODR 800
#define DIS_ACC_DATA_RANGE 16
#define DIS_ACC_DATA_ODR 800
#define DIS_PI 3.141593
#define DIS_FOV_PREC_BITS 20
#define DIS_CASE_NUM 40
#define DIS_MAX_PTHREAD_NUM 40
#define DIS_INVALID_PTHREAD_ID (-1U)
#define DIS_FRAME_RATE_60 60
#define WDR_MAX_PTS_DIFF 10000
#define DIS_FL_LENGTH 128
#define SLEEP_TIME (4)
#define NUMBER_BASE_10 (10)
#define DIS_DEFAULT_NUM_VALUE 0
#define TEMP_LEN 20
#define DISGYRO_YUV_PATH "./send_data/"    /* input path */
#define DIS_UP_TO_DOWN 0 /* rolling shutter direction up to down */
#define DIS_DOWN_TO_UP 1 /* rolling shutter direction down to up */
#define VI_SEND_FRAME_USER_DATA 888
#define DIS_SEND_FRAME_RATE_RESTRICT 30

typedef ot_void *(*fn_dis_pthread_proc_func)(ot_void *param);
typedef ot_u32 dis_pthread_id;

typedef struct {
    ot_eis_dis_cfg  dis_cfg;
    ot_eis_dis_attr dis_attr;
    ot_msensor_param msensor_param;
    ot_eis_mfusion_six_side_calibration six_side_calibration;
    ot_eis_mfusion_drift gyro_drift;
    ot_eis_mfusion_temperature_drift temp_drift;
} dis_send_load_motionfusion_param;

static dis_send_load_motionfusion_param g_motionfusion_param;
typedef struct {
    dis_pthread_id    id;
    ot_void             *param;
    ot_bool             thread_err;
} dis_pthread_param;

typedef struct {
    ot_bool                 created;
    pthread_t               thread_id;
    ot_bool                 start;
    dis_pthread_param     pthread_param;
} dis_pthread_info;

static dis_pthread_info g_dis_pthread_info[DIS_MAX_PTHREAD_NUM];

typedef struct {
    ot_u32 route_num;
    ot_u32 virt_route_num;
    ot_u32 send_pthread_num;
    ot_u32 send_pthread_id[DIS_MAX_PTHREAD_NUM];
    ot_msensor_buf_attr gyro_buf_attr;
    ot_u16 dump_cnt;
    ot_bool is_equal_set_send_cnt;
    ot_bool is_start_venc;
    ot_s32 is_dumping;
    ot_bool is_reading;
    ot_bool is_msensor_start;
} dis_test_ctx;

typedef struct {
    ot_vi_dev       vi_dev[OT_VI_MAX_DEV_NUM];
    ot_u32          dev_num;
    ot_wdr_mode     wdr_mode[OT_VI_MAX_DEV_NUM];
    ot_u32          pipe_num[OT_VI_MAX_DEV_NUM];
    ot_vi_pipe      vi_pipe[OT_VI_MAX_DEV_NUM][OT_VI_MAX_PHYS_PIPE_NUM];
    ot_vi_pipe      master_pipe[OT_VI_MAX_DEV_NUM];
    ot_u32          master_pipe_num;
} dis_multi_dev_info;

typedef struct {
    ot_u32                  pipe_num;
    ot_vi_pipe              pipe_id[OT_VI_MAX_VIRT_PIPE_NUM];
} dis_vi_virt_pipe_bind_info;

typedef struct {
    ot_eis_vi_vproc_mode_type mode_type;
    ot_size vrb_size;
    ot_u32 vrb_cnt;
    ot_bool is_no_motion_mask;
} dis_vrb_info;

typedef struct {
    ot_char file_name[DIS_SOURE_FILE_LENGTH];
    ot_u32  send_cnt;
} dis_send_pipe_srs_info;

typedef struct {
    ot_u32 frame_rate;    /* RW; Frame rate. */
    ot_s32 hmax_times;    /* sensor hmax_times,unit is ns */
    ot_s32 read_out_time; /* defined in us,like -8040 : -0.00804s */
    ot_s32 time_delay;    /* defined in us */
    ot_s32 first_called_flag;
} dis_send_sns_param;

typedef struct {
    dis_send_sns_param     sns_param;
    ot_bool                 is_exposure_time;
    ot_u32                  vmax;
    ot_char                 pts_file[DIS_SRS_GYRO_FILE_LENGTH];
    ot_char                 gyrodata_file[DIS_SRS_GYRO_FILE_LENGTH];
    ot_char                 acc_file[DIS_SRS_GYRO_FILE_LENGTH];
} dis_send_gyro_srs_param;

typedef struct {
    ot_eis_dis_cfg dis_config;                 /* DIS_CONFIG */
    ot_eis_dis_attr dis_attr;                  /* DIS_ATTR */
} dis_vi_dis_attr;

typedef struct {
    ot_vi_dev vi_dev;
    ot_vi_pipe vi_pipe[OT_VI_MAX_PIPE_NUM];
    ot_u32 pipe_num;
    ot_wdr_mode wdr_mode;
    ot_size vi_size;
    ot_compress_mode compress_mode;
    ot_compress_mode chn_compress_mode;
    ot_eis_img_pixel_format pixel_format;
    ot_frame_interrupt_type interrupt_type;
    //ot_isp_pub_attr isp_attr;
    //ot_isp_run_wakeup_select run_wakeup;
    ot_bool run_wakeup;
    ot_u8 be_buf_num;
    dis_send_pipe_srs_info send_srs_info;
    dis_send_gyro_srs_param send_gyro_info;
    dis_vi_dis_attr vi_dis_attr;
    ot_bool isp_start_gyro;
    ot_bool isp_need_start;
    ot_bool isp_need_run;
    ot_bool isp_start_venc;
    ot_bool is_send_dev_raw;
    ot_bool is_send_pipe_raw;
    ot_bool is_send_pipe_yuv;
} dis_vi_info;

typedef struct {
    ot_vproc_grp vproc_grp;
    ot_vproc_chn vproc_chn;
    ot_bool is_dis_gyro_support;
    ot_bool vnr_en;
    ot_bool low_delay_en;
    ot_size img_size;
} dis_vproc_info;

typedef struct {
    ot_bool enable;
    ot_eis_mfusion_attr mfusion_attr;
    ot_eis_mfusion_temperature_drift temperature_drift;
    ot_eis_mfusion_drift drift;
    ot_eis_mfusion_six_side_calibration six_side_calibration;
    ot_bool is_zreo_both;
    ot_bool is_temperature_both;
    ot_bool is_temperature_zero;
} dis_mfusion_attr;

typedef struct {
    ot_eis_dis_pdt_type pdt_type;
    size_t buffer_size;
    ot_msensor_param msensor_param;
    dis_mfusion_attr sum_mfusion_attr;
    ot_bool is_open_mng_dev;
    ot_bool is_msensor_start;
} dis_gyro_info;

typedef struct {
    ot_s32 frame_cnt;
    ot_s32 send_width;
    ot_s32 send_height;
} dis_pic_size;

typedef struct {
    dis_vrb_info vrb_info;
    ot_u32 route_num;
    dis_vi_info vi_info[OT_VI_MAX_PIPE_NUM];
    dis_vproc_info vproc_info[DIS_MAX_VPROC_GRP];
    dis_gyro_info gyro_info;
    ot_bool stitch_en;
    //ot_vi_stitch_cfg_mode stitch_mode;
    dis_pic_size pic_size;
} dis_send_info;
static dis_send_info g_dis_send_info = { 0 };

typedef struct {
    ot_size          size;
    ot_eis_img_pixel_format  pixel_format;
    ot_eis_img_layout        video_format;
    ot_eis_img_compress_mode compress_mode;
    ot_dynamic_range dynamic_range;
} dis_vi_get_frame_vrb_cfg;

typedef struct {
    ot_vi_pipe vi_pipe;
    ot_wdr_mode wdr_mode;
    ot_bool is_run_be;
    ot_bool is_send_gyro;
    ot_bool loop_send;
    ot_s32 milli_sec;

    ot_char *file_name;
    ot_u32 frame_cnt;
    dis_send_gyro_srs_param send_gyro_info;
    dis_vi_get_frame_vrb_cfg get_frame_vrb_cfg;
} dis_send_pipe_info;

typedef ot_void *(*fn_dis_pthread_frame_func)(ot_void *param,
    ot_video_frame_info frame_info[][OT_VI_MAX_WDR_FRAME_NUM], ot_u32 dev_num, ot_u32 pipe_num[]);
typedef struct {
    dis_multi_dev_info    *multi_dev_info;
    //ot_isp_run_wakeup_select    run_wakeup;
    ot_bool run_wakeup;
    ot_s32                      mill_sec;
    fn_dis_pthread_frame_func pfn_before_send;
    ot_void                     *before_param;
    fn_dis_pthread_frame_func pfn_after_send;
    ot_void                     *after_param;
} dis_send_pipe_frame_info;

typedef enum {
    OT_FOV_TYPE_DIAGONAL = 0,
    OT_FOV_TYPE_HOROZONTAL,
    OT_FOV_TYPE_VERTICAL,
    OT_FOV_TYPE_BUTT
} dis_fov_type;

typedef struct {
    ot_u32 width;
    ot_u32 height;
    dis_fov_type type; /* 0--diagonal,1--horizontal,2--vertical */
    ot_u32 fov;        /* decimal bits 20bit */
} dis_fov_attr;

/* -------------------------------------------- hong --------------------------------------- */
#ifndef dis_trace
#define dis_trace(fmt...)                      \
    do {                                             \
        printf("[%s]-%d: ", __FUNCTION__, __LINE__); \
        printf(fmt);                                 \
    } while (0)
#endif

#ifndef dis_err_void_goto
#define dis_err_void_goto(express, msg, err, label)                                                        \
    do {                                                                                                     \
        ot_s32 temp_ret;                                                                                     \
        temp_ret = express;                                                                                  \
        if (err != temp_ret) {                                                                               \
            printf("<%s> failed LINE: %d:, should ret: 0x%x, but ret 0x%x\n", msg, __LINE__, err, temp_ret); \
            goto label;                                                                                      \
        }                                                                                                    \
    } while (0)
#endif

#define dis_check_fd_closed(fd)                                                                              \
    do { \
        if ((fd) != OT_NULL) { \
            fclose(fd); \
            (fd) = OT_NULL; \
        } \
    } while (0)

#define dis_send_check_pointer(ptr, args...)                              \
    do {                                                                   \
        if (ptr == OT_NULL) {                                              \
            printf("get %s\n", args); \
            return OT_FAILURE;                                             \
        }                                                                  \
    } while (0)

/* -------------------------------------------- global param --------------------------------------- */

static volatile sig_atomic_t g_signal_flag = 0;

static dis_test_ctx g_dis_test_ctx = { 0 };

dis_pthread_info *dis_get_pthread_info(ot_s32 id)
{
    return &g_dis_pthread_info[id];
}

dis_test_ctx *dis_get_test_ctx(void)
{
    return &g_dis_test_ctx;
}

ot_u32 *dis_get_ctx_route_num(void)
{
    return &g_dis_test_ctx.route_num;
}

ot_u32 *dis_get_ctx_virt_route_num(void)
{
    return &g_dis_test_ctx.virt_route_num;
}

ot_s32 dis_send_check_range(ot_s32 val, ot_s32 min, ot_s32 max)
{
    if ((val) < (min) || (val) > (max)) {
        dis_trace("val:%d is out of range[%d,%d]\n", val, min, max);
        return OT_FAILURE;
    }
    return OT_SUCCESS;
}

ot_s32 dis_send_str_to_long(const ot_char *str, size_t str_len, ot_slong *int_ptr)
{
    if (str == OT_NULL || int_ptr == OT_NULL || str_len == 0) {
        dis_trace("data error !");
        return OT_FAILURE;
    }
    ot_char *end_ptr = OT_NULL;
    errno = 0;
    ot_slong ret = strtol(str, &end_ptr, NUMBER_BASE_10);
    if ((errno == ERANGE && (ret == LONG_MIN || ret == LONG_MAX)) || (errno != 0 && ret == 0)) {
        dis_trace("strtol error !\n");
        return OT_FAILURE;
    }
    if (end_ptr == str) {
        dis_trace("no digit found !\n");
        return OT_FAILURE;
    }
    *int_ptr = ret;
    return OT_SUCCESS;
}

ot_void dis_send_array_conversion(const ot_char *begin_str, ot_s32 count, ot_s32 *mae_weight, ot_bool bx,
    ot_s32 idx)
{
    ot_s32 ret = 0;
    ot_char temp[TEMP_LEN] = {0};
    ot_slong tmp = 0;
    ret = memset_s((ot_void *)temp, TEMP_LEN, 0, TEMP_LEN);
    if (ret < 0) {
        dis_trace("memset_s failed");
    }
    count = MIN2(TEMP_LEN, count);
    ret = memcpy_s(temp, count * sizeof(ot_char), begin_str, count * sizeof(ot_char));
    if (ret < 0) {
        dis_trace("memcpy_s failed");
    }
    if (bx == OT_TRUE) {
        char *str;
        tmp = strtol(temp + 2, &str, 16); // 2 : address offset, Decimal. 16 : hex
        mae_weight[idx] = (ot_s32)tmp;
    } else {
        ret = dis_send_str_to_long(temp, strlen(temp), &tmp);
        mae_weight[idx] = (ot_s32)tmp;
    }
}
ot_s32 dis_send_get_longstr(const ot_char *b, ot_s32 *mae_weight, ot_s32 size)
{
    const ot_char *psz_vr_begin = b;
    const ot_char *psz_vr_end = psz_vr_begin;
    const ot_char *cur_begin = psz_vr_begin;
    ot_s32 count = 0;
    ot_s32 valid_count = 0;
    ot_u32 cur_pos = 0;
    ot_u32 length = (ot_u32)strlen(b);
    ot_s32 idx = 0;
    ot_bool bx = OT_FALSE;

    while (psz_vr_end != OT_NULL && idx < size) {
        if (cur_pos >= length) {
            break;
        }
        if ((*psz_vr_end == ' ') || (*psz_vr_end == '\\')) {
            psz_vr_end++;
            psz_vr_begin = psz_vr_end;
            cur_pos++;
            continue;
        }
        while ((*psz_vr_end != '|') && (*psz_vr_end != '\0') && (*psz_vr_end != ',')) {
            if (*psz_vr_end == 'x') {
                bx = OT_TRUE;
            }
            psz_vr_end++;
            count++;
            cur_pos++;
        }
        cur_begin = psz_vr_begin;
        valid_count = 0;
        while ((*cur_begin != ' ') && valid_count < count) {
            valid_count++;
            cur_begin++;
        }

        dis_send_array_conversion(psz_vr_begin, valid_count, mae_weight, bx, idx);
        count = 0;
        psz_vr_end++;
        psz_vr_begin = psz_vr_end;
        cur_pos++;
        idx++;
    }
    return idx;
}

ot_void dis_send_motionsensor_get_default_param(ot_eis_dis_pdt_type pdt_type, ot_msensor_param *msensor_param)
{
    ot_s32 gyro_data_range = DIS_GYRO_DATA_DV_RANGE;
    if (pdt_type == OT_EIS_DIS_PDT_TYPE_DV) {
        gyro_data_range = DIS_GYRO_DATA_DV_RANGE;
    } else if (pdt_type == OT_EIS_DIS_PDT_TYPE_RECORDER) {
        gyro_data_range = DIS_GYRO_DATA_RECORDER_RANGE;
    }
    /* set device work mode */
    msensor_param->attr.device_mask = OT_MSENSOR_DEVICE_GYRO | OT_MSENSOR_DEVICE_ACC;
    msensor_param->attr.temperature_mask = OT_MSENSOR_TEMP_GYRO | OT_MSENSOR_TEMP_ACC;
    /* set gyro dfmrate and full scale range */
    msensor_param->config.gyro_config.odr = DIS_GYRO_DATA_ODR * OT_MSENSOR_GRADIENT;
    msensor_param->config.gyro_config.fsr = gyro_data_range * OT_MSENSOR_GRADIENT;
    /* set accel dfmrate and full scale range */
    msensor_param->config.acc_config.odr = DIS_ACC_DATA_ODR * OT_MSENSOR_GRADIENT;
    msensor_param->config.acc_config.fsr = DIS_ACC_DATA_RANGE * OT_MSENSOR_GRADIENT;
}

static ot_void dis_send_motionfusion_get_default_attr_parm(ot_eis_dis_pdt_type mode, ot_eis_mfusion_attr *mfusion_attr)
{
    mfusion_attr->steady_detect_attr.steady_time_thr = 3;                    /* 3: default value for IPC */
    mfusion_attr->steady_detect_attr.gyro_offset = (ot_s32)(10 * (1 << 15)); /* 10, 15: default value */
    mfusion_attr->steady_detect_attr.acc_offset = (ot_s32)(0.1 * (1 << 15)); /* 0.1, 15: default value */
    mfusion_attr->steady_detect_attr.gyro_rms = (ot_s32)(0.054 * (1 << 15)); /* 0.054, 15: default value */
    mfusion_attr->steady_detect_attr.acc_rms =
        (ot_s32)(1.3565 * (1 << 15) / 1000);                                      /* 1.3565, 15, 1000: default value */
    mfusion_attr->steady_detect_attr.gyro_offset_factor = (ot_s32)(2 * (1 << 4)); /* 2, 4: default value */
    mfusion_attr->steady_detect_attr.acc_offset_factor = (ot_s32)(2 * (1 << 4));  /* 2, 4: default value */
    mfusion_attr->steady_detect_attr.gyro_rms_factor = (ot_s32)(8 * (1 << 4));    /* 8, 4: default value */
    mfusion_attr->steady_detect_attr.acc_rms_factor = (ot_s32)(10 * (1 << 4));    /* 10, 4: default value */

    mfusion_attr->device_mask = OT_EIS_MFUSION_DEVICE_GYRO | OT_EIS_MFUSION_DEVICE_ACC;
    mfusion_attr->temperature_mask = OT_EIS_MFUSION_TEMPERATURE_GYRO | OT_EIS_MFUSION_TEMPERATURE_ACC;

    if (mode == OT_EIS_DIS_PDT_TYPE_RECORDER) {
        mfusion_attr->steady_detect_attr.steady_time_thr = 3;                      /* 3: default value for IPC */
        mfusion_attr->steady_detect_attr.gyro_rms_factor = (ot_s32)(8 * (1 << 4)); /* 8, 4: default value for IPC */
        mfusion_attr->steady_detect_attr.acc_rms_factor = (ot_s32)(10 * (1 << 4)); /* 10, 4: default value for IPC */
    } else if (mode == OT_EIS_DIS_PDT_TYPE_DV) {
        mfusion_attr->steady_detect_attr.steady_time_thr = 1;                         /* 1: default value for DV */
        mfusion_attr->steady_detect_attr.gyro_rms_factor = (ot_s32)(12.5 * (1 << 4)); /* 12.5,4:default value for DV */
        mfusion_attr->steady_detect_attr.acc_rms_factor = (ot_s32)(100 * (1 << 4));   /* 100, 4: default value for DV */
    }
}

static ot_void dis_send_motionfusion_get_default_temperature_drift_parm(ot_eis_mfusion_temperature_drift *temperature_drift)
{
    ot_u32 i;
    temperature_drift->enable = OT_TRUE;
    temperature_drift->mode = OT_EIS_IMU_TEMPERATURE_DRIFT_LUT;
    temperature_drift->temperature_lut.range_min = 20 * 1024; /* 1024: 2^10, 10 bit precision; 20 degree */
    temperature_drift->temperature_lut.range_max = 49 * 1024; /* 1024: 2^10, 10 bit precision; 49 degree */
    temperature_drift->temperature_lut.step = 1 * 1024;       /* 1024: 2^10, 10 bit precision */

    for (i = 0; i < OT_EIS_MFUSION_TEMPERATURE_LUT_SAMPLES; i++) {
        temperature_drift->temperature_lut.gyro_lut_status[i][0] = INT_MAX;
        temperature_drift->temperature_lut.gyro_lut_status[i][1] = INT_MAX;
    }

    (ot_void)memset_s(temperature_drift->temperature_lut.imu_lut,
        OT_EIS_MFUSION_TEMPERATURE_LUT_SAMPLES * OT_EIS_MFUSION_AXIS_NUM * sizeof(ot_s32), 0,
        OT_EIS_MFUSION_TEMPERATURE_LUT_SAMPLES * OT_EIS_MFUSION_AXIS_NUM * sizeof(ot_s32));
    return;
}

ot_void dis_send_motionfusion_get_default_six_side_calibration(ot_eis_mfusion_six_side_calibration *calibration)
{
    calibration->enable = OT_TRUE;
    ot_s32 dfm_imu_rotation_matrix[OT_EIS_MFUSION_MATRIX_NUM] = {0, 32768, 0, 32768, 0, 0, 0, 0, -32768};
    if (memcpy_s(calibration->matrix, OT_EIS_MFUSION_MATRIX_NUM * sizeof(ot_s32), dfm_imu_rotation_matrix,
        OT_EIS_MFUSION_MATRIX_NUM * sizeof(ot_s32)) != EOK) {
        dis_trace("memcpy_s calibration->matrix default failed\n");
    }
}

ot_void dis_send_motionfusion_get_default_gyro_drift(ot_eis_mfusion_drift *mfusion_drift)
{
    mfusion_drift->enable = OT_TRUE;
    ot_s32 gyro_drift[OT_EIS_MFUSION_AXIS_NUM] = {0, 0, 0};
    if (memcpy_s(mfusion_drift->drift, OT_EIS_MFUSION_AXIS_NUM * sizeof(ot_s32), gyro_drift,
        OT_EIS_MFUSION_AXIS_NUM * sizeof(ot_s32)) != EOK) {
        dis_trace("memcpy_s default_gyro_drift failed\n");
    }
}

ot_void dis_send_gyro_get_default_param(ot_eis_dis_pdt_type pdt_type, dis_gyro_info *gyro_info)
{
    ot_msensor_param *msensor_param = &gyro_info->msensor_param;
    ot_eis_mfusion_attr *mfusion_attr = &gyro_info->sum_mfusion_attr.mfusion_attr;
    ot_eis_mfusion_temperature_drift *temperature_drift = &gyro_info->sum_mfusion_attr.temperature_drift;
    ot_eis_mfusion_six_side_calibration *calibration = &gyro_info->sum_mfusion_attr.six_side_calibration;
    ot_eis_mfusion_drift *mfusion_drift = &gyro_info->sum_mfusion_attr.drift;
    gyro_info->pdt_type = pdt_type;
    gyro_info->is_open_mng_dev = OT_TRUE;
    gyro_info->buffer_size = DIS_GYRO_BUF_SIZE + DIS_ACC_BUF_SIZE;
    dis_send_motionsensor_get_default_param(pdt_type, msensor_param);
    dis_send_motionfusion_get_default_attr_parm(pdt_type, mfusion_attr);
    dis_send_motionfusion_get_default_temperature_drift_parm(temperature_drift);
    dis_send_motionfusion_get_default_six_side_calibration(calibration);
    dis_send_motionfusion_get_default_gyro_drift(mfusion_drift);
}

ot_void dis_send_gyrodis_single_get_common_parm(dis_send_info *dis_send_info, ot_eis_dis_pdt_type pdt_type)
{
    dis_send_gyro_get_default_param(pdt_type, &dis_send_info->gyro_info);
    dis_send_info->vi_info[0].is_send_pipe_yuv = OT_TRUE;
    dis_send_info->route_num = 1;
}
static ot_void dis_send_set_dis_attr(dis_vi_dis_attr *vi_dis_attr, ot_s32 timelag)
{
    vi_dis_attr->dis_attr.enable = g_motionfusion_param.dis_attr.enable;
    vi_dis_attr->dis_attr.moving_subject_level = 0;
    vi_dis_attr->dis_attr.rolling_shutter_coef = 0;
    vi_dis_attr->dis_attr.timelag = timelag;
    vi_dis_attr->dis_attr.still_crop = 0;
    vi_dis_attr->dis_attr.hor_limit = 512; /* hor_limit : 512 */
    vi_dis_attr->dis_attr.ver_limit = 512; /* ver_limit : 512 */
    vi_dis_attr->dis_attr.strength = g_motionfusion_param.dis_attr.strength;
}

static ot_void dis_send_set_ldc_config(dis_vi_dis_attr *vi_dis_attr)
{
    vi_dis_attr->dis_attr.dis_ldc_en = g_motionfusion_param.dis_attr.dis_ldc_en;
    vi_dis_attr->dis_attr.dis_ldc_attr.focal_len_x = g_motionfusion_param.dis_attr.dis_ldc_attr.focal_len_x;
    vi_dis_attr->dis_attr.dis_ldc_attr.focal_len_y = g_motionfusion_param.dis_attr.dis_ldc_attr.focal_len_y;
    vi_dis_attr->dis_attr.dis_ldc_attr.coord_shift_x = g_motionfusion_param.dis_attr.dis_ldc_attr.coord_shift_x;
    vi_dis_attr->dis_attr.dis_ldc_attr.coord_shift_y = g_motionfusion_param.dis_attr.dis_ldc_attr.coord_shift_y;
    vi_dis_attr->dis_attr.dis_ldc_attr.max_du = g_motionfusion_param.dis_attr.dis_ldc_attr.max_du;
    for (ot_s32 i = 0; i < OT_SRC_LENS_COEF_NUM; i++) {
        vi_dis_attr->dis_attr.dis_ldc_attr.src_calibration_ratio[i] =
            g_motionfusion_param.dis_attr.dis_ldc_attr.src_calibration_ratio[i];
    }
    for (ot_s32 i = 0; i < OT_DST_LENS_COEF_NUM; i++) {
        vi_dis_attr->dis_attr.dis_ldc_attr.dst_calibration_ratio[i] =
            g_motionfusion_param.dis_attr.dis_ldc_attr.dst_calibration_ratio[i];
    }
}

static ot_void dis_send_set_dis_config(dis_vi_dis_attr *vi_dis_attr, ot_eis_dis_pdt_type pdt_type)
{
    vi_dis_attr->dis_config.mode = OT_EIS_DIS_MODE_GYRO;
    vi_dis_attr->dis_config.motion_level = OT_EIS_DIS_MOTION_LEVEL_NORM;
    vi_dis_attr->dis_config.crop_ratio = g_motionfusion_param.dis_cfg.crop_ratio;
    vi_dis_attr->dis_config.buf_num = 10; /* buf_num : 10 */
    vi_dis_attr->dis_config.frame_rate = g_motionfusion_param.dis_cfg.frame_rate;
    vi_dis_attr->dis_config.pdt_type = pdt_type;
    vi_dis_attr->dis_config.scale = g_motionfusion_param.dis_cfg.scale;
    vi_dis_attr->dis_config.camera_steady = g_motionfusion_param.dis_cfg.camera_steady;
}
ot_s32 dis_send_check_fd(const ot_char *gyrodata_file, ot_u32 file_len, ot_char *path, ot_u32 path_max)
{
    if (file_len > path_max) {
        dis_trace("file_len is too long,it is invalid!\n");
        return OT_FAILURE;
    }
    if (realpath(gyrodata_file, path) == OT_NULL) {
        dis_trace("gyrodata_file realpath err, invalid file path!\n");
        return OT_FAILURE;
    }
    return OT_SUCCESS;
}

ot_s32 dis_send_get_gyro_drift(const ot_char *gyrodata_file, ot_u32 file_len,
                               ot_eis_dis_pdt_type pdt_type, ot_s32 drift[], ot_u32 drift_len)
{
    ot_s32 x, y, z, tmp;
    ot_u64 pts;
    ot_u32 i = 0;
    const ot_u32 avs_num = 10000;
    ot_u32 gyro_num = 0;
    ot_s64 x_sum = 0;
    ot_s64 y_sum = 0;
    ot_s64 z_sum = 0;
    ot_s32 gyro_data_range = DIS_GYRO_DATA_RECORDER_RANGE;
    ot_char path[PATH_MAX] = {0};
    gyro_data_range = pdt_type == OT_EIS_DIS_PDT_TYPE_DV ? DIS_GYRO_DATA_DV_RANGE : DIS_GYRO_DATA_RECORDER_RANGE;
    gyro_data_range = ini_get_int(mes_dir, (ot_char *)"gyro_config:fsr", 0) / 1024; /* data width : 1024 */
    if (dis_send_check_fd(gyrodata_file, file_len, path, PATH_MAX) != OT_SUCCESS) {
        return OT_FAILURE;
    }
    FILE *gyrodata_fd = fopen(path, "r");
    if (gyrodata_fd == OT_NULL) {
        dis_trace("open %s failed\n", gyrodata_file);
        return OT_FAILURE;
    }
    while (i++ < avs_num) {
        int items_matched = fscanf_s(gyrodata_fd, "%d,%d,%d,%d,%llu",
                                    &x, &y, &z, &tmp, &pts);

        if (items_matched == EOF) {
            if (feof(gyrodata_fd)) {  // 文件正常结束，不是错误
                break;
            } else {  // 真正的读取错误
                dis_trace("read error at line %u: %s\n", i, strerror(errno));
                fclose(gyrodata_fd);
                return OT_FAILURE;
            }
        }

        x_sum += x;
        y_sum += y;
        z_sum += z;
        gyro_num++;
    }
    (ot_void)fseek(gyrodata_fd, 0, SEEK_SET);
    if (drift_len < OT_EIS_MFUSION_AXIS_NUM) {
        fclose(gyrodata_fd);
        return OT_FAILURE;
    }
    if (gyro_num == 0) {
        for (i = 0; i < OT_EIS_MFUSION_AXIS_NUM; i++) {
            drift[i] = g_motionfusion_param.gyro_drift.drift[i];
        }
        fclose(gyrodata_fd);
        return OT_SUCCESS;
    }
    drift[0] = (ot_s32)round((ot_double)x_sum / gyro_num) * gyro_data_range;
    drift[1] = (ot_s32)round((ot_double)y_sum / gyro_num) * gyro_data_range;
    drift[2] = (ot_s32)round((ot_double)z_sum / gyro_num) * gyro_data_range; /* index : 2 */
    printf("Multiply by range drift: %d, %d, %d\n", drift[0], drift[1], drift[2]); /* index : 2 */
    (ot_void)fclose(gyrodata_fd);
    return OT_SUCCESS;
}

static ot_void dis_send_set_gyro_info(dis_gyro_info *gyro_info, dis_send_gyro_srs_param *send_gyro_info,
    ot_eis_dis_pdt_type pdt_type)
{
    ot_s32 ret;
    for (int i = 0; i < OT_EIS_MFUSION_MATRIX_NUM; i++) {
        gyro_info->sum_mfusion_attr.six_side_calibration.matrix[i] =
            g_motionfusion_param.six_side_calibration.matrix[i];
    }
    gyro_info->sum_mfusion_attr.drift.enable = g_motionfusion_param.gyro_drift.enable;
    ret = dis_send_get_gyro_drift(send_gyro_info->gyrodata_file, DIS_SRS_GYRO_FILE_LENGTH,
        pdt_type, gyro_info->sum_mfusion_attr.drift.drift, OT_EIS_MFUSION_AXIS_NUM);
    if (ret != OT_SUCCESS) {
        dis_trace("dis_send_get_gyro_drift failed!\n");
    }
    gyro_info->is_open_mng_dev = OT_TRUE;
}

static ot_void dis_send_set_gyro_temp_drift(dis_gyro_info *gyro_info)
{
    gyro_info->sum_mfusion_attr.temperature_drift.enable = g_motionfusion_param.temp_drift.enable;
    gyro_info->sum_mfusion_attr.temperature_drift.temperature_lut = g_motionfusion_param.temp_drift.temperature_lut;
    gyro_info->sum_mfusion_attr.temperature_drift.mode = g_motionfusion_param.temp_drift.mode;
}

static ot_void dis_send_set_sns_param(dis_send_gyro_srs_param *send_gyro_info, ot_s32 hmax_time,
    ot_u32 height, ot_u32 frame_rate)
{
    send_gyro_info->sns_param.first_called_flag = OT_FALSE;
    send_gyro_info->sns_param.hmax_times = hmax_time;
    send_gyro_info->sns_param.read_out_time = (hmax_time * height) / 1000; /* second : 1000 */
    send_gyro_info->sns_param.frame_rate = frame_rate;
    send_gyro_info->sns_param.time_delay = g_motionfusion_param.dis_attr.timelag;
    send_gyro_info->is_exposure_time = OT_TRUE;
    send_gyro_info->vmax = g_vmax;
    return;
}

static ot_void dis_send_set_all_config(dis_send_info *dis_send_info)
{
    ot_size img_size = { 0 };
    ot_eis_dis_pdt_type pdt_type = dis_send_info->gyro_info.pdt_type;
    img_size.width = dis_send_info->pic_size.send_width;
    img_size.height = dis_send_info->pic_size.send_height;
    ot_u32 frame_rate = g_motionfusion_param.dis_cfg.frame_rate;
    ot_s32 timelag = g_motionfusion_param.dis_attr.timelag;

    dis_send_gyrodis_single_get_common_parm(dis_send_info, pdt_type); /* get default parameter */
    dis_send_info->vrb_info.vrb_size.width = img_size.width;
    dis_send_info->vrb_info.vrb_size.height = img_size.height;

    dis_vi_info *vi_info = &dis_send_info->vi_info[0];
    vi_info->vi_pipe[0] = g_vi_send_pipe;
    vi_info->vi_size.width = img_size.width;
    vi_info->vi_size.height = img_size.height;
    vi_info->isp_start_gyro = OT_TRUE; /* open vproc_gyro_dis */
    vi_info->is_send_pipe_yuv = OT_TRUE;
    dis_send_pipe_srs_info *send_srs_info = &vi_info->send_srs_info;
    if (strcpy_s(send_srs_info->file_name, sizeof(ot_char) * DIS_SRS_GYRO_FILE_LENGTH,
        DISGYRO_YUV_PATH"input_yuv.yuv") != EOK) {
        dis_trace("strcpy_s input_yuv.yuv failed!\n");
    }
    send_srs_info->send_cnt = dis_send_info->pic_size.frame_cnt;
    /* send gyro info */
    dis_send_gyro_srs_param *send_gyro_info = &vi_info->send_gyro_info;
    dis_send_set_sns_param(send_gyro_info, g_hmax, vi_info->vi_size.height, frame_rate);
    if (strcpy_s(send_gyro_info->pts_file, sizeof(ot_char) * DIS_SRS_GYRO_FILE_LENGTH,
        DISGYRO_YUV_PATH"timestamp.txt") != EOK) {
        dis_trace("strcpy_s timestamp.txt failed!\n");
    }
    if (strcpy_s(send_gyro_info->gyrodata_file, sizeof(ot_char) * DIS_SRS_GYRO_FILE_LENGTH,
        DISGYRO_YUV_PATH"gyrodata.txt") != EOK) {
        dis_trace("strcpy_s gyrodata.txt failed!\n");
    }

    if (strcpy_s(send_gyro_info->acc_file, sizeof(ot_char) * DIS_SRS_GYRO_FILE_LENGTH,
        DISGYRO_YUV_PATH"accdata.txt") != EOK) {
        dis_trace("strcpy_s accdata.txt failed!\n");
    }
    dis_vi_dis_attr *vi_dis_attr = &vi_info->vi_dis_attr;
    dis_gyro_info *gyro_info = &dis_send_info->gyro_info;
    dis_send_set_dis_attr(vi_dis_attr, timelag); /* set dis attr */
    dis_send_set_ldc_config(vi_dis_attr); /* set ldc config */
    dis_send_set_dis_config(vi_dis_attr, pdt_type); /* set dis config */
    dis_send_set_gyro_info(gyro_info, send_gyro_info, pdt_type); /* set gyro info: motionfusion, motionsensor */
    dis_send_set_gyro_temp_drift(gyro_info);
}

ot_s32 dis_send_get_gyro_buffer_malloc(ot_u32 buf_size)
{
    ot_s32 ret = OT_SUCCESS;
    ot_msensor_buf_attr buf_attr = { 0 };
    dis_test_ctx *dis_ctx = dis_get_test_ctx();
    ot_smr_alloc_attr smr_alloc_attr = { 0 };

    ret = memset_s(&dis_ctx->gyro_buf_attr, sizeof(ot_msensor_buf_attr), 0, sizeof(ot_msensor_buf_attr));
    if (ret != EOK) {
        dis_trace("memset_s failed!\n");
        return ret;
    }

    smr_alloc_attr.len = buf_size;
    smr_alloc_attr.cached = OT_FALSE;
    strcpy(smr_alloc_attr.region_name, "");
    ot_smr_init();
    ret = ot_smr_alloc(&smr_alloc_attr, (ot_phys_addr_t *)&buf_attr.phys_addr, (ot_void **)&buf_attr.virt_addr);
    if (ret != OT_SUCCESS) {
        dis_trace("alloc smr for Motionsensor failed,ret:%x !\n", ret);
        ret = OT_ERR_EIS_VI_NO_MEM;
        return ret;
    }

    ret = memset_s((ot_void *)(ot_uintptr_t)buf_attr.virt_addr, buf_size, 0, buf_size);
    if (ret != EOK) {
        dis_trace("memset_s failed!\n");
        return ret;
    }

    buf_attr.buf_len = buf_size;
    ret = memcpy_s(&dis_ctx->gyro_buf_attr, sizeof(ot_msensor_buf_attr), &buf_attr, sizeof(ot_msensor_buf_attr));
    if (ret != EOK) {
        dis_trace("memcpy_s failed!\n");
        return ret;
    }
    return ret;
}

ot_s32 dis_send_motionsensor_init(ot_msensor_param *param, ot_u32 buf_size, ot_bool is_open_mng_dev)
{
    ot_s32 ret = OT_FAILURE;
    g_dis_msensor_chip_dev = open("/dev/motionsensor_chip", O_RDWR);
    if (g_dis_msensor_chip_dev < 0) {
        dis_trace("Error: cannot open MotionSensor device.may not load motionsensor driver !\n");
        return OT_FAILURE;
    }
    if (is_open_mng_dev) {
        g_dis_msensor_mng_dev = open("/dev/motionsensor_mng", O_RDWR);
        if (g_dis_msensor_mng_dev < 0) {
            dis_trace("Error: cannot open MotionSensor device.may not load motionsensor driver !\n");
            goto exit;
        }
    }
    ret = dis_send_get_gyro_buffer_malloc(buf_size);
    if (ret != OT_SUCCESS) {
        dis_trace("malloc buffer failed \n");
        return ret;
    }

    dis_test_ctx *dis_ctx = dis_get_test_ctx();
    ret = memcpy_s(&param->buf_attr, sizeof(ot_msensor_buf_attr), &dis_ctx->gyro_buf_attr, sizeof(ot_msensor_buf_attr));
    if (ret != EOK) {
        dis_trace("memcpy_s failed\n");
        return -1;
    }

    ret = ioctl(g_dis_msensor_chip_dev, MSENSOR_CMD_INIT, param);
    if (ret != OT_SUCCESS) {
        dis_trace("MSENSOR_CMD_INIT\n");
        return -1;
    }
    return ret;
exit:
    close(g_dis_msensor_chip_dev);
//    g_dis_msensor_chip_dev = -1;
    return ret;
}

ot_s32 dis_send_motionfusion_init_param(ot_eis_dis_pdt_type pdt_type, dis_mfusion_attr *sum_mfusion_attr)
{
    ot_s32 ret;
    const ot_u32 fusion_id = 0;
    ret = ot_eis_mfusion_set_attr(fusion_id, &sum_mfusion_attr->mfusion_attr);
    if (ret != OT_SUCCESS) {
        goto end1;
    }
    ot_eis_mfusion_six_side_calibration *six_side_calibration = &sum_mfusion_attr->six_side_calibration;
    ret = ot_eis_mfusion_set_gyro_six_side_calibration(fusion_id, six_side_calibration);
    if (ret != OT_SUCCESS) {
        dis_trace("omi_motionfusion_set_gyro_six_side_cal failed! ret :%#x\n", ret);
        goto end1;
    }
    if (pdt_type == OT_EIS_DIS_PDT_TYPE_RECORDER) {
        dis_trace("pdt_type %d, use online drift !\n", pdt_type);
        ret = ot_eis_mfusion_set_gyro_online_drift(fusion_id, &sum_mfusion_attr->drift);
        if (ret != OT_SUCCESS) {
            dis_trace("omi_motionfusion_set_gyro_online_drift failed! ret :%#x\n", ret);
            goto end1;
        }
    } else if (pdt_type == OT_EIS_DIS_PDT_TYPE_DV) {
        dis_trace("pdt_type %d, use temperature drift !\n", pdt_type);
        ret = ot_eis_mfusion_set_gyro_online_temperature_drift(fusion_id, &sum_mfusion_attr->temperature_drift);
        if (ret != OT_SUCCESS) {
            dis_trace("ot_eis_mfusion_set_gyro_online_temperature_drift failed!\n");
            goto end1;
        }
    }

end1:
    return ret;
}

ot_s32 dis_send_gyro_buffer_free(ot_msensor_buf_attr *buf_attr)
{
    ot_s32 ret = OT_SUCCESS;
    if (buf_attr->virt_addr != (ot_u64)(ot_uintptr_t)NULL) {
        ret = ot_smr_free((ot_phys_addr_t)buf_attr->phys_addr);
        if (ret != OT_SUCCESS) {
            dis_trace("motionsensor smr free failed, ret:0x%x !\n", ret);
        }

        buf_attr->phys_addr = 0;
        buf_attr->virt_addr = (ot_u64)(ot_uintptr_t)NULL;
    }

    return ret;
}

ot_s32 dis_send_motionsensor_deinit(void)
{
    ot_s32 ret = OT_FAILURE;
    if (g_dis_msensor_chip_dev < 0) {
        dis_trace("g_dis_msensor_chip_dev < 0!\n");
        return OT_FAILURE;
    }

    ret = ioctl(g_dis_msensor_chip_dev, MSENSOR_CMD_DEINIT, NULL);
    if (ret != OT_SUCCESS) {
        dis_trace("motionsensor deinit failed , ret:0x%x !\n", ret);
    }

    dis_test_ctx *dis_ctx = dis_get_test_ctx();
    if (dis_ctx != OT_NULL) {
        dis_send_gyro_buffer_free(&dis_ctx->gyro_buf_attr);
    } else {
        dis_trace("dis_ctx is null !\n");
        ret = OT_FAILURE;
    }

    close(g_dis_msensor_chip_dev);
    g_dis_msensor_chip_dev = -1;

    if (g_dis_msensor_mng_dev >= 0) {
        close(g_dis_msensor_mng_dev);
        g_dis_msensor_mng_dev = -1;
    }
    dis_trace("dis_send_motionsensor_deinit success !\n");

    return ret;
}

ot_s32 dis_send_gyrodis_init_gyro(dis_gyro_info *gryo_info)
{
    ot_s32 ret;
    ret = dis_send_motionsensor_init(&gryo_info->msensor_param, gryo_info->buffer_size, gryo_info->is_open_mng_dev);
    if (ret != OT_SUCCESS) {
        dis_trace("init gyro fail.ret:0x%x !\n", ret);
        return ret;
    }
    ret = dis_send_motionfusion_init_param(gryo_info->pdt_type, &gryo_info->sum_mfusion_attr);
    if (ret != OT_SUCCESS) {
        dis_trace("motionfusion set param fail.ret:0x%x !\n", ret);
        goto motionfusion_init_fail;
    }
    dis_trace("motionfusion and motionsensor start success !\n");
    return OT_SUCCESS;

motionfusion_init_fail:
    dis_send_motionsensor_deinit();
    return ret;
}

ot_s32 dis_send_vi_set_chn_dis_cfg(ot_vi_pipe vi_pipe, ot_vi_chn vi_chn, ot_eis_dis_cfg *dis_cfg, ot_eis_dis_attr *dis_attr)
{
    ot_s32 ret;
    ot_vi_pipe_handle vi_pipe_hdl;
    ot_vi_chn_handle vi_chn_hdl;

    ret = ot_eis_vi_get_pipe_hdl_by_pipe_id(vi_pipe, &vi_pipe_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vi_get_pipe_hdl_by_pipe_id failed with %#x\n", ret);
        return OT_FAILURE;
    }

    ret = ot_eis_vi_get_chn_hdl_by_ids(vi_pipe, vi_chn, &vi_chn_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vi_get_chn_hdl_by_ids failed with %#x\n", ret);
        return OT_FAILURE;
    }

    ret = ot_eis_vi_set_chn_dis_cfg(vi_pipe_hdl, vi_chn_hdl, dis_cfg);
    if (ret != OT_SUCCESS) {
        dis_trace("set dis config failed.ret:0x%x !\n", ret);
        return ret;
    }

    ret = ot_eis_vi_set_chn_dis_attr(vi_pipe_hdl, vi_chn_hdl, dis_attr);
    if (ret != OT_SUCCESS) {
        dis_trace("set dis attr failed.ret:0x%x !\n", ret);
        return ret;
    }

    return OT_SUCCESS;
}

ot_s32 dis_send_set_vi_pipe_dis_chn_attr(dis_vi_info vi_info[])
{
    ot_s32 ret;
    ret = dis_send_vi_set_chn_dis_cfg(vi_info[0].vi_pipe[0], 0, &vi_info[0].vi_dis_attr.dis_config,
                                      &vi_info[0].vi_dis_attr.dis_attr);
    if (ret != OT_SUCCESS) {
        dis_trace("dis_set_vi_pipe_dis_chn_attr faild!!!\n");
        return ret;
    }
    return OT_SUCCESS;
}

static dis_pthread_id dis_send_get_free_pthread_info(ot_void)
{
    ot_s32 i;

    for (i = 0; i < DIS_MAX_PTHREAD_NUM; i++) {
        dis_pthread_info *pthread_info = dis_get_pthread_info(i);
        if (pthread_info->created == OT_FALSE) {
            return i;
        }
    }
    return DIS_INVALID_PTHREAD_ID;
}

dis_pthread_id dis_pthread_create(fn_dis_pthread_proc_func proc_fun, ot_void *param)
{
    ot_s32 ret;
    dis_pthread_id pthread_id;
    dis_pthread_info *pthread_info;

    if (proc_fun == NULL || param == NULL) {
        dis_trace("null pointer!\n");
        return DIS_INVALID_PTHREAD_ID;
    }

    pthread_id = dis_send_get_free_pthread_info();
    if (pthread_id == DIS_INVALID_PTHREAD_ID) {
        dis_trace("aiisp pthread num is full!\n");
        return DIS_INVALID_PTHREAD_ID;
    }

    pthread_info = dis_get_pthread_info(pthread_id);

    pthread_info->pthread_param.id = pthread_id;
    pthread_info->pthread_param.param = param;
    pthread_info->pthread_param.thread_err = OT_FALSE;
    pthread_info->created = OT_TRUE;
    pthread_info->start = OT_TRUE;
    ret = pthread_create(&pthread_info->thread_id, OT_NULL, proc_fun, (ot_void *)&pthread_info->pthread_param);
    if (ret != OT_SUCCESS) {
        dis_trace("aiisp create pthread failed!\n");
        pthread_info->created = OT_FALSE;
        pthread_info->start = OT_FALSE;
        return DIS_INVALID_PTHREAD_ID;
    }

    return pthread_id;
}

ot_bool dis_send_is_pthread_start(dis_pthread_id pthread_id)
{
    volatile dis_pthread_info *pthread_info = dis_get_pthread_info((ot_s32)pthread_id);
    return pthread_info->start;
}

ot_bool dis_check_acc_data_temp_state(ot_char *accdata_filename, ot_s32 file_len)
{
    ot_s32 x, y, z;
    ot_u64 tmp;
    ot_bool is_have_acc = OT_FALSE;
    FILE *send_acc_fd = OT_NULL;
    ot_char path[PATH_MAX] = {0};
    if (file_len > PATH_MAX) {
        dis_trace("file_len is too long,it is invalid!.\n");
        return OT_FALSE;
    }
    if (realpath(accdata_filename, path) == OT_NULL) {
        dis_trace("accdata_filename realpath err, invalid file path!.\n");
        return OT_FALSE;
    }
    send_acc_fd = fopen(path, "r");
    if (send_acc_fd == OT_NULL) {
        dis_trace("open acc failed\n");
    }
    if (fscanf_s(send_acc_fd, "%d,%d,%d,%llu", &x, &y, &z, &tmp) == EOF) {
            dis_trace("fscanf_s from send_acc_fd failed\n");
    }
    if (tmp > 1000000) { /* tmp_max : 1000000 */
        is_have_acc = OT_FALSE;
    } else {
        is_have_acc = OT_TRUE;
    }

    dis_check_fd_closed(send_acc_fd);
    return is_have_acc;
}

ot_s32 dis_send_prepare_gryo_file_fd(dis_send_gyro_srs_param *send_gyro_info, ot_bool *is_have_acc)
{
    ot_char path[PATH_MAX] = {0};
    if (send_gyro_info == OT_NULL) {
        return OT_FAILURE;
    }

    dis_trace("pts file %s\n", send_gyro_info->pts_file);
    dis_trace("gyro file %s\n", send_gyro_info->gyrodata_file);
    dis_trace("acc file %s\n", send_gyro_info->acc_file);
    if (realpath(send_gyro_info->pts_file, path) == OT_NULL) {
        dis_trace("pts_file realpath err, invalid file path!.\n");
        return OT_FAILURE;
    }
    g_send_pts_fd = fopen(path, "r");
    if (g_send_pts_fd == OT_NULL) {
        dis_trace("open pts failed\n");
        return OT_FAILURE;
    }
    if (realpath(send_gyro_info->gyrodata_file, path) == OT_NULL) {
        dis_trace("gyrodata_file realpath err, invalid file path!.\n");
        goto exit2;
    }
    g_send_gyrodata_fd = fopen(path, "r");
    if (g_send_gyrodata_fd == OT_NULL) {
        dis_trace("open g_send_gyrodata_fd failed\n");
        goto exit2;
    }
    *is_have_acc = dis_check_acc_data_temp_state(send_gyro_info->acc_file, DIS_SRS_GYRO_FILE_LENGTH);
    if (*is_have_acc) {
        dis_trace("check acc tempature%d !\n", *is_have_acc);
    }
    if (realpath(send_gyro_info->acc_file, path) == OT_NULL) {
        dis_trace("acc_file realpath err, invalid file path!.\n");
        goto exit1;
    }
    g_send_acc_fd = fopen(path, "r");
    if (g_send_acc_fd == OT_NULL) {
        dis_trace("open acc failed\n");
        goto exit1;
    }
    return OT_SUCCESS;

exit1:
    dis_check_fd_closed(g_send_gyrodata_fd);
exit2:
    dis_check_fd_closed(g_send_pts_fd);
    return OT_FAILURE;
}

ot_s32 dis_send_pipe_prepare_file_fd(ot_char *file_name, dis_send_gyro_srs_param *send_gyro_info,
                                     ot_bool *is_have_acc_tmp)
{
    ot_s32 ret;
    ot_char path[PATH_MAX] = {0};
    dis_trace("reading frame file: %s \n", file_name);
    if (realpath(file_name, path) == OT_NULL) {
        dis_trace("file_name realpath err, invalid file path!.\n");
        return OT_FAILURE;
    }

    g_dis_send_srs_fd = fopen(path, "rb");
    if (!g_dis_send_srs_fd) {
        dis_trace("open file \"%s\" failed!\n", file_name);
        return OT_FAILURE;
    }

    ret = dis_send_prepare_gryo_file_fd(send_gyro_info, is_have_acc_tmp);
    dis_err_void_goto(ret, "dis_send_prepare_gryo_file_fd", OT_SUCCESS, exit1);
    
    return OT_SUCCESS;

exit1:
    dis_check_fd_closed(g_dis_send_srs_fd);
    return OT_FAILURE;
}

ot_void dis_send_set_img_attr(ot_eis_img_attr *img_attr, dis_vi_get_frame_vrb_cfg *get_frame_vrb_cfg)
{
    ot_eis_img_pixel_bit_depth bit_width;
    bit_width = (get_frame_vrb_cfg->dynamic_range == OT_DYNAMIC_RANGE_SDR8) ? OT_EIS_PIXEL_BIT_DEPTH_8 : OT_EIS_PIXEL_BIT_DEPTH_10;
    img_attr->bit_depth     = bit_width;
    img_attr->width         = get_frame_vrb_cfg->size.width;
    img_attr->height        = get_frame_vrb_cfg->size.height;
    img_attr->pixel_fmt     = get_frame_vrb_cfg->pixel_format;
    img_attr->layout        = get_frame_vrb_cfg->video_format;
    img_attr->compress_mode = get_frame_vrb_cfg->compress_mode;
}

static ot_void sample_comm_vi_free_frame_blk(ot_eis_handle pool_hdl, sample_vi_user_frame_info *user_frame_info)
{
    ot_s32 ret;
    ot_u32 blk_size = user_frame_info->blk_size;
    ot_void *virt_addr = user_frame_info->frame_info.buff.virt_addr[0];

    ret = ot_smr_munmap(virt_addr, blk_size);
    if (ret != OT_SUCCESS) {
        sample_print("ot_smr_munmap failure!\n");
    }

    ret = ot_buffer_pool_blk_release(pool_hdl, &user_frame_info->pool_blk);
    if (ret != OT_SUCCESS) {
        sample_print("ot_buffer_pool_blk_release block failure\n");
    }

    user_frame_info->pool_blk.phys_addr = 0;
    user_frame_info->pool_blk.block_size = 0;
}

static ot_s32 dis_send_convert_chroma_planar_to_sp42x(FILE *file, ot_u8 *chroma_data, ot_u32 luma_stride,
    ot_u32 chroma_width, ot_u32 chroma_height)
{
    ot_u32 chroma_stride = luma_stride >> 1;
    ot_u8 *dst = OT_NULL;
    ot_u32 row;
    ot_u32 list;
    ot_u8 *temp = OT_NULL;

    temp = (ot_u8 *)malloc(chroma_stride);
    if (temp == OT_NULL) {
        dis_trace("vi malloc failed!\n");
        return OT_FAILURE;
    }
    if (memset_s(temp, chroma_stride, 0, chroma_stride) != EOK) {
        dis_trace("vi memset_s failed!\n");
        free(temp);
        temp = OT_NULL;
        return OT_FAILURE;
    }

    /* U */
    printf("U----");
    dst = chroma_data + 1;
    for (row = 0; row < chroma_height; ++row) {
        (ot_void)fread(temp, chroma_width, 1, file); /* sp420 U-component data starts 1/2 way from the beginning */
        for (list = 0; list < chroma_stride; ++list) {
            *dst = *(temp + list);
            dst += 2; /* traverse 2 steps away to the next U-component data */
        }
        dst = chroma_data + 1;
        dst += (row + 1) * luma_stride;
    }

    /* V */
    printf("V----");
    dst = chroma_data;
    for (row = 0; row < chroma_height; ++row) {
        (ot_void)fread(temp, chroma_width, 1, file); /* sp420 V-component data starts 1/2 way from the beginning */
        for (list = 0; list < chroma_stride; ++list) {
            *dst = *(temp + list);
            dst += 2; /* traverse 2 steps away to the next V-component data */
        }
        dst = chroma_data;
        dst += (row + 1) * luma_stride;
    }

    free(temp);

    return OT_SUCCESS;
}

static ot_s32 dis_send_read_yuv_from_file(FILE *file, ot_eis_video_frame *frame)
{
    ot_u8 *luma = (ot_u8 *)(ot_uintptr_t)frame->buff.virt_addr[0];
    ot_u8 *chroma = (ot_u8 *)(ot_uintptr_t)frame->buff.virt_addr[1];
    ot_u32 luma_width = frame->attr.width;
    ot_u32 chroma_width = luma_width >> 1;
    ot_u32 luma_height = frame->attr.height;
    ot_u32 chroma_height = luma_height;
    ot_u32 luma_stride = frame->buff.stride[0];

    ot_u8 *dst = OT_NULL;
    ot_u32 row;
    if (frame->attr.layout == OT_EIS_IMAGE_LAYOUT_LINEAR) {
        /* Y */
        printf("read Y----");
        dst = luma;
        for (row = 0; row < luma_height; ++row) {
            fread(dst, luma_width, 1, file);
            dst += luma_stride;
        }
        if (frame->attr.pixel_fmt == OT_EIS_IMAGE_FORMAT_YUV_400) {
            return OT_SUCCESS;
        } else if (frame->attr.pixel_fmt == OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR) {
            chroma_height = chroma_height >> 1;
        }
        if (dis_send_convert_chroma_planar_to_sp42x(file, chroma, luma_stride, chroma_width, chroma_height) !=
            OT_SUCCESS) {
            return OT_FAILURE;
        }
    } else {
        fread(luma, luma_stride * luma_height * 3 / 2, 1, file); /* Tile 64x16 size = stride x height * 3 / 2 */
    }
    printf("done!\n");
    return OT_SUCCESS;
}

ot_s32 dis_send_read_frame_file(sample_vi_user_frame_info user_frame_info[], ot_s32 frame_cnt)
{
    ot_s32 i;

    for (i = 0; i < frame_cnt; i++) {
        ot_eis_video_frame *video_frame = &user_frame_info[i].frame_info;

        if ((video_frame->attr.pixel_fmt == OT_EIS_IMAGE_FORMAT_YVU_422_SEMIPLANAR) ||
            (video_frame->attr.pixel_fmt == OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR) ||
            (video_frame->attr.pixel_fmt == OT_EIS_IMAGE_FORMAT_YUV_400)) {
            dis_send_read_yuv_from_file(g_dis_send_srs_fd, video_frame);
        } else {
            dis_trace("unsupport pixel format (%d)!\n", video_frame->attr.pixel_fmt);
        }
    }

    return OT_SUCCESS;
}

ot_void dis_send_get_imu_pts_range(ot_u64 frame_pts, ot_u32 exposure_time, ot_u64 *begin_pts, ot_u64 *end_pts,
    dis_send_sns_param *sns_param)
{
    ot_s32 rs_dir = (sns_param->hmax_times >= 0) ? DIS_UP_TO_DOWN : DIS_DOWN_TO_UP;
    ot_u32 gyro_data_gap = 1000; /* data_gap : 1000 */
    ot_s32 time_delay = sns_param->time_delay - (exposure_time / 2);
    static ot_u64 pre_end_pts;
    frame_pts += time_delay;
    dis_trace("-----time_delay %d\n", time_delay);

    if (sns_param->first_called_flag == 0) {
        if (DIS_UP_TO_DOWN == rs_dir) {
            *begin_pts = frame_pts;
            *end_pts = frame_pts + sns_param->read_out_time;
        } else {
            *begin_pts = frame_pts + sns_param->read_out_time;
            *end_pts = frame_pts;
        }
        sns_param->first_called_flag = 1;
    } else {
        if (DIS_UP_TO_DOWN == rs_dir) {
            *begin_pts = pre_end_pts;
            *end_pts = frame_pts + sns_param->read_out_time;
        } else {
            *begin_pts = pre_end_pts;
            *end_pts = frame_pts;
        }
    }
    pre_end_pts = *end_pts;
    *begin_pts = *begin_pts - 2 * gyro_data_gap ; /* double : 2 */
    *end_pts = *end_pts + 2 * (ot_s64)gyro_data_gap; /* double : 2 */
}

ot_s32 dis_send_try_send_user_gyro_data(ot_u64 end_pts, ot_msensor_data *usr_data, ot_bool is_have_acc_tmp)
{
    ot_s32 x, y, z, tmp;
    ot_u64 pts;
    ot_s32 ret = OT_SUCCESS;
    ot_s32 i = 0;
    if (g_first_time_to_read != OT_TRUE) {
        return OT_SUCCESS;
    }
    while (1) {
        if (fscanf_s(g_send_gyrodata_fd, "%d,%d,%d,%d,%llu\r", &x, &y, &z, &tmp, &pts) == EOF) {
            dis_trace("read g_send_gyrodata_fd failed !\n");
            return OT_FAILURE;
        }

        usr_data->gyro_buf.data_num = 1;
        usr_data->gyro_buf.gyro_data[i].x = x;
        usr_data->gyro_buf.gyro_data[i].y = y;
        usr_data->gyro_buf.gyro_data[i].z = z;
        usr_data->gyro_buf.gyro_data[i].temperature = tmp;
        usr_data->gyro_buf.gyro_data[i].pts = pts;
        if (is_have_acc_tmp) {
            if (fscanf_s(g_send_acc_fd, "%d,%d,%d,%d,%llu\r", &x, &y, &z, &tmp, &pts) == EOF) {
                dis_trace("read g_send_acc_fd failed, %d,%d,%d,%d,%llu \n", x, y, z, tmp, pts);
                return OT_FAILURE;
            }
        } else {
            if (fscanf_s(g_send_acc_fd, "%d,%d,%d,%llu\r", &x, &y, &z, &pts) == EOF) {
                dis_trace("read g_send_gyrodata_fd failed, %d,%d,%d,%llu!\n", x, y, z, pts);
                return OT_FAILURE;
            }
            tmp = usr_data->gyro_buf.gyro_data[i].temperature;
        }

        usr_data->acc_buf.data_num = 1;
        usr_data->acc_buf.acc_data[i].x = x;
        usr_data->acc_buf.acc_data[i].y = y;
        usr_data->acc_buf.acc_data[i].z = z;
        usr_data->acc_buf.acc_data[i].temperature = tmp;
        usr_data->acc_buf.acc_data[i].pts = pts;

        ret = ioctl(g_dis_msensor_mng_dev, MSENSOR_CMD_SEND_DATA, &usr_data); // motionsensor_write_data_to_mng_buff
        if (ret != OT_SUCCESS) {
            dis_trace("g_dis_msensor_mng_dev %d, send gyro data failed %#x. i %d, %d, %d,%d,%d,%llu \n",
                g_dis_msensor_mng_dev, ret, i, x, y, z, tmp, pts);
        }

        g_first_time_to_read = OT_FALSE;
        if (pts > end_pts) {
            break;
        }
    }
    return ret;
}

ot_s32 dis_send_user_gyro_data(ot_u64 prt_pts, ot_u32 exposure_time, dis_send_sns_param *sns_param,
    ot_bool is_have_acc_tmp)
{
    ot_s32 ret = OT_SUCCESS;
    ot_s32 i = 0;
    ot_msensor_data usr_data; // msensor_data
    ot_u64 begin_pts, end_pts;
    if (memset_s(&usr_data, sizeof(ot_msensor_data), 0, sizeof(ot_msensor_data)) != EOK) {
        dis_trace("memset_s failed!!!\n");
    }
    usr_data.gyro_buf.data_num = 0;
    usr_data.attr.device_mask = OT_MSENSOR_DEVICE_GYRO | OT_MSENSOR_DEVICE_ACC;
    usr_data.attr.temperature_mask = OT_MSENSOR_TEMP_GYRO | OT_MSENSOR_TEMP_ACC;

    dis_send_get_imu_pts_range(prt_pts, exposure_time, &begin_pts, &end_pts, sns_param);
    if (dis_send_try_send_user_gyro_data(end_pts, &usr_data, is_have_acc_tmp) != OT_SUCCESS) {
        return ret;
    }
    while (i < OT_MSENSOR_MAX_DATA_NUM) {
        if (fscanf_s(g_send_gyrodata_fd, "%d,%d,%d,%d,%llu\r", (int *)&usr_data.gyro_buf.gyro_data[i].x,
            (int *)&usr_data.gyro_buf.gyro_data[i].y, (int *)&usr_data.gyro_buf.gyro_data[i].z,
            (int *)&usr_data.gyro_buf.gyro_data[i].temperature, &usr_data.gyro_buf.gyro_data[i].pts) == EOF) {
            break;
        }

        if (is_have_acc_tmp) {
            if (fscanf_s(g_send_acc_fd, "%d,%d,%d,%d,%llu\r", (int *)&usr_data.acc_buf.acc_data[i].x,
                (int *)&usr_data.acc_buf.acc_data[i].y, (int *)&usr_data.acc_buf.acc_data[i].z,
                (int *)&usr_data.acc_buf.acc_data[i].temperature, &usr_data.acc_buf.acc_data[i].pts) == EOF) {
                break;
            }
        } else {
            if (fscanf_s(g_send_acc_fd, "%d,%d,%d,%llu\r", (int *)&usr_data.acc_buf.acc_data[i].x,
                (int *)&usr_data.acc_buf.acc_data[i].y, (int *)&usr_data.acc_buf.acc_data[i].z,
                &usr_data.acc_buf.acc_data[i].pts) == EOF) {
                break;
            }
            usr_data.acc_buf.acc_data[i].temperature = usr_data.gyro_buf.gyro_data[i].temperature;
        }

        i++;

        if (usr_data.gyro_buf.gyro_data[i - 1].pts > end_pts) {
            break;
        }
    }
    if (i > 0) {
        usr_data.gyro_buf.data_num = i;
        usr_data.acc_buf.data_num = i;
        ret = ioctl(g_dis_msensor_mng_dev, MSENSOR_CMD_SEND_DATA, &usr_data); // motionsensor_write_data_to_mng_buff
        if (ret != OT_SUCCESS) {
            dis_trace("send gyro data failed.\n");
        }
    }
    return ret;
}

ot_s32 dis_send_gyro_send_one_frame_user_data(dis_send_gyro_srs_param *send_gyro_info, ot_u64 *pts,
    ot_u32 *exposure_time, ot_bool is_have_acc_tmp)
{
    ot_s32 ret;
    ot_bool is_exposure_time = send_gyro_info->is_exposure_time;
    ot_u64 tmp_pts = 0;
    ot_u32 expor_time = 0;

    if (is_exposure_time) {
        if (fscanf_s(g_send_pts_fd, "%u,%llu\r", &expor_time, &tmp_pts) == EOF) {
            dis_trace("read frm_pts_file failed !\n");
            return OT_FAILURE;
        }
    } else {
        *exposure_time = 0;
        if (fscanf_s(g_send_pts_fd, "%llu\r", &tmp_pts) == EOF) {
            dis_trace("read frm_pts_file failed !\n");
            return OT_FAILURE;
        }
    }

    ret = dis_send_user_gyro_data(tmp_pts, expor_time, &send_gyro_info->sns_param, is_have_acc_tmp);
    if (ret  == OT_FAILURE) {
        dis_trace("send user gryo data is failed !!!\n");
        return OT_FAILURE;
    }
    *pts = tmp_pts;
    *exposure_time = expor_time;

    return OT_SUCCESS;
}

ot_s32 dis_send_release_frame_blk(sample_vi_user_frame_info user_frame_info[], ot_s32 frame_cnt)
{
    ot_s32 ret = OT_SUCCESS;
    ot_s32 i;
    ot_eis_handle pool_hdl;
    ot_buffer_pool_blk pool_blk;

    for (i = 0; i < frame_cnt; i++) {
        ot_u32 blk_size = user_frame_info[i].blk_size;
        ot_void *virt_addr = user_frame_info[i].frame_info.buff.virt_addr[0];
        ret = ot_smr_munmap(virt_addr, blk_size);
        if (ret != OT_SUCCESS) {
            dis_trace("ot_smr_munmap failure!\n");
        }

        pool_hdl = user_frame_info[i].frame_info.pool_hdl;
        pool_blk = user_frame_info[i].pool_blk;
        ret = ot_buffer_pool_blk_release(pool_hdl, &pool_blk);
        if (ret != OT_SUCCESS) {
            dis_trace("ot_buffer_pool_blk_release failed\n");
        }
    }
    return ret;
}

ot_s32 dis_send_gyro_prepare_isp_frameinfo(ot_void **virt_addr, ot_isp_frame_info *isp_frame_info, ot_buffer_pool_blk vrb_blk)
{
    ot_s32 ret = OT_SUCCESS;
    ot_eis_img_extra_attr supplement_temp = { 0 };

    ret = ot_buffer_pool_get_supplement_addr(&vrb_blk, &supplement_temp);
    if (OT_SUCCESS != ret) {
        dis_trace("ot_buffer_pool_get_supplement_addr failed ,return 0x%x\n", ret);
        return ret;
    }

    ret = ot_smr_mmap(supplement_temp.isp_attr_phys_addr, sizeof(ot_isp_frame_info), OT_TRUE, virt_addr);
    if (*virt_addr == OT_NULL) {
        dis_trace("Info:ot_omi_sys_mmap failded \n");
        return ret;
    }

    if (memcpy_s(*virt_addr, sizeof(ot_isp_frame_info), isp_frame_info, sizeof(ot_isp_frame_info)) != EOK) {
        dis_trace("memcpy_s failded \n");
        return ret;
    }

    return ret;
}

static ot_s32 dis_send_pipe_frame(dis_send_pipe_info *send_info, ot_eis_img_pixel_format pixel_format,
                                  const ot_eis_video_frame *frame_info[], ot_s32 frame_num)
{
    ot_s32 ret = OT_SUCCESS;
    ot_vi_pipe vi_pipe = send_info->vi_pipe;
    ot_s32 milli_sec = send_info->milli_sec;
    ot_eis_handle vi_pipe_hdl;
    ot_eis_video_frame frame_array[OT_VI_MAX_WDR_FRAME_NUM] = { 0 };

    ret = ot_eis_vi_get_pipe_hdl_by_pipe_id(vi_pipe, &vi_pipe_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vi_get_pipe_hdl_by_pipe_id failed with %#x\n", ret);
        return OT_FAILURE;
    }

#ifdef DIS_SEND_FRAME_RATE_RESTRICT
    usleep(1000000 / DIS_SEND_FRAME_RATE_RESTRICT); /* limit the speed of send frame : 1000000 */
#endif
    for (ot_u32 i = 0; i < frame_num; i++) {
        frame_array[i] = *(frame_info[i]);
    }

    ret = ot_eis_vi_pipe_send_frame(vi_pipe_hdl, frame_array, frame_num, milli_sec);
    if (ret != OT_SUCCESS) {
        dis_trace("ot_eis_vi_pipe_send_frame failed!\n");
        return ret;
    }

    return ret;
}

ot_void dis_check_all_fd_closed(ot_void)
{
    dis_check_fd_closed(g_send_pts_fd);
    dis_check_fd_closed(g_send_gyrodata_fd);
    dis_check_fd_closed(g_send_acc_fd);
    dis_check_fd_closed(g_dis_send_srs_fd);
}

static ot_s32 sample_comm_vi_malloc_frame_blk(ot_eis_handle pool_hdl, ot_eis_img_attr *img_attr,
                                              ot_eis_buf_size_calc_cfg *calc_cfg, sample_vi_user_frame_info *user_frame_info)
{
    ot_s32 ret;
    ot_phys_addr phys_addr;
    ot_buffer_pool_blk blk;
    ot_void *virt_addr = OT_NULL;
    ot_eis_video_frame *frame_info = OT_NULL;

    ret = ot_buffer_pool_blk_acquire(pool_hdl, calc_cfg->blk_size, &blk);
    if (ret != OT_SUCCESS) {
        sample_print("ot_buffer_pool_blk_acquire err, size:%u\n", calc_cfg->blk_size);
        return OT_FAILURE;
    }

    phys_addr = blk.phys_addr;
    ret = ot_smr_mmap(phys_addr, calc_cfg->blk_size, OT_FALSE, &virt_addr);
    if (ret != OT_SUCCESS) {
        sample_print("ot_smr_mmap err!\n");
        ot_buffer_pool_blk_release(pool_hdl, &blk);
        return OT_FAILURE;
    }

    memcpy_s(&user_frame_info->pool_blk, sizeof(ot_buffer_pool_blk), &blk, sizeof(ot_buffer_pool_blk));
    user_frame_info->blk_size = calc_cfg->blk_size;

    frame_info = &user_frame_info->frame_info;

    frame_info->pool_hdl           = pool_hdl;
    frame_info->mod_id             = OT_EIS_MODULE_VI;
    frame_info->buff.phys_addr[0]  = phys_addr;
    frame_info->buff.phys_addr[1]  = frame_info->buff.phys_addr[0] + calc_cfg->main_y_size;
    frame_info->buff.virt_addr[0]  = virt_addr;
    frame_info->buff.virt_addr[1]  = frame_info->buff.virt_addr[0] + calc_cfg->main_y_size;
    frame_info->buff.stride[0]     = calc_cfg->main_stride;
    frame_info->buff.stride[1]     = calc_cfg->main_stride;
    frame_info->attr.width         = img_attr->width;
    frame_info->attr.height        = img_attr->height;
    frame_info->attr.pixel_fmt     = img_attr->pixel_fmt;
    frame_info->attr.layout        = img_attr->layout;
    frame_info->attr.compress_mode = img_attr->compress_mode;
    frame_info->color_space        = OT_EIS_IMAGE_COLOR_SPACE_BT601;

    return OT_SUCCESS;
}

ot_void *dis_send_pipe_frame_thread(ot_void *param)
{
    ot_s32 ret, i, i0, j;
    ot_u32 frame_num = 1;
    ot_u32 frame_cnt;
    ot_u32 send_cnt = 0;
    ot_u64 pts = 0;
    ot_u32 exposure_time = 0;
    ot_bool is_have_acc_tmp = OT_FALSE;
    dis_send_pipe_info send_pipe_frame_info = { 0 };
    sample_vi_user_frame_info *user_frame_info = OT_NULL;
    ot_isp_frame_info isp_frame_info = { 0 };
    const ot_eis_video_frame *frame_info[OT_VI_MAX_WDR_FRAME_NUM];
    dis_pthread_param *pthread_param = (dis_pthread_param *)param;
    dis_test_ctx *dis_ctx = dis_get_test_ctx();
    g_first_time_to_read = OT_TRUE;
    ot_eis_handle pool_hdl;
    ot_video_buffer_attr pool_attr = { 0 };
    ot_eis_buf_size_calc_cfg calc_cfg = { 0 };
    ot_eis_img_attr img_attr = { 0 };

    if (memcpy_s(&send_pipe_frame_info, sizeof(dis_send_pipe_info), pthread_param->param, sizeof(dis_send_pipe_info)) != EOK) {
        dis_trace("memcpy_s failed!!!\n");
    }
    dis_send_gyro_srs_param *send_gyro_info = &send_pipe_frame_info.send_gyro_info;
    frame_cnt = send_pipe_frame_info.frame_cnt;
    printf("frame_cnt : %u\n", frame_cnt);

    printf("thread ------:need send cnt %u \n", frame_cnt);
    user_frame_info = (sample_vi_user_frame_info *)malloc(frame_num * sizeof(sample_vi_user_frame_info));
    if (user_frame_info == OT_NULL) {
        dis_trace("malloc sample_vi_user_frame_info failed!\n");
        return OT_NULL;
    }
    ret = dis_send_pipe_prepare_file_fd(send_pipe_frame_info.file_name, send_gyro_info, &is_have_acc_tmp);
    dis_err_void_goto(ret, "vi_set_pipe_frame_source", OT_SUCCESS, prepare_file_fd_failed);

    dis_send_set_img_attr(&img_attr, &send_pipe_frame_info.get_frame_vrb_cfg);
    sample_common_get_pic_buf_cfg(&img_attr, &calc_cfg);

    ret = ot_buffer_pool_init();
    if (ret != OT_SUCCESS) {
        printf("buffer_pool init fail! ret[%#x]\n", ret);
        goto pool_init_failed;
    }

    pool_attr.cnt = 1;
    pool_attr.buf_blks[0].cnt = 30;
    pool_attr.buf_blks[0].size = calc_cfg.blk_size;
    printf("cnt = %d, size = %d\n", frame_cnt, calc_cfg.blk_size);
    ret = ot_buffer_pool_create(&pool_hdl, &pool_attr);
    if (ret != OT_SUCCESS){
       printf("buffer_pool create fail! ret[%#x]\n", ret);
       goto pool_create_failed;
    }

    ot_u32 time_ref = 0;
    dis_trace("------------------------send_gyro_info->vmax %u\n", send_gyro_info->vmax);
    while (send_cnt < frame_cnt) {
        printf("send_cnt: %u frame_cnt : %u\n", send_cnt, frame_cnt);
        if (dis_send_is_pthread_start(pthread_param->id) == OT_FALSE) {
            goto not_pthread_start;
        }

        if (memset_s(user_frame_info, frame_num * sizeof(sample_vi_user_frame_info), 0,
            frame_num * sizeof(sample_vi_user_frame_info)) != EOK) {
            dis_trace("memset_s failed\n");
        }

        for (i0 = 0; i0 < frame_num; i0++) {
            ret = sample_comm_vi_malloc_frame_blk(pool_hdl, &img_attr, &calc_cfg, &user_frame_info[i0]);
            if (ret != OT_SUCCESS) {
                goto malloc_blk_failed;
            }
        }

        ret = dis_send_read_frame_file(user_frame_info, frame_num);
        if (ret != OT_SUCCESS) {
            goto malloc_blk_failed;
        }

        send_pipe_frame_info.is_send_gyro = OT_TRUE;
        if (send_pipe_frame_info.is_send_gyro) {
            ret = dis_send_gyro_send_one_frame_user_data(send_gyro_info, &pts, &exposure_time, is_have_acc_tmp);
            if (ret != OT_SUCCESS) {
                dis_trace("dis_gyro_send_one_frame_user_data failed, send_cnt : %u\n", send_cnt);
                goto malloc_blk_failed;
            }
        }
        for (i = 0; i < frame_num; i++) {
            user_frame_info[i].frame_info.pts = pts;
            frame_info[i] = &user_frame_info[i].frame_info;
        }

        ot_void *virt_addr = OT_NULL;
        isp_frame_info.exposure_time[0] = exposure_time;
        isp_frame_info.hmax_times = send_gyro_info->sns_param.hmax_times;
        isp_frame_info.vmax = send_gyro_info->vmax;
        ret = dis_send_gyro_prepare_isp_frameinfo(&virt_addr, &isp_frame_info, user_frame_info[0].pool_blk);
        if (ret != OT_SUCCESS) {
            dis_trace("dis_gyro_prepare_isp_frameinfo failed with %#x!\n", ret);
            goto malloc_blk_failed;
        }

        ot_eis_img_pixel_format pixel_format = send_pipe_frame_info.get_frame_vrb_cfg.pixel_format;
        ret = dis_send_pipe_frame(&send_pipe_frame_info, pixel_format, frame_info, frame_num);
        if (ret != OT_SUCCESS) {
            dis_trace("vi send pipe frame failed with %#x!\n", ret);
            ot_smr_munmap(virt_addr, sizeof(ot_isp_frame_info));
            continue;
        }

        ret = ot_smr_munmap(virt_addr, sizeof(ot_isp_frame_info));
        if (ret != OT_SUCCESS) {
            dis_trace("ot_smr_munmap failed %u \n", send_cnt);
        }

        for (j = i0 - 1; j >= 0; j--) {
            sample_comm_vi_free_frame_blk(pool_hdl, &user_frame_info[j]);
        }

        dis_trace("send cnt %u, pts %llu \n", send_cnt, frame_info[0]->pts);
        send_cnt += frame_num;
        time_ref += 2; /* gap : 2 */
    }

    if (send_cnt >= frame_cnt) {
        dis_ctx->is_equal_set_send_cnt = OT_TRUE;
        if (send_pipe_frame_info.loop_send == OT_TRUE) {
            ot_buffer_pool_destroy(pool_hdl);
            dis_ctx->is_reading = OT_FALSE;
            ot_buffer_pool_deinit();
            dis_check_fd_closed(g_dis_send_srs_fd);
            dis_check_all_fd_closed();
            free(user_frame_info);
            send_cnt = 0;
        }
        dis_trace("vi set pipe send_cnt %u \n", send_cnt);
    }

    return OT_NULL;

malloc_blk_failed:
    for (j = i0 - 1; j >= 0; j--) {
        sample_comm_vi_free_frame_blk(pool_hdl, &user_frame_info[j]);
    }
    ot_buffer_pool_destroy(pool_hdl);
not_pthread_start:
    dis_ctx->is_reading = OT_FALSE;
pool_create_failed:
    ot_buffer_pool_deinit();
pool_init_failed:
    dis_check_fd_closed(g_dis_send_srs_fd);
prepare_file_fd_failed:
    free(user_frame_info);
    dis_trace("---- exit thread success---- \n");
    return OT_NULL;
}

static ot_void dis_send_multi_pipe_send_frame(dis_vi_info vi_info[], ot_u32 dev_num, ot_eis_img_pixel_format pixel_format,
                                              ot_bool is_run_be)
{
    ot_u32 i;
    dis_send_pipe_info send_pipe_frame_info[OT_VI_MAX_DEV_NUM] = { 0 };
    dis_test_ctx *dis_ctx = dis_get_test_ctx();
    ot_u32 send_thread_num;
    dis_pthread_id send_id;

    for (i = 0; i < dev_num; i++) {
        send_pipe_frame_info[i].vi_pipe = g_vi_send_pipe;
        printf("send_pipe_frame_info[%u].vi_pipe : %d\n",  i, send_pipe_frame_info[i].vi_pipe);
        send_pipe_frame_info[i].wdr_mode = vi_info[i].wdr_mode;
        send_pipe_frame_info[i].loop_send = OT_FALSE;
        send_pipe_frame_info[i].milli_sec = 5000; /* defaut num: 5000 */
        send_pipe_frame_info[i].file_name = vi_info[i].send_srs_info.file_name;
        send_pipe_frame_info[i].frame_cnt = vi_info[i].send_srs_info.send_cnt;
        if (send_pipe_frame_info[i].frame_cnt <= 0 || send_pipe_frame_info[i].file_name == OT_NULL) {
            dis_trace("error send_pipe_frame_info, pls check\n");
            return;
        }

        send_pipe_frame_info[i].get_frame_vrb_cfg.size.width = vi_info[i].vi_size.width;
        send_pipe_frame_info[i].get_frame_vrb_cfg.size.height = vi_info[i].vi_size.height;
        send_pipe_frame_info[i].get_frame_vrb_cfg.pixel_format = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
        send_pipe_frame_info[i].get_frame_vrb_cfg.video_format = OT_EIS_IMAGE_LAYOUT_LINEAR;
        send_pipe_frame_info[i].get_frame_vrb_cfg.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
        send_pipe_frame_info[i].get_frame_vrb_cfg.dynamic_range = OT_DYNAMIC_RANGE_SDR8;
        if (memcpy_s(&send_pipe_frame_info[i].send_gyro_info, sizeof(dis_send_gyro_srs_param),
            &vi_info[i].send_gyro_info, sizeof(dis_send_gyro_srs_param)) != EOK) {
            dis_trace("memcpy_s failed!!!\n");
        }
        send_pipe_frame_info[i].is_send_gyro = OT_FALSE;
        send_id = dis_pthread_create(dis_send_pipe_frame_thread, (ot_void *)&send_pipe_frame_info[i]);
        send_thread_num = dis_ctx->send_pthread_num;
        dis_ctx->send_pthread_id[send_thread_num] = send_id;
        dis_ctx->send_pthread_num++;
    }
    return;
}

ot_s32 dis_send_creat_send_thread(dis_vi_info vi_info[], ot_u32 route_num)
{
    if ((vi_info[0].is_send_pipe_yuv || vi_info[0].is_send_pipe_raw)) {
        dis_send_multi_pipe_send_frame(vi_info, route_num, vi_info[0].pixel_format, vi_info[0].run_wakeup == 1/* OT_ISP_RUN_WAKEUP_BE_END */);//todo
    }

    return OT_SUCCESS;
}


ot_s32 dis_send_motionfusion_deinit_param()
{
    const ot_u32 fusion_id = 0;
    ot_s32 ret = OT_SUCCESS;
    ot_eis_mfusion_drift drift = { 0 };
    ot_eis_mfusion_temperature_drift temperature_drift = { 0 };
    ret = ot_eis_mfusion_get_gyro_online_drift(fusion_id, &drift);
    if (ret != OT_SUCCESS) {
        dis_trace("omi_mfusion_get_gyro_online_drift failed!\n");
        goto end;
    }

    drift.enable = OT_FALSE;
    ret = ot_eis_mfusion_set_gyro_online_drift(fusion_id, &drift);
    if (ret != OT_SUCCESS) {
        dis_trace("omi_mfusion_set_gyro_online_drift failed!\n");
        goto end;
    }
    ret = ot_eis_mfusion_get_gyro_online_temperature_drift(fusion_id, &temperature_drift);
    if (ret != OT_SUCCESS) {
        dis_trace("omi_mfusion_get_gyro_online_temperature_drift failed!\n");
        goto end;
    }

    temperature_drift.enable = OT_FALSE;
    ret = ot_eis_mfusion_set_gyro_online_temperature_drift(fusion_id, &temperature_drift);
    if (ret != OT_SUCCESS) {
        dis_trace("omi_mfusion_set_gyro_online_temperature_drift failed!\n");
        goto end;
    }
    sleep(1);
end:
    return ret;
}

ot_s32 dis_send_motionsensor_stop(void)
{
    ot_s32 ret;
    ret = ioctl(g_dis_msensor_chip_dev, MSENSOR_CMD_STOP, NULL);
    if (ret != OT_SUCCESS) {
        dis_trace("stop motionsensor failed!\n");
    }

    return ret;
}

ot_s32 dis_send_gyrodis_stop_gyro()
{
    ot_s32 ret = OT_SUCCESS;
    dis_test_ctx *dis_ctx = dis_get_test_ctx();
    ret = dis_send_motionfusion_deinit_param();
    if (dis_ctx->is_msensor_start) {
        ret += dis_send_motionsensor_stop();
    }
    ret += dis_send_motionsensor_deinit();
    if (ret != OT_SUCCESS) {
        return ret;
    }
    return ret;
}

ot_s32 dis_send_gyrodis_start(dis_send_info *dis_send_info)
{
    ot_s32 ret = OT_SUCCESS;
    dis_gyro_info *gyro_info = &dis_send_info->gyro_info;

    ret = dis_send_gyrodis_init_gyro(gyro_info);
    if (ret != OT_SUCCESS) {
        dis_trace("dis start gyro route failed\n");
        goto exit1;
    }

    ret = dis_send_set_vi_pipe_dis_chn_attr(dis_send_info->vi_info);
    if (ret != OT_SUCCESS) {
        dis_trace("dis_set_vi_multi_pipe_dis_chn_attr failed\n");
        goto exit1;
    }

    ret = dis_send_creat_send_thread(dis_send_info->vi_info, dis_send_info->route_num);
    dis_err_void_goto(ret, "dis creat send thread", OT_SUCCESS, exit1);
    (ot_void)getchar();

exit1:
    ret += dis_send_gyrodis_stop_gyro();
    return ret;
}

static ot_s32 dis_send_get_ini_dir()
{
    ot_s32 ret;
    ot_char ldc_path[DIS_FL_LENGTH] = {0};
    ot_char msensor_path[DIS_FL_LENGTH] = {0};
    ret = snprintf_s(ldc_path, DIS_FL_LENGTH, DIS_FL_LENGTH-1, "./send_data/dis_ldc.ini");
    if (ret < 0) {
        dis_trace("snprintf_s dis_ldc.ini failed\n");
        return OT_FAILURE;
    }
    ret = snprintf_s(msensor_path, DIS_FL_LENGTH, DIS_FL_LENGTH-1, "./send_data/msensor_param.ini");
    if (ret < 0) {
        dis_trace("snprintf_s msensor_param.ini failed\n");
        return OT_FAILURE;
    }

    ldc_dir = ini_process_file(ldc_path);
    if (ldc_dir == OT_NULL) {
        dis_trace("load %s failed\n", ldc_path);
        return OT_FAILURE;
    }

    mes_dir = ini_process_file(msensor_path);
    if (mes_dir == OT_NULL) {
        dis_trace("load %s failed\n", msensor_path);
        goto free_dir;
    }

    printf("dis_send_get_dir_name success!\n");
    return OT_SUCCESS;

free_dir:
    free_ini_info_dict(ldc_dir);
    return OT_FAILURE;
}

ot_s32 dis_send_load_temp_motionfusionlut(ini_dictionary        * dictionary)
{
    ot_s32 ret, line;
    ot_u32 i;
    const ot_char *temp_string = OT_NULL;
    ot_s32 lut[g_msensor_lut_count];

    ot_eis_mfusion_temperature_drift *temp_drift = &g_motionfusion_param.temp_drift;
    temp_drift->enable = (ot_bool)ini_get_bool(dictionary, "temperature_drift_attr:enable", OT_FALSE);

    temp_drift->mode = (ot_eis_mfusion_temperature_drift_mode)ini_get_int(dictionary,
        "temperature_drift_attr:mode", OT_EIS_IMU_TEMPERATURE_DRIFT_CURV);

    if (temp_drift->mode == OT_EIS_IMU_TEMPERATURE_DRIFT_CURV) {
        temp_string = ini_get_string(dictionary, "temperature_drift_attr:matrix", OT_NULL);
        dis_send_check_pointer(temp_string, "temperature_drift_attr:matrix fail.\n");
        ret = dis_send_get_longstr(temp_string, temp_drift->temperature_matrix, OT_EIS_MFUSION_MATRIX_TEMPERATURE_NUM);
        if (dis_send_check_range(ret, 1, OT_EIS_MFUSION_MATRIX_TEMPERATURE_NUM) != OT_SUCCESS) {
            dis_trace("matrix is fail");
            return OT_FAILURE;
        }
    } else if (temp_drift->mode == OT_EIS_IMU_TEMPERATURE_DRIFT_LUT) {
        temp_string = ini_get_string(dictionary, "temperature_drift_attr:temp_lut", OT_NULL);
        dis_send_check_pointer(temp_string, "temperature_drift_attr:temp_lut fail.\n");
        if (memset_s(lut, sizeof(lut), 0, sizeof(lut))  != EOK) {
            dis_trace("memset_s failed!\n");
        }
        ret = dis_send_get_longstr(temp_string, lut, g_msensor_lut_count);
        if (dis_send_check_range(ret, 1, g_msensor_lut_count) != OT_SUCCESS) {
            dis_trace("matrix is fail.\n");
            return OT_FAILURE;
        }
        ot_eis_mfusion_temperature_drift_lut *drift_lut = &temp_drift->temperature_lut;
        for (i = 0; i < g_msensor_lut_count;) {
            line = i / g_msensor_line_count;
            drift_lut->imu_lut[line][0] = lut[i + 0];          /* 0:index */
            drift_lut->imu_lut[line][1] = lut[i + 1];          /* 1:index */
            drift_lut->imu_lut[line][2] = lut[i + 2];          /* 2:index */
            drift_lut->gyro_lut_status[line][0] = lut[i + 3];  /* 3:index */
            drift_lut->gyro_lut_status[line][1] = lut[i + 4];  /* 4:index */
            printf("lut(%2d):%10d %10d %10d %15d %15d\n", line, drift_lut->imu_lut[line][0],
                drift_lut->imu_lut[line][1], drift_lut->imu_lut[line][2], /* index : 2 */
                drift_lut->gyro_lut_status[line][0], drift_lut->gyro_lut_status[line][1]);
            i += g_msensor_line_count;
        }
        temp_drift->temperature_lut.range_min = (ot_s32)ini_get_int(dictionary,
            "temperature_drift_attr:range_min", DIS_DEFAULT_NUM_VALUE);
        temp_drift->temperature_lut.range_max = (ot_s32)ini_get_int(dictionary,
            "temperature_drift_attr:range_max", DIS_DEFAULT_NUM_VALUE);
        temp_drift->temperature_lut.step = (ot_u32)ini_get_int(dictionary,
            "temperature_drift_attr:step", DIS_DEFAULT_NUM_VALUE);
    }
    return OT_SUCCESS;
}
static ot_s32 dis_send_get_ldc_param()
{
    ot_s32 ret;
    const ot_char *src_calibration_ratio_string = OT_NULL;
    const ot_char *dst_calibration_ratio_string = OT_NULL;
    g_motionfusion_param.dis_attr.dis_ldc_en = ini_get_int(ldc_dir,  "dis_ldc_en:enable", 0);
    ot_eis_dis_ldc_attr *ldc_calibration = &g_motionfusion_param.dis_attr.dis_ldc_attr;
    ldc_calibration->focal_len_x = ini_get_int(ldc_dir,  "dis_ldc_attr:focal_len_x", 0);
    ldc_calibration->focal_len_y = ini_get_int(ldc_dir,  "dis_ldc_attr:focal_len_y", 0);
    ldc_calibration->coord_shift_x = ini_get_int(ldc_dir,  "dis_ldc_attr:coord_shift_x", 0);
    ldc_calibration->coord_shift_y = ini_get_int(ldc_dir,  "dis_ldc_attr:coord_shift_y", 0);
    ldc_calibration->max_du = ini_get_int(ldc_dir,  "dis_ldc_attr:max_du", 0);

    src_calibration_ratio_string = ini_get_string(ldc_dir, "dis_ldc_attr:src_calibration_ratio", 0);
    dis_send_check_pointer(src_calibration_ratio_string,  "dis_ldc_attr:src_calibration_ratio fail.\n");
    ret = dis_send_get_longstr(src_calibration_ratio_string, ldc_calibration->src_calibration_ratio,
        OT_SRC_LENS_COEF_NUM);
    if (dis_send_check_range(ret, 1, OT_SRC_LENS_COEF_NUM) != OT_SUCCESS) {
        dis_trace("src_calibration_ratio_string is null");
        return OT_FAILURE;
    }
    dst_calibration_ratio_string = ini_get_string(ldc_dir, "dis_ldc_attr:dst_calibration_ratio", 0);
    dis_send_check_pointer(dst_calibration_ratio_string,  "dis_ldc_attr:src_calibration_ratio fail.\n");
    ret = dis_send_get_longstr(dst_calibration_ratio_string, ldc_calibration->dst_calibration_ratio,
                               OT_DST_LENS_COEF_NUM);
    if (dis_send_check_range(ret, 1, OT_DST_LENS_COEF_NUM) != OT_SUCCESS) {
        dis_trace("dst_calibration_ratio_string is null");
        return OT_FAILURE;
    }
    printf("dis_send_get_ldc_param success!\n");
    return OT_SUCCESS;
}

static ot_s32 dis_send_get_six_side_calibration_param()
{
    ot_s32 ret;
    const ot_char *six_side_calibration_string = OT_NULL;
    ot_eis_mfusion_six_side_calibration *six_calibration = &g_motionfusion_param.six_side_calibration;
    six_calibration->enable = ini_get_int(mes_dir, "six_side_calibration:enable", 1);
    six_side_calibration_string = ini_get_string(mes_dir, "six_side_calibration:matrix", 0);
    dis_send_check_pointer(six_side_calibration_string,  "six_side_calibration:matrix fail.\n");
    ret = dis_send_get_longstr(six_side_calibration_string, six_calibration->matrix, OT_EIS_MFUSION_MATRIX_NUM);
    if (dis_send_check_range(ret, 1, OT_EIS_MFUSION_MATRIX_NUM) != OT_SUCCESS) {
        dis_trace("tempString is null");
        return OT_FAILURE;
    }
    printf("dis_send_get_six_side_calibration_param success!\n");
    return OT_SUCCESS;
}

static ot_s32 dis_send_get_gyro_drift_attr()
{
    ot_s32 ret;
    const ot_char *gyro_drift_attr_string = OT_NULL;
    ot_eis_mfusion_drift *drift = &g_motionfusion_param.gyro_drift;

    drift->enable = ini_get_int(mes_dir, "gyro_drift_attr:enable", 0);
    gyro_drift_attr_string = ini_get_string(mes_dir, "gyro_drift_attr:matrix", 0);
    dis_send_check_pointer(gyro_drift_attr_string,  "gyro_drift_attr:matrix fail.\n");
    ret = dis_send_get_longstr(gyro_drift_attr_string, drift->drift, OT_EIS_MFUSION_AXIS_NUM);
    if (dis_send_check_range(ret, 1, OT_EIS_MFUSION_AXIS_NUM) != OT_SUCCESS) {
        dis_trace("gyro_drift_attr is null");
        return OT_FAILURE;
    }
    printf("dis_send_get_gyro_drift_attr success!\n");
    return OT_SUCCESS;
}

static ot_void dis_send_get_dis_cfg()
{
    ot_eis_dis_cfg *dis_cfg = &g_motionfusion_param.dis_cfg;
    dis_cfg->pdt_type = (ot_eis_dis_pdt_type)ini_get_int(mes_dir, "dis_cfg:pdt_type", 0);
    dis_cfg->crop_ratio = (ot_u32)ini_get_int(mes_dir, "dis_cfg:crop_ratio", 80); /* default crop_ratio : 80 */
    dis_cfg->frame_rate = (ot_u32)ini_get_int(mes_dir, "dis_cfg:frame_rate", 25); /* default frame_rate : 25 */
    dis_cfg->camera_steady = (ot_bool)ini_get_int(mes_dir, "dis_cfg:camera_steady", 1);
    dis_cfg->scale = (ot_bool)ini_get_int(mes_dir, "dis_cfg:scale", 1);
    printf("dis_send_get_dis_cfg success!\n");
    return;
}

static ot_void dis_send_get_dis_attr()
{
    ot_eis_dis_attr *dis_attr = &g_motionfusion_param.dis_attr;
    dis_attr->enable = (ot_bool)ini_get_int(mes_dir, "dis_attr:enable", 0);
    dis_attr->timelag = (ot_u32)ini_get_int(mes_dir, "dis_attr:timelag", 2850); /* default timelag : 2850 */
    dis_attr->strength = (ot_u32)ini_get_int(mes_dir, "dis_attr:strength", 1024); /* default strength : 1024 */
    printf("dis_send_get_dis_attr success!\n");
    return;
}

static ot_void dis_send_get_msensor_param()
{
    ot_msensor_param *msensor_param = &g_motionfusion_param.msensor_param;
    msensor_param->attr.device_mask = (ot_u32)ini_get_int(mes_dir, "msensor_attr:device_mask", 3); /* device_mask:3 */
    msensor_param->attr.temperature_mask = (ot_u32)ini_get_int(mes_dir, "msensor_attr:temperature_mask", 3);
    msensor_param->config.gyro_config.odr =
        (ot_u32)ini_get_int(mes_dir, "gyro_config:odr", 800); /* deafult odr : 800 */
    msensor_param->config.gyro_config.fsr =
        (ot_u32)ini_get_int(mes_dir, "gyro_config:fsr", 256000); /* deafult fsr : 256000 */
    msensor_param->config.gyro_config.data_width =
        (ot_u8)ini_get_int(mes_dir, "gyro_config:data_width", 16); /* deafult data_width : 16 */
    msensor_param->config.gyro_config.temperature_max =
        (ot_s32)ini_get_int(mes_dir, "gyro_config:temperature_max", 80); /* deafult temperature_max : 80 */
    msensor_param->config.gyro_config.temperature_min =
        (ot_s32)ini_get_int(mes_dir, "gyro_config:temperature_min", -40); /* deafult temperature_min : -40 */
    printf("dis_send_get_dis_attr success!\n");
    return;
}

static ot_s32 dis_send_get_ini_param()
{
    dis_send_get_msensor_param(); /* get msensor_param */
    dis_send_get_dis_cfg(); /* get dis_cfg */
    dis_send_get_dis_attr(); /* get dis_attr */
    if (dis_send_get_six_side_calibration_param() != OT_SUCCESS) { /* get sixCalibration */
        dis_trace("dis_send_get_six_side_calibration_param failed\n");
        return OT_FAILURE;
    }
    if (dis_send_get_ldc_param() != OT_SUCCESS) { /* get ldc_param */
        dis_trace("dis_send_get_ldc_param failed\n");
        return OT_FAILURE;
    }
    if (dis_send_get_gyro_drift_attr() != OT_SUCCESS) { /* get gyro_drift_attr */
        dis_trace("dis_send_get_gyro_drift_attr failed\n");
        return OT_FAILURE;
    }
    if (dis_send_load_temp_motionfusionlut(mes_dir) != OT_SUCCESS) { /* get gyro_temp_motionfusionlut */
        dis_trace("dis_send_load_temp_motionfusionlut failed\n");
        return OT_FAILURE;
    }
    return OT_SUCCESS;
}

static ot_void dis_send_process(dis_send_info *dis_send_info)
{
    ot_s32 ret;
    ret = dis_send_get_ini_dir(); /* get ini dir */
    if (ret != OT_SUCCESS) {
        dis_trace("dis_send_get_ini_dir failed\n");
        return;
    }
    ret = dis_send_get_ini_param(); /* get ini param */
    if (ret != OT_SUCCESS) {
        dis_trace("dis_send_get_ini_param failed\n");
        goto free_ini;
        return;
    }
    dis_send_set_all_config(dis_send_info); /* set parameter */

    ret = dis_send_gyrodis_start(dis_send_info); /* start send data */
    if (ret != OT_SUCCESS) {
        dis_trace("dis_gyrodis_start failed\n");
    } else {
        dis_trace("dis_send success\n");
    }

free_ini:
    free_ini_info_dict(ldc_dir);
    free_ini_info_dict(mes_dir);
}

static ot_s32 dis_send_set_info(ot_s32 mode, ot_s32 frame_cnt, ot_s32 send_width, ot_s32 send_height,
    dis_send_info *dis_send_info)
{
    if (!value_between(mode, DIS_IPC_MODE, DIS_DV_MODE)) {
        printf("pipe id must be [0,%d]!!!!\n\n", DIS_DV_MODE);
        return OT_FAILURE;
    }
    dis_send_info->gyro_info.pdt_type = mode; /* 0:IPC_MOD 1:DV_MOD */
    if (frame_cnt <= MIN_FRAME_COUNT) {
        printf("the frame cnt(%d) is invalid!\nthe frame cnt should be greater than 1!\n\n", frame_cnt);
        return OT_FAILURE;
    }
    dis_send_info->pic_size.frame_cnt = frame_cnt;
    if (send_width <= 0) {
        printf("the picture width :(%d) is invalid!\nthe picture width should be greater than 1!\n\n", send_width);
        return OT_FAILURE;
    }
    dis_send_info->pic_size.send_width = send_width;
    if (send_height <= 0) {
        printf("the picture height :(%d) is invalid!\nthe picture height should be greater than 1!\n\n", send_height);
        return OT_FAILURE;
    }
    dis_send_info->pic_size.send_height = send_height;
    return OT_SUCCESS;
}

static ot_s32 dis_send_set_hvmax(ot_s32 hmax, ot_s32 vmax)
{
    if (hmax <= 0) {
        printf("hmax :(%d) is invalid!\nhmax should be greater than 1!\n\n", hmax);
        return OT_FAILURE;
    }
    g_hmax = hmax;
    if (vmax <= 0) {
        printf("vmax :(%d) is invalid!\nvmax should be greater than 1!\n\n", vmax);
        return OT_FAILURE;
    }
    g_vmax = vmax;
    return OT_SUCCESS;
}

static ot_s32 dis_send_check_argv(char *argv[], ot_s32 index, ot_s32 *val)
{
    ot_char *end_ptr = OT_NULL;
    ot_slong result;
    errno = 0;
    result = strtol(argv[index], &end_ptr, DIS_SEND_ARG_BASE);
    if ((errno == ERANGE && (result == LONG_MAX || result == LONG_MIN)) || (errno != 0 && result == 0)) {
        return OT_FAILURE;
    }
    if ((end_ptr == argv[index]) || (*end_ptr != '\0')) {
        return OT_FAILURE;
    }

    *val = (ot_s32)result;

    return OT_SUCCESS;
}

static ot_s32 dis_send_set_mode(dis_send_info *dis_send_info, char *argv[], int argc)
{
    ot_s32 mode;
    ot_s32 frm_cnt;
    ot_s32 send_width;
    ot_s32 send_height;
    ot_s32 hmax_time;
    ot_s32 vmax;

    if (dis_send_check_argv(argv, 1, &mode) != OT_SUCCESS) {
        return OT_FAILURE;
    }

    if (dis_send_check_argv(argv, 2, &frm_cnt) != OT_SUCCESS) { /* 2 args */
        return OT_FAILURE;
    }
    if (dis_send_check_argv(argv, 3, &send_width) != OT_SUCCESS) { /* 3 args */
        return OT_FAILURE;
    }

    if (dis_send_check_argv(argv, 4, &send_height) != OT_SUCCESS) { /* 4 args */
        return OT_FAILURE;
    }

    if (dis_send_check_argv(argv, 5, &hmax_time) != OT_SUCCESS) { /* 5 args */
        return OT_FAILURE;
    }

    if (dis_send_check_argv(argv, 6, &vmax) != OT_SUCCESS) { /* 6 args */
        return OT_FAILURE;
    }

    if (dis_send_set_info(mode, frm_cnt, send_width, send_height, dis_send_info) != OT_SUCCESS) {
        return OT_FAILURE;
    }
    if (dis_send_set_hvmax(hmax_time, vmax) != OT_SUCCESS) {
        return OT_FAILURE;
    }
    return OT_SUCCESS;
}

static ot_bool dis_send_yuv_check_valid_digit_str(const ot_char *str)
{
    ot_u32 i;
    ot_u32 str_len;

    str_len = (ot_u32)strlen(str);
    if (str_len > DIS_MAX_DIGIT_LEN) {
        return OT_FALSE;
    }

    for (i = 0; i < str_len; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return OT_FALSE;
        }
    }
    return OT_TRUE;
}

void dis_send_handle_sig(int signo)
{
    if (g_signal_flag) {
        return;
    }

    if (signo == SIGINT || signo == SIGTERM) {
        g_signal_flag = 1;
    }
    return;
}

static ot_void dis_send_usage(ot_void)
{
    printf("\n"
           "*************************************************\n"
           "Usage: ./dis_send [mode] [FrmCnt] [width] [height] [hmax] [vmax] \n"
           "1)mode: \n"
           "   if mode == 0 IPC_MODE\n"
           "   if mode == 1 DV_MODE\n"
           "2)FrmCnt: \n"
           "   the count of frame to be should be send ,the value should be greater than 1\n"
           "3)width: \n"
           "   width: picture width\n"
           "4)height: \n"
           "   height: picture height\n"
           "5)hmax: \n"
           "   hmax: hmax_time\n"
           "6)vmax: \n"
           "   vmax: picture vmax\n"
           "*)Example:\n"
           "   e.g : ./dis_send 0 1000 3840 2160 18518 2160\n"
           "*************************************************\n"
           "\n");
}

#ifdef __LITEOS__
ot_s32 dis_send(int argc, char *argv[])
#else
ot_s32 main(int argc, char *argv[])
#endif
{
    ot_s32 i;
    sig_t sig_handler;
    printf("\nNOTICE: This tool only can be used for send imu data and yuv!\n");
    printf("\tTo see more usage, please enter: ./dis_send -h\n\n");
    if (argv == OT_NULL) {
        dis_send_usage();
        return OT_SUCCESS;
    }
    if (argc > 1) {
        if (!strncmp(argv[1], "-h", 2)) { /* 2 help */
            dis_send_usage();
            return OT_SUCCESS;
        }
    }
    if (argc != 7) { /* 7 args */
        dis_send_usage();
        return OT_SUCCESS;
    }

    for (i = 1; i < argc; i++) {
        if (!dis_send_yuv_check_valid_digit_str(argv[i])) {
            printf("the %dth value :'%s' is invalid, must be reasonable non negative integers!!!!\n\n", i, argv[i]);
            dis_send_usage();
            return -1;
        }
    }
    if (dis_send_set_mode(&g_dis_send_info, argv, argc) != OT_SUCCESS) {
        return -1;
    }

#ifndef __LITEOS__
    sig_handler = signal(SIGINT, dis_send_handle_sig);
    if (sig_handler == SIG_ERR) {
        perror("error\n");
    }
    sig_handler = signal(SIGTERM, dis_send_handle_sig);
    if (sig_handler == SIG_ERR) {
        perror("error\n");
    }
#endif
    dis_send_process(&g_dis_send_info);
    return OT_SUCCESS;
}

