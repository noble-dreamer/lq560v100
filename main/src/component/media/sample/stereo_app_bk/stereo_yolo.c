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

#define STEREO_YOLO_CLASS_NUM   (80)
#define STEREO_YOLO_ANCHOR_NUM  (3)
#define STEREO_YOLO_OBJ_THRESH  (0.6f)
#define STEREO_YOLO_NMS_THRESH  (0.2f)

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

ot_s32 stereo_yolo_preprocess(const ot_u8 *yuv420sp)
{
    if (!g_yolo_inited || yuv420sp == NULL) {
        return OT_FAILURE;
    }
    if (g_yolo.input.dtype != OT_AVP_DTYPE_UINT8 ||
        g_yolo.input.len != STEREO_YOLO_INPUT_BYTES) {
        stereo_log_write("[stereo_yolo] unexpected input: dtype=%d len=%llu\n",
                         g_yolo.input.dtype, (unsigned long long)g_yolo.input.len);
        return OT_FAILURE;
    }
    memcpy((ot_u8 *)g_yolo.input.virt_addr, yuv420sp, STEREO_YOLO_INPUT_BYTES);
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
