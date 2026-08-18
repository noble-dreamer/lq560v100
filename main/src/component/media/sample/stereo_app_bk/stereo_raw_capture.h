#ifndef __STEREO_RAW_CAPTURE_H__
#define __STEREO_RAW_CAPTURE_H__

#include "ot_common.h"
#include "ot_eis_vi_api.h"

#ifdef __cplusplus
extern "C" {
#endif

ot_s32 stereo_raw_capture_start(ot_vi_chn_handle left_chn_hdl,
                                 ot_vi_chn_handle right_chn_hdl);
void stereo_raw_capture_stop(void);

#ifdef __cplusplus
}
#endif

#endif
