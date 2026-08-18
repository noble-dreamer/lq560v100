/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef __STEREO_NPU_H__
#define __STEREO_NPU_H__

#include "ot_common.h"
#include "stereo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STEREO_NPU_MODEL_PATH   "/data/model/stereo_match.ortm"
#define STEREO_NPU_MAX_INPUT    2
#define STEREO_NPU_MAX_OUTPUT   2

/**
 * @brief Initialize NPU, load the stereo matching model.
 *        Supports both single-output and dual-output models:
 *          Single-output: float32 disparity [1, 1, 240, 320] (includes subpixel)
 *          Dual-output: cost volume [128, H, W] + integer disparity [H, W] uint8
 *        Model expects uint8 RGB888 planar input (Preprocess node handles subtract-128).
 * @return OT_SUCCESS or failure code.
 */
ot_s32 stereo_npu_init(void);

/**
 * @brief Deinitialize NPU: free datasets, unload model, deinit NPU hardware.
 */
void stereo_npu_deinit(void);

/**
 * @brief Run stereo matching inference (dual-output model) in serial mode.
 *        Zero-copy: directly binds CVE output phys addrs as NPU inputs.
 *        A single NPU output buffer set is reused only after synchronous
 *        SubPixel processing in the same pipeline stage finishes.
 *
 * @param left_crop    Left CVE 640x448 RGB888 planar image (raw uint8)
 * @param right_crop   Right CVE 640x448 RGB888 planar image (raw uint8)
 * @param cost_data    [out] Pointer to cost volume in NPU buffer
 * @param cost_size    [out] Cost volume total bytes
 * @param disp_data    [out] Pointer to integer disparity (uint8) in NPU buffer
 * @param disp_size    [out] Integer disparity total bytes
 * @param buf_set_idx  [out] Compatibility output, always 0 in serial mode
 * @return OT_SUCCESS or failure code.
 */
ot_s32 stereo_npu_infer(const ot_avp_cve_img *left_crop,
                         const ot_avp_cve_img *right_crop,
                         void **cost_data, ot_u32 *cost_size,
                         void **disp_data, ot_u32 *disp_size,
                         ot_u32 *buf_set_idx);

/**
 * @brief Compatibility hook for the previous double-buffer pipeline.
 *        It is a no-op while NPU and SubPixel run serially.
 *
 * @param buf_set_idx  Compatibility buffer set index, ignored.
 */
void stereo_npu_release_buf_set(ot_u32 buf_set_idx);

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_NPU_H__ */
