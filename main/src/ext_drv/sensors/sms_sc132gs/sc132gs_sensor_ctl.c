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

#include "sc132gs_cmos.h"
#include "ot_eis_vi_api.h"
#include "sc132gs_30fps_raw12.h"

#define I2C_DEV_FILE_NUM     16
#define I2C_BUF_NUM          8
#define I2C_BUS_ID_DEF       -1

static int g_fd[OT_EIS_VI_MAX_PIPE_NUM] = {[0 ...(OT_EIS_VI_MAX_PIPE_NUM - 1)] = -1};

int sc132gs_i2c_init(ot_vi_pipe_id vi_pipe)
{
    if (g_fd[vi_pipe] >= 0) {
        return OT_SUCCESS;
    }
#ifdef OT_GPIO_I2C
    g_fd[vi_pipe] = open("/dev/gpioi2c_ex", O_RDONLY, S_IRUSR);
    if (g_fd[vi_pipe] < 0) {
        sns_err_trace("Open gpioi2c_ex error!\n");
        return OT_FAILURE;
    }
#else
    int ret;
    char dev_file[I2C_DEV_FILE_NUM] = {0};
    ot_u8 dev_num;
    ot_eis_sns_bus_info *sc132gs_businfo = OT_NULL;
    sc132gs_businfo = sc132gs_get_bus_info(vi_pipe);
    dev_num = sc132gs_businfo->com_bus.i2c_dev;
    (ot_void)snprintf_s(dev_file, sizeof(dev_file), sizeof(dev_file) - 1, "/dev/i2c-%u", dev_num);

    g_fd[vi_pipe] = open(dev_file, O_RDWR, S_IRUSR | S_IWUSR);
    if (g_fd[vi_pipe] < 0) {
        sns_err_trace("Open /dev/ot_i2c_drv-%u error!\n", dev_num);
        return OT_FAILURE;
    }

    ret = ioctl(g_fd[vi_pipe], OT_EIS_I2C_SLAVE_FORCE, (sc132gs_businfo->bus_addr >> 1));
    if (ret < 0) {
        sns_err_trace("I2C_SLAVE_FORCE error!\n");
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return ret;
    }
#endif

    return OT_SUCCESS;
}


int sc132gs_i2c_exit(ot_vi_pipe_id vi_pipe)
{
    if (g_fd[vi_pipe] >= 0) {
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return OT_SUCCESS;
    }
    return OT_FAILURE;
}

