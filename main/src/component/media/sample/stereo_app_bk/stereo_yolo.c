/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
  tiny-yolov3_yuv420sp detection model for the stereo_app pipeline.
  IO shapes are queried at runtime; detection decode reads the stride-aligned
  NPU outputs in place (lazy objectness-first reads, no bulk copy).
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stereo_yolo.h"
#include "stereo_types.h"
#include "ot_avp_npu_rts.h"
#include "ot_smr.h"

#define STEREO_YOLO_CLASS_NUM   (80)
#define STEREO_YOLO_ANCHOR_NUM  (3)
#define STEREO_YOLO_OBJ_THRESH  (0.6f)
#define STEREO_YOLO_NMS_THRESH  (0.2f)
#define STEREO_YOLO_CROP_Y      (60)   /* detection region offset in the left frame */
#define STEREO_YOLO_CROP_H      (960)  /* detection region height in the left frame */

typedef struct {
    ot_avp_handle        handle;
    ot_u32               input_num;
    ot_u32               output_num;
    ot_avp_tensor        input;
    ot_avp_tensor        outputs[2];
    ot_avp_npu_dataset  *in_ds;
    ot_avp_npu_dataset  *out_ds;
    void                *runtime_buf;
} stereo_yolo_ctx_t;

static stereo_yolo_ctx_t  g_yolo;
static ot_bool            g_yolo_inited = OT_FALSE;

static ot_s32 stereo_yolo_prepare_io(ot_bool is_output)
{
    ot_u32 num = is_output ? g_yolo.output_num : g_yolo.input_num;
    ot_avp_tensor *tensors = is_output ? g_yolo.outputs : &g_yolo.input;
    ot_avp_npu_dataset *ds = is_output ? g_yolo.out_ds : g_yolo.in_ds;

    for (ot_u32 i = 0; i < num; i++) {
        ot_avp_npu_shape shape;
        ot_avp_data_type dtype;
        ot_u32 size;
        ot_u32 stride;
        ot_s32 ret;

        if (is_output) {
            ot_avp_npu_get_output_shape_by_index(g_yolo.handle, i, &shape);
            ot_avp_npu_get_output_dtype_by_index(g_yolo.handle, i, &dtype);
            size   = ot_avp_npu_get_output_size_by_index(g_yolo.handle, i);
            stride = ot_avp_npu_get_output_default_stride(g_yolo.handle, i);
        } else {
            ot_avp_npu_get_input_shape_by_index(g_yolo.handle, i, &shape);
            ot_avp_npu_get_input_dtype_by_index(g_yolo.handle, i, &dtype);
            size   = ot_avp_npu_get_input_size_by_index(g_yolo.handle, i);
            stride = ot_avp_npu_get_input_default_stride(g_yolo.handle, i);
        }

        tensors[i].len = size;
        tensors[i].shape = shape;
        tensors[i].dtype = dtype;
        tensors[i].stride.dims[0] = stride;
        ret = ot_avp_npu_malloc((ot_void **)&tensors[i].virt_addr, size);
        if (ret != 0) {
            stereo_log_write("[stereo_yolo] alloc %s[%u] fail\n",
                             is_output ? "output" : "input", i);
            return ret;
        }
        memset((ot_u8 *)tensors[i].virt_addr, 0, size);
        ret = ot_avp_npu_add_buffer(ds, (ot_u8 *)tensors[i].virt_addr, 0, size, stride);
        if (ret != 0) {
            stereo_log_write("[stereo_yolo] add %s[%u] buffer fail\n",
                             is_output ? "output" : "input", i);
            return ret;
        }
    }
    return OT_SUCCESS;
}

