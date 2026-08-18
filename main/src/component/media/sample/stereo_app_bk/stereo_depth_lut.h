/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.

  Disparity pseudo-color lookup table.
  Maps an 8-bit disparity value (0..255) to a YUV color, producing a
  jet-style colormap: near (large disparity) -> red, far (small disparity)
  -> blue. Disparity 0 (invalid) is rendered black.

  Adapted from sample/uvc_app/depth_lut.{h,c}. Symbols are prefixed with
  "stereo_" to avoid clashing with the uvc_app version when both objects
  are linked into the same image.
*/
#ifndef STEREO_DEPTH_LUT_H
#define STEREO_DEPTH_LUT_H

#include <stdint.h>

/* LUT size — full 8-bit disparity range. */
#define STEREO_DEPTH_LUT_SIZE 256

typedef struct {
    uint8_t y;
    uint8_t u;
    uint8_t v;
} stereo_yuv_color;

/* Return the YUV color mapped from an 8-bit disparity value. */
const stereo_yuv_color *stereo_depth_to_yuv(uint8_t disparity);

#endif /* STEREO_DEPTH_LUT_H */
