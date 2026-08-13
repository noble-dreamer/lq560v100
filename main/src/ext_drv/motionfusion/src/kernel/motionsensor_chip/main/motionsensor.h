/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef MOTIONSENSOR_H
#define MOTIONSENSOR_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include "ot_common_motionsensor.h"
#include "module/osal_workqueue.h"
#include "module/osal_semaphore.h"
#include "module/osal_wait.h"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#define MSENSOR_DEV_NAME "motionsensor_chip"

#define DATA_RATE_RESERVED 0x00
#define DATA_RATE_25HZ     0x06
#define DATA_RATE_50HZ     0x07
#define DATA_RATE_100HZ    0x08
#define DATA_RATE_200HZ    0x09
#define DATA_RATE_400HZ    0x0A
#define DATA_RATE_800HZ    0x0B
#define DATA_RATE_1600HZ   0x0C
#define DATA_RATE_3200HZ   0x0D

#define GYRO_FULL_SCALE_RANGE_250DPS (250 << 10)
#define GYRO_FULL_SCALE_RANGE_500DPS (500 << 10)
#define GYRO_FULL_SCALE_RANGE_1KDPS  (1000 << 10)
#define GYRO_FULL_SCALE_RANGE_2KDPS  (2000 << 10)
#define GYRO_FULL_SCALE_RANGE_31DPS  ((3125 << 10) / 100)
#define GYRO_FULL_SCALE_RANGE_62DPS  ((625 << 10) / 10)
#define GYRO_FULL_SCALE_RANGE_125DPS (125 << 10)

#define ACCEL_UI_FULL_SCALE_RANGE_2G  1
#define ACCEL_UI_FULL_SCALE_RANGE_4G  2
#define ACCEL_UI_FULL_SCALE_RANGE_8G  3
#define ACCEL_UI_FULL_SCALE_RANGE_16G 4

#define ACCEL_OIS_FULL_SCALE_RANGE_1G 5
#define ACCEL_OIS_FULL_SCALE_RANGE_2G 6
#define ACCEL_OIS_FULL_SCALE_RANGE_4G 7
#define ACCEL_OIS_FULL_SCALE_RANGE_8G 8

#define MOTIONSENSOR_MAX_TEMP 85
#define MOTIONSENSOR_MIN_TEMP (-40)

typedef enum {
    GYRO_OUTPUT_DATA_RATE_32KHZ = 32000,
    GYRO_OUTPUT_DATA_RATE_8KHZ = 8000,
    GYRO_OUTPUT_DATA_RATE_3200HZ = 3200,
    GYRO_OUTPUT_DATA_RATE_1600HZ = 1600,
    GYRO_OUTPUT_DATA_RATE_800HZ = 800,
    GYRO_OUTPUT_DATA_RATE_400HZ = 400,
    GYRO_OUTPUT_DATA_RATE_200HZ = 200,
    GYRO_OUTPUT_DATA_RATE_100HZ = 100,
    GYRO_OUTPUT_DATA_RATE_50HZ = 50,
    GYRO_OUTPUT_DATA_RATE_25HZ = 25,
    GYRO_OUTPUT_DATA_RATE_UNDER_1KHZ,
    GYRO_OUTPUT_DATA_RATE_BUTT
} msensor_gyro_output_data_rate;

typedef enum {
    ACCEL_OUTPUT_DATA_RATE_4KHZ = 4000,
    ACCEL_OUTPUT_DATA_RATE_1KHZ = 1000,
    ACCEL_OUTPUT_DATA_RATE_1600HZ = 1600,
    ACCEL_OUTPUT_DATA_RATE_800HZ = 800,
    ACCEL_OUTPUT_DATA_RATE_400HZ = 400,
    ACCEL_OUTPUT_DATA_RATE_200HZ = 200,
    ACCEL_OUTPUT_DATA_RATE_100HZ = 100,
    ACCEL_OUTPUT_DATA_RATE_50HZ = 50,
    ACCEL_OUTPUT_DATA_RATE_25HZ = 25,
    ACCEL_OUTPUT_DATA_RATE_UNDER_1KHZ,
    ACCEL_OUTPUT_DATA_RATE_BUTT
} msensor_accel_output_data_rate;