ot_s32 stereo_yolo_init(void)
{
    ot_avp_npu_mdl_config mdl_cfg = {
        .priority_level = OT_AVP_MDL_PRI_MEDIUM,
        .priority_preemp = true,
    };
    ot_u32 runtime_size = 0;
    ot_s32 ret;

    memset(&g_yolo, 0, sizeof(g_yolo));
    ret = ot_avp_npu_load_model(STEREO_YOLO_MODEL_PATH, &g_yolo.handle);
    if (ret != 0) {
        stereo_log_write("[stereo_yolo] load model fail: %s\n", STEREO_YOLO_MODEL_PATH);
        return ret;
    }
    ot_avp_npu_get_input_num(g_yolo.handle, &g_yolo.input_num);
    ot_avp_npu_get_output_num(g_yolo.handle, &g_yolo.output_num);
    if (g_yolo.input_num != 1 || g_yolo.output_num > 2) {
        stereo_log_write("[stereo_yolo] unexpected io: in=%u out=%u\n",
                         g_yolo.input_num, g_yolo.output_num);
        goto fail;
    }

    ret = ot_avp_npu_get_runtime_buffer_size(g_yolo.handle, &runtime_size);
    if (ret == 0 && runtime_size != 0) {
        ret = ot_avp_npu_malloc(&g_yolo.runtime_buf, runtime_size);
        if (ret == 0) {
            ret = ot_avp_npu_set_runtime_buffer(g_yolo.handle,
                                                 (ot_u8 *)g_yolo.runtime_buf,
                                                 runtime_size);
        }
        if (ret != 0) {
            stereo_log_write("[stereo_yolo] runtime buffer fail\n");
            goto fail;
        }
    }

    ret = ot_avp_npu_set_model_config(g_yolo.handle, &mdl_cfg);
    if (ret != 0) {
        stereo_log_write("[stereo_yolo] set config fail, keep default\n");
    }

    g_yolo.in_ds  = ot_avp_npu_create_dataset();
    g_yolo.out_ds = ot_avp_npu_create_dataset();
    if (g_yolo.in_ds == NULL || g_yolo.out_ds == NULL) {
        goto fail;
    }
    ret = stereo_yolo_prepare_io(false);
    if (ret == OT_SUCCESS) {
        ret = stereo_yolo_prepare_io(true);
    }
    if (ret != OT_SUCCESS) {
        goto fail;
    }

    g_yolo_inited = OT_TRUE;
    stereo_log_write("[stereo_yolo] model ready (in=%u out=%u)\n",
                     g_yolo.input_num, g_yolo.output_num);
    return OT_SUCCESS;

fail:
    stereo_yolo_deinit();
    return ret;
}

void stereo_yolo_deinit(void)
{
    if (!g_yolo_inited && g_yolo.handle == NULL) {
        memset(&g_yolo, 0, sizeof(g_yolo));
        return;
    }
    if (g_yolo.in_ds)  ot_avp_npu_destroy_dataset(g_yolo.in_ds);
    if (g_yolo.out_ds) ot_avp_npu_destroy_dataset(g_yolo.out_ds);
    if (g_yolo.input.virt_addr)  ot_avp_npu_free((ot_void *)g_yolo.input.virt_addr);
    for (ot_u32 i = 0; i < g_yolo.output_num; i++) {
        if (g_yolo.outputs[i].virt_addr) {
            ot_avp_npu_free((ot_void *)g_yolo.outputs[i].virt_addr);
        }
    }
    if (g_yolo.runtime_buf) ot_avp_npu_free(g_yolo.runtime_buf);
    if (g_yolo.handle) ot_avp_npu_unload_model(g_yolo.handle);
    memset(&g_yolo, 0, sizeof(g_yolo));
    g_yolo_inited = OT_FALSE;
}

static void stereo_yolo_fill_rows(ot_u8 *dst, ot_u32 dst_stride, ot_u32 rows,
                                  const ot_u8 *src, ot_u32 src_stride, ot_u8 fill)
{
    for (ot_u32 r = 0; r < rows; r++) {
        if (src != NULL) {
            memcpy(dst + (size_t)r * dst_stride, src + (size_t)r * src_stride,
                   dst_stride);
        } else {
            memset(dst + (size_t)r * dst_stride, fill, dst_stride);
        }
    }
}

