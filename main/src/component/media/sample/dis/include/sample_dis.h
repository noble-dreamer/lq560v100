/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef OT_SAMPLE_DIS_H
#define OT_SAMPLE_DIS_H

#include "ot_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

ot_bool sample_dis_get_save_stream(void);
ot_void sample_dis_set_save_stream(ot_bool save_stream);
ot_bool sample_dis_get_send_data_statue(ot_void);
ot_void sample_dis_set_send_data_statue(ot_bool send_data);
ot_void sample_dis_set_venc_chn_size(ot_eis_img_size img_size);
ot_void sample_dis_pause(ot_char *s);
ot_s32 sample_dis_start_sample(sample_vi_cfg *vi_cfg, sample_vproc_attr *vproc_attr, sample_vo_attr *vo_attr, ot_eis_img_size *img_size);
ot_s32 sample_dis_stop_sample(sample_vi_cfg *vi_cfg, sample_vproc_attr *vproc_attr, sample_vo_attr *vo_attr);
ot_void sample_dis_stop_sample_without_sys_exit(sample_vi_cfg *vi_cfg, sample_vproc_attr *vproc_attr, sample_vo_attr *vo_attr);

#ifdef __cplusplus
}
#endif /* End of __cplusplus */

#endif /* OT_SAMPLE_DIS_H */