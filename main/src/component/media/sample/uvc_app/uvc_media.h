/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef __UVC_MEDIA_H__
#define __UVC_MEDIA_H__
#include "sample_comm.h"
#include "ot_camera.h"
#include "uvc_dpu.h"

typedef struct {
  ot_u32            dev_no;
  sample_sns_type   sns_type;
  ot_s32            sns_num;
  ot_s32            dev_id[2];
  sample_vi_cfg     vi_cfg[2];
  sample_vproc_attr vp_cfg[2];

  sample_dpu_cfg dpu_cfg;

  ot_bool   is_stream_run;

  ot_bool   get_frm_run;
  pthread_t get_frm_tid;

  ot_bool   dpu_run;
  pthread_t dpu_tid;
  
} uvc_media_ctx;

#ifdef __cplusplus
extern "C" {
#endif

uvc_media_ctx *sample_uvc_get_media_ctx(int dev_no);

ot_s32  sample_uvc_sc_set_idr(ot_u32 dev_no);
ot_s32  sample_uvc_sc_init(ot_void);
ot_s32  sample_uvc_sc_deinit(ot_void);
ot_s32  sample_uvc_sc_startup(ot_u32 dev_no);
ot_s32  sample_uvc_sc_shutdown(ot_u32 dev_no);

ot_s32  sample_uvc_sc_set_property(ot_u32 dev_no, encoder_property *p);
ot_void sample_uvc_sc_get_property(ot_u32 dev_no, encoder_property *p);
ot_s32  sample_uvc_sc_get_send(ot_u32 dev_no);

#ifdef __cplusplus
}
#endif

#endif /* __SAMPLE_UVC_MEDIA_H__ */