ot_s32 stereo_yolo_preprocess(const ot_eis_img_frame *frame)
{
    const ot_u8 *y = NULL;
    const ot_u8 *uv = NULL;
    ot_void *map = NULL;
    ot_u32 map_size;
    ot_u64 c_off;
    ot_u8 *dst;
    ot_u32 top_pad = STEREO_YOLO_TOP_PAD;
    ot_u32 y_size = STEREO_YOLO_INPUT_DIM * STEREO_YOLO_INPUT_DIM;

    if (!g_yolo_inited || frame == NULL) {
        return OT_FAILURE;
    }
    if (g_yolo.input.dtype != OT_AVP_DTYPE_UINT8 ||
        g_yolo.input.len != STEREO_YOLO_INPUT_BYTES) {
        stereo_log_write("[stereo_yolo] unexpected input: dtype=%d len=%llu\n",
                         g_yolo.input.dtype, (unsigned long long)g_yolo.input.len);
        return OT_FAILURE;
    }
    if (frame->attr.width != STEREO_YOLO_DET_W ||
        frame->attr.height != STEREO_YOLO_DET_H) {
        return OT_FAILURE;
    }

    map_size = frame->buff.stride[0] * frame->attr.height +
               frame->buff.stride[1] * frame->attr.height / 2;
    c_off = frame->buff.phys_addr[1] - frame->buff.phys_addr[0];
    if (c_off >= map_size) {
        return OT_FAILURE;
    }
    if (ot_smr_mmap(frame->buff.phys_addr[0], map_size, OT_TRUE, &map) !=
        OT_SUCCESS) {
        return OT_FAILURE;
    }
    y  = (const ot_u8 *)map;
    uv = y + c_off;
    dst = (ot_u8 *)g_yolo.input.virt_addr;

    /* Y plane: 52 gray rows, 312 content rows, 52 gray rows */
    stereo_yolo_fill_rows(dst, STEREO_YOLO_INPUT_DIM, top_pad, NULL, 0, 128);
    stereo_yolo_fill_rows(dst + (size_t)top_pad * STEREO_YOLO_INPUT_DIM,
                          STEREO_YOLO_INPUT_DIM, STEREO_YOLO_DET_H, y,
                          frame->buff.stride[0], 128);
    stereo_yolo_fill_rows(dst + (size_t)(top_pad + STEREO_YOLO_DET_H) *
                          STEREO_YOLO_INPUT_DIM, STEREO_YOLO_INPUT_DIM,
                          top_pad, NULL, 0, 128);

    /* UV plane: 26 gray rows, 156 content rows, 26 gray rows */
    stereo_yolo_fill_rows(dst + y_size, STEREO_YOLO_INPUT_DIM, top_pad / 2,
                          NULL, 0, 128);
    stereo_yolo_fill_rows(dst + y_size + (size_t)(top_pad / 2) *
                          STEREO_YOLO_INPUT_DIM, STEREO_YOLO_INPUT_DIM,
                          STEREO_YOLO_DET_H / 2, uv, frame->buff.stride[1], 128);
    stereo_yolo_fill_rows(dst + y_size + (size_t)(top_pad / 2 +
                          STEREO_YOLO_DET_H / 2) * STEREO_YOLO_INPUT_DIM,
                          STEREO_YOLO_INPUT_DIM, top_pad / 2, NULL, 0, 128);

    ot_smr_munmap(map, map_size);
    return OT_SUCCESS;
}

ot_s32 stereo_yolo_trigger(void)
{
    if (!g_yolo_inited) {
        return OT_FAILURE;
    }
    return ot_avp_npu_trigger(g_yolo.handle, g_yolo.in_ds, g_yolo.out_ds);
}

ot_s32 stereo_yolo_wait(void)
{
    if (!g_yolo_inited) {
        return OT_FAILURE;
    }
    return ot_avp_npu_wait(g_yolo.handle, -1);
}

static float stereo_yolo_sigmoid(float x)
{
    return 1.0f / (1.0f + expf(-x));
}

static float stereo_yolo_iou(const stereo_yolo_box_t *a, const stereo_yolo_box_t *b)
{
    float ix1 = a->x1 > b->x1 ? a->x1 : b->x1;
    float iy1 = a->y1 > b->y1 ? a->y1 : b->y1;
    float ix2 = a->x2 < b->x2 ? a->x2 : b->x2;
    float iy2 = a->y2 < b->y2 ? a->y2 : b->y2;
    float iw = ix2 - ix1;
    float ih = iy2 - iy1;
    float inter;

    if (iw <= 0.0f || ih <= 0.0f) {
        return 0.0f;
    }
    inter = iw * ih;
    return inter / ((a->x2 - a->x1) * (a->y2 - a->y1) +
                    (b->x2 - b->x1) * (b->y2 - b->y1) - inter + 1e-9f);
}

/* Decode one NHWC [1,H,W,255] output in place: read objectness first and only
 * touch the 85-float group when it passes the threshold. */
