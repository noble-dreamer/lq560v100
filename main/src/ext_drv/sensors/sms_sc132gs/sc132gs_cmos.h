#ifndef SC132GS_CMOS_H
#define SC132GS_CMOS_H


#include "ot_eis_sns_ctrl.h"
#include "securec.h"
#include "ot_eis_video.h"
#include "ot_eis_vi_3a.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SC132GS_I2C_ADDR        0x60
#define SC132GS_ADDR_BYTE         2
#define SC132GS_DATA_BYTE         1
#define I2C_DATA_BYTE 1
#define I2C_REG_ADDR_2BYTE 2
#define I2C_REG_ADDR_1BYTE 1
#define sc132gs_sensor_get_ctx(pipe, ctx)   ((ctx) = sc132gs_get_ctx(pipe))

#define SC132GS_FULL_LINES_MAX 							0xffff
#define SC132GS_FULL_LINES_MAX_LINEAR 			0xffff
#define SC132GS_VMAX_1080X1280_30FPS_LINEAR                (0x578)

typedef enum {
    SC132GS_1080X1280_30FPS_RAW12_LINEAR_MODE = 0,
    SC132GS_MODE_BUTT
} sc132gs_res_mode;

typedef struct {
    ot_u32      ver_lines;
    ot_u32      max_ver_lines;
    ot_float    max_fps;
    ot_float    min_fps;
    ot_u32      width;
    ot_u32      height;
    ot_u8       sns_mode;
    ot_eis_wdr_mode wdr_mode;
    const char *mode_name;
} sc132gs_video_mode_tbl;

typedef struct {
    ot_u32 dec[OT_EIS_VI_WDR_MAX_FRAME_NUM];
    ot_u32 inc[OT_EIS_VI_WDR_MAX_FRAME_NUM];
} time_step;
ot_eis_sns_state *sc132gs_get_ctx(ot_vi_pipe_id vi_pipe);
ot_eis_sns_reg_table *sc132gs_get_ext_sensor_table(ot_vi_pipe_id vi_pipe);
ot_u8 sc132gs_get_ext_sensor_flag(ot_vi_pipe_id vi_pipe);
ot_eis_sns_bus_info *sc132gs_get_bus_info(ot_vi_pipe_id vi_pipe);

ot_void sc132gs_init(ot_vi_pipe_id vi_pipe);
ot_void sc132gs_exit(ot_vi_pipe_id vi_pipe);
ot_void sc132gs_standby(ot_vi_pipe_handle pipe_hdl);
ot_void sc132gs_restart(ot_vi_pipe_handle pipe_hdl);
ot_void sc132gs_blc_clamp(ot_vi_pipe_handle pipe_hdl, ot_eis_sns_blc_clamp blc_clamp);
ot_void sc132gs_set_blc_clamp_value(ot_vi_pipe_id vi_pipe, ot_bool clamp_en);

ot_s32 sc132gs_write_register(ot_vi_pipe_id vi_pipe, ot_u32 addr, ot_u32 data);
ot_s32 sc132gs_read_register(ot_vi_pipe_id vi_pipe, ot_u32 addr, ot_u32 *data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif /* SC132GS_CMOS_H */