typedef enum {
    GYRO_FULL_SCALE_SET_250DPS = 250 << 10,         /* 250 dps */
    GYRO_FULL_SCALE_SET_500DPS = 500 << 10,         /* 500 dps */
    GYRO_FULL_SCALE_SET_1KDPS = 1000 << 10,         /* 1000 dps */
    GYRO_FULL_SCALE_SET_2KDPS = 2000 << 10,         /* 2000 dps */
    GYRO_FULL_SCALE_SET_31DPS = (3125 << 10) / 100, /* 31.25 dps : 3125 / 100 */
    GYRO_FULL_SCALE_SET_62DPS = (625 << 10) / 10,   /* 62.5 dps : 625 / 10 */
    GYRO_FULL_SCALE_SET_125DPS = 125 << 10,         /* 125 dps */
    GYRO_FULL_SCALE_SET_BUTT
} msensor_gyro_full_scale_range;

typedef enum {
    ACCEL_UI_FULL_SCALE_SET_2G = 2 << 10,
    ACCEL_UI_FULL_SCALE_SET_4G = 4 << 10,
    ACCEL_UI_FULL_SCALE_SET_8G = 8 << 10,
    ACCEL_UI_FULL_SCALE_SET_16G = 16 << 10,
    ACCEL_UI_FULL_SCALE_SET_BUTT
} msensor_accel_ui_full_scale_range;

typedef enum {
    ACCEL_OIS_FULL_SCALE_SET_1G = 1,
    ACCEL_OIS_FULL_SCALE_SET_2G = 2,
    ACCEL_OIS_FULL_SCALE_SET_4G = 4,
    ACCEL_OIS_FULL_SCALE_SET_8G = 8,
    ACCEL_OIS_FULL_SCALE_SET_BUTT
} msensor_accel_ois_full_scale_range;

typedef enum {
    TRIGER_TIMER = 0,
    TRIGER_EXTERN_INTERRUPT,
    TRIGER_BUTT
} msensor_triger_mode;

typedef struct {
    ot_u32 interval; /* unit :us */
} timer_param;

typedef struct {
    ot_u32 interrupt_num;
} extern_interrupt_param;

typedef union {
    timer_param timer_config;
    extern_interrupt_param extern_interrupt_config;
} msensor_triger_info;

typedef struct {
    msensor_triger_mode triger_mode;
    msensor_triger_info triger_info;
} triger_config;

typedef struct {
    ot_u32 cmd;
    ot_s32 (*func)(uintptr_t arg);
} msensor_chip_ioctl_info;

typedef struct {
    ot_msensor_gyro_config gyro_config;
    ot_u32 band_width;
    ot_u64 last_pts;
} msensor_gyro_status;

typedef struct {
    ot_msensor_acc_config acc_config;
    ot_u32 band_width;
    ot_u64 last_pts;
} msensor_acc_status;

typedef struct {
    ot_msensor_magn_config magn_config;
    ot_u32 band_width;
    ot_u64 last_pts;
} msensor_magn_status;

typedef struct {
    ot_msensor_sample_data accel_cur_data;
    ot_msensor_sample_data gyro_cur_data;
    ot_msensor_sample_data magn_data;
    struct i2c_client *client;
    struct spi_device *ot_spi;
    struct task_struct *read_data_task;
    struct task_struct *get_data_kthread;
    osal_workqueue work;
    ot_u8 power_mode;
    ot_u8 flag_acc_fifo_enabled;
    ot_u8 flag_gyro_fifo_enabled;
    ot_u8 flag_fifo_incomming;
    ot_u8 *fifo_buf;
    ot_u32 fifo_length;
    ot_u8 record_num;
    ot_u8 enable_kthread;
    ot_u8 fifo_en;
    triger_config triger_data;
    struct hrtimer hrtimer;
    ot_u8 thread_wakeup;
    osal_semaphore g_sem;
    msensor_acc_status acc_status;
    msensor_gyro_status gyro_status;
    msensor_magn_status magn_status;
    ot_s32 temperature;
    ot_s32 irq_num;
    ot_s32 workqueue_call_times;
    osal_wait wait_call_stop_working;
} imu_dev_info;

ot_msensor_param *msensor_chip_get_param(ot_void);

ot_s32 msensor_chip_int_callback(ot_msensor_data *msensor_data);

#define print_info(fmt, arg...) \
    osal_printk("fun:%s,%d " fmt, __func__, __LINE__, ##arg) \

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif
