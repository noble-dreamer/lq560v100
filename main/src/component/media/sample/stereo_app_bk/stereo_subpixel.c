/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.

  Sub-pixel disparity refinement using 5-tap parabolic fitting.
  Adapted from costImgSubDisp.cpp reference implementation for
  the dual-output stereo matching model on SD3589C NPU.

  Algorithm: for each pixel, take the integer disparity d from the model,
  then use cost values at d-2, d-1, d, d+1, d+2 to fit a sub-pixel offset.
  Output is Q5 fixed-point: real_disparity = output / 32.

  Performance optimization (v2): Batch-row processing with prefetch and integer
  arithmetic. Under VPROC GDC DMA bandwidth contention (~144MB/s), scattered
  per-row reads cause 60ms latency. Key optimizations:
  1. Process 4 rows per batch — same channel's 4 rows are adjacent in memory
  2. Integer-only arithmetic (no float) — eliminates FPU stalls on A55
  3. __builtin_prefetch on next channel — hides DRAM latency
  4. Static workspace — avoids per-frame malloc overhead
 */

#include <string.h>
#include <stdlib.h>
#include "stereo_subpixel.h"
#include "stereo_types.h"

/* Static workspace for row batch cache.
   Max size: 128 channels * 320 width * 4 rows = 163840 bytes (~160KB) */
#define SUBPIXEL_MAX_CHANNELS  128
#define SUBPIXEL_MAX_WIDTH     320
#define SUBPIXEL_BATCH_ROWS    4

static ot_u8 g_batch_cache[SUBPIXEL_MAX_CHANNELS * SUBPIXEL_MAX_WIDTH * SUBPIXEL_BATCH_ROWS];

ot_s32 stereo_subpixel_process(ot_u32 width, ot_u32 height, ot_u32 channel,
                                const ot_u8 *cost_data,
                                const ot_u8 *disp_data,
                                ot_u16 *sub_disp)
{
    ot_u32 img_size = width * height;
    ot_u32 row, col, r;

    if (!cost_data || !disp_data || !sub_disp) {
        return OT_FAILURE;
    }

    /* Process in batches of SUBPIXEL_BATCH_ROWS rows */
    for (row = 0; row < height; row += SUBPIXEL_BATCH_ROWS) {
        ot_u32 batch_rows = (row + SUBPIXEL_BATCH_ROWS <= height)
                          ? SUBPIXEL_BATCH_ROWS : (height - row);
        ot_u32 batch_offset = row * width;
        ot_u32 batch_size = batch_rows * width;  /* bytes per channel in this batch */

        /* Pass 1: scan batch disparities to find global [d_min, d_max] */
        ot_u8 d_min = 255, d_max = 0;
        for (r = 0; r < batch_rows; r++) {
            const ot_u8 *disp_row = disp_data + (row + r) * width;
            for (col = 0; col < width; col++) {
                ot_u8 d = disp_row[col];
                if (d < d_min) d_min = d;
                if (d > d_max) d_max = d;
            }
        }

        /* Clamp range with 2-pixel margin for the 5-tap filter */
        ot_u32 c_start = (d_min >= 2) ? (d_min - 2) : 0;
        ot_u32 c_end = (d_max + 2 < channel) ? (d_max + 2) : (channel - 1);
        /* Preload needed channels for this batch.
           Layout in cache: selected channels × batch_rows × width, sequential per channel.
           Reading batch_rows consecutive rows from the same channel is nearly
           sequential in DRAM (stride = width = 320B, well within page). */
        for (ot_u32 c = c_start; c <= c_end; c++) {
            ot_u32 cache_off = (c - c_start) * batch_size;
            const ot_u8 *src = cost_data + c * img_size + batch_offset;

            /* Prefetch next channel's data while copying current */
            if (c + 1 <= c_end) {
                const ot_u8 *next_src = cost_data + (c + 1) * img_size + batch_offset;
                __builtin_prefetch(next_src, 0, 1);
                __builtin_prefetch(next_src + 64, 0, 1);
                __builtin_prefetch(next_src + 128, 0, 1);
                __builtin_prefetch(next_src + 192, 0, 1);
                __builtin_prefetch(next_src + 256, 0, 1);
            }

            memcpy(g_batch_cache + cache_off, src, batch_size);
        }

        /* Pass 2: compute sub-pixel disparity using integer arithmetic */
        for (r = 0; r < batch_rows; r++) {
            const ot_u8 *disp_row = disp_data + (row + r) * width;
            ot_u16 *out_row = sub_disp + (row + r) * width;
            ot_u32 row_off_in_batch = r * width;

            for (col = 0; col < width; col++) {
                ot_u8 min_disp = disp_row[col];
                ot_s32 sub_val = (ot_s32)min_disp * 32;  /* Q5 base */

                if (min_disp >= 2 && min_disp <= (channel - 3)) {
                    ot_u32 local_d = min_disp - c_start;
                    ot_u32 idx = row_off_in_batch + col;

                    /* Read 5 cost values from batch cache (all int) */
                    ot_s32 e0 = (ot_s32)g_batch_cache[(local_d - 2) * batch_size + idx];
                    ot_s32 e1 = (ot_s32)g_batch_cache[(local_d - 1) * batch_size + idx];
                    ot_s32 e2 = (ot_s32)g_batch_cache[(local_d)     * batch_size + idx];
                    ot_s32 e3 = (ot_s32)g_batch_cache[(local_d + 1) * batch_size + idx];
                    ot_s32 e4 = (ot_s32)g_batch_cache[(local_d + 2) * batch_size + idx];

                    /* tmp1 = e1 - 2*e2 + e3 (second derivative at center) */
                    ot_s32 tmp1 = e1 - e2 - e2 + e3;

                    if (tmp1 != 0) {
                        ot_s32 d2 = e1 - e3;  /* first derivative sign */
                        ot_s32 tmp1x2 = tmp1 * 2;  /* tmp1 * 2 */

                        ot_s32 tmp2;
                        if (d2 >= 0) {
                            tmp2 = e1 - e2 - e3 + e4;
                        } else {
                            tmp2 = e0 - e1 - e2 + e3;
                        }
                        if (tmp2 <= 0) {
                            tmp2 = 1;
                        }

                        /* Original float: offset = (tmp1x2 + tmp2) * d2 / (tmp1x2 * tmp2 * 2)
                           We need Q5 offset, so multiply numerator by 32 first,
                           then add 16 for rounding (equivalent to +0.5 in Q5) */
                        ot_s32 numer = (ot_s32)(tmp1x2 + tmp2) * d2 * 32;
                        ot_s32 denom = tmp1x2 * tmp2 * 2;

                        /* Integer division with rounding toward nearest */
                        ot_s32 offset;
                        if ((numer >= 0) == (denom >= 0)) {
                            /* same sign: add half-denom for rounding */
                            ot_s32 abs_d = (denom > 0) ? denom : -denom;
                            offset = (numer + abs_d / 2) / denom;
                        } else {
                            /* different sign: subtract half-denom for rounding */
                            ot_s32 abs_d = (denom > 0) ? denom : -denom;
                            offset = (numer - abs_d / 2) / denom;
                        }

                        sub_val += offset;
                        if (sub_val < 0) sub_val = 0;
                        if (sub_val > (ot_s32)(channel * 32)) sub_val = channel * 32;
                    }
                }

                out_row[col] = (ot_u16)sub_val;
            }
        }
    }

    return OT_SUCCESS;
}
