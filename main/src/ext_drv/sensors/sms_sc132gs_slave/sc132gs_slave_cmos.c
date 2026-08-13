/*
  Copyright (c), 2021-2024, Oritek Tech. Co., Ltd.
*/

#include <stdio.h>
#include "ot_eis_errno.h"
#include "ot_eis_vi_sns.h"
#include "sc132gs_slave_cmos.h"
#include "sc132gs_slave_cmos_ex.h"
#include "ot_eis_vi_api.h"
#include "ot_eis_vi_alg_api.h"
#include "ot_eis_vi_ae_api.h"
#include "ot_eis_vi_awb_api.h"
#include "ot_eis_vi_3a.h"
#include "math.h"

#define SC132GS_SLAVE_ID   132

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/
#define SC132GS_SLAVE_SHT_min_row 			    (0)
#define SC132GS_SLAVE_SHT_max_row_offset 		(8)
#define SC132GS_SLAVE_SHT_max_row_vs 		    (0xb0)
#if 0
#define SC132GS_SLAVE_SHT_DCG_H            	(0x5e88)
#define SC132GS_SLAVE_SHT_DCG_L            	(0x5e89)
#define SC132GS_SLAVE_SHT_VS_H             	(0x5e8c)
#define SC132GS_SLAVE_SHT_VS_L             	(0x5e8d)

#define SC132GS_SLAVE_AGAIN_HCG_H         	(0x5e7c)
#define SC132GS_SLAVE_AGAIN_HCG_L         	(0x5e7d)
#define SC132GS_SLAVE_AGAIN_LCG_H         	(0x5e7e)
#define SC132GS_SLAVE_AGAIN_LCG_L        		(0x5e7f)
#define SC132GS_SLAVE_AGAIN_VS_H          	(0x5e80)
#define SC132GS_SLAVE_AGAIN_VS_L          	(0x5e81)

#define SC132GS_SLAVE_VMAX_H                  (0x320e)
#define SC132GS_SLAVE_VMAX_L                  (0x320f)

#define SC132GS_SLAVE_AWB_B_GAIN              (0x5ac8)
#define SC132GS_SLAVE_AWB_R_GAIN              (0x5ac9)
#endif
#define higher_4bits(x) (((x) & 0xf0000) >> 16)
#define high_8bits(x) (((x) & 0xff00) >> 8)
#define low_8bits(x)  ((x) & 0x00ff)
#define high_8_in_24bits(x) (((x) & 0xff0000) >> 16)
#define mid_8_in_24bits(x) (((x) & 0x00ff00) >> 8)
#define low_8_in_24bits(x) ((x) & 0x0000ff)

#define SC132GS_SLAVE_SHT_COARSE_L                       (0x3e02)
#define SC132GS_SLAVE_SHT_COARSE_M                       (0x3e01)
#define SC132GS_SLAVE_SHT_COARSE_H                       (0x3e00)


#define SC132GS_SLAVE_AGAIN_L                         (0x3e09)
#define SC132GS_SLAVE_AGAIN_H                         (0x3e08)
#define SC132GS_SLAVE_DGAIN_L                         (0x3e07)
#define SC132GS_SLAVE_DGAIN_H                         (0x3e06)


#define SC132GS_SLAVE_VMAX_L                           (0x320F)
#define SC132GS_SLAVE_VMAX_H                           (0x320E)

#define high_8bits(x) (((x) & 0xff00) >> 8)
#define low_8bits(x)  ((x) & 0x00ff)

#define MAX2(x, y)       ((x) > (y) ? (x) : (y))
#define MIN2(x, y)       ((x) < (y) ? (x) : (y))

/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/
#define sc132gs_slave_sensor_set_ctx(pipe, ctx)   ((g_sc132gs_slave_sns_state[pipe]) = (ctx))
#define sc132gs_slave_sensor_reset_ctx(pipe)      (g_sc132gs_slave_sns_state[pipe] = OT_NULL)

static ot_eis_vi_fswdr_mode g_fswdr_mode[OT_EIS_VI_MAX_PIPE_NUM] = {
    [0 ... OT_EIS_VI_MAX_PIPE_NUM - 1] = OT_EIS_VI_FSWDR_NORMAL_MODE
};

static ot_u32 g_max_time_get_cnt[OT_EIS_VI_MAX_PIPE_NUM] = {0};
static ot_u32 g_init_exposure[OT_EIS_VI_MAX_PIPE_NUM]  = {0};
static ot_u32 g_init_int_time[OT_EIS_VI_MAX_PIPE_NUM]  = {0};
static ot_u32 g_init_again[OT_EIS_VI_MAX_PIPE_NUM]  = {0};
static ot_u32 g_init_isp_dgain[OT_EIS_VI_MAX_PIPE_NUM]  = {0};
static ot_u32 g_lines_per500ms[OT_EIS_VI_MAX_PIPE_NUM] = {0};
static ot_u8 g_ae_stat_pos[OT_EIS_VI_MAX_PIPE_NUM] = {0};

static ot_u16 g_init_wb_gain[OT_EIS_VI_MAX_PIPE_NUM][OT_EIS_VI_RGB_CHN_NUM] = {{0}};
static ot_u16 g_sample_r_gain[OT_EIS_VI_MAX_PIPE_NUM] = {0};
static ot_u16 g_sample_b_gain[OT_EIS_VI_MAX_PIPE_NUM] = {0};
static ot_bool g_quick_start_en[OT_EIS_VI_MAX_PIPE_NUM] = {OT_FALSE};

static ot_bool g_ae_route_ex_valid[OT_EIS_VI_MAX_PIPE_NUM] = {0};
static ot_eis_vi_ae_route g_init_ae_route[OT_EIS_VI_MAX_PIPE_NUM] = {{0}};
static ot_eis_vi_ae_route_ex g_init_ae_route_ex[OT_EIS_VI_MAX_PIPE_NUM] = {{0}};
static ot_eis_vi_ae_route g_init_ae_route_sf[OT_EIS_VI_MAX_PIPE_NUM] = {{0}};
static ot_eis_vi_ae_route_ex g_init_ae_route_sf_ex[OT_EIS_VI_MAX_PIPE_NUM] = {{0}};

static ot_eis_vi_sns_alg_default  g_ext_sns_alg[OT_EIS_VI_MAX_PIPE_NUM] = {0};
static ot_eis_vi_awb_sensor_default g_ext_sns_awb[OT_EIS_VI_MAX_PIPE_NUM] = {{0}};
static ot_eis_sns_reg_table  g_ext_table[OT_EIS_VI_MAX_PIPE_NUM][5] = {0};
static ot_u8  g_ext_alg_flag[OT_EIS_VI_MAX_PIPE_NUM] = { 0 };
static ot_u8  g_ext_awb_flag[OT_EIS_VI_MAX_PIPE_NUM] = { 0 };
static ot_u8  g_ext_sensor_table_flag[OT_EIS_VI_MAX_PIPE_NUM] = { 0 };

ot_eis_sns_bus_info g_sc132gs_slave_bus_info[OT_EIS_VI_MAX_PIPE_NUM] = {
        [0] = {
            .com_bus.i2c_dev = 0,
            .bus_addr = SC132GS_SLAVE_I2C_ADDR
        },
        [1 ... OT_EIS_VI_MAX_PIPE_NUM - 1] = {
            .com_bus.i2c_dev = -1,
            .bus_addr = SC132GS_SLAVE_I2C_ADDR
        }
};

ot_eis_vi_slave_sns_sync_attr g_sc132gs_slave_sync[OT_EIS_VI_MAX_PIPE_NUM] = {0};
ot_s32 g_sc132gs_slave_bind_dev[OT_EIS_VI_MAX_PIPE_NUM] = {0, 0, 1, 1, 2, 2, 3};

ot_eis_sns_state *g_sc132gs_slave_sns_state[OT_EIS_VI_MAX_PIPE_NUM] = {OT_NULL};

static ot_bool blc_clamp_info[OT_EIS_VI_MAX_PIPE_NUM] = {[0 ...(OT_EIS_VI_MAX_PIPE_NUM - 1)] = OT_TRUE};

ot_eis_sns_reg_table *sc132gs_slave_get_ext_sensor_table(ot_vi_pipe_id vi_pipe)
{
    return &g_ext_table[vi_pipe][0];
}

ot_u8 sc132gs_slave_get_ext_sensor_flag(ot_vi_pipe_id vi_pipe)
{
    return g_ext_sensor_table_flag[vi_pipe];
}

ot_eis_sns_bus_info *sc132gs_slave_get_bus_info(ot_vi_pipe_id vi_pipe)
{
    return &g_sc132gs_slave_bus_info[vi_pipe];
}

ot_eis_sns_state *sc132gs_slave_get_ctx(ot_vi_pipe_id vi_pipe)
{
    return g_sc132gs_slave_sns_state[vi_pipe];
}

ot_void sc132gs_slave_set_blc_clamp_value(ot_vi_pipe_id vi_pipe, ot_bool clamp_en)
{
    blc_clamp_info[vi_pipe] = clamp_en;
}

const sc132gs_slave_video_mode_tbl g_sc132gs_slave_mode_tbl[SC132GS_SLAVE_MODE_BUTT]  = {
    {24000000, 355, 800000, SC132GS_SLAVE_VMAX_1080X1280_30FPS_LINEAR,  SC132GS_SLAVE_FULL_LINES_MAX_LINEAR,
     30, 0.5, 1080, 1280, 0, OT_EIS_WDR_MODE_NONE,  "SC132GS_SLAVE_EXPOSURE_1080X1280_30FPS_RAW12_LINEAR_MODE"},
    {24000000, 355, 800000, SC132GS_SLAVE_VMAX_1080X1280_30FPS_LINEAR,  SC132GS_SLAVE_FULL_LINES_MAX_LINEAR,
     30, 0.5, 1080, 1280, 1, OT_EIS_WDR_MODE_NONE,  "SC132GS_SLAVE_READOUT_1080X1280_30FPS_RAW12_LINEAR_MODE"},
};

#define sc132gs_slave_err_mode_print(sns_image_mode, sns_state) \
do { \
    sns_err_trace("Not support! Width:%u, Height:%u, Fps:%f, WDRMode:%d\n", \
                  (sns_image_mode)->width, \
                  (sns_image_mode)->height, \
                  (sns_image_mode)->fps, \
                  (sns_state)->wdr_mode); \
} while (0)