ot_s32 sc132gs_write_register(ot_vi_pipe_id vi_pipe, ot_u32 addr, ot_u32 data)
{
    if (g_fd[vi_pipe] < 0) {
        return OT_SUCCESS;
    }

#ifdef OT_GPIO_I2C
    i2c_data.dev_addr = SC132GS_I2C_ADDR;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = SC220AR_ADDR_BYTE;
    i2c_data.data = data;
    i2c_data.data_byte_num = SC132GS_DATA_BYTE;

    ret = ioctl(g_fd[vi_pipe], GPIO_I2C_WRITE, &i2c_data);
    if (ret) {
        sns_err_trace("GPIO-I2C write failed!\n");
        return ret;
    }
#else
    ot_u32 idx = 0;
    ot_s32 ret;
    ot_u8 buf[I2C_BUF_NUM];

    if (SC132GS_ADDR_BYTE == 2) {  /* 2 byte */
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

    ot_eis_sns_bus_info *bus_info = sc132gs_get_bus_info(vi_pipe);
    ret = write(g_fd[vi_pipe], buf, SC132GS_ADDR_BYTE + SC132GS_DATA_BYTE);
    if (ret < 0) {
        sns_err_trace("i2c%d devaddr 0x%x write 0x%x data 0x%x failed!\n",
            bus_info->com_bus.i2c_dev, bus_info->bus_addr, addr, data);
        return OT_FAILURE;
    }

#endif
    return OT_SUCCESS;
}

ot_s32 sc132gs_read_register(ot_vi_pipe_id vi_pipe, ot_u32 addr, ot_u32 *data)
{
    if (g_fd[vi_pipe] < 0) {
        return OT_SUCCESS;
    }

    ot_u32 idx = 0;
    ot_s32 ret;
    ot_u8 buf[I2C_BUF_NUM];
    ot_u8 buf_read[I2C_BUF_NUM];

    if (SC132GS_ADDR_BYTE == 2) {  /* 2 byte */
        buf[idx++] = (addr >> 8) & 0xff;  /* shift 8 */
        buf[idx++] = addr & 0xff;
    } else {
        buf[idx++] = addr & 0xff;
    }

    ret = write(g_fd[vi_pipe], buf, SC132GS_ADDR_BYTE);
    if (ret < 0) {
        return OT_FAILURE;
    }

    ret = read(g_fd[vi_pipe], buf_read, SC132GS_DATA_BYTE);
    if (ret < 0) {
        return OT_FAILURE;
    }

    if (SC132GS_DATA_BYTE == 2) {  /* 2 byte */
        ot_u32 high = buf_read[0];
        ot_u32 low = buf_read[1];
        *data = (high << 8) + low;
    } else {
        *data = buf_read[0];
    }

    return OT_SUCCESS;
}

void sc132gs_prog(ot_vi_pipe_id vi_pipe, const ot_u32 *rom)
{
    return;
}

void sc132gs_standby(ot_vi_pipe_handle pipe_hdl)
{
    return;
}

void sc132gs_restart(ot_vi_pipe_handle pipe_hdl)
{
    return;
}

static void sc132gs_fme(ot_vi_pipe_id vi_pipe)              //for sensor fme ae_mode      in sensor_init:ox03f_init
{
}

ot_void sc132gs_blc_clamp(ot_vi_pipe_handle pipe_hdl, ot_eis_sns_blc_clamp blc_clamp)
{
    ot_s32 ret = OT_SUCCESS;
    ot_vi_pipe_id vi_pipe[OT_EIS_VI_PIPE_ID_NUM_MAX] = { 0 };
    ot_s32 pipe_num = 0;
    ret = ot_eis_vi_get_pipe_id_by_pipe_hdl(pipe_hdl, vi_pipe, &pipe_num);
    if (ret != OT_SUCCESS) {
        return;
    }
    sc132gs_set_blc_clamp_value(vi_pipe[0], blc_clamp.blc_clamp_en);
    return;
}

void sc132gs_default_reg_init(ot_vi_pipe_id vi_pipe)
{
    ot_u32 i;
    ot_s32 ret = OT_SUCCESS;
    ot_eis_sns_state *pastsc132gs = OT_NULL;

    pastsc132gs = sc132gs_get_ctx(vi_pipe);
    for (i = 0; i < pastsc132gs->regs_info[0].reg_num; i++) {
        ret += sc132gs_write_register(vi_pipe,
            pastsc132gs->regs_info[0].i2c_data[i].reg_addr,
            pastsc132gs->regs_info[0].i2c_data[i].data);
    }
    if (ret != OT_SUCCESS) {
        sns_err_trace("write register failed!\n");
    }
    return;
}

static void sc132gs_cfg_i2c_seq_raw12(ot_vi_pipe_id vi_pipe)
{
    ot_u32 i;

    printf("Load 30fps raw12 1080x1280 sequence!\n");
    for (i = 0; i < sizeof(g_sc132gs_raw12_seq) / sizeof(g_sc132gs_raw12_seq[0]); i++) {
        sc132gs_write_register(vi_pipe, g_sc132gs_raw12_seq[i][0], g_sc132gs_raw12_seq[i][1]);
    }
}

ot_s32 sc132gs_get_i2c_dev_addr(ot_vi_pipe_id vi_pipe)
{
    ot_eis_sns_state *pastsc132gs = sc132gs_get_ctx(vi_pipe);
    ot_eis_sns_bus_info *sc132gsbusinfo = sc132gs_get_bus_info(vi_pipe);

    if (pastsc132gs->init != OT_TRUE || sc132gsbusinfo->bus_addr == 0)
        return OT_FAILURE;
    return sc132gsbusinfo->bus_addr;
}

static ot_bool sc132gs_get_quickboot_flag(ot_vi_pipe_id vi_pipe)
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

#define SC132GS_LOOP_CNT 200
static ot_bool sc132gs_check_exist(ot_vi_pipe_id vi_pipe)
{
    ot_s32 ret;
    ot_u32 data = 0;

    for (int i = 0; i < SC132GS_LOOP_CNT; i++) {
        ret = sc132gs_read_register(vi_pipe, 0x0100, &data);
        if (ret == OT_SUCCESS) {
            return OT_TRUE;
        }
        usleep(1000); // 1ms, total 200ms
    }
    sns_err_trace("sc132gs pipe%d don't exist\n", vi_pipe);
    return OT_FALSE;
}

void sc132gs_init(ot_vi_pipe_id vi_pipe)
{
    ot_bool init;
    ot_s32 ret;
    ot_eis_sns_state *pastsc132gs = OT_NULL;
    ot_u8 image_mode;
    ot_bool quickboot_en;

    pastsc132gs = sc132gs_get_ctx(vi_pipe);
    init = pastsc132gs->init;

    ret = sc132gs_i2c_init(vi_pipe);
    if (ret != OT_SUCCESS) {
        sns_err_trace("i2c init failed!\n");
        return;
    }

    quickboot_en = sc132gs_get_quickboot_flag(vi_pipe);
    image_mode = pastsc132gs->img_mode;

    /* When sensor first init, config all registers */
    if ((init == OT_FALSE) && (quickboot_en == OT_FALSE) && (sc132gs_check_exist(vi_pipe) == OT_TRUE)) {
        if (image_mode < SC132GS_MODE_BUTT) {
            sc132gs_fme(vi_pipe);              //sensor FULL ME mode
            if (image_mode == SC132GS_1080X1280_30FPS_RAW12_LINEAR_MODE) {
                sc132gs_cfg_i2c_seq_raw12(vi_pipe);
            }
        }
    }

    pastsc132gs->init = OT_TRUE;
    return;
}

void sc132gs_exit(ot_vi_pipe_id vi_pipe)
{
    ot_s32 ret;
    ot_eis_sns_state *pastsc132gs = OT_NULL;

    ret = sc132gs_i2c_exit(vi_pipe);
    if (ret != OT_SUCCESS) {
        sns_err_trace("sc132gs exit failed!\n");
    }

    pastsc132gs = sc132gs_get_ctx(vi_pipe);
    pastsc132gs->init = OT_FALSE;

    return;
}
