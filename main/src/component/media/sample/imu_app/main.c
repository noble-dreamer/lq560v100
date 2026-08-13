#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <limits.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>

#include "sample_comm.h"

#include "ot_common_motionsensor.h"
#include "ot_motionsensor_chip_cmd.h"
#include "ot_motionsensor_mng_cmd.h"
#include "ot_motionfusion.h"

#include "ux_protocol.h"

#define NUMS_IN_BUF            4000  /* max numbers of gyro data or acc data in buffer */
#define GYRO_BUF_SIZE          (sizeof(ot_msensor_sample_data) * NUMS_IN_BUF)
#define ACC_BUF_SIZE           (sizeof(ot_msensor_sample_data) * NUMS_IN_BUF)

#define GYRO_DATA_RECORDER_RANGE   250
#define GYRO_DATA_DV_RANGE         1000
#define GYRO_DATA_ODR              400
#define ACC_DATA_RANGE             16
#define ACC_DATA_ODR               400

#define OT_MFUSION_MATRIX_NUM       9

const ot_s32 g_imu_rotation_matrix[OT_MFUSION_MATRIX_NUM] = {
    0, 32768, 0,
    32768, 0, 0,
    0, 0, -32768
};

typedef struct {
    ot_eis_dis_pdt_type pdt_type;
    ot_s32              chipfd;
    ot_s32              mngfd;
    ot_s32              usrfd;
    ot_double           data_range;
    ot_msensor_param    param;
    ot_msensor_buf_attr buf_attr;

    ux_handle           hux;
    uint8_t             ux_buf[UX_MAX_MSG_LEN];
} msns_dump_ctx;

static ot_s32  msns_init(msns_dump_ctx *ctx);
static ot_void msns_deinit(msns_dump_ctx *ctx);

static ot_s32 msns_set_mfusion_attr(ot_u32 fusion_id, ot_eis_dis_pdt_type mode)
{
    ot_eis_mfusion_attr mfusion_attr = { 0 };

    mfusion_attr.steady_detect_attr.steady_time_thr = 3; /* 3: default value for IPC */
    mfusion_attr.steady_detect_attr.gyro_offset = (ot_s32)(10 * (1 << 15)); /* 10, 15: default value */
    mfusion_attr.steady_detect_attr.acc_offset  = (ot_s32)(0.1 * (1 << 15)); /* 0.1, 15: default value */
    mfusion_attr.steady_detect_attr.gyro_rms    = (ot_s32)(0.054 * (1 << 15)); /* 0.054, 15: default value */
    mfusion_attr.steady_detect_attr.acc_rms     = (ot_s32)(1.3565 * (1 << 15) / 1000); /* 1.3565, 15, 1000: default value */
    mfusion_attr.steady_detect_attr.gyro_offset_factor = (ot_s32)(2 * (1 << 4)); /* 2, 4: default value */
    mfusion_attr.steady_detect_attr.acc_offset_factor  = (ot_s32)(2 * (1 << 4)); /* 2, 4: default value */
    mfusion_attr.steady_detect_attr.gyro_rms_factor    = (ot_s32)(8 * (1 << 4)); /* 8, 4: default value */
    mfusion_attr.steady_detect_attr.acc_rms_factor     = (ot_s32)(10 * (1 << 4)); /* 10, 4: default value */

    mfusion_attr.device_mask      = OT_EIS_MFUSION_DEVICE_GYRO | OT_EIS_MFUSION_DEVICE_ACC;
    mfusion_attr.temperature_mask = OT_EIS_MFUSION_TEMPERATURE_GYRO | OT_EIS_MFUSION_TEMPERATURE_ACC;

    if (mode == OT_EIS_DIS_PDT_TYPE_RECORDER) {
        mfusion_attr.steady_detect_attr.steady_time_thr = 3; /* 3: default value for IPC */
        mfusion_attr.steady_detect_attr.gyro_rms_factor = (ot_s32)(8 * (1 << 4)); /* 8, 4: default value for IPC */
        mfusion_attr.steady_detect_attr.acc_rms_factor  = (ot_s32)(10 * (1 << 4)); /* 10, 4: default value for IPC */
    } else if (mode == OT_EIS_DIS_PDT_TYPE_DV) {
        mfusion_attr.steady_detect_attr.steady_time_thr = 1; /* 1: default value for DV */
        mfusion_attr.steady_detect_attr.gyro_rms_factor = (ot_s32)(12.5 * (1 << 4)); /* 12.5,4:default value for DV */
        mfusion_attr.steady_detect_attr.acc_rms_factor  = (ot_s32)(100 * (1 << 4)); /* 100, 4: default value for DV */
    }

    return ot_eis_mfusion_set_attr(fusion_id, &mfusion_attr);
}