static ot_void cmos_get_ae_comm_default(ot_vi_pipe_id vi_pipe, ot_eis_vi_ae_sensor_default *ae_sns_dft,
    const ot_eis_sns_state *sns_state)
{

    ae_sns_dft->full_lines_std = sns_state->fl_std;
    ae_sns_dft->flicker_freq = 50 * 256; /* light flicker freq: 50Hz, accuracy: 256 */
    ae_sns_dft->full_lines_max = SC132GS_SLAVE_FULL_LINES_MAX_LINEAR;
    ae_sns_dft->hmax_times = (1000000000) / (sns_state->fl_std * 30); /* 1000000000ns, 30fps */

    ae_sns_dft->again_accu.accu_type = OT_EIS_VI_AE_ACCURACY_TABLE;
    ae_sns_dft->again_accu.accuracy  = 1;

    ae_sns_dft->isp_dgain_shift = 8; /* accuracy: 8 */
    ae_sns_dft->min_isp_dgain_target = 1 << ae_sns_dft->isp_dgain_shift;
    ae_sns_dft->max_isp_dgain_target = 12 << ae_sns_dft->isp_dgain_shift; /* max 12 */
    if (g_lines_per500ms[vi_pipe] == 0) {
        ae_sns_dft->lines_per500ms = sns_state->fl_std * 30 / 2; /* 30fps, div 2 */
    } else {
        ae_sns_dft->lines_per500ms = g_lines_per500ms[vi_pipe];
    }
    ae_sns_dft->max_iris_fno = OT_EIS_VI_IRIS_F_NO_1_0;
    ae_sns_dft->min_iris_fno = OT_EIS_VI_IRIS_F_NO_32_0;

    ae_sns_dft->ae_route_ex_valid = OT_FALSE;
    ae_sns_dft->ae_route_attr.total_num = 0;
    ae_sns_dft->ae_route_attr_ex.total_num = 0;
    ae_sns_dft->quick_start.quick_start_enable = g_quick_start_en[vi_pipe];
    ae_sns_dft->quick_start.black_frame_num = 0;
    ae_sns_dft->ae_stat_pos = g_ae_stat_pos[vi_pipe]; /* 1 use be stat to AE */
    return;
}

static ot_void cmos_get_ae_built_in_wdr_default(ot_vi_pipe_id vi_pipe, ot_eis_vi_ae_sensor_default *ae_sns_dft,
    const ot_eis_sns_state *sns_state)
{

    ae_sns_dft->int_time_accu.accu_type = OT_EIS_VI_AE_ACCURACY_LINEAR;
    ae_sns_dft->int_time_accu.accuracy = 1; /* accuracy 1 */
    ae_sns_dft->int_time_accu.offset = 0;
    ae_sns_dft->max_again = 29232; /* max 16384 */
    ae_sns_dft->min_again = 1024;  /* min 1024 */
    ae_sns_dft->max_again_target = ae_sns_dft->max_again;
    ae_sns_dft->min_again_target = ae_sns_dft->min_again;

    ae_sns_dft->max_dgain = 32256; /* max 2047 */ //31.5x
    ae_sns_dft->min_dgain = 1024;  /* min 128 */
    ae_sns_dft->max_dgain_target = ae_sns_dft->max_dgain;
    ae_sns_dft->min_dgain_target = ae_sns_dft->min_dgain;

    ae_sns_dft->isp_dgain_shift = 8; /* accuracy: 8 */
    ae_sns_dft->min_isp_dgain_target = 1 << ae_sns_dft->isp_dgain_shift;
    ae_sns_dft->max_isp_dgain_target = 16 << ae_sns_dft->isp_dgain_shift;

    ae_sns_dft->ae_compensation = 0x30;
    ae_sns_dft->ae_exp_mode = OT_EIS_VI_AE_EXP_HIGHLIGHT_PRIOR;

    ae_sns_dft->init_exposure = g_init_exposure[vi_pipe] ? g_init_exposure[vi_pipe] : 76151; /* init 76151 */
    ae_sns_dft->init_int_time = g_init_int_time[vi_pipe];
    ae_sns_dft->init_again = g_init_again[vi_pipe];
    ae_sns_dft->init_isp_dgain = g_init_isp_dgain[vi_pipe];

    ae_sns_dft->max_int_time = sns_state->fl_std - SC132GS_SLAVE_SHT_max_row_offset;
    ae_sns_dft->min_int_time = SC132GS_SLAVE_SHT_min_row;
    ae_sns_dft->max_int_time_target = 65535;
    ae_sns_dft->min_int_time_target = SC132GS_SLAVE_SHT_min_row;

    ae_sns_dft->ae_route_ex_valid = g_ae_route_ex_valid[vi_pipe];

    (ot_void)memcpy_s(&ae_sns_dft->ae_route_attr, sizeof(ot_eis_vi_ae_route),
                      &g_init_ae_route[vi_pipe],  sizeof(ot_eis_vi_ae_route));
    (ot_void)memcpy_s(&ae_sns_dft->ae_route_attr_ex, sizeof(ot_eis_vi_ae_route_ex),
                      &g_init_ae_route_ex[vi_pipe], sizeof(ot_eis_vi_ae_route_ex));

    return;
}

static ot_s32 cmos_get_ae_default(ot_vi_pipe_id vi_pipe, ot_eis_vi_ae_sensor_default *ae_sns_dft)
{
    ot_eis_sns_state *sns_state = OT_NULL;

    sns_check_pointer_return(ae_sns_dft);
    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    (ot_void)memset_s(&ae_sns_dft->ae_route_attr, sizeof(ot_eis_vi_ae_route), 0, sizeof(ot_eis_vi_ae_route));

    cmos_get_ae_comm_default(vi_pipe, ae_sns_dft, sns_state);

    switch (sns_state->wdr_mode){
        case OT_EIS_WDR_MODE_NONE:   /* linear mode */
            cmos_get_ae_built_in_wdr_default(vi_pipe, ae_sns_dft, sns_state);
            break;
        default:
            cmos_get_ae_built_in_wdr_default(vi_pipe, ae_sns_dft, sns_state);
            break;
    }

    return OT_SUCCESS;
}

static ot_void cmos_config_vmax(ot_eis_sns_state *sns_state, ot_u32 vmax)
{
    if (sns_state->wdr_mode == OT_EIS_WDR_MODE_NONE) {
        sns_state->regs_info[0].i2c_data[7].data = low_8bits(vmax);
        sns_state->regs_info[0].i2c_data[8].data = high_8bits(vmax);
        //printf("20250915 cmos_config_vmax = 0x%x\n", vmax);
    } else {
        printf("invalid wdr_mode:%d\n",sns_state->wdr_mode);
    }

    return;
}

static ot_void cmos_fps_set(ot_vi_pipe_id vi_pipe, ot_float fps, ot_eis_vi_ae_sensor_default *ae_sns_dft)
{
    ot_u32 lines, lines_max, vmax;
    ot_bool achieve_fps_flag;
    ot_float max_fps, min_fps;
    ot_eis_sns_state *sns_state = OT_NULL;

    sns_check_pointer_void_return(ae_sns_dft);
    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    lines = g_sc132gs_slave_mode_tbl[sns_state->img_mode].ver_lines;
    lines_max = g_sc132gs_slave_mode_tbl[sns_state->img_mode].max_ver_lines;
    max_fps = g_sc132gs_slave_mode_tbl[sns_state->img_mode].max_fps;
    min_fps = g_sc132gs_slave_mode_tbl[sns_state->img_mode].min_fps;

    if ((fps > max_fps) || (fps < min_fps)) {
        sns_err_trace("Not support Fps: %f\n", fps);
        return;
    }

    achieve_fps_flag = OT_TRUE;
    vmax = (ot_u32)((lines * max_fps + (fps/2) ) / sns_div_0_to_1_float(fps));  //round
    vmax = (vmax > lines_max) ? lines_max : vmax;
    cmos_config_vmax(sns_state, vmax);

    sns_state->fl_std = vmax;
    ae_sns_dft->lines_per500ms = lines * max_fps / 2; /* div 2 */
    g_lines_per500ms[vi_pipe] = ae_sns_dft->lines_per500ms;

    ae_sns_dft->fps = lines * max_fps * 0x40 / vmax / 0x40;
    ae_sns_dft->fps = (achieve_fps_flag) ? fps : (ae_sns_dft->fps);
    ae_sns_dft->full_lines_std = sns_state->fl_std;
    ae_sns_dft->max_int_time = sns_state->fl_std - SC132GS_SLAVE_SHT_max_row_offset;
    sns_state->fl[0] = sns_state->fl_std;
    ae_sns_dft->full_lines = sns_state->fl[0];
    ae_sns_dft->hmax_times =
        (ot_u32)((1000000000) / (sns_state->fl_std * sns_div_0_to_1_float(fps))); /* 1000000000ns */

    ot_eis_vi_get_sns_slave_attr(g_sc132gs_slave_bind_dev[vi_pipe], &g_sc132gs_slave_sync[vi_pipe]);
    g_sc132gs_slave_sync[vi_pipe].vs_time =
        g_sc132gs_slave_mode_tbl[sns_state->img_mode].inck_per_vs * (max_fps / sns_div_0_to_1_float(fps));
    ot_eis_vi_set_sns_slave_attr(g_sc132gs_slave_bind_dev[vi_pipe], &g_sc132gs_slave_sync[vi_pipe]);

    sns_state->regs_info[0].slv_sync.slave_vs_time = g_sc132gs_slave_sync[vi_pipe].vs_time;

    return;
}

static ot_void cmos_slow_framerate_set(ot_vi_pipe_id vi_pipe, ot_u32 full_lines, ot_eis_vi_ae_sensor_default *ae_sns_dft)
{
    ot_eis_sns_state *sns_state = OT_NULL;

    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    return;
}

static ot_void cmos_inttime_update_built_in_wdr(ot_vi_pipe_id vi_pipe, ot_u32 int_time)
{
    ot_eis_sns_state *sns_state = OT_NULL;
    ot_u32 value;

    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    value = int_time*16;

    sns_state->regs_info[0].i2c_data[0].data = low_8_in_24bits(value);
	sns_state->regs_info[0].i2c_data[1].data = mid_8_in_24bits(value);
    sns_state->regs_info[0].i2c_data[2].data = higher_4bits(value);
    //printf("20250915 cmos inttime_update exp = 0x%x\n", value);
    return;
}

