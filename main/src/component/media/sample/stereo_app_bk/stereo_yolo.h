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

/* /data (35MB UBIFS) holds the 28MB encrypted stereo model and cannot fit
   the 9MB yolo model; /opt/model persists across reboot the same way. */
#define STEREO_YOLO_MODEL_PATH   "/opt/model/tiny-yolov3_yuv420sp_b.ortm"
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

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_YOLO_H__ */