static ot_u32 stereo_yolo_decode_tensor(const ot_avp_tensor *tensor,
                                        ot_s32 grid_h, ot_s32 grid_w,
                                        const ot_s32 mask[STEREO_YOLO_ANCHOR_NUM],
                                        stereo_yolo_box_t *boxes,
                                        ot_u32 box_num, ot_u32 max_boxes)
{
    static const float anchors[6][2] = {
        {10.0f, 14.0f}, {23.0f, 27.0f}, {37.0f, 58.0f},
        {81.0f, 82.0f}, {135.0f, 169.0f}, {344.0f, 319.0f},
    };
    const ot_u8 *base = (const ot_u8 *)tensor->virt_addr;
    const ot_u64 cell_stride = tensor->stride.dims[0];

    for (ot_s32 cell = 0; cell < grid_h * grid_w; cell++) {
        const float *cell_data =
            (const float *)(base + (ot_u64)cell * cell_stride);
        ot_s32 row = cell / grid_w;
        ot_s32 col = cell % grid_w;

        for (ot_s32 a = 0; a < STEREO_YOLO_ANCHOR_NUM; a++) {
            const float *p = cell_data + a * (4 + 1 + STEREO_YOLO_CLASS_NUM);
            float conf = stereo_yolo_sigmoid(p[4]);
            float best = 0.0f;
            ot_u32 best_id = 0;

            if (conf < STEREO_YOLO_OBJ_THRESH) {
                continue;
            }
            for (ot_s32 c = 0; c < STEREO_YOLO_CLASS_NUM; c++) {
                float cls = stereo_yolo_sigmoid(p[5 + c]);
                if (cls > best) {
                    best = cls;
                    best_id = (ot_u32)c;
                }
            }
            if (box_num >= max_boxes) {
                return box_num;
            }
            {
                /* exp computed once and reused for both box edges */
                float cx = (stereo_yolo_sigmoid(p[0]) + (float)col) *
                           STEREO_YOLO_INPUT_DIM / (float)grid_w;
                float cy = (stereo_yolo_sigmoid(p[1]) + (float)row) *
                           STEREO_YOLO_INPUT_DIM / (float)grid_h;
                float bw = expf(p[2]) * anchors[mask[a]][0];
                float bh = expf(p[3]) * anchors[mask[a]][1];
                boxes[box_num].x1 = cx - bw * 0.5f;
                boxes[box_num].y1 = cy - bh * 0.5f;
                boxes[box_num].x2 = cx + bw * 0.5f;
                boxes[box_num].y2 = cy + bh * 0.5f;
            }
            boxes[box_num].score = conf * best;
            boxes[box_num].class_id = best_id;
            boxes[box_num].suppressed = OT_FALSE;
            box_num++;
        }
    }
    return box_num;
}

static int stereo_yolo_score_desc(const void *lhs, const void *rhs)
{
    const stereo_yolo_box_t *a = (const stereo_yolo_box_t *)lhs;
    const stereo_yolo_box_t *b = (const stereo_yolo_box_t *)rhs;

    return (a->score < b->score) - (a->score > b->score);
}

ot_u32 stereo_yolo_decode(stereo_yolo_box_t *boxes, ot_u32 max_boxes)
{
    static const ot_s32 masks[2][STEREO_YOLO_ANCHOR_NUM] = {
        {0, 1, 2}, {3, 4, 5},
    };
    ot_u32 box_num = 0;
    ot_u32 kept = 0;

    if (!g_yolo_inited || boxes == NULL || max_boxes == 0) {
        return 0;
    }
    if (g_yolo.output_num != 2 ||
        g_yolo.outputs[0].dtype != OT_AVP_DTYPE_F32 ||
        g_yolo.outputs[1].dtype != OT_AVP_DTYPE_F32) {
        stereo_log_write("[stereo_yolo] unexpected outputs for decode\n");
        return 0;
    }

    for (ot_s32 o = 0; o < g_yolo.output_num; o++) {
        box_num = stereo_yolo_decode_tensor(&g_yolo.outputs[o],
                                            g_yolo.outputs[o].shape.dims[1],
                                            g_yolo.outputs[o].shape.dims[2],
                                            masks[o], boxes, box_num, max_boxes);
    }

    qsort(boxes, box_num, sizeof(stereo_yolo_box_t), stereo_yolo_score_desc);
    for (ot_u32 i = 0; i < box_num; i++) {
        if (boxes[i].suppressed) {
            continue;
        }
        for (ot_u32 j = i + 1; j < box_num; j++) {
            if (!boxes[j].suppressed &&
                stereo_yolo_iou(&boxes[i], &boxes[j]) > STEREO_YOLO_NMS_THRESH) {
                boxes[j].suppressed = OT_TRUE;
            }
        }
    }

    /* Pack kept boxes at the front */
    for (ot_u32 i = 0; i < box_num; i++) {
        if (!boxes[i].suppressed) {
            if (kept != i) {
                boxes[kept] = boxes[i];
            }
            kept++;
        }
    }
    return kept;
}