/* while vi notify ae to update sensor regs, ae call these funcs. */
static ot_void cmos_inttime_update(ot_vi_pipe_id vi_pipe, ot_u32 int_time)
{
    ot_eis_sns_state *sns_state = OT_NULL;

    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    if (sns_state->wdr_mode == OT_EIS_WDR_MODE_NONE) {
        cmos_inttime_update_built_in_wdr(vi_pipe, int_time);
    }

    return;
}

#define AGAIN_NODE_NUM   154
static ot_u32 g_again_table[AGAIN_NODE_NUM] = {
1024, 1055, 1088, 1120, 1150, 1183, 1216, 1248, 1280, 1311, 1344, 1376, 1408, 1439, 1472, 1504, 1536, 1567, 1600, 1632,
1664, 1695, 1728, 1760, 1792, 1823, 1856, 1913, 1972, 2029, 2087, 2146, 2203, 2262, 2320, 2377, 2436, 2494, 2551, 2610,
2667, 2725, 2784, 2841, 2899, 2958, 2913, 3074, 3132, 3189, 3248, 3306, 3363, 3422, 3479, 3537, 3596, 3653, 3712, 3827,
3944, 4060, 4175, 4291, 4408, 4524, 4639, 4756, 4872, 4987, 5103, 5220, 5336, 5451, 5568, 5684, 5799, 5915, 6032, 6148,
6263, 6379, 6496, 6611, 6727, 6844, 6960, 7075, 7191, 7308, 7424, 7656, 7887, 8120, 8351, 8584, 8815, 9048, 9280, 9511,
9744, 9975, 10208, 10439, 10672, 10903, 11136, 11368, 11599, 11832, 12063, 12296, 12527, 12760, 12992, 13223, 13456, 13687, 13920, 14151,
14384, 14615, 14848, 15311, 15775, 16239, 16704, 17168, 17632, 18096, 18560, 19023, 19487, 19951, 20416, 20880, 21344, 21808, 22272, 22735,
23199, 23663, 24128, 24592, 25056, 25520, 25984, 26447, 26891, 27375, 27840, 28304, 28768, 29232
};

#if 0
static ot_float g_againdb_table[AGAIN_NODE_NUM] = {
0.00,  0.27,  0.53,  0.78, 1.02,  1.26,  1.50,  1.72, 1.94,  2.15,  2.37,  2.57, 2.77,  2.96,  3.16,  3.34, 3.52,  3.70,  3.88,  4.05,
4.22,  4.38,  4.55,  4.71, 4.86,  5.01,  5.17,  5.43, 5.69,  5.94,  6.19,  6.43, 6.66,  6.88,  7.11,  7.32, 7.53,  7.73,  7.93,  8.13,
8.32,  8.50,  8.69,  8.87, 9.04,  9.21,  9.08,  9.55, 9.71,  9.87,  10.03,  10.18, 10.33,  10.48, 10.62,  10.77, 10.91,  11.05, 11.19,  11.45,
11.71,  11.96, 12.21,  12.45, 12.68,  12.90, 13.12,  13.34, 13.55,  13.75, 13.95,  14.15, 14.34,  14.52, 14.71,  14.89, 15.06,  15.23, 15.40,  15.57,
15.73,  15.89, 16.05,  16.20, 16.35,  16.50, 16.65,  16.79, 16.93,  17.07, 17.21,  17.47, 17.73,  17.99, 18.23,  18.47, 18.70,  18.93, 19.15,  19.36,
19.57,  19.77, 19.97,  20.17, 20.36,  20.55, 20.73,  20.91, 21.08,  21.26, 21.42,  21.59, 21.75,  21.91, 22.07,  22.22, 22.37,  22.52, 22.67,  22.81,
22.95,  23.09, 23.23,  23.49, 23.75,  24.01, 24.25,  24.49, 24.72,  24.95, 25.17,  25.38, 25.59,  25.79, 25.99,  26.19, 26.38,  26.57, 26.75,  26.93,
27.10,  27.28, 27.44,  27.61, 27.77,  27.93, 28.09,  28.24, 28.39,  28.54, 28.69,  28.83, 28.97,  29.11
};
#endif

static ot_u32 g_again_register_table[AGAIN_NODE_NUM] = {
0x0320, 0x0321, 0x0322, 0x0323, 0x0324, 0x0325, 0x0326, 0x0327, 0x0328, 0x0329, 0x032a, 0x032b, 0x032c, 0x032d, 0x032e, 0x032f, 0x0330, 0x0331, 0x0332, 0x0333,
0x0334, 0x0335, 0x0336, 0x0337, 0x0338, 0x0339, 0x2320, 0x2321, 0x2322, 0x2323, 0x2324, 0x2325, 0x2326, 0x2327, 0x2328, 0x2329, 0x232a, 0x232b, 0x232c, 0x232d,
0x232e, 0x232f, 0x2330, 0x2331, 0x2332, 0x2333, 0x2334, 0x2335, 0x2336, 0x2337, 0x2338, 0x2339, 0x233a, 0x233b, 0x233c, 0x233d, 0x233e, 0x233f, 0x2720, 0x2721,
0x2722, 0x2723, 0x2724, 0x2725, 0x2726, 0x2727, 0x2728, 0x2729, 0x272a, 0x272b, 0x272c, 0x272d, 0x272e, 0x272f, 0x2730, 0x2731, 0x2732, 0x2733, 0x2734, 0x2735,
0x2736, 0x2737, 0x2738, 0x2739, 0x273a, 0x273b, 0x273c, 0x273d, 0x273e, 0x273f, 0x2f20, 0x2f21, 0x2f22, 0x2f23, 0x2f24, 0x2f25, 0x2f26, 0x2f27, 0x2f28, 0x2f29,
0x2f2a, 0x2f2b, 0x2f2c, 0x2f2d, 0x2f2e, 0x2f2f, 0x2f30, 0x2f31, 0x2f32, 0x2f33, 0x2f34, 0x2f35, 0x2f36, 0x2f37, 0x2f38, 0x2f39, 0x2f3a, 0x2f3b, 0x2f3c, 0x2f3d,
0x3f3e, 0x2f3f, 0x3f20, 0x3f21, 0x3f22, 0x3f23, 0x3f24, 0x3f25, 0x3f26, 0x3f27, 0x3f28, 0x3f29, 0x3f2a, 0x3f2b, 0x3f2c, 0x3f2d, 0x3f2e, 0x3f2f, 0x3f30, 0x3f31,
0x3f32, 0x3f33, 0x3f34, 0x3f35, 0x3f36, 0x3f37, 0x3f38, 0x3f39, 0x3f3a, 0x3f3b, 0x3f3c, 0x3f3d, 0x3f3e, 0x3f3f
};

#define DGAIN_NODE_NUM   160
static ot_u32 g_dgain_table[DGAIN_NODE_NUM] = {
1024, 1055, 1088, 1120, 1152, 1183, 1216, 1248, 1280, 1311, 1344, 1376, 1408, 1439, 1472, 1504, 1536, 1567, 1600, 1632,
1664, 1695, 1728, 1760, 1792, 1823, 1856, 1888, 1920, 1951, 1984, 2016, 2048, 2112, 2176, 2240, 2304, 2368, 2432, 2496,
2560, 2624, 2688, 2752, 2816, 2880, 2944, 3008, 3072, 3136, 3200, 3264, 3328, 3290, 3456, 3520, 3584, 3648, 3712, 3776,
3840, 3904, 3968, 4032, 4096, 4224, 4352, 4480, 4608, 4736, 4864, 4992, 5120, 5248, 5376, 5504, 5632, 5760, 5888, 6016,
6144, 6272, 6400, 6528, 6656, 6784, 6912, 7040, 7168, 7296, 7424, 7552, 7680, 7808, 7936, 8064, 8192, 8448, 8704, 8960,
9216, 9472, 9728, 9984, 10240, 10496, 10752, 11008, 11264, 11520, 11776, 12032, 12288, 12544, 12800, 13056, 13312, 13568, 13824, 14080,
14336, 14592, 14848, 15104, 15360, 15616, 15872, 16128, 16384, 16896, 17408, 17920, 18432, 18944, 19456, 19968, 20480, 20992, 21504, 22016,
22528, 23040, 23552, 24064, 24576, 25088, 25600, 26112, 26624, 27136, 27648, 28160, 28672, 29184, 29696, 30208, 30720, 31232, 31744, 32256
};

#if 0
static ot_float g_dgaindb_table[DGAIN_NODE_NUM] = {
0.00, 0.27, 0.53, 0.78, 1.02, 1.26, 1.50, 1.72, 1.94, 2.15, 2.37, 2.57, 2.77, 2.96, 3.16, 3.34, 3.52, 3.70, 3.88, 4.05,
4.22, 4.38, 4.55, 4.71, 4.86, 5.01, 5.17, 5.32, 5.46, 5.60, 5.75, 5.88, 6.02, 6.29, 6.55, 6.80, 7.04, 7.28, 7.51, 7.74,
7.96, 8.17, 8.38, 8.59, 8.79, 8.98, 9.17, 9.36, 9.54, 9.72, 9.90, 10.07, 10.24, 10.14, 10.57, 10.73, 10.88, 11.04, 11.19, 11.34,
11.48, 11.63, 11.77, 11.91, 12.04, 12.31, 12.57, 12.82, 13.06, 13.30, 13.53, 13.76, 13.98, 14.19, 14.40, 14.61, 14.81, 15.00, 15.19, 15.38,
15.56, 15.74, 15.92, 16.09, 16.26, 16.42, 16.59, 16.75, 16.90, 17.06, 17.21, 17.36, 17.50, 17.64, 17.79, 17.93, 18.06, 18.33, 18.59, 18.84,
19.08, 19.32, 19.55, 19.78, 20.00, 20.21, 20.42, 20.63, 20.83, 21.02, 21.21, 21.40, 21.58, 21.76, 21.94, 22.11, 22.28, 22.44, 22.61, 22.77,
22.92, 23.08, 23.23, 23.38, 23.52, 23.67, 23.81, 23.95, 24.08, 24.35, 24.61, 24.86, 25.11, 25.34, 25.58, 25.80, 26.02, 26.24, 26.44, 26.65,
26.85, 27.04, 27.23, 27.42, 27.60, 27.78, 27.96, 28.13, 28.30, 28.46, 28.63, 28.79, 28.94, 29.10, 29.25, 29.40, 29.54, 29.69, 29.83, 29.97
};
#endif