static ot_s32 msns_set_temperature_drift(ot_u32 fusion_id , ot_bool is_enable)
{
    ot_eis_mfusion_temperature_drift temperature_drift = { 0 };
    ot_u32 i;
    ot_s32 ret;

    temperature_drift.enable = is_enable;

    temperature_drift.mode = OT_EIS_IMU_TEMPERATURE_DRIFT_LUT;
    temperature_drift.temperature_lut.range_min = 20 * 1024; /* 1024: 2^10, 10 bit precision; 20 degree */
    temperature_drift.temperature_lut.range_max = 78 * 1024; /* 1024: 2^10, 10 bit precision; 78 degree */
    temperature_drift.temperature_lut.step      =  2 * 1024; /* 1024: 2^10, 10 bit precision; 2 step */

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

ot_s32 msns_init_param(ot_eis_dis_pdt_type mode)
{
    ot_s32 ret;
    const ot_u32 fusion_id = 0;
    ot_s32 gyro_drift[OT_EIS_MFUSION_AXIS_NUM] = {0, 0, 0};
    ot_eis_mfusion_six_side_calibration six_side_calibration = { 0 };
    ot_eis_mfusion_drift drift = { 0 };

    ret = msns_set_mfusion_attr(fusion_id, mode);
    if (ret != OT_SUCCESS) {
        goto end;
    }

    six_side_calibration.enable = OT_FALSE;
    (ot_void)memcpy_s(six_side_calibration.matrix, OT_MFUSION_MATRIX_NUM * sizeof(ot_s32), g_imu_rotation_matrix, OT_MFUSION_MATRIX_NUM * sizeof(ot_s32));

    ret = ot_eis_mfusion_set_gyro_six_side_calibration(fusion_id, &six_side_calibration);
    if (ret != OT_SUCCESS) {
        sample_print("motionfusion_set_gyro_six_side_cal failed!\n");
        goto end;
    }

    if (mode == OT_EIS_DIS_PDT_TYPE_RECORDER) {
        drift.enable = OT_FALSE;
        (ot_void)memcpy_s(drift.drift, OT_EIS_MFUSION_AXIS_NUM * sizeof(ot_s32), gyro_drift, OT_EIS_MFUSION_AXIS_NUM * sizeof(ot_s32));
        ret = ot_eis_mfusion_set_gyro_online_drift(fusion_id, &drift);
        if (ret != OT_SUCCESS) {
            sample_print("motionfusion_set_gyro_online_drift failed!\n");
            goto end;
        }
    } else if (mode == OT_EIS_DIS_PDT_TYPE_DV) {
        ret = msns_set_temperature_drift(fusion_id, OT_FALSE);
        if (ret != OT_SUCCESS) {
            goto end;
        }
    }

end:
    return ret;
}


ot_s32 msns_deinit_param()
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

    usleep(100 * 1000);
end:
    return ret;
}

