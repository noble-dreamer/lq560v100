/*
  Copyright (c), 2021-2024, Oritek Tech. Co., Ltd.
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "ot_eis_errno.h"

#ifdef OT_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "ot_eis_i2c.h"
#endif

#include "sc132gs_slave_cmos.h"
#include "ot_eis_vi_api.h"
#include "sc132gs_slave_readout_30fps_raw12.h"
#include "sc132gs_slave_exposure_30fps_raw12.h"

#define I2C_DEV_FILE_NUM     16
#define I2C_BUF_NUM          8
#define I2C_BUS_ID_DEF       -1

static int g_fd[OT_EIS_VI_MAX_PIPE_NUM] = {[0 ...(OT_EIS_VI_MAX_PIPE_NUM - 1)] = -1};
extern ot_eis_vi_slave_sns_sync_attr g_sc132gs_slave_sync[];
extern ot_s32 g_sc132gs_slave_bind_dev[];
extern const sc132gs_slave_video_mode_tbl g_sc132gs_slave_mode_tbl[];

int sc132gs_slave_i2c_init(ot_vi_pipe_id vi_pipe)
{
    int ret;
    char dev_file[I2C_DEV_FILE_NUM] = {0};
    ot_u8 dev_num;
    ot_eis_sns_bus_info *sc132gs_slave_businfo = OT_NULL;

    if (g_fd[vi_pipe] >= 0) {
        return OT_SUCCESS;
    }
    sc132gs_slave_businfo = sc132gs_slave_get_bus_info(vi_pipe);
    dev_num = sc132gs_slave_businfo->com_bus.i2c_dev;
    (ot_void)snprintf_s(dev_file, sizeof(dev_file), sizeof(dev_file) - 1, "/dev/i2c-%u", dev_num);

    g_fd[vi_pipe] = open(dev_file, O_RDWR, S_IRUSR | S_IWUSR);
    if (g_fd[vi_pipe] < 0) {
        sns_err_trace("Open /dev/ot_i2c_drv-%u error!\n", dev_num);
        return OT_FAILURE;
    }

    ret = ioctl(g_fd[vi_pipe], OT_EIS_I2C_SLAVE_FORCE, (sc132gs_slave_businfo->bus_addr >> 1));
    if (ret < 0) {
        sns_err_trace("I2C_SLAVE_FORCE error!\n");
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return ret;
    }

    return OT_SUCCESS;
}


int sc132gs_slave_i2c_exit(ot_vi_pipe_id vi_pipe)
{
    if (g_fd[vi_pipe] >= 0) {
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return OT_SUCCESS;
    }
    return OT_FAILURE;
}

ot_s32 sc132gs_slave_write_register(ot_vi_pipe_id vi_pipe, ot_u32 addr, ot_u32 data)
{
    ot_u32 idx = 0;
    ot_s32 ret;
    ot_u8 buf[I2C_BUF_NUM] = {0};

    if (g_fd[vi_pipe] < 0) {
        return OT_SUCCESS;
    }

    if (SC132GS_SLAVE_ADDR_BYTE == 2) {  /* 2 byte */
        buf[idx++] = (addr >> 8) & 0xff;  /* shift 8 */
        buf[idx++] = addr & 0xff;
    } else {
        buf[idx++] = addr & 0xff;
    }

    if (I2C_DATA_BYTE == 2) {  /* 2 byte */
        buf[idx++] = (data >> 8) & 0xff;  /* shift 8 */
        buf[idx++] = data & 0xff;
    } else {
        buf[idx++] = data & 0xff;
    }

    ot_eis_sns_bus_info *bus_info = sc132gs_slave_get_bus_info(vi_pipe);
    ret = write(g_fd[vi_pipe], buf, SC132GS_SLAVE_ADDR_BYTE + SC132GS_SLAVE_DATA_BYTE);
    if (ret < 0) {
        sns_err_trace("i2c%d devaddr 0x%x write 0x%x data 0x%x failed!\n",
            bus_info->com_bus.i2c_dev, bus_info->bus_addr, addr, data);
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

ot_s32 sc132gs_slave_read_register(ot_vi_pipe_id vi_pipe, ot_u32 addr, ot_u32 *data)
{
    ot_u32 idx = 0;
    ot_s32 ret;
    ot_u8 buf[I2C_BUF_NUM];
    ot_u8 buf_read[I2C_BUF_NUM];

    if (g_fd[vi_pipe] < 0) {
        return OT_SUCCESS;
    }

    if (SC132GS_SLAVE_ADDR_BYTE == 2) {  /* 2 byte */
        buf[idx++] = (addr >> 8) & 0xff;  /* shift 8 */
        buf[idx++] = addr & 0xff;
    } else {
        buf[idx++] = addr & 0xff;
    }

    ret = write(g_fd[vi_pipe], buf, SC132GS_SLAVE_ADDR_BYTE);
    if (ret < 0) {
        return OT_FAILURE;
    }

    ret = read(g_fd[vi_pipe], buf_read, SC132GS_SLAVE_DATA_BYTE);
    if (ret < 0) {
        return OT_FAILURE;
    }

    if (SC132GS_SLAVE_DATA_BYTE == 2) {  /* 2 byte */
        ot_u32 high = buf_read[0];
        ot_u32 low = buf_read[1];
        *data = (high << 8) + low;
    } else {
        *data = buf_read[0];
    }

    return OT_SUCCESS;
}

void sc132gs_slave_prog(ot_vi_pipe_id vi_pipe, const ot_u32 *rom)
{
    return;
}

void sc132gs_slave_standby(ot_vi_pipe_handle pipe_hdl)
{
    return;
}

void sc132gs_slave_restart(ot_vi_pipe_handle pipe_hdl)
{
    return;
}

static void sc132gs_slave_fme(ot_vi_pipe_id vi_pipe)              //for sensor fme ae_mode      in sensor_init:ox03f_init
{
}

ot_void sc132gs_slave_blc_clamp(ot_vi_pipe_handle pipe_hdl, ot_eis_sns_blc_clamp blc_clamp)
{
    ot_s32 ret = OT_SUCCESS;
    ot_vi_pipe_id vi_pipe[OT_EIS_VI_PIPE_ID_NUM_MAX] = { 0 };
    ot_s32 pipe_num = 0;
    ret = ot_eis_vi_get_pipe_id_by_pipe_hdl(pipe_hdl, vi_pipe, &pipe_num);
    if (ret != OT_SUCCESS) {
        return;
    }
    sc132gs_slave_set_blc_clamp_value(vi_pipe[0], blc_clamp.blc_clamp_en);
    return;
}

void sc132gs_slave_default_reg_init(ot_vi_pipe_id vi_pipe)
{
    ot_u32 i;
    ot_s32 ret = OT_SUCCESS;
    ot_eis_sns_state *pastsc132gs_slave = OT_NULL;

    pastsc132gs_slave = sc132gs_slave_get_ctx(vi_pipe);
    for (i = 0; i < pastsc132gs_slave->regs_info[0].reg_num; i++) {
        ret += sc132gs_slave_write_register(vi_pipe,
            pastsc132gs_slave->regs_info[0].i2c_data[i].reg_addr,
            pastsc132gs_slave->regs_info[0].i2c_data[i].data);
    }
    if (ret != OT_SUCCESS) {
        sns_err_trace("write register failed!\n");
    }
    return;
}

static void sc132gs_slave_exposure_cfg_i2c_seq_raw12(ot_vi_pipe_id vi_pipe)
{
    ot_u32 i;

    printf("Load 30fps raw12 1080x1280 trigger-exposure sequence!\n");
    for (i = 0; i < sizeof(g_sc132gs_slave_exposure_raw12_seq) / sizeof(g_sc132gs_slave_exposure_raw12_seq[0]); i++) {
        sc132gs_slave_write_register(vi_pipe, g_sc132gs_slave_exposure_raw12_seq[i][0], g_sc132gs_slave_exposure_raw12_seq[i][1]);
    }
}

static void sc132gs_slave_readout_cfg_i2c_seq_raw12(ot_vi_pipe_id vi_pipe)
{
    ot_u32 i;

    printf("Load 30fps raw12 1080x1280 trigger-readout sequence!\n");
    for (i = 0; i < sizeof(g_sc132gs_slave_readout_raw12_seq) / sizeof(g_sc132gs_slave_readout_raw12_seq[0]); i++) {
        sc132gs_slave_write_register(vi_pipe, g_sc132gs_slave_readout_raw12_seq[i][0], g_sc132gs_slave_readout_raw12_seq[i][1]);
    }
}

ot_s32 sc132gs_slave_get_i2c_dev_addr(ot_vi_pipe_id vi_pipe)
{
    ot_eis_sns_state *pastsc132gs_slave = sc132gs_slave_get_ctx(vi_pipe);
    ot_eis_sns_bus_info *sc132gs_slavebusinfo = sc132gs_slave_get_bus_info(vi_pipe);

    if (pastsc132gs_slave->init != OT_TRUE || sc132gs_slavebusinfo->bus_addr == 0)
        return OT_FAILURE;
    return sc132gs_slavebusinfo->bus_addr;
}

static ot_bool sc132gs_slave_get_quickboot_flag(ot_vi_pipe_id vi_pipe)
{
    ot_s32 ret;
    ot_vi_pipe_handle pipe_hdl;
    ot_eis_vi_quickboot_attr quickboot_attr = { 0 };

    ret = ot_eis_vi_get_pipe_hdl_by_pipe_id(vi_pipe, &pipe_hdl);
    if (ret != OT_SUCCESS) {
        sns_err_trace("get pipe_hdl failed!\n");
        return OT_FALSE;
    }

    ret = ot_eis_vi_pipe_get_quickboot_attr(pipe_hdl, &quickboot_attr);
    if (ret != OT_SUCCESS) {
        sns_err_trace("get quickboot attr failed!\n");
        return OT_FALSE;
    }

    return quickboot_attr.enable;
}

#define SC132GS_SLAVE_LOOP_CNT 200
static ot_bool sc132gs_slave_check_exist(ot_vi_pipe_id vi_pipe)
{
    ot_s32 ret;
    ot_u32 data = 0;

    for (int i = 0; i < SC132GS_SLAVE_LOOP_CNT; i++) {
        ret = sc132gs_slave_read_register(vi_pipe, 0x0100, &data);
        if (ret == OT_SUCCESS) {
            return OT_TRUE;
        }
        usleep(1000); // 1ms, total 200ms
    }
    sns_err_trace("sc132gs_slave pipe%d don't exist\n", vi_pipe);
    return OT_FALSE;
}

void sc132gs_slave_init(ot_vi_pipe_id vi_pipe)
{
    ot_bool init;
    ot_s32 ret;
    ot_eis_sns_state *pastsc132gs_slave = OT_NULL;
    ot_u8 image_mode;
    ot_bool quickboot_en;
    ot_s32 slave_dev;

    pastsc132gs_slave = sc132gs_slave_get_ctx(vi_pipe);
    init = pastsc132gs_slave->init;

    ret = sc132gs_slave_i2c_init(vi_pipe);
    if (ret != OT_SUCCESS) {
        sns_err_trace("i2c init failed!\n");
        return;
    }

    quickboot_en = sc132gs_slave_get_quickboot_flag(vi_pipe);
    image_mode = pastsc132gs_slave->img_mode;

    slave_dev = g_sc132gs_slave_bind_dev[vi_pipe];
    ret = ot_eis_vi_get_sns_slave_attr(slave_dev, &g_sc132gs_slave_sync[vi_pipe]);
    if (ret != OT_SUCCESS) {
        sns_err_trace("get slave attr failed!\n");
        return;
    }
    g_sc132gs_slave_sync[vi_pipe].cfg.bits.bit_h_inv = 0;
    g_sc132gs_slave_sync[vi_pipe].cfg.bits.bit_v_inv = 0;
    g_sc132gs_slave_sync[vi_pipe].cfg.bits.bit_h_enable = 0;
    g_sc132gs_slave_sync[vi_pipe].cfg.bits.bit_v_enable = 1;
    g_sc132gs_slave_sync[vi_pipe].vs_cyc = 3;
    g_sc132gs_slave_sync[vi_pipe].hs_cyc = 3;
    g_sc132gs_slave_sync[vi_pipe].hs_time = g_sc132gs_slave_mode_tbl[image_mode].inck_per_hs;
    g_sc132gs_slave_sync[vi_pipe].slave_mode_time = 0;
    ret = ot_eis_vi_set_sns_slave_attr(slave_dev, &g_sc132gs_slave_sync[vi_pipe]);
    if (ret != OT_SUCCESS) {
        sns_err_trace("set slave attr failed!\n");
        return;
    }


    /* When sensor first init, config all registers */
    if ((init == OT_FALSE) && (quickboot_en == OT_FALSE) && (sc132gs_slave_check_exist(vi_pipe) == OT_TRUE)) {
        if (image_mode < SC132GS_SLAVE_MODE_BUTT) {
            if (image_mode == SC132GS_SLAVE_EXPOSURE_1080X1280_30FPS_RAW12_LINEAR_MODE) {
                sc132gs_slave_exposure_cfg_i2c_seq_raw12(vi_pipe);
            } else {
                sc132gs_slave_readout_cfg_i2c_seq_raw12(vi_pipe);
            }
            sc132gs_slave_fme(vi_pipe);              //sensor FULL ME mode
        }
    }

    pastsc132gs_slave->init = OT_TRUE;
    return;
}

void sc132gs_slave_exit(ot_vi_pipe_id vi_pipe)
{
    ot_s32 ret;
    ot_eis_sns_state *pastsc132gs_slave = OT_NULL;

    ret = sc132gs_slave_i2c_exit(vi_pipe);
    if (ret != OT_SUCCESS) {
        sns_err_trace("sc132gs_slave exit failed!\n");
    }

    pastsc132gs_slave = sc132gs_slave_get_ctx(vi_pipe);
    pastsc132gs_slave->init = OT_FALSE;

    return;
}