static ot_u32 g_dgain_register_table[DGAIN_NODE_NUM] = {
0x0080, 0x0084, 0x0088, 0x008c, 0x0090, 0x0094, 0x0098, 0x009c, 0x00a0, 0x00a4, 0x00a8, 0x00ac, 0x00b0, 0x00b4, 0x00b8, 0x00bc, 0x00c0, 0x00c4, 0x00c8, 0x00cc,
0x00d0, 0x00d4, 0x00d8, 0x00dc, 0x00e0, 0x00e4, 0x00e8, 0x00ec, 0x00f0, 0x00f4, 0x00f8, 0x00fc, 0x0180, 0x0184, 0x0188, 0x018c, 0x0190, 0x0194, 0x0198, 0x019c,
0x01a0, 0x01a4, 0x01a8, 0x01ac, 0x01b0, 0x01b4, 0x01b8, 0x01bc, 0x01c0, 0x01c4, 0x01c8, 0x01cc, 0x01d0, 0x01d4, 0x01d8, 0x01dc, 0x01e0, 0x01e4, 0x01e8, 0x01ec,
0x01f0, 0x01f4, 0x01f8, 0x01fc, 0x0380, 0x0384, 0x0388, 0x038c, 0x0390, 0x0394, 0x0398, 0x039c, 0x03a0, 0x03a4, 0x03a8, 0x03ac, 0x03b0, 0x03b4, 0x03b8, 0x03bc,
0x03c0, 0x03c4, 0x03c8, 0x03cc, 0x03d0, 0x03d4, 0x03d8, 0x03dc, 0x03e0, 0x03e4, 0x03e8, 0x03ec, 0x03f0, 0x03f4, 0x03f8, 0x03fc, 0x0780, 0x0784, 0x0788, 0x078c,
0x0790, 0x0794, 0x0798, 0x079c, 0x07a0, 0x07a4, 0x07a8, 0x07ac, 0x07b0, 0x07b4, 0x07b8, 0x07bc, 0x07c0, 0x07c4, 0x07c8, 0x07cc, 0x07d0, 0x07d4, 0x07d8, 0x07dc,
0x07e0, 0x07e4, 0x07e8, 0x07ec, 0x07f0, 0x07f4, 0x07f8, 0x07fc, 0x0f80, 0x0f84, 0x0f88, 0x0f8c, 0x0f90, 0x0f94, 0x0f98, 0x0f9c, 0x0fa0, 0x0fa4, 0x0fa8, 0x0fac,
0x0fb0, 0x0fb4, 0x0fb8, 0x0fbc, 0x0fc0, 0x0fc4, 0x0fc8, 0x0fcc, 0x0fd0, 0x0fd4, 0x0fd8, 0x0fdc, 0x0fe0, 0x0fe4, 0x0fe8, 0x0fec, 0x0ff0, 0x0ff4, 0x0ff8, 0x0ffc
};

static ot_void cmos_again_calc_table(ot_vi_pipe_id vi_pipe, ot_u32 *again_lin, ot_u32 *again_db)
{
    int i;

    sns_check_pointer_void_return(again_lin);
    sns_check_pointer_void_return(again_db);

    if (*again_lin >= g_again_table[AGAIN_NODE_NUM - 1]) {
        *again_lin = g_again_table[AGAIN_NODE_NUM - 1];
        *again_db = AGAIN_NODE_NUM - 1;

        return;
    }

    for (i = 1; i < AGAIN_NODE_NUM; i++) {
        if (*again_lin < g_again_table[i]) {
            *again_lin = g_again_table[i - 1];
            //*again_db = (g_againdb_table[i - 1]);
            *again_db = i-1;
            break;
        }
    }

    return;
}

ot_void cmos_dgain_calc_table(ot_vi_pipe_id vi_pipe, ot_u32 *dgain_lin, ot_u32 *dgain_db)
{
    int i;

    sns_check_pointer_void_return(dgain_lin);
    sns_check_pointer_void_return(dgain_db);

    if (*dgain_lin >= g_dgain_table[DGAIN_NODE_NUM - 1]) {
        *dgain_lin = g_dgain_table[DGAIN_NODE_NUM - 1];
        *dgain_db = DGAIN_NODE_NUM - 1;

        return;
    }

    for (i = 1; i < DGAIN_NODE_NUM; i++) {
        if (*dgain_lin < g_dgain_table[i]) {
            *dgain_lin = g_dgain_table[i - 1];
            *dgain_db = i - 1;
            break;
        }
    }

    return;
}

static ot_void cmos_gains_update(ot_vi_pipe_id vi_pipe, ot_u32 again, ot_u32 dgain)
{
    ot_eis_sns_state *sns_state = OT_NULL;

    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);
    dgain=0;
    //printf("20250915 cmos_gains_update again = %d, dgain = %d\n", again, dgain);

    if(again>AGAIN_NODE_NUM)
    {
        again = AGAIN_NODE_NUM;
        printf("again value error\n");
    }
    again = g_again_register_table[again];
    sns_state->regs_info[0].i2c_data[3].data = low_8bits(again);
    sns_state->regs_info[0].i2c_data[4].data = high_8bits(again);

    if(again < 0x2324)
    {
        sns_state->regs_info[0].i2c_data[9].data = 0x01;
        sns_state->regs_info[0].i2c_data[10].data = 0xf0;
    }
    else
    {
        sns_state->regs_info[0].i2c_data[9].data = 0x02;
        sns_state->regs_info[0].i2c_data[10].data = 0x14;
    }

    if(dgain>DGAIN_NODE_NUM)
    {
        dgain = DGAIN_NODE_NUM;
        printf("dgain value error\n");
    }
    dgain = g_dgain_register_table[dgain];
    sns_state->regs_info[0].i2c_data[5].data = low_8bits(dgain);
    sns_state->regs_info[0].i2c_data[6].data = high_8bits(dgain);
    //printf("20250915 cmos_gains_update again_value = 0x%x, dgain_value = 0x%x\n", again, dgain);

    return;
}

static ot_void cmos_get_inttime_max(ot_vi_pipe_id vi_pipe, ot_u16 man_ratio_enable, ot_u32 *ratio,
    ot_eis_vi_ae_int_time_range *int_time, ot_u32 *lf_max_int_time)
{
    ot_eis_sns_state *sns_state = OT_NULL;

    sns_check_pointer_void_return(ratio);
    sns_check_pointer_void_return(int_time);
    sns_check_pointer_void_return(lf_max_int_time);
    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    switch (sns_state->wdr_mode) {
        default:
            break;
    }

    return;
}

/* Only used in LINE_WDR mode */
static ot_void cmos_ae_fswdr_attr_set(ot_vi_pipe_id vi_pipe, ot_eis_vi_ae_fswdr_attr *ae_fswdr_attr)
{
    sns_check_pointer_void_return(ae_fswdr_attr);

    g_fswdr_mode[vi_pipe] = ae_fswdr_attr->fswdr_mode;
    g_max_time_get_cnt[vi_pipe] = 0;

    return;
}
/* awb static param for Fuji Lens New IR_Cut */                  //AWB calibration
#define CALIBRATE_STATIC_TEMP                         4950
#define CALIBRATE_STATIC_WB_R_GAIN                    310
#define CALIBRATE_STATIC_WB_GR_GAIN                   256
#define CALIBRATE_STATIC_WB_GB_GAIN                   256
#define CALIBRATE_STATIC_WB_B_GAIN                    468

/* Calibration results for Auto WB Planck */                       //AWB calibration
#define CALIBRATE_AWB_P1                              (65)
#define CALIBRATE_AWB_P2                              (109)
#define CALIBRATE_AWB_Q1                              (-81)
#define CALIBRATE_AWB_A1                              (176)
#define CALIBRATE_AWB_B1                              (23443)
#define CALIBRATE_AWB_C1                              (2847499)

/* Rgain and Bgain of the golden sample */
#define GOLDEN_RGAIN                                  0
#define GOLDEN_BGAIN                                  0

