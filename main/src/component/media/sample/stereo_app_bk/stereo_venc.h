/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef __STEREO_VENC_H__
#define __STEREO_VENC_H__

#include "ot_common.h"
#include "ot_eis_venc.h"
#include "stereo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize 2 VENC JPEG channels:
 *        ch0: left  1280x1080 -> JPEG
 *        ch1: right 1280x1080 -> JPEG
 * @return OT_SUCCESS or failure code.
 */
ot_s32 stereo_venc_init(void);

/**
 * @brief Destroy all VENC channels.
 */
void stereo_venc_deinit(void);

/**
 * @brief Encode one frame triplet: left/right JPEG + disparity LZ4.
 *        Disparity is uint16 Q5 sub-pixel data, LZ4 compressed for network.
 *
 * @param left_frame       Full-res left VPROC frame
 * @param right_frame      Full-res right VPROC frame
 * @param disparity_data   640x448 uint16 sub-pixel disparity buffer (as ot_u8*)
 * @param disparity_bytes  Total bytes of disparity data (W*H*2)
 * @param output           Output packets (left JPEG, right JPEG, disparity LZ4)
 * @return OT_SUCCESS or failure code.
 */
ot_s32 stereo_venc_encode(const ot_eis_img_frame *left_frame,
                           const ot_eis_img_frame *right_frame,
                           const ot_u8 *disparity_data,
                           ot_u32 disparity_bytes,
                           stereo_venc_output_t *output);

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_VENC_H__ */
