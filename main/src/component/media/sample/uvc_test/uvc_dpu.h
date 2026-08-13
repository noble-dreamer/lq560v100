/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */
#ifndef SAMPLE_DPU_H
#define SAMPLE_DPU_H
#include <stdio.h>
#include <stdint.h>
#include "ot_smr.h"
#include "ot_buffer_pool.h"
#include "ot_avp_dpu_rect.h"
#include "ot_avp_dpu_match.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define SAMPLE_DPU_ALIGN_16             16
#define SAMPLE_DPU_FRAME_NUM            1
#define SAMPLE_DPU_DISPARITY_NUM        64
#define SAMPLE_UVC_DEPU_START_POS       (0)

#define SAMPLE_DPU_IN_WIDTH             640
#define SAMPLE_DPU_IN_HEIGHT            480
#define SAMPLE_DPU_DST_WIDTH            640
#define SAMPLE_DPU_DST_HEIGHT           480
#define SAMPLE_DPU_CHN_NUM              2
#define SAMPLE_DPU_TIME_OUT             2000
#define SAMPLE_DPU_BLOCK                (-1)
#define SAMPLE_DPU_NO_RATE_CTRL         (-1)
#define SAMPLE_DPU_MAX_POOL_CNT         128
#define SAMPLE_DPU_BLK_CNT_10           10
#define SAMPLE_DPU_BLK_CNT_8            8
#define SAMPLE_DPU_VI_CHN_CNT           2
#define SAMPLE_DPU_VI_PIPE_CNT          2
#define SAMPLE_DPU_VPROC_GRP_NUM        2
#define SAMPLE_DPU_PER_CHN_BLK_CNT      4
#define SAMPLE_DPU_VPROC_MAX_WIDTH      3840
#define SAMPLE_DPU_VPROC_MAX_HEIGHT     2160
#define SAMPLE_DPU_BG_COLOR             0xff
#define SAMPLE_DPU_COMMON_POOL_2        2
#define SAMPLE_DPU_COMMON_POOL_3        3
#define SAMPLE_DPU_OUTPUT_NUM           3
#define SAMPLE_DPU_OUTPUT_DISP_IDX      0
#define SAMPLE_DPU_OUTPUT_CONF_IDX      1
#define SAMPLE_DPU_OUTPUT_DEPTH_IDX     2

#define SAMPLE_DPU_INVALID_DISP                 (SAMPLE_UVC_DEPU_START_POS - 1)
#define SAMPLE_DPU_MATCH_A0_UNIQ_RATIO          24
#define SAMPLE_DPU_MATCH_A1_UNIQ_RATIO          28
#define SAMPLE_DPU_MATCH_A2_UNIQ_RATIO          31
#define SAMPLE_DPU_MATCH_D0_AGGREGATE_COEFF     0
#define SAMPLE_DPU_MATCH_D1_AGGREGATE_COEFF     4
#define SAMPLE_DPU_MATCH_D2_AGGREGATE_COEFF     6
#define SAMPLE_DPU_MATCH_D3_AGGREGATE_COEFF     8
#define SAMPLE_DPU_MATCH_FIRST_PENALTY_COEF     32
#define SAMPLE_DPU_MATCH_SECOND_PENALTY_COEF    127
#define SAMPLE_DPU_MATCH_FG_INIT_COST_ZERO_THR  8
#define SAMPLE_DPU_MATCH_LOCAL_FAETURE          2
#define SAMPLE_DPU_MATCH_CONF_FILTER_DISP_THR   10
#define SAMPLE_DPU_MATCH_FB_NORM_BITS_8         8
#define SAMPLE_DPU_MATCH_FB_NORM_BITS_10        10
#define SAMPLE_DPU_MATCH_FB_NORM_BITS_12        12
#define SAMPLE_DPU_MATCH_FB_NORM_BITS           SAMPLE_DPU_MATCH_FB_NORM_BITS_12

#define SAMPLE_DPU_MATCH_FB                     48720

#define SAMPLE_DPU_MATCH_Z0                     0
#define SAMPLE_DPU_MATCH_MIN_DEPTH_THR          100
#define SAMPLE_DPU_MATCH_MAX_DEPTH_THR          90000
#define SAMPLE_DPU_MATCH_INVALID_DEPTH          0
#define SAMPLE_DPU_MATCH_DISP_COEF              (-1)
#define AVP_DPU_PATH_MAX                        300

#define sample_avp_dpu_convert_addr_to_ptr(type, addr) ((type *)(uintptr_t)(addr))

#define sample_avp_dpu_smr_free(phys) \
do { \
    if (((phys) != 0)) { \
        ot_smr_free((ot_phys_addr)(phys)); \
        (phys) = 0; \
    } \
} while (0)

typedef enum {
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D0_A11 = 0x0,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D1_A10 = 0x1,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D2_A9  = 0x2,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D3_A8  = 0x3,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D4_A7  = 0x4,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D5_A6  = 0x5,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D6_A5  = 0x6,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D7_A4  = 0x7,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D8_A3  = 0x8,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D9_A2  = 0x9,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D10_A1 = 0xa,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D11_A0 = 0xb,
    OT_DPU_MATCH_DENSITY_ACCURACY_MODE_BUTT
} ot_avp_dpu_match_density_accuracy_mode;

