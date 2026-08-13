#ifndef DEPTH_LUT_H
#define DEPTH_LUT_H

#include <stdint.h>

// LUT表大小 - 支持0-255深度值
#define LUT_SIZE 256

// YUV颜色结构体
typedef struct {
    uint8_t Y;
    uint8_t U;
    uint8_t V;
} YUVColor;

YUVColor *depth_to_yuv(uint8_t depth);

#endif