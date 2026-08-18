/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef __STEREO_CVE_H__
#define __STEREO_CVE_H__

#include "ot_common.h"
#include "ot_avp_cve.h"
#include "stereo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize CVE module and pre-allocate intermediate buffers.
 *        Creates:
 *         - left/right 640x540 downsampled images (SMR-backed)
 *         - left/right 640x448 cropped images (SMR-backed)
 *        Also pre-allocates the CVE resize auxiliary memory.
 * @return OT_SUCCESS or failure code.
 */
ot_s32 stereo_cve_init(void);

/**
 * @brief Free all CVE images and auxiliary memory.
 */
void stereo_cve_deinit(void);

/**
 * @brief Run the full CVE preprocessing on a stereo frame pair:
 *        1. SMR-mmap VPROC frames (zero-copy)
 *        2. Bilinear downsample 1280x1080 -> 640x540 for left and right
 *        3. DMA 2D center crop 640x540 -> 640x448 for left and right
 *        Results are stored in the pre-allocated CVE images.
 *
 * @param frame_pair   Input: full-res VPROC frames
 * @param left_crop    Output: left 640x448 CVE image handle (fills phys/virt addresses)
 * @param right_crop   Output: right 640x448 CVE image handle
 * @return OT_SUCCESS or failure code.
 */
ot_s32 stereo_cve_process(const stereo_frame_pair_t *frame_pair,
                           ot_avp_cve_img *left_crop,
                           ot_avp_cve_img *right_crop);

/**
 * @brief Get the pre-allocated CVE crop image for left camera.
 */
const ot_avp_cve_img *stereo_cve_get_left_crop(void);

/**
 * @brief Get the pre-allocated CVE crop image for right camera.
 */
const ot_avp_cve_img *stereo_cve_get_right_crop(void);

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_CVE_H__ */