static ot_s32 msns_init(msns_dump_ctx *ctx)
{
    ot_s32 ret = OT_SUCCESS;
    ot_u32 buf_size = GYRO_BUF_SIZE + ACC_BUF_SIZE;
    ot_msensor_param param_set;
    ot_smr_alloc_attr smr_alloc_attr = { 0 };

    memset(ctx, 0, sizeof(msns_dump_ctx));
    ctx->mngfd = ctx->usrfd = ctx->chipfd = -1;
    ctx->pdt_type = OT_EIS_DIS_PDT_TYPE_DV;

    ctx->chipfd = open("/dev/motionsensor_chip", O_RDWR);
    if (ctx->chipfd < 0) {
        sample_print("Error: cannot open MotionSensor device.may not load motionsensor driver !\n");
        return OT_FAILURE;
    }
    memset(&smr_alloc_attr, 0, sizeof(ot_smr_alloc_attr));
    smr_alloc_attr.align = 64;
    smr_alloc_attr.len   = buf_size;
    smr_alloc_attr.cached = OT_TRUE;
    strcpy(smr_alloc_attr.region_name , "");
    ret = ot_smr_alloc(&smr_alloc_attr, &ctx->buf_attr.phys_addr, (ot_void **)&ctx->buf_attr.virt_addr);
    if (ret != OT_SUCCESS) {
        sample_print("alloc smr for Motionsensor failed,ret:%x !\n", ret);
        ret =  OT_ERR_SMR_NO_MEM;
        goto label_exit;
    }

    (ot_void)memset_s((ot_void *)(uintptr_t)ctx->buf_attr.virt_addr, buf_size, 0, buf_size);

    ctx->buf_attr.buf_len = buf_size;

    if (ctx->pdt_type == OT_EIS_DIS_PDT_TYPE_DV) {
        ctx->data_range = GYRO_DATA_DV_RANGE;
    } else if (ctx->pdt_type == OT_EIS_DIS_PDT_TYPE_RECORDER) {
        ctx->data_range = GYRO_DATA_RECORDER_RANGE;
    }
    
    /* set device work mode */
    param_set.attr.device_mask       = OT_MSENSOR_DEVICE_GYRO | OT_MSENSOR_DEVICE_ACC;
    param_set.attr.temperature_mask  = OT_MSENSOR_TEMP_GYRO | OT_MSENSOR_TEMP_ACC;
    /* set gyro samplerate and full scale range */
    param_set.config.gyro_config.odr = GYRO_DATA_ODR * OT_MSENSOR_GRADIENT;
    param_set.config.gyro_config.fsr = ctx->data_range * OT_MSENSOR_GRADIENT;
    /* set accel samplerate and full scale range */
    param_set.config.acc_config.odr  = ACC_DATA_ODR * OT_MSENSOR_GRADIENT;
    param_set.config.acc_config.fsr  = ACC_DATA_RANGE * OT_MSENSOR_GRADIENT;

    (ot_void)memcpy_s(&param_set.buf_attr, sizeof(ot_msensor_buf_attr), &ctx->buf_attr, sizeof(ot_msensor_buf_attr));

    ret = ioctl(ctx->chipfd, MSENSOR_CMD_INIT, &param_set);
    if (ret) {
        sample_print("MSENSOR_CMD_INIT failed\n");
        goto label_exit;
    }

    ret = ioctl(ctx->chipfd, MSENSOR_CMD_START, NULL);
    if (ret != OT_SUCCESS) {
        sample_print("MSENSOR_CMD_START failed\n");
        goto label_exit;
    }

    ret = msns_init_param(ctx->pdt_type);
    if (ret != OT_SUCCESS) {
        sample_print("msns_init_param failed\n");
        goto label_exit;
    }

    ctx->mngfd = open("/dev/motionsensor_mng", O_RDWR);
    if (ctx->mngfd < 0) {
        printf("open motionsensor_mng failed\n");
        goto label_exit;
    }

    ret = ioctl(ctx->mngfd, MSENSOR_CMD_ADD_USER, &ctx->usrfd);
    if (ret != OT_SUCCESS) {
        printf("MSENSOR_CMD_ADD_USER failed, ret:%x !\n", ret);
        goto label_exit;
    }

    ctx->hux = ux_init("/dev/ttyGS0", 1500000); // 1.5Mbps
    if (ctx->hux == OT_NULL) {
        printf("ux_init failed\n");
        goto label_exit;
    }

    return OT_SUCCESS;

label_exit:

    msns_deinit(ctx);
    return ret;
}