void stereo_yolo_draw_left(const stereo_yolo_box_t *boxes, ot_u32 box_count,
                           const ot_eis_img_frame *left)
{
    const ot_u32 outline = 3;
    ot_void *map = NULL;
    ot_u32 map_size;
    ot_u64 c_off;
    ot_u8 *y = NULL;
    ot_u8 *vu = NULL;
    ot_u32 w;
    ot_u32 h;
    ot_u32 sy;
    ot_u32 suv;

    if (boxes == NULL || box_count == 0 || left == NULL) {
        return;
    }
    w = left->attr.width;
    h = left->attr.height;
    if (w == 0 || h == 0) {
        return;
    }
    map_size = left->buff.stride[0] * h + left->buff.stride[1] * h / 2;
    c_off = left->buff.phys_addr[1] - left->buff.phys_addr[0];
    if (c_off >= map_size) {
        return;
    }
    if (ot_smr_mmap(left->buff.phys_addr[0], map_size, OT_TRUE, &map) !=
        OT_SUCCESS) {
        return;
    }
    y  = (ot_u8 *)map;
    vu = (ot_u8 *)map + c_off;
    sy  = left->buff.stride[0];
    suv = left->buff.stride[1];

    for (ot_u32 b = 0; b < box_count; b++) {
        /* 416-space -> left 1280x1080: content rows 52..363 of the letterbox
           map to left rows 60..1019 (960 rows). Clamp to the frame. */
        ot_s32 x1 = (ot_s32)(boxes[b].x1 * w / STEREO_YOLO_INPUT_DIM);
        ot_s32 x2 = (ot_s32)(boxes[b].x2 * w / STEREO_YOLO_INPUT_DIM);
        ot_s32 y1 = (ot_s32)(STEREO_YOLO_CROP_Y +
                     (boxes[b].y1 - STEREO_YOLO_TOP_PAD) * STEREO_YOLO_CROP_H /
                     STEREO_YOLO_DET_H);
        ot_s32 y2 = (ot_s32)(STEREO_YOLO_CROP_Y +
                     (boxes[b].y2 - STEREO_YOLO_TOP_PAD) * STEREO_YOLO_CROP_H /
                     STEREO_YOLO_DET_H);

        if (x1 < 0) x1 = 0;
        if (y1 < 0) y1 = 0;
        if (x2 >= (ot_s32)w) x2 = (ot_s32)w - 1;
        if (y2 >= (ot_s32)h) y2 = (ot_s32)h - 1;
        if (x2 < x1 || y2 < y1) {
            continue;
        }

        /* Red rectangle outline on Y + chroma, gray chroma inside the box */
        for (ot_s32 r = y1; r <= y2; r++) {
            for (ot_s32 c = x1; c <= x2; c++) {
                ot_bool border =
                    (r < y1 + (ot_s32)outline || r > y2 - (ot_s32)outline ||
                     c < x1 + (ot_s32)outline || c > x2 - (ot_s32)outline);
                ot_u8 *vp = vu + (ot_u32)(r / 2) * suv + (ot_u32)(c / 2) * 2;

                if (border) {
                    y[(ot_u32)r * sy + (ot_u32)c] = 76;
                    /* YVU order: V=170, U=90 -> saturated red */
                    vp[0] = 170;
                    vp[1] = 90;
                } else {
                    vp[0] = 128;
                    vp[1] = 128;
                }
            }
        }
    }

    /* Cached mapping: write the CPU-drawn rows back so the VENC DMA sees them */
    ot_smr_flush_cache(left->buff.phys_addr[0], map, map_size);
    ot_smr_munmap(map, map_size);
}