typedef struct {
    ot_eis_handle rect_grp_hdl;
    ot_eis_handle rect_chn_hdl[OT_AVP_DPU_RECT_MAX_CHN_NUM];
    ot_avp_dpu_rect_grp_attr dpu_rect_grp_attr;
    ot_s32 lut_id[OT_AVP_DPU_RECT_MAX_PIPE_NUM];
    ot_avp_dpu_rect_mem_info dpu_rect_mem_info[OT_AVP_DPU_RECT_MAX_PIPE_NUM];
    ot_eis_handle pipe_vrb_pool[OT_AVP_DPU_RECT_MAX_PIPE_NUM];
    ot_buffer_pool_blk vrb_blk[OT_AVP_DPU_RECT_MAX_PIPE_NUM];

    ot_eis_handle match_grp_hdl;
    ot_eis_handle match_chn_hdl[OT_AVP_DPU_MATCH_MAX_CHN_NUM];
    ot_avp_dpu_match_grp_attr dpu_match_grp_attr;
} sample_dpu_cfg;

#define sample_avp_dpu_printf(level_str, msg, ...) \
do { \
    fprintf(stderr, "[level]:%s,[func]:%s [line]:%d [info]:"msg, \
        level_str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
} while (0)
#define sample_avp_dpu_printf_red(level_str, msg, ...) \
do { \
    fprintf(stderr, "\033[0;31m [level]:%s,[func]:%s [line]:%d [info]:"msg"\033[0;39m\n", \
        level_str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
} while (0)

/* error conditions */
#define sample_avp_dpu_trace_err(msg, ...)     sample_avp_dpu_printf_red("Error", msg, ##__VA_ARGS__)
#define sample_avp_dpu_trace_info(msg, ...)    sample_avp_dpu_printf("Info", msg, ##__VA_ARGS__)
/* exps is true, goto */
#define sample_avp_dpu_check_exps_goto(exps, label, msg, ...)                  \
do {                                                                              \
    if ((exps)) {                                                                 \
        sample_avp_dpu_trace_err(msg, ## __VA_ARGS__);                                \
        goto label;                                                               \
    }                                                                             \
} while (0)

/* exps is true, return ret */
#define sample_avp_dpu_check_exps_return(exps, ret, msg, ...)                 \
do {                                                                             \
    if ((exps)) {                                                                \
        sample_avp_dpu_trace_err(msg, ##__VA_ARGS__);                                \
        return (ret);                                                            \
    }                                                                            \
} while (0)                                                                      \

ot_u32 sample_dpu_calc_stride(ot_u32 width, ot_u8 align);

ot_s32 sample_dpu_rect_load_lut(const ot_char *file_name, ot_avp_dpu_rect_mem_info *mem_info, ot_s32 *lut_id);

ot_void sample_dpu_rect_unload_lut(ot_avp_dpu_rect_mem_info *mem_info, ot_s32 lut_id);

/* function : start dpu rect grp. */
ot_s32 sample_dpu_rect_start(ot_eis_handle* rect_grp_hdl, ot_avp_dpu_rect_grp_attr *grp_attr, ot_avp_dpu_rect_chn_attr *chn_attr, ot_eis_handle rect_chn_hdl[OT_AVP_DPU_RECT_MAX_CHN_NUM]);

/* function : stop dpu rect grp */
ot_s32 sample_dpu_rect_stop(ot_eis_handle rect_grp_hdl, ot_eis_handle rect_chn_hdl[OT_AVP_DPU_RECT_MAX_CHN_NUM], ot_avp_dpu_rect_mode rect_mode);

/* function : write frame to file. */
ot_s32 sample_dpu_write_frame_to_file(FILE *fp, ot_u32 ele_size, ot_eis_img_frame *frame_info);

/* function : start dpu match grp. */
ot_s32 sample_dpu_match_start(ot_eis_handle *dpu_match_grp, ot_eis_handle match_chn_hdl[OT_AVP_DPU_MATCH_MAX_CHN_NUM], ot_avp_dpu_match_grp_attr *grp_attr, ot_avp_dpu_match_chn_attr *chn_attr);
/* function : stop dpu match grp */
ot_s32 sample_dpu_match_stop(ot_eis_handle dpu_match_grp, ot_eis_handle match_chn_hdl[OT_AVP_DPU_MATCH_MAX_CHN_NUM]);

ot_s32 sample_dpu_match_create_mem_info(ot_avp_dpu_match_mem_info *mem_info, ot_char *mmb, ot_char *zone, ot_u32 size);

ot_s32 sample_dpu_get_fg_aggregate_coef_and_unique_ratio(ot_avp_dpu_match_density_accuracy_mode density_accuracy_mode, ot_u8 *aggregate_coef, ot_u8 *unique_ratio);

// proc functions
ot_s32  sample_dpu_rect_init(sample_dpu_cfg *dpu_cfg, const ot_eis_img_size *src_size, const ot_eis_img_size *dst_size, const ot_char *lut_file[], ot_u32 file_num);
ot_void sample_dpu_rect_deinit(sample_dpu_cfg *dpu_cfg);
ot_s32  sample_dpu_match_init(sample_dpu_cfg *dpu_cfg, ot_eis_img_size *match_size);
ot_void sample_dpu_match_deinit(sample_dpu_cfg *dpu_cfg);

ot_s32 sample_dpu_rect_bind_match(ot_eis_handle pip_hdl, ot_eis_handle rect_chn_handle, ot_eis_handle match_chn_handle);
ot_s32 sample_dpu_rect_unbind_match(ot_eis_handle pip_hdl, ot_eis_handle rect_chn_handle, ot_eis_handle match_chn_handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* SAMPLE_DPU_H */