static ot_void msns_deinit(msns_dump_ctx *ctx)
{
    if (ctx->hux != OT_NULL) {
        ux_deinit(ctx->hux);
        ctx->hux = OT_NULL;
    }

    if (ctx->usrfd >= 0) {
        ioctl(ctx->mngfd, MSENSOR_CMD_DELETE_USER, &ctx->usrfd);
        ctx->usrfd = -1;
    }

    if (ctx->mngfd >= 0) {
        close(ctx->mngfd);
        ctx->mngfd = -1;
    }

    if (ctx->buf_attr.virt_addr !=  (ot_u64)(uintptr_t)NULL) {
        ot_smr_free(ctx->buf_attr.phys_addr);
        ctx->buf_attr.phys_addr = 0;
        ctx->buf_attr.virt_addr = (ot_u64)(uintptr_t)NULL;
    }

    if (ctx->chipfd >= 0) {
        msns_deinit_param();
        ioctl(ctx->chipfd, MSENSOR_CMD_STOP, NULL);
        ioctl(ctx->chipfd, MSENSOR_CMD_DEINIT, NULL);
        close(ctx->chipfd);
        ctx->chipfd = -1;
    }
}

static ot_s32 msns_get_send(msns_dump_ctx *ctx, ot_msensor_data_type data_type, ot_u64 begin_pts, ot_u64 end_pts)
{
    ot_s32 ret = OT_SUCCESS;

    ot_msensor_data_info data_info = {0};
    data_info.id        = ctx->usrfd;
    data_info.data_type = data_type;
    data_info.begin_pts = begin_pts;
    data_info.end_pts   = end_pts;

    static uint32_t last_pts[2] = {0, 0}, counter[2] = {0, 0};

    ret = ioctl(ctx->mngfd, MSENSOR_CMD_GET_DATA, &data_info);
    if (ret != OT_SUCCESS) {
        printf("MSENSOR_CMD_GET_DATA failed\n");
        return ret;
    }

    uint8_t *msg_ptr = ctx->ux_buf;
    uint32_t msg_len = 0, pts = 0;

    if (data_info.data[0].num == 0 && data_info.data[1].num == 0) {
        printf("No data to send\n");
        return ret;
    }

    for (ot_u32 i = 0; i < 2; i++) { /* cyclic buffer 2 */
        ot_u32 num = data_info.data[i].num;
        ot_s32 * x_viraddr = (ot_s32 *)(uintptr_t)(ctx->buf_attr.virt_addr + ((ot_u64)(uintptr_t)data_info.data[i].x_phys_addr - ctx->buf_attr.phys_addr));
        ot_s32 * y_viraddr = (ot_s32 *)(uintptr_t)(ctx->buf_attr.virt_addr + ((ot_u64)(uintptr_t)data_info.data[i].y_phys_addr - ctx->buf_attr.phys_addr));
        ot_s32 * z_viraddr = (ot_s32 *)(uintptr_t)(ctx->buf_attr.virt_addr + ((ot_u64)(uintptr_t)data_info.data[i].z_phys_addr - ctx->buf_attr.phys_addr));
        ot_s32 * t_viraddr = (ot_s32 *)(uintptr_t)(ctx->buf_attr.virt_addr + ((ot_u64)(uintptr_t)data_info.data[i].temperature_phys_addr - ctx->buf_attr.phys_addr));
        ot_u64 * p_viraddr = (ot_u64 *)(uintptr_t)(ctx->buf_attr.virt_addr + ((ot_u64)(uintptr_t)data_info.data[i].pts_phys_addr - ctx->buf_attr.phys_addr));

        for (ot_u32 j = 0; j < num; j++) {
            pts = (p_viraddr[j] / 1000) & 0xFFFFFFFF; // us to ms
            msg_ptr[0]  = (uint8_t)(((uint32_t)data_type) & 0xFF);
            msg_ptr[1]  = (pts >> 24) & 0xFF;
            msg_ptr[2]  = (pts >> 16) & 0xFF;
            msg_ptr[3]  = (pts >>  8) & 0xFF;
            msg_ptr[4]  = (pts) & 0xFF;
            msg_ptr[5]  = (x_viraddr[j] >> 8) & 0xFF;
            msg_ptr[6]  = (x_viraddr[j]) & 0xFF;
            msg_ptr[7]  = (y_viraddr[j] >> 8) & 0xFF;
            msg_ptr[8]  = (y_viraddr[j]) & 0xFF;
            msg_ptr[9]  = (z_viraddr[j] >> 8) & 0xFF;
            msg_ptr[10] = (z_viraddr[j]) & 0xFF;
            msg_ptr[11] = (t_viraddr[j] >> 8) & 0xFF;
            msg_ptr[12] = (t_viraddr[j]) & 0xFF;

            msg_ptr += 13;
            msg_len += 13;

            if (last_pts[data_type] == 0) {
                last_pts[data_type] = pts;
            }
            else {
                if (pts < last_pts[data_type]) {
                    printf("invalid pts: data_type=%d, pts=%u, last_pts=%u\n", data_type, pts, last_pts[data_type]);
                }
                else if (pts - last_pts[data_type] > 5) {
                    printf("jumpped pts:  data_type=%d, pts=%u, last_pts=%u\n", data_type, pts, last_pts[data_type]);
                }
                last_pts[data_type] = pts;
            }
            
            counter[data_type]++;
            if (counter[data_type] % 500 == 0) {
                printf("type=%d, pts=%u\n", data_type, pts);
            }

            if (msg_len + 13 > UX_MAX_MSG_LEN) {
                ux_send(ctx->hux, ctx->ux_buf, msg_len);
                msg_ptr = ctx->ux_buf;
                msg_len = 0;
            }
       }
    }
    if (msg_len > 0) {
        ux_send(ctx->hux, ctx->ux_buf, msg_len);
    }

    ret = ioctl(ctx->mngfd, MSENSOR_CMD_RELEASE_BUF, &data_info);
    if (ret != OT_SUCCESS) {
        printf("MSENSOR_CMD_RELEASE_BUF failed, ret:%x \n", ret);
        return ret;
    }

    return ret;
}

