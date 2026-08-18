/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.

  Stereo rectification via VPROC GDC 2D XY-LUT.
  The LUT files combine lens undistortion and stereo epipolar rectification
  in native 1080x1280 sensor space before full-frame crop and Rotation270.
 */

#ifndef __STEREO_RECTIFY_H__
#define __STEREO_RECTIFY_H__

#include "ot_common.h"
#include "ot_eis_vproc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STEREO_LUT_LEFT_PATH    "/opt/stereo/lut_left.bin"
#define STEREO_LUT_RIGHT_PATH   "/opt/stereo/lut_right.bin"

#define STEREO_CAM_LEFT   0
#define STEREO_CAM_RIGHT  1

/**
 * @brief Load precomputed XY-LUT files for full undistort + stereo rectification.
 *        Must be called before VPROC group start so callers can adjust grp max size.
 *
 * @param src_width   Source image width  (sensor native, e.g. 1080)
 * @param src_height  Source image height (sensor native, e.g. 1280)
 * @return OT_SUCCESS if both LUTs are loaded, first error code otherwise
 */
ot_s32 stereo_rectify_load_xylut(ot_u32 src_width, ot_u32 src_height);

/**
 * @brief Check if XY-LUT rectification is loaded and ready.
 */
ot_bool stereo_rectify_is_xylut_enabled(void);

/**
 * @brief Get XY-LUT GDC config pointer.
 * @param cam_idx  STEREO_CAM_LEFT or STEREO_CAM_RIGHT
 * @return Pointer to static ot_eis_vproc_gdc_cfg, or OT_NULL
 */
ot_eis_vproc_gdc_cfg *stereo_rectify_get_gdc_cfg(int cam_idx);

/**
 * @brief Get XY-LUT config pointer.
 * @param cam_idx  STEREO_CAM_LEFT or STEREO_CAM_RIGHT
 * @return Pointer to static ot_eis_vproc_xylut_param, or OT_NULL
 */
ot_eis_vproc_xylut_param *stereo_rectify_get_xylut_param(int cam_idx);

/**
 * @brief Apply GDC 2D XY-LUT rectification after VPROC groups are started.
 *        This applies undistort + stereo rectification in one hardware GDC stage.
 *
 * @param left_grp_hdl   Left VPROC group handle
 * @param right_grp_hdl  Right VPROC group handle
 * @return OT_SUCCESS on success, first error code on failure
 */
ot_s32 stereo_rectify_apply_xylut(ot_eis_handle left_grp_hdl, ot_eis_handle right_grp_hdl);

/**
 * @brief De-initialize stereo rectification and release LUT resources.
 */
void stereo_rectify_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_RECTIFY_H__ */