static ot_s32 cmos_get_awb_default(ot_vi_pipe_id vi_pipe, ot_eis_vi_awb_sensor_default *awb_sns_dft)
{
    ot_eis_sns_state *sns_state = OT_NULL;

    sns_check_pointer_return(awb_sns_dft);
    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    (ot_void)memset_s(awb_sns_dft, sizeof(ot_eis_vi_awb_sensor_default), 0, sizeof(ot_eis_vi_awb_sensor_default));

    if (g_ext_awb_flag[vi_pipe] == 1) {
        //g_ext_sns_awb include wb_cali_param & ccm & preawb param
        (ot_void)memcpy_s(awb_sns_dft, sizeof(ot_eis_vi_awb_sensor_default), &g_ext_sns_awb[vi_pipe],sizeof(ot_eis_vi_awb_sensor_default));
        awb_sns_dft->init_rgain = g_init_wb_gain[vi_pipe][0]; /* 0: Rgain */
        awb_sns_dft->init_ggain = g_init_wb_gain[vi_pipe][1]; /* 1: Ggain */
        awb_sns_dft->init_bgain = g_init_wb_gain[vi_pipe][2]; /* 2: Bgain */
        awb_sns_dft->sample_rgain = g_sample_r_gain[vi_pipe];
        awb_sns_dft->sample_bgain = g_sample_b_gain[vi_pipe];
        printf("g_ext_awb_flag[vi_pipe] == 1\n");
    } else {
        awb_sns_dft->wb_ref_temp = CALIBRATE_STATIC_TEMP; /* wb_ref_temp 4950 */

        awb_sns_dft->gain_offset[0] = CALIBRATE_STATIC_WB_R_GAIN;
        awb_sns_dft->gain_offset[1] = CALIBRATE_STATIC_WB_GR_GAIN;
        awb_sns_dft->gain_offset[2] = CALIBRATE_STATIC_WB_GB_GAIN; /* index 2 */
        awb_sns_dft->gain_offset[3] = CALIBRATE_STATIC_WB_B_GAIN; /* index 3 */

        awb_sns_dft->wb_para[0] = CALIBRATE_AWB_P1;
        awb_sns_dft->wb_para[1] = CALIBRATE_AWB_P2;
        awb_sns_dft->wb_para[2] = CALIBRATE_AWB_Q1; /* index 2 */
        awb_sns_dft->wb_para[3] = CALIBRATE_AWB_A1; /* index 3 */
        awb_sns_dft->wb_para[4] = CALIBRATE_AWB_B1; /* index 4 */
        awb_sns_dft->wb_para[5] = CALIBRATE_AWB_C1; /* index 5 */

        awb_sns_dft->golden_rgain = GOLDEN_RGAIN;
        awb_sns_dft->golden_bgain = GOLDEN_BGAIN;

        awb_sns_dft->awb_run_interval = 1;

        switch (sns_state->wdr_mode) {
            case OT_EIS_WDR_MODE_NONE:
                (ot_void)memcpy_s(&awb_sns_dft->ccm, sizeof(ot_eis_vi_awb_ccm), &g_awb_ccm, sizeof(ot_eis_vi_awb_ccm));
                (ot_void)memcpy_s(&awb_sns_dft->agc_tbl, sizeof(ot_eis_vi_awb_agc_table),&g_awb_agc_table, sizeof(ot_eis_vi_awb_agc_table));
                break;

            case OT_EIS_WDR_MODE_BUILT_IN:
                (ot_void)memcpy_s(&awb_sns_dft->ccm, sizeof(ot_eis_vi_awb_ccm), &g_awb_ccm_built_in, sizeof(ot_eis_vi_awb_ccm));
                (ot_void)memcpy_s(&awb_sns_dft->agc_tbl, sizeof(ot_eis_vi_awb_agc_table),&g_awb_agc_table, sizeof(ot_eis_vi_awb_agc_table));
                break;

            default:
                (ot_void)memcpy_s(&awb_sns_dft->ccm, sizeof(ot_eis_vi_awb_ccm), &g_awb_ccm, sizeof(ot_eis_vi_awb_ccm));
                (ot_void)memcpy_s(&awb_sns_dft->agc_tbl, sizeof(ot_eis_vi_awb_agc_table),&g_awb_agc_table, sizeof(ot_eis_vi_awb_agc_table));
                break;
        }
        awb_sns_dft->init_rgain = g_init_wb_gain[vi_pipe][0]; /* 0: Rgain */
        awb_sns_dft->init_ggain = g_init_wb_gain[vi_pipe][1]; /* 1: Ggain */
        awb_sns_dft->init_bgain = g_init_wb_gain[vi_pipe][2]; /* 2: Bgain */
        awb_sns_dft->sample_rgain = g_sample_r_gain[vi_pipe];
        awb_sns_dft->sample_bgain = g_sample_b_gain[vi_pipe];

        awb_sns_dft->preawb_attr.enable = 1;

        ot_u16 color_temp_array[OT_EIS_VI_AWB_PREAWB_LUT_SIZE]     = { 2400, 2800, 4000, 5000, 6500, 7500 };
        ot_u32 preawb_ratio_r_array[OT_EIS_VI_AWB_PREAWB_LUT_SIZE] = { 60702, 60702, 59379, 59116, 58656, 57339 }; /* 60702, 60702, 59379, 59116, 58656, 57339 */
        ot_u32 preawb_ratio_b_array[OT_EIS_VI_AWB_PREAWB_LUT_SIZE] = { 66074, 66074, 68168, 72199, 72180, 72491 }; /* 66074, 66074, 68168, 72199, 72180, 72491 */
        for (int i = 0; i < OT_EIS_VI_AWB_PREAWB_LUT_SIZE; i++) {
            awb_sns_dft->preawb_attr.color_temp_array[i] = color_temp_array[i];
            awb_sns_dft->preawb_attr.ratio_r_array[i] = preawb_ratio_r_array[i];
            awb_sns_dft->preawb_attr.ratio_b_array[i] = preawb_ratio_b_array[i];
        }
    }
    return OT_SUCCESS;
}

#define PREAWB_GAIN_PREC   10
#define PREAWB_GAIN_MAX 16383
#define PREAWB_GAIN_MIN 0
#define AWB_INFO_GAIN_PREC 8
static ot_s32 cmos_set_awb_preawb(ot_vi_pipe_id vi_pipe, ot_eis_vi_awb_preawb_info *preawb_info, ot_eis_vi_awb_info *awb_info)
{
    ot_eis_sns_state *sns_state = OT_NULL;

    ot_u16 awb_rgain, awb_bgain;
    ot_u16 awb_rgain_i, awb_rgain_f, awb_bgain_i, awb_bgain_f;

    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    if (preawb_info->enable) {
    awb_bgain = awb_info->b_gain << (PREAWB_GAIN_PREC - AWB_INFO_GAIN_PREC);  // 0~16383, 4.10
    awb_rgain = awb_info->r_gain << (PREAWB_GAIN_PREC - AWB_INFO_GAIN_PREC);  // 0~16383, 4.10
    awb_bgain_i = awb_bgain >> PREAWB_GAIN_PREC;
    awb_bgain_f = awb_bgain % (1 << PREAWB_GAIN_PREC);
    awb_rgain_i = awb_rgain >> PREAWB_GAIN_PREC;
    awb_rgain_f = awb_rgain % (1 << PREAWB_GAIN_PREC);

    sns_state->regs_info[0].i2c_data[12].data = low_8bits((int)((awb_bgain_i << 5) + (awb_bgain_f >> 5)));
    sns_state->regs_info[0].i2c_data[13].data = low_8bits((int)((awb_rgain_i << 5) + (awb_rgain_f >> 5)));
    }

    return OT_SUCCESS;
}

static void cmos_get_isp_linear_default(ot_eis_vi_sns_alg_default *isp_def, ot_vi_pipe_id vi_pipe)
{
    isp_def->key.bit1_demosaic         = 1;
    isp_def->demosaic                  = &g_cmos_demosaic;
    isp_def->key.bit1_sharpen          = 1;
    isp_def->sharpen                   = &g_cmos_yuv_sharpen;
    isp_def->key.bit1_drc              = 1;
    isp_def->drc                       = &g_cmos_drc;
    isp_def->key.bit1_bayer_nr         = 1;
    isp_def->bayer_nr                  = &g_cmos_bayer_nr;
    isp_def->key.bit1_anti_false_color = 1;
    isp_def->anti_false_color          = &g_cmos_anti_false_color;
    isp_def->key.bit1_cac              = 1;
    isp_def->cac                       = &g_cmos_cac;
    isp_def->key.bit1_bshp             = 1;
    isp_def->bshp                      = &g_cmos_bayershp;
    isp_def->key.bit1_ldci             = 1;
    isp_def->ldci                      = &g_cmos_ldci;
    isp_def->key.bit1_gamma            = 1;
    isp_def->gamma                     = &g_cmos_gamma;
#ifdef CONFIG_OT_ISP_CR_SUPPORT
    isp_def->key.bit1_ge               = 1;
    isp_def->ge                        = &g_cmos_ge;
#endif
    isp_def->key.bit1_dehaze = 1;
    isp_def->dehaze = &g_cmos_dehaze;
    isp_def->key.bit1_ca = 0;
    (ot_void)memcpy_s(&isp_def->noise_calibration, sizeof(ot_eis_vi_noise_calibration),&g_cmos_noise_calibration, sizeof(ot_eis_vi_noise_calibration));
    return;
}

