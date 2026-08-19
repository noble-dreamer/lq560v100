/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
  tiny-yolov3 detection model: load, async inference, in-place decode + NMS.
 */

#ifndef __STEREO_YOLO_H__
#define __STEREO_YOLO_H__

#include "ot_common.h"
#include "ot_eis_video.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 新相机 /data 分区 64MB（剩余 ~32MB），yolo 模型放 /data/model 持久化。 */
#define STEREO_YOLO_MODEL_PATH   "/data/model/tiny-yolov3_yuv420sp_b.ortm"
#define STEREO_YOLO_INPUT_DIM    (416)
#define STEREO_YOLO_INPUT_BYTES  (STEREO_YOLO_INPUT_DIM * STEREO_YOLO_INPUT_DIM * 3 / 2)
#define STEREO_YOLO_DET_W        (416)
#define STEREO_YOLO_DET_H        (312)
#define STEREO_YOLO_TOP_PAD      ((STEREO_YOLO_INPUT_DIM - STEREO_YOLO_DET_H) / 2)
#define STEREO_YOLO_MAX_BOX      (4096)

typedef struct {
    float   x1, y1, x2, y2;   /* 416x416 model-input space */
    float   score;
    ot_u32  class_id;
    ot_bool suppressed;
} stereo_yolo_box_t;

ot_s32 stereo_yolo_init(void);
void   stereo_yolo_deinit(void);

/* mmap the 416x312 detection frame and letterbox it (52/26 gray rows) into
   the 416x416 model input tensor. USER-mode virt_addr is unreliable, so the
   copy maps phys_addr[0] with ot_smr_mmap and locates UV at phys[1]-phys[0]. */
ot_s32 stereo_yolo_preprocess(const ot_eis_img_frame *frame);

/* Async two-phase inference (MEDIUM priority, same as the stereo model). */
ot_s32 stereo_yolo_trigger(void);
ot_s32 stereo_yolo_wait(void);

/* In-place decode + NMS on the stride-aligned outputs. Returns the kept
 * box count; kept boxes are packed at the front of `boxes`. */
ot_u32 stereo_yolo_decode(stereo_yolo_box_t *boxes, ot_u32 max_boxes);

/* Map 416x416 boxes to the 1280x1080 left frame (x*1280/416,
 * y=60+(y-52)*960/312, clamped) and draw a colored rectangle outline on the
 * Y/UV planes: the frame is USER-mode, so it is mapped via ot_smr_mmap and
 * the box interior is left untouched. Each class gets its own saturated
 * outline color (golden-angle hue -> YUV), and a "CLASS_NAME SCORE" label
 * (5x7 bitmap font, class-colored background, adaptive text) is drawn at
 * each box top-left. If disp_q5 (640x448 uint16 Q5) is provided, the stereo
 * disparity is sampled at the box's left/right edge midpoints, linearly
 * interpolated to the center point and converted to distance
 * (Z = fx*baseline/disp); "X.XXm" is drawn centered in the box when valid. */
void stereo_yolo_set_depth_calib(float fx_disp, float baseline_mm);
void stereo_yolo_draw_left(const stereo_yolo_box_t *boxes, ot_u32 box_count,
                           const ot_eis_img_frame *left,
                           const ot_u16 *disp_q5, ot_u32 disp_w, ot_u32 disp_h);

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_YOLO_H__ */
