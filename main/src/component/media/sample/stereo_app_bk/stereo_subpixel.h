/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.

  Sub-pixel disparity refinement from dual-output stereo model.
  Takes the cost volume [C, H, W] and integer disparity [H, W],
  produces a Q5 fixed-point uint16 sub-pixel disparity [H, W].
  Real disparity = output_value / 32.
 */

#ifndef __STEREO_SUBPIXEL_H__
#define __STEREO_SUBPIXEL_H__

#include "ot_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute sub-pixel disparity from cost volume and integer disparity.
 *        Uses 5-tap parabolic fitting on the cost volume around the integer
 *        minimum to refine to sub-pixel accuracy.
 *
 *        Cost volume layout: channel-major [C, H*W], element type uint8.
 *        Integer disparity: [H*W], element type uint8.
 *        Output: [H*W], element type uint16 (Q5 fixed-point, real = val/32).
 *
 * @param width       Disparity map width
 * @param height      Disparity map height
 * @param channel     Number of cost volume channels (disparity candidates)
 * @param cost_data   Cost volume pointer, [channel * width * height] uint8
 * @param disp_data   Integer disparity pointer, [width * height] uint8
 * @param sub_disp    Output sub-pixel disparity, [width * height] uint16, caller-allocated
 * @return OT_SUCCESS or OT_FAILURE
 */
ot_s32 stereo_subpixel_process(ot_u32 width, ot_u32 height, ot_u32 channel,
                                const ot_u8 *cost_data,
                                const ot_u8 *disp_data,
                                ot_u16 *sub_disp);

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_SUBPIXEL_H__ */