static void cmos_get_isp_built_in_default(ot_eis_vi_sns_alg_default *isp_def, ot_vi_pipe_id vi_pipe)
{
    isp_def->key.bit1_demosaic         = 1;
    isp_def->demosaic                  = &g_cmos_demosaic_built_in;
    isp_def->key.bit1_sharpen          = 1;
    isp_def->sharpen                   = &g_cmos_yuv_sharpen_built_in;
    isp_def->key.bit1_drc              = 1;
    isp_def->drc                       = &g_cmos_drc_built_in;
    isp_def->key.bit1_bayer_nr         = 1;
    isp_def->bayer_nr                  = &g_cmos_bayer_nr_built_in;
    isp_def->key.bit1_anti_false_color = 1;
    isp_def->anti_false_color          = &g_cmos_anti_false_color;
    isp_def->key.bit1_ldci             = 1;
    isp_def->ldci                      = &g_cmos_ldci_built_in;
    isp_def->key.bit1_gamma            = 1;
    isp_def->gamma                     = &g_cmos_gamma;

    isp_def->key.bit1_expander         = 1;
    isp_def->expander                  = &g_cmos_expander;

#ifdef CONFIG_OT_ISP_CR_SUPPORT
    isp_def->key.bit1_ge               = 1;
    isp_def->ge                        = &g_cmos_ge;
#endif
    isp_def->key.bit1_dehaze = 1;
    isp_def->dehaze = &g_cmos_dehaze;
    (ot_void)memcpy_s(&isp_def->noise_calibration, sizeof(ot_eis_vi_noise_calibration),
                      &g_cmos_noise_calibration, sizeof(ot_eis_vi_noise_calibration));
    return;
}
static ot_s32 cmos_get_vi_alg_default(ot_vi_pipe_id vi_pipe, ot_eis_vi_sns_alg_default *isp_def)
{
    ot_eis_sns_state *sns_state = OT_NULL;

    sns_check_pointer_return(isp_def);
    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    (ot_void)memset_s(isp_def, sizeof(ot_eis_vi_sns_alg_default), 0, sizeof(ot_eis_vi_sns_alg_default));

    if (g_ext_alg_flag[vi_pipe] == 1) {
        isp_def->key.bit1_clut             = g_ext_sns_alg[vi_pipe].key.bit1_clut;
        isp_def->clut                      = g_ext_sns_alg[vi_pipe].clut;
        isp_def->key.bit1_dpc              = g_ext_sns_alg[vi_pipe].key.bit1_dpc;
        isp_def->dpc                       = g_ext_sns_alg[vi_pipe].dpc;
        isp_def->key.bit1_wdr              = g_ext_sns_alg[vi_pipe].key.bit1_wdr;
        isp_def->wdr                       = g_ext_sns_alg[vi_pipe].wdr;
        isp_def->key.bit1_lsc              = g_ext_sns_alg[vi_pipe].key.bit1_lsc;
        isp_def->lsc                       = g_ext_sns_alg[vi_pipe].lsc;
        isp_def->key.bit1_acs              = g_ext_sns_alg[vi_pipe].key.bit1_acs;
        isp_def->acs                       = g_ext_sns_alg[vi_pipe].acs;
        isp_def->key.bit1_demosaic         = g_ext_sns_alg[vi_pipe].key.bit1_demosaic;
        isp_def->demosaic                  = g_ext_sns_alg[vi_pipe].demosaic;
        isp_def->key.bit1_sharpen          = g_ext_sns_alg[vi_pipe].key.bit1_sharpen;
        isp_def->sharpen                   = g_ext_sns_alg[vi_pipe].sharpen;
        isp_def->key.bit1_drc              = g_ext_sns_alg[vi_pipe].key.bit1_drc;
        isp_def->drc                       = g_ext_sns_alg[vi_pipe].drc;
        isp_def->key.bit1_bayer_nr         = g_ext_sns_alg[vi_pipe].key.bit1_bayer_nr;
        isp_def->bayer_nr                  = g_ext_sns_alg[vi_pipe].bayer_nr;
        isp_def->key.bit1_anti_false_color = g_ext_sns_alg[vi_pipe].key.bit1_anti_false_color;
        isp_def->anti_false_color          = g_ext_sns_alg[vi_pipe].anti_false_color;
        isp_def->key.bit1_ldci             = g_ext_sns_alg[vi_pipe].key.bit1_ldci;
        isp_def->ldci                      = g_ext_sns_alg[vi_pipe].ldci;
        isp_def->key.bit1_gamma            = g_ext_sns_alg[vi_pipe].key.bit1_gamma;
        isp_def->gamma                     = g_ext_sns_alg[vi_pipe].gamma;
        isp_def->key.bit1_expander         = g_ext_sns_alg[vi_pipe].key.bit1_expander;
        isp_def->expander                  = g_ext_sns_alg[vi_pipe].expander;
        isp_def->key.bit1_cac         = g_ext_sns_alg[vi_pipe].key.bit1_cac;
        isp_def->cac                  = g_ext_sns_alg[vi_pipe].cac;
        isp_def->key.bit1_bshp         = g_ext_sns_alg[vi_pipe].key.bit1_bshp;
        isp_def->bshp                  = g_ext_sns_alg[vi_pipe].bshp;
        isp_def->key.bit1_lblc         = g_ext_sns_alg[vi_pipe].key.bit1_lblc;
        isp_def->lblc                  = g_ext_sns_alg[vi_pipe].lblc;
#ifdef CONFIG_OT_ISP_CA_SUPPORT
        isp_def->key.bit1_ca      = g_ext_sns_alg[vi_pipe].key.bit1_ca;
        isp_def->key.ca           = g_ext_sns_alg[vi_pipe].ca;
#endif
#ifdef CONFIG_OT_ISP_CR_SUPPORT
        isp_def->key.bit1_ge               = g_ext_sns_alg[vi_pipe].key.bit1_ge;
        isp_def->ge                        = g_ext_sns_alg[vi_pipe].ge;
#endif
        isp_def->key.bit1_dehaze           = g_ext_sns_alg[vi_pipe].key.bit1_dehaze;
        isp_def->dehaze                    = g_ext_sns_alg[vi_pipe].dehaze;
        (ot_void)memcpy_s(&isp_def->noise_calibration, sizeof(ot_eis_vi_noise_calibration),
                          &g_ext_sns_alg[vi_pipe].noise_calibration, sizeof(ot_eis_vi_noise_calibration));
    } else {
        isp_def->key.bit1_clut    = 1;
        isp_def->clut             = &g_cmos_clut;
        isp_def->key.bit1_dpc     = 1;
        isp_def->dpc              = &g_cmos_dpc;
        isp_def->key.bit1_wdr     = 1;
        isp_def->wdr              = &g_cmos_wdr;
        isp_def->key.bit1_lsc      = 0;
        isp_def->lsc               = &g_cmos_lsc;
        isp_def->key.bit1_acs      = 0;
        isp_def->acs               = &g_cmos_acs;
#ifdef CONFIG_OT_ISP_CA_SUPPORT
        isp_def->key.bit1_ca      = 0;
#endif
        switch (sns_state->wdr_mode) {
            case OT_EIS_WDR_MODE_NONE:
                cmos_get_isp_built_in_default(isp_def, vi_pipe);
                break;
            case OT_EIS_WDR_MODE_BUILT_IN:
                cmos_get_isp_built_in_default(isp_def, vi_pipe);
                break;
            default:
                cmos_get_isp_linear_default(isp_def, vi_pipe);
                break;
        }

    }

    isp_def->wdr_switch_attr.exp_ratio[0] = 0x40;

    isp_def->sns_mode.sns_id = SC132GS_SLAVE_ID;
    isp_def->sns_mode.sns_mode = sns_state->img_mode;

    return OT_SUCCESS;
}

static ot_s32 cmos_get_vi_black_level(ot_vi_pipe_id vi_pipe, ot_eis_vi_sns_black_level *black_level)
{
    ot_eis_sns_state *sns_state = OT_NULL;
    const ot_eis_vi_sns_black_level *cmos_blc_def = OT_NULL;

    sns_check_pointer_return(black_level);
    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    if (0) {
        cmos_blc_def = g_ext_sns_alg[vi_pipe].sns_blc;
    } else {
        if (sns_state->wdr_mode == OT_EIS_WDR_MODE_NONE || sns_state->wdr_mode == OT_EIS_WDR_MODE_BUILT_IN) {
            cmos_blc_def = &g_cmos_blc;
        } else {
            cmos_blc_def = &g_cmos_blc_wdr;
        }
    }

    (ot_void)memcpy_s(black_level, sizeof(ot_eis_vi_sns_black_level), cmos_blc_def, sizeof(ot_eis_vi_sns_black_level));

    return OT_SUCCESS;
}

static ot_s32 cmos_get_blc_clamp_info(ot_vi_pipe_id vi_pipe, ot_bool *blc_clamp_en)
{
    sns_check_pointer_return(blc_clamp_en);

    *blc_clamp_en = blc_clamp_info[vi_pipe];

    return OT_SUCCESS;
}

static ot_void cmos_set_pixel_detect(ot_vi_pipe_id vi_pipe, ot_bool enable)
{
    ot_u32 full_lines_5fps;
    ot_eis_sns_state *sns_state = OT_NULL;

    ot_eis_unused(full_lines_5fps);
    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    if (sns_state->wdr_mode == OT_EIS_WDR_MODE_2To1_LINE) {
        return;
    } else {
        if ((sns_state->img_mode == SC132GS_SLAVE_EXPOSURE_1080X1280_30FPS_RAW12_LINEAR_MODE) ||
            (sns_state->img_mode == SC132GS_SLAVE_READOUT_1080X1280_30FPS_RAW12_LINEAR_MODE)) {
            full_lines_5fps = (SC132GS_SLAVE_VMAX_1080X1280_30FPS_LINEAR * 30) / 5; /* 30fps, 5fps */
        } else {
            return;
        }
    }

    return;
}
static ot_s32 cmos_set_wdr_mode(ot_vi_pipe_id vi_pipe, ot_u8 mode)
{
    ot_eis_sns_state *sns_state = OT_NULL;

    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    sns_state->sync_init = OT_FALSE;

    switch (mode & 0x3F) {
        case OT_EIS_WDR_MODE_NONE:
            sns_state->wdr_mode = OT_EIS_WDR_MODE_NONE;
            printf("linear mode\n");
            break;
        case OT_EIS_WDR_MODE_BUILT_IN:
            sns_state->wdr_mode = OT_EIS_WDR_MODE_BUILT_IN;
            printf("built-in mode\n");
            break;
        default:
            sns_err_trace("NOT support this mode!\n");
            return OT_FAILURE;
    }

    (ot_void)memset_s(sns_state->wdr_int_time, sizeof(sns_state->wdr_int_time), 0, sizeof(sns_state->wdr_int_time));
    return OT_SUCCESS;
}

static ot_void cmos_comm_sns_reg_info_init(ot_vi_pipe_id vi_pipe, ot_eis_sns_state *sns_state)
{
    ot_u32 i;
    sns_state->regs_info[0].sns_type = OT_EIS_VI_SNS_TYPE_I2C;
    sns_state->regs_info[0].com_bus.i2c_dev = g_sc132gs_slave_bus_info[vi_pipe].com_bus.i2c_dev;
    sns_state->regs_info[0].cfg2_valid_delay_max = 2; /* delay_max 2 */
    sns_state->regs_info[0].reg_num = 11; // TODO

    for (i = 0; i < sns_state->regs_info[0].reg_num; i++) {
        sns_state->regs_info[0].i2c_data[i].update = OT_TRUE;
        sns_state->regs_info[0].i2c_data[i].dev_addr = g_sc132gs_slave_bus_info[vi_pipe].bus_addr;
        sns_state->regs_info[0].i2c_data[i].addr_byte_num = SC132GS_SLAVE_ADDR_BYTE;
        sns_state->regs_info[0].i2c_data[i].data_byte_num = SC132GS_SLAVE_DATA_BYTE;
    }

    return;
}

static ot_void cmos_linear_sns_reg_info_init(ot_vi_pipe_id vi_pipe, ot_eis_sns_state *sns_state)
{
    sns_state->regs_info[0].i2c_data[0].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[0].reg_addr = SC132GS_SLAVE_SHT_COARSE_L;
    sns_state->regs_info[0].i2c_data[1].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[1].reg_addr = SC132GS_SLAVE_SHT_COARSE_M;
    sns_state->regs_info[0].i2c_data[2].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[2].reg_addr = SC132GS_SLAVE_SHT_COARSE_H;
    sns_state->regs_info[0].i2c_data[3].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[3].reg_addr = SC132GS_SLAVE_AGAIN_L;
    sns_state->regs_info[0].i2c_data[4].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[4].reg_addr = SC132GS_SLAVE_AGAIN_H;
    sns_state->regs_info[0].i2c_data[5].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[5].reg_addr = SC132GS_SLAVE_DGAIN_L;
    sns_state->regs_info[0].i2c_data[6].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[6].reg_addr = SC132GS_SLAVE_DGAIN_H;
    sns_state->regs_info[0].i2c_data[7].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[7].reg_addr = SC132GS_SLAVE_VMAX_L;
    sns_state->regs_info[0].i2c_data[8].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[8].reg_addr = SC132GS_SLAVE_VMAX_H;
    sns_state->regs_info[0].i2c_data[9].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[9].reg_addr = 0x33fa;
    sns_state->regs_info[0].i2c_data[10].delay_frame_num = 0;
    sns_state->regs_info[0].i2c_data[10].reg_addr = 0x3317;
    return;
}