volatile sig_atomic_t gs_loop = 1;
static void signal_handler(int signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        gs_loop = 0;
    }
}

static ot_void register_sig_handler(ot_void(*sig_handle)(ot_s32))
{
    struct sigaction sa;

    (ot_void)memset_s(&sa, sizeof(struct sigaction), 0, sizeof(struct sigaction));
    sa.sa_handler = sig_handle;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, OT_NULL);
    sigaction(SIGTERM, &sa, OT_NULL);
}

int main(int argc, char *argv[])
{
    ot_s32 ret = OT_SUCCESS;
    msns_dump_ctx ctx;

    register_sig_handler(signal_handler);
    ret = ot_smr_init();
    if(ret != 0){
        printf("smr init fail!ret[%d]\n",ret);
        return ret;
    }
    ret = msns_init(&ctx);
    if (ret != OT_SUCCESS) {
        printf("msns_init failed\n");
        goto msns_init_failed;
    }
    
    ot_u64 beg_pts = -1;
    ret = ioctl(ctx.mngfd, MSENSOR_CMD_SYNC_TIME, &beg_pts);
    if (ret != OT_SUCCESS) {
        printf("MSENSOR_CMD_SYNC_TIME failed\n");
        goto sync_pts_failed;
    }

    ot_u64 end_pts = beg_pts + 1;
    while (gs_loop) {
        msns_get_send(&ctx, MSENSOR_DATA_GYRO, beg_pts, end_pts);
        msns_get_send(&ctx, MSENSOR_DATA_ACC,  beg_pts, end_pts);
        usleep(10 * 1000);
        beg_pts = end_pts + 1;
        ioctl(ctx.mngfd, MSENSOR_CMD_SYNC_TIME, &end_pts);
    }

sync_pts_failed:
    msns_deinit(&ctx);
msns_init_failed:
    ot_smr_deinit();
    return 0;
}