static ot_void cmos_built_in_sns_reg_info_init(ot_vi_pipe_id vi_pipe, ot_eis_sns_state *sns_state)
{


    return;
}

static ot_void cmos_sns_reg_info_update(ot_vi_pipe_id vi_pipe, ot_eis_sns_state *sns_state)
{
    ot_u32 i;

    for (i = 0; i < sns_state->regs_info[0].reg_num; i++) {
        if (sns_state->regs_info[0].i2c_data[i].data ==
            sns_state->regs_info[1].i2c_data[i].data) {
            sns_state->regs_info[0].i2c_data[i].update = OT_FALSE;
        } else {
            sns_state->regs_info[0].i2c_data[i].update = OT_TRUE;
        }
    }
    return;
}

static ot_s32 cmos_get_sns_regs_info(ot_vi_pipe_id vi_pipe, ot_eis_vi_sns_regs_info *sns_regs_info)
{
    ot_eis_sns_state *sns_state = OT_NULL;

    sns_check_pointer_return(sns_regs_info);
    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    if ((sns_state->sync_init == OT_FALSE) || (sns_regs_info->config == OT_FALSE)) {
        cmos_comm_sns_reg_info_init(vi_pipe, sns_state);
        if (sns_state->wdr_mode == OT_EIS_WDR_MODE_NONE) {
            cmos_linear_sns_reg_info_init(vi_pipe, sns_state);
        }
        if (sns_state->wdr_mode == OT_EIS_WDR_MODE_BUILT_IN) {
            cmos_built_in_sns_reg_info_init(vi_pipe, sns_state);
        }
        sns_state->sync_init = OT_TRUE;
    } else {
        cmos_sns_reg_info_update(vi_pipe, sns_state);
    }

    sns_state->regs_info[0].slv_sync.update = OT_TRUE;
    sns_state->regs_info[0].slv_sync.delay_frame_num = 1;
    sns_state->regs_info[0].slv_sync.slave_bind_dev = g_sc132gs_slave_bind_dev[vi_pipe];

    sns_regs_info->config = OT_FALSE;
    (ot_void)memcpy_s(sns_regs_info, sizeof(ot_eis_vi_sns_regs_info),
                      &sns_state->regs_info[0], sizeof(ot_eis_vi_sns_regs_info));
    (ot_void)memcpy_s(&sns_state->regs_info[1], sizeof(ot_eis_vi_sns_regs_info),
                      &sns_state->regs_info[0], sizeof(ot_eis_vi_sns_regs_info));
    sns_state->fl[1] = sns_state->fl[0];

    return OT_SUCCESS;
}

static ot_void cmos_config_image_mode_param(ot_vi_pipe_id vi_pipe, ot_u8 sns_image_mode,
    ot_eis_sns_state *sns_state)
{
    switch (sns_image_mode) {
        case SC132GS_SLAVE_EXPOSURE_1080X1280_30FPS_RAW12_LINEAR_MODE:
        case SC132GS_SLAVE_READOUT_1080X1280_30FPS_RAW12_LINEAR_MODE:
            sns_state->fl_std = SC132GS_SLAVE_VMAX_1080X1280_30FPS_LINEAR;
            break;
        default:
            sns_state->fl_std = SC132GS_SLAVE_VMAX_1080X1280_30FPS_LINEAR;
            break;
    }

    return;
}

static ot_s32 cmos_set_image_mode(ot_vi_pipe_id vi_pipe, const ot_eis_vi_sns_image_mode *sns_image_mode)
{
    ot_u32 i;
    ot_u8 image_mode;
    ot_eis_sns_state *sns_state = OT_NULL;
    sns_check_pointer_return(sns_image_mode);
    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    image_mode = sns_state->img_mode;

    for (i = 0; i < SC132GS_SLAVE_MODE_BUTT; i++) {
        if (sns_image_mode->fps <= g_sc132gs_slave_mode_tbl[i].max_fps &&
            sns_image_mode->width <= g_sc132gs_slave_mode_tbl[i].width &&
            sns_image_mode->height <= g_sc132gs_slave_mode_tbl[i].height &&
            sns_image_mode->sns_mode == g_sc132gs_slave_mode_tbl[i].sns_mode &&
            sns_state->wdr_mode == g_sc132gs_slave_mode_tbl[i].wdr_mode) {
            image_mode = (sc132gs_slave_res_mode)i;
            break;
        }
    }

    if (i >= SC132GS_SLAVE_MODE_BUTT) {
        sc132gs_slave_err_mode_print(sns_image_mode, sns_state);
        return OT_FAILURE;
    }

    cmos_config_image_mode_param(vi_pipe, image_mode, sns_state);

    if ((sns_state->init == OT_TRUE) && (image_mode == sns_state->img_mode)) {
        return OT_EIS_VI_DO_NOT_NEED_SWITCH_IMAGEMODE; /* Don't need to switch image_mode */
    }

    sns_state->sync_init = OT_FALSE;
    sns_state->img_mode = image_mode;
    sns_state->fl[0] = sns_state->fl_std;
    sns_state->fl[1] = sns_state->fl[0];

    return OT_SUCCESS;
}

static ot_void sensor_global_init(ot_vi_pipe_id vi_pipe)
{
    ot_eis_sns_state *sns_state = OT_NULL;

    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    sns_state->init      = OT_FALSE;
    sns_state->sync_init = OT_FALSE;
    sns_state->img_mode  = SC132GS_SLAVE_EXPOSURE_1080X1280_30FPS_RAW12_LINEAR_MODE;
    sns_state->wdr_mode  = OT_EIS_WDR_MODE_BUILT_IN;
    sns_state->fl_std    = SC132GS_SLAVE_VMAX_1080X1280_30FPS_LINEAR;
    sns_state->fl[0]     = SC132GS_SLAVE_VMAX_1080X1280_30FPS_LINEAR;
    sns_state->fl[1]     = SC132GS_SLAVE_VMAX_1080X1280_30FPS_LINEAR;

    (ot_void)memset_s(&sns_state->regs_info[0], sizeof(ot_eis_vi_sns_regs_info), 0, sizeof(ot_eis_vi_sns_regs_info));
    (ot_void)memset_s(&sns_state->regs_info[1], sizeof(ot_eis_vi_sns_regs_info), 0, sizeof(ot_eis_vi_sns_regs_info));

    return;
}

static ot_s32 sensor_ctx_init(ot_vi_pipe_id vi_pipe)
{
    ot_eis_sns_state *sns_state_ctx = OT_NULL;
    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state_ctx);
    if (sns_state_ctx == OT_NULL) {
        sns_state_ctx = (ot_eis_sns_state *)malloc(sizeof(ot_eis_sns_state));
        if (sns_state_ctx == OT_NULL) {
            sns_err_trace("Isp[%d] SnsCtx malloc memory failed!\n", vi_pipe);
            return OT_FAILURE;
        }
    }

    (ot_void)memset_s(sns_state_ctx, sizeof(ot_eis_sns_state), 0, sizeof(ot_eis_sns_state));
    sc132gs_slave_sensor_set_ctx(vi_pipe, sns_state_ctx);

    return OT_SUCCESS;
}

static ot_void sensor_ctx_exit(ot_vi_pipe_id vi_pipe)
{
    ot_eis_sns_state *sns_state_ctx = OT_NULL;
    sc132gs_slave_sensor_get_ctx(vi_pipe, sns_state_ctx);
    sns_free(sns_state_ctx);
    sc132gs_slave_sensor_reset_ctx(vi_pipe);

    return;
}

static ot_eis_vi_sns_register_func g_sc132gs_slave_register_to_vi_func = {
    .pfn_sns_init                = sc132gs_slave_init,
    .pfn_sns_exit                = sc132gs_slave_exit,
    .pfn_sns_global_init         = sensor_global_init,
    .pfn_sns_set_image_mode      = cmos_set_image_mode,
    .pfn_sns_set_wdr_mode        = cmos_set_wdr_mode,
    .pfn_sns_get_vi_alg_default  = cmos_get_vi_alg_default,
    .pfn_sns_get_vi_black_level  = cmos_get_vi_black_level,
    .pfn_sns_get_blc_clamp_info  = cmos_get_blc_clamp_info,
    .pfn_sns_set_pixel_detect    = cmos_set_pixel_detect,
    .pfn_sns_get_reg_info        = cmos_get_sns_regs_info,
};

static ot_eis_vi_ae_sensor_register_func g_sc132gs_slave_register_to_ae_func = {
    .pfn_sns_get_ae_default         = cmos_get_ae_default,
    .pfn_sns_cfg_fps                = cmos_fps_set,
    .pfn_sns_cfg_slow_framerate     = cmos_slow_framerate_set,
    .pfn_sns_update_inttime         = cmos_inttime_update,
    .pfn_sns_update_gains           = cmos_gains_update,
    .pfn_sns_calc_table_again       = cmos_again_calc_table,
    //.pfn_sns_calc_table_dgain       = cmos_dgain_calc_table,
    .pfn_sns_get_inttime_max        = cmos_get_inttime_max,
    .pfn_sns_set_fswdr_attr         = cmos_ae_fswdr_attr_set,
    .pfn_sns_set_quick_start_status = OT_NULL,
    .pfn_sns_convert_exp_param      = OT_NULL,
    .pfn_sns_get_thermo_default     = OT_NULL,
} ;

static ot_eis_vi_awb_sensor_register_func g_sc132gs_slave_register_to_awb_func = {
    .pfn_sns_get_awb_default = cmos_get_awb_default,
    .pfn_sns_set_awb_preawb = cmos_set_awb_preawb,
};

static ot_s32 sensor_register_callback(ot_vi_pipe_handle pipe_hdl)
{
    ot_s32 ret;
    ot_vi_pipe_id vi_pipe[OT_EIS_VI_PIPE_ID_NUM_MAX] = { 0 };
    ot_vi_pipe_id master_pipe_id = 0;
    ot_s32 pipe_num = 0;

    ret = ot_eis_vi_get_pipe_id_by_pipe_hdl(pipe_hdl, vi_pipe, &pipe_num);
    if (ret != OT_SUCCESS) {
        return ret;
    }

    master_pipe_id = vi_pipe[0];

    ret = sensor_ctx_init(master_pipe_id);
    if (ret != OT_SUCCESS) {
        return OT_FAILURE;
    }

    ret = ot_eis_vi_sensor_register_to_vi(pipe_hdl, &g_sc132gs_slave_register_to_vi_func, SC132GS_SLAVE_ID);
    if (ret != OT_SUCCESS) {
        sns_err_trace("sensor register to vi failed!\n");
        goto free_sns_ctx;
    }

    ret = ot_eis_vi_sensor_register_to_ae(pipe_hdl, &g_sc132gs_slave_register_to_ae_func, SC132GS_SLAVE_ID);
    if (ret != OT_SUCCESS) {
        sns_err_trace("sensor register to ae lib failed!\n");
        goto sns_unreg_to_vi;
    }

    ret = ot_eis_vi_sensor_register_to_awb(pipe_hdl, &g_sc132gs_slave_register_to_awb_func, SC132GS_SLAVE_ID);
    if (ret != OT_SUCCESS) {
        sns_err_trace("sensor register to awb lib failed!\n");
        goto sns_unreg_to_ae;
    }

    return OT_SUCCESS;

sns_unreg_to_ae:
    ot_eis_vi_sensor_unregister_to_ae(pipe_hdl, SC132GS_SLAVE_ID);
sns_unreg_to_vi:
    ot_eis_vi_sensor_unregister_to_vi(pipe_hdl, SC132GS_SLAVE_ID);
free_sns_ctx:
    sensor_ctx_exit(master_pipe_id);
    return ret;
}

static ot_s32 sensor_unregister_callback(ot_vi_pipe_handle pipe_hdl)
{
    ot_s32 ret;
    ot_vi_pipe_id vi_pipe[OT_EIS_VI_PIPE_ID_NUM_MAX] = { 0 };
    ot_vi_pipe_id master_pipe_id = 0;
    ot_s32 pipe_num = 0;

    ret = ot_eis_vi_get_pipe_id_by_pipe_hdl(pipe_hdl, vi_pipe, &pipe_num);
    if (ret != OT_SUCCESS) {
        return ret;
    }
    master_pipe_id = vi_pipe[0];
    ret = ot_eis_vi_sensor_unregister_to_vi(pipe_hdl, SC132GS_SLAVE_ID);
    if (ret != OT_SUCCESS) {
        sns_err_trace("sensor unregister callback function failed!\n");
        return ret;
    }
    ret = ot_eis_vi_sensor_unregister_to_ae(pipe_hdl, SC132GS_SLAVE_ID);
    if (ret != OT_SUCCESS) {
        sns_err_trace("sensor unregister callback function to ae lib failed!\n");
        return ret;
    }

    ret = ot_eis_vi_sensor_unregister_to_awb(pipe_hdl, SC132GS_SLAVE_ID);
    if (ret != OT_SUCCESS) {
        sns_err_trace("sensor unregister callback function to awb lib failed!\n");
        return ret;
    }

    sensor_ctx_exit(master_pipe_id);

    return OT_SUCCESS;
}

static ot_s32 sc132gs_slave_set_bus_info(ot_vi_pipe_handle pipe_hdl, ot_eis_sns_bus_info *sns_bus_info)
{
    ot_s32 ret;
    ot_vi_pipe_id vi_pipe[OT_EIS_VI_PIPE_ID_NUM_MAX] = { 0 };
    ot_vi_pipe_id master_pipe_id = 0;
    ot_s32 pipe_num = 0;
    ret = ot_eis_vi_get_pipe_id_by_pipe_hdl(pipe_hdl, vi_pipe, &pipe_num);
    if (ret != OT_SUCCESS) {
        return ret;
    }
    master_pipe_id = vi_pipe[0];
    memcpy_s(&g_sc132gs_slave_bus_info[master_pipe_id], sizeof(ot_eis_sns_bus_info), sns_bus_info, sizeof(ot_eis_sns_bus_info));

    return OT_SUCCESS;
}

static ot_s32 sensor_set_init(ot_vi_pipe_handle pipe_hdl, ot_eis_vi_init_attr *init_attr)
{
    ot_s32 ret;
    ot_vi_pipe_id vi_pipe[OT_EIS_VI_PIPE_ID_NUM_MAX] = { 0 };
    ot_vi_pipe_id master_pipe_id = 0;
    ot_s32 pipe_num = 0;
    ret = ot_eis_vi_get_pipe_id_by_pipe_hdl(pipe_hdl, vi_pipe, &pipe_num);
    if (ret != OT_SUCCESS) {
        return ret;
    }
    master_pipe_id = vi_pipe[0];

    sns_check_pointer_return(init_attr);

    g_init_exposure[master_pipe_id]  = init_attr->exposure;
    g_init_int_time[master_pipe_id]  = init_attr->exp_time;
    g_init_again[master_pipe_id]     = init_attr->a_gain;
    g_init_isp_dgain[master_pipe_id] = init_attr->ispd_gain;
    g_lines_per500ms[master_pipe_id] = init_attr->lines_per500ms;
    g_init_wb_gain[master_pipe_id][0] = init_attr->wb_r_gain; /* 0: rgain */
    g_init_wb_gain[master_pipe_id][1] = init_attr->wb_g_gain; /* 1: ggain */
    g_init_wb_gain[master_pipe_id][2] = init_attr->wb_b_gain; /* 2: bgain */
    g_sample_r_gain[master_pipe_id] = init_attr->sample_r_gain;
    g_sample_b_gain[master_pipe_id] = init_attr->sample_b_gain;
    g_quick_start_en[master_pipe_id] = init_attr->quick_start_en;
    g_ae_stat_pos[master_pipe_id]       = init_attr->ae_stat_pos;
    g_ae_route_ex_valid[master_pipe_id] = init_attr->ae_route_ex_valid;
    (ot_void)memcpy_s(&g_init_ae_route[master_pipe_id], sizeof(ot_eis_vi_ae_route),
                      &init_attr->ae_route, sizeof(ot_eis_vi_ae_route));
    (ot_void)memcpy_s(&g_init_ae_route_ex[master_pipe_id], sizeof(ot_eis_vi_ae_route_ex),
                      &init_attr->ae_route_ex, sizeof(ot_eis_vi_ae_route_ex));
    (ot_void)memcpy_s(&g_init_ae_route_sf[master_pipe_id], sizeof(ot_eis_vi_ae_route),
                      &init_attr->ae_route_sf, sizeof(ot_eis_vi_ae_route));
    (ot_void)memcpy_s(&g_init_ae_route_sf_ex[master_pipe_id], sizeof(ot_eis_vi_ae_route_ex),
                      &init_attr->ae_route_sf_ex, sizeof(ot_eis_vi_ae_route_ex));

    return OT_SUCCESS;
}

static ot_s32 sc132gs_slave_set_alg_default(ot_vi_pipe_handle pipe_hdl, ot_eis_vi_sns_alg_default *sns_alg_def)
{
    ot_s32 ret;
    ot_vi_pipe_id vi_pipe[OT_EIS_VI_PIPE_ID_NUM_MAX] = { 0 };
    ot_vi_pipe_id master_pipe_id = 0;
    ot_s32 pipe_num = 0;

    ret = ot_eis_vi_get_pipe_id_by_pipe_hdl(pipe_hdl, vi_pipe, &pipe_num);
    if (ret != OT_SUCCESS) {
        return ret;
    }
    master_pipe_id = vi_pipe[0];
    if (sns_alg_def == OT_NULL) {
        return OT_FAILURE;
    }
    g_ext_sns_alg[master_pipe_id] = *sns_alg_def;
    g_ext_alg_flag[master_pipe_id] = 1;

    return OT_SUCCESS;
}

static ot_s32 sc132gs_slave_set_awb_default(ot_vi_pipe_handle pipe_hdl, ot_eis_vi_awb_sensor_default *sns_awb_def)
{
    ot_s32 ret;
    ot_vi_pipe_id vi_pipe[OT_EIS_VI_PIPE_ID_NUM_MAX] = { 0 };
    ot_vi_pipe_id master_pipe_id = 0;
    ot_s32 pipe_num = 0;

    ret = ot_eis_vi_get_pipe_id_by_pipe_hdl(pipe_hdl, vi_pipe, &pipe_num);
    if (ret != OT_SUCCESS) {
        return ret;
    }
    master_pipe_id = vi_pipe[0];

    if(sns_awb_def == OT_NULL) {
        return OT_FAILURE;
    }
    g_ext_sns_awb[master_pipe_id] = *sns_awb_def;
    g_ext_awb_flag[master_pipe_id] = 1;
    return OT_SUCCESS;
}

static ot_s32 sc132gs_slave_set_reg_table(ot_vi_pipe_handle pipe_hdl, ot_eis_sns_reg_table *reg_table, ot_s32 table_cnt)
{
    ot_s32 ret;
    ot_vi_pipe_id vi_pipe[OT_EIS_VI_PIPE_ID_NUM_MAX] = { 0 };
    ot_vi_pipe_id master_pipe_id = 0;
    ot_s32 pipe_num = 0;

    ret = ot_eis_vi_get_pipe_id_by_pipe_hdl(pipe_hdl, vi_pipe, &pipe_num);
    if (ret != OT_SUCCESS) {
        return ret;
    }
    master_pipe_id = vi_pipe[0];

    g_ext_table[master_pipe_id][0] = *reg_table;
    g_ext_table[master_pipe_id][1] = *(reg_table + 1);
    g_ext_table[master_pipe_id][2] = *(reg_table + 2);
    g_ext_table[master_pipe_id][3] = *(reg_table + 3);
    g_ext_table[master_pipe_id][4] = *(reg_table + 4);
    g_ext_sensor_table_flag[master_pipe_id] = 1;
    return OT_SUCCESS;
}

ot_eis_sns_obj g_sns_sc132gs_slave_obj = {
    .pfn_sns_register              = sensor_register_callback,
    .pfn_sns_unregister            = sensor_unregister_callback,
    .pfn_sns_standby               = sc132gs_slave_standby,
    .pfn_sns_restart               = sc132gs_slave_restart,
    .pfn_sns_set_mirror_flip       = OT_NULL,
    .pfn_sns_set_blc_clamp         = sc132gs_slave_blc_clamp,
    .pfn_sns_set_bus_info          = sc132gs_slave_set_bus_info,
    .pfn_sns_set_init              = sensor_set_init,
    .pfn_sns_set_alg_default       = sc132gs_slave_set_alg_default,
    .pfn_sns_set_awb_default       = sc132gs_slave_set_awb_default,
    .pfn_sns_set_reg_table         = sc132gs_slave_set_reg_table,
};