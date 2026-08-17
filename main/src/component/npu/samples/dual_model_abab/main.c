/*
 * dual_model_abab: two board offline models run in an ABAB async pipeline.
 *
 * usage:
 *   sample_dual_model_abab [modelA] [inputA] [modelB] [inputB] [repeat] [output_dir]
 */
#include "ot_avp_npu_rts.h"
#include "file_utils.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef SIMULATOR
#include "ot_smr.h"
#ifndef OT_AVP_NPU_V200
#include "ot_eis_media_sys.h"
#endif
#endif

#define MODEL_NUM        (2)
#define MAX_IO_NUM       (8)
#define NAME_MAX         (16)
#define TOP_K            (5)
#define YOLO_CLASS_NUM   (80)
#define YOLO_ANCHOR_NUM  (3)
#define YOLO_OUTPUT_NUM  (2)
#define YOLO_INPUT_DIM   (416)
#define YOLO_OBJ_THRESH  (0.6f)
#define YOLO_NMS_THRESH  (0.2f)
#define YOLO_MAX_BOX     (4096)

#define DEFAULT_MODEL_A  "../data/model/classification/resnet50_binary_b.ortm"
#define DEFAULT_INPUT_A  "../data/ImageNet/binary/ILSVRC2012_val_00024327.bin"
#define DEFAULT_MODEL_B  "../data/model/classification/mobilenetv2_rgbplanar_b.ortm"
#define DEFAULT_INPUT_B  "../data/ImageNet/rgbplanar/ILSVRC2012_val_00024327.rgb"

typedef enum {
    MODEL_KIND_RAW = 0,
    MODEL_KIND_CLASSIFY,
    MODEL_KIND_DETECT_YOLOV3,
} model_kind;

typedef struct {
    char name[NAME_MAX];
    ot_avp_handle handle;
    ot_u32 input_num;
    ot_u32 output_num;
    ot_avp_tensor inputs[MAX_IO_NUM];
    ot_avp_tensor outputs[MAX_IO_NUM];
    ot_avp_npu_dataset *input_dataset;
    ot_avp_npu_dataset *output_dataset;
    void *runtime_buf;
    model_kind kind;
} model_slot;

static void usage(const char *prog)
{
    printf("Usage: %s [modelA] [inputA] [modelB] [inputB] [repeat] [output_dir]\n", prog);
    printf("output_dir is optional; omit it to run in perf mode without saving results.\n");
    printf("Defaults:\n  A: %s <- %s\n  B: %s <- %s\n",
           DEFAULT_MODEL_A, DEFAULT_INPUT_A, DEFAULT_MODEL_B, DEFAULT_INPUT_B);
}

static ot_s32 model_prepare_io(model_slot *slot, bool is_output)
{
    ot_u32 num = is_output ? slot->output_num : slot->input_num;
    ot_avp_tensor *tensors = is_output ? slot->outputs : slot->inputs;
    ot_avp_npu_dataset *dataset = is_output ? slot->output_dataset : slot->input_dataset;
    ot_u32 i;

    for (i = 0; i < num; i++) {
        ot_avp_npu_shape shape;
        ot_avp_data_type dtype;
        ot_avp_npu_dformat format;
        ot_u32 size;
        ot_u32 stride;
        ot_s32 ret;

        if (is_output) {
            ot_avp_npu_get_output_shape_by_index(slot->handle, i, &shape);
            ot_avp_npu_get_output_dtype_by_index(slot->handle, i, &dtype);
            ot_avp_npu_get_output_format_by_index(slot->handle, i, &format);
            size = ot_avp_npu_get_output_size_by_index(slot->handle, i);
            stride = ot_avp_npu_get_output_default_stride(slot->handle, i);
        } else {
            ot_avp_npu_get_input_shape_by_index(slot->handle, i, &shape);
            ot_avp_npu_get_input_dtype_by_index(slot->handle, i, &dtype);
            ot_avp_npu_get_input_format_by_index(slot->handle, i, &format);
            size = ot_avp_npu_get_input_size_by_index(slot->handle, i);
            stride = ot_avp_npu_get_input_default_stride(slot->handle, i);
        }

        tensors[i].len = size;
        tensors[i].shape = shape;
        tensors[i].dtype = dtype;
        tensors[i].format = format;
        tensors[i].stride.dims[0] = stride;
        ret = ot_avp_npu_malloc((ot_void **)&tensors[i].virt_addr, size);
        if (ret != 0) {
            printf("[%s] alloc %s[%u] fail\n",
                   slot->name, is_output ? "output" : "input", i);
            return ret;
        }
        memset((ot_u8 *)tensors[i].virt_addr, 0, size);
        ret = ot_avp_npu_add_buffer(dataset, (ot_u8 *)tensors[i].virt_addr, 0, size, stride);
        if (ret != 0) {
            printf("[%s] add %s[%u] buffer fail\n",
                   slot->name, is_output ? "output" : "input", i);
            return ret;
        }
    }
    return 0;
}

/* 依据查询到的输出特征识别模型类型，供预处理/后处理分发使用：
 * tiny-yolov3: 两路 F32 [1,H,W,255] 输出（NHWC，H/W 为 26/13）
 * 分类模型: 单路 F32 [1,N] 输出
 * 其他: 按原始数据落盘 */
static model_kind model_detect_kind(const model_slot *slot)
{
    ot_u32 i;

    if (slot->output_num == 2) {
        bool yolov3 = true;

        for (i = 0; i < slot->output_num; i++) {
            const ot_avp_tensor *out = &slot->outputs[i];

            if (out->dtype != OT_AVP_DTYPE_F32 || out->shape.dim_size != 4 ||
                out->shape.dims[0] != 1 || out->shape.dims[1] != out->shape.dims[2] ||
                out->shape.dims[3] != YOLO_ANCHOR_NUM * (4 + 1 + YOLO_CLASS_NUM)) {
                yolov3 = false;
                break;
            }
        }
        if (yolov3) {
            return MODEL_KIND_DETECT_YOLOV3;
        }
    }

    if (slot->output_num >= 1 &&
        slot->outputs[0].dtype == OT_AVP_DTYPE_F32 &&
        slot->outputs[0].shape.dim_size == 2 &&
        slot->outputs[0].shape.dims[0] == 1) {
        return MODEL_KIND_CLASSIFY;
    }
    return MODEL_KIND_RAW;
}

static ot_s32 model_init(model_slot *slot, const char *name, const char *model_path)
{
    ot_avp_npu_mdl_config mdl_cfg = {
        .priority_level = OT_AVP_MDL_PRI_MEDIUM,
        .priority_preemp = true,
    };
    ot_u32 runtime_size = 0;
    ot_s32 ret;

    snprintf(slot->name, sizeof(slot->name), "%s", name);
    ret = ot_avp_npu_load_model(model_path, &slot->handle);
    if (ret != 0) {
        printf("[%s] load fail: %s\n", slot->name, model_path);
        return ret;
    }

    ret = ot_avp_npu_get_input_num(slot->handle, &slot->input_num);
    if (ret == 0) {
        ret = ot_avp_npu_get_output_num(slot->handle, &slot->output_num);
    }
    if (ret != 0 || slot->input_num > MAX_IO_NUM || slot->output_num > MAX_IO_NUM) {
        printf("[%s] invalid io num: in=%u out=%u\n",
               slot->name, slot->input_num, slot->output_num);
        return -1;
    }

    /* 部分模型需要中间临时内存，大小由 API 返回 */
    ret = ot_avp_npu_get_runtime_buffer_size(slot->handle, &runtime_size);
    if (ret == 0 && runtime_size != 0) {
        ret = ot_avp_npu_malloc(&slot->runtime_buf, runtime_size);
        if (ret == 0) {
            ret = ot_avp_npu_set_runtime_buffer(slot->handle, slot->runtime_buf, runtime_size);
        }
        if (ret != 0) {
            printf("[%s] runtime buffer fail\n", slot->name);
            return ret;
        }
    }

    /* trigger+trigger+wait 要求两模型优先级一致，这里统一设置 */
    ret = ot_avp_npu_set_model_config(slot->handle, &mdl_cfg);
    if (ret != 0) {
        /* 默认优先级同为 MEDIUM，且本样例先 wait A 再 wait B，失败不影响调度正确性 */
        printf("[%s] set config fail, keep default priority\n", slot->name);
    }

    slot->input_dataset = ot_avp_npu_create_dataset();
    slot->output_dataset = ot_avp_npu_create_dataset();
    if (slot->input_dataset == NULL || slot->output_dataset == NULL) {
        printf("[%s] create dataset fail\n", slot->name);
        return -1;
    }

    ret = model_prepare_io(slot, false);
    if (ret == 0) {
        ret = model_prepare_io(slot, true);
    }
    if (ret != 0) {
        return ret;
    }

    slot->kind = model_detect_kind(slot);
    printf("[%s] model ready: %s (in=%u out=%u)\n",
           slot->name, model_path, slot->input_num, slot->output_num);
    printf("[%s] model kind: %s\n", slot->name,
           slot->kind == MODEL_KIND_DETECT_YOLOV3 ? "detect-yolov3" :
           slot->kind == MODEL_KIND_CLASSIFY ? "classify" : "raw");
    return 0;
}

static void model_destroy(model_slot *slot)
{
    ot_u32 i;

    if (slot->input_dataset != NULL) {
        ot_avp_npu_destroy_dataset(slot->input_dataset);
    }
    if (slot->output_dataset != NULL) {
        ot_avp_npu_destroy_dataset(slot->output_dataset);
    }
    for (i = 0; i < slot->input_num; i++) {
        if (slot->inputs[i].virt_addr != 0) {
            ot_avp_npu_free((ot_void *)slot->inputs[i].virt_addr);
        }
    }
    for (i = 0; i < slot->output_num; i++) {
        if (slot->outputs[i].virt_addr != 0) {
            ot_avp_npu_free((ot_void *)slot->outputs[i].virt_addr);
        }
    }
    if (slot->runtime_buf != NULL) {
        ot_avp_npu_free(slot->runtime_buf);
    }
    if (slot->handle != NULL) {
        ot_avp_npu_unload_model(slot->handle);
    }
}

/* 通用预处理：读裸数据文件（单输入传文件，多输入传目录 0,1,2...） */
static ot_s32 load_raw_input_file(model_slot *slot, const char *input_path)
{
    char **files = (char **)calloc(slot->input_num, sizeof(char *));
    ot_s32 ret = 0;
    ot_u32 i;

    if (files == NULL) {
        return -1;
    }
    for (i = 0; i < slot->input_num; i++) {
        files[i] = (char *)calloc(1, FILE_PATH_MAX);
        if (files[i] == NULL) {
            ret = -1;
            goto finish;
        }
    }

    /* 单输入传文件路径；多输入传目录，目录内文件命名为 0,1,2... */
    ret = get_input_file(input_path, files, slot->input_num);
    if (ret != 0) {
        printf("[%s] get input fail: %s\n", slot->name, input_path);
        goto finish;
    }

    for (i = 0; i < slot->input_num; i++) {
        ret = load_data_from_file(files[i], (ot_u8 *)slot->inputs[i].virt_addr,
                                  slot->inputs[i].shape, slot->inputs[i].stride.dims[0],
                                  slot->inputs[i].dtype);
        if (ret != 0) {
            printf("[%s] preprocess input[%u] fail: %s\n", slot->name, i, files[i]);
            goto finish;
        }
    }
finish:
    for (i = 0; i < slot->input_num; i++) {
        free(files[i]);
    }
    free(files);
    return ret;
}

/* mobilenetv2_rgbplanar：输入是模型要求的 RGB planar 裸数据（UINT8 NCHW）。
 * 按模型查询到的 shape/stride 逐行拷入输入 tensor；resize/crop/mean/std 已
 * 在离线模型转换时固化到 Preprocess 节点，无需在这里再做。 */
static ot_s32 mobilenetv2_rgbplanar_preprocess(model_slot *slot, const char *input_path)
{
    const ot_avp_tensor *input;
    ot_u64 expect;

    if (slot->input_num != 1) {
        printf("[%s] mobilenetv2_rgbplanar expects 1 input, got %u\n",
               slot->name, slot->input_num);
        return -1;
    }
    input = &slot->inputs[0];
    if (input->dtype != OT_AVP_DTYPE_UINT8 || input->shape.dim_size != 4 ||
        input->shape.dims[0] != 1 || input->shape.dims[1] != 3) {
        printf("[%s] mobilenetv2_rgbplanar expects UINT8 NCHW [1,3,H,W] input\n",
               slot->name);
        return -1;
    }
    expect = (ot_u64)3 * (ot_u64)input->shape.dims[2] * (ot_u64)input->shape.dims[3];
    if (input->len != expect) {
        printf("[%s] mobilenetv2_rgbplanar input size %llu != 3*H*W (%llu)\n",
               slot->name, (unsigned long long)input->len,
               (unsigned long long)expect);
        return -1;
    }

    if (load_data_from_file(input_path, (ot_u8 *)input->virt_addr, input->shape,
                            input->stride.dims[0], input->dtype) != 0) {
        printf("[%s] mobilenetv2_rgbplanar preprocess fail: %s\n",
               slot->name, input_path);
        return -1;
    }
    return 0;
}

/* tiny-yolov3_yuv420sp：模型转换时已声明 YUV420SP 输入（内部完成
 * YUV420SP->RGBPlanar 和 letterbox），因此把整块 YUV420SP 数据读入输入
 * tensor 即可，软件侧无需做 YUV->RGB/resize。 */
static ot_s32 tiny_yolov3_yuv420sp_preprocess(model_slot *slot, const char *input_path)
{
    const ot_avp_tensor *input;
    FILE *fp;
    size_t nread;

    if (slot->input_num != 1) {
        printf("[%s] tiny_yolov3_yuv420sp expects 1 input, got %u\n",
               slot->name, slot->input_num);
        return -1;
    }
    input = &slot->inputs[0];
    if (input->dtype != OT_AVP_DTYPE_UINT8) {
        printf("[%s] tiny_yolov3_yuv420sp expects UINT8 input\n", slot->name);
        return -1;
    }

    fp = fopen(input_path, "rb");
    if (fp == NULL) {
        printf("[%s] open input fail: %s\n", slot->name, input_path);
        return -1;
    }
    nread = fread((ot_u8 *)input->virt_addr, 1, (size_t)input->len, fp);
    fclose(fp);
    if (nread != (size_t)input->len) {
        printf("[%s] input %s has %zu bytes, model expects %llu\n",
               slot->name, input_path, nread, (unsigned long long)input->len);
        return -1;
    }
    return 0;
}

/* 按识别出的模型类型分发预处理；不匹配专用函数的模型退回通用裸数据加载 */
static ot_s32 model_preprocess(model_slot *slot, const char *input_path)
{
    const ot_avp_tensor *input;

    if (slot->kind == MODEL_KIND_DETECT_YOLOV3) {
        return tiny_yolov3_yuv420sp_preprocess(slot, input_path);
    }

    input = &slot->inputs[0];
    if (input->dtype == OT_AVP_DTYPE_UINT8 && input->shape.dim_size == 4 &&
        input->shape.dims[0] == 1 && input->shape.dims[1] == 3) {
        return mobilenetv2_rgbplanar_preprocess(slot, input_path);
    }
    return load_raw_input_file(slot, input_path);
}

static void print_topk(const char *name, const ot_avp_tensor *tensor, ot_u32 frame)
{
    const float *data = (const float *)tensor->virt_addr;
    ot_u32 idx[TOP_K] = {0};
    float val[TOP_K];
    ot_u32 n;
    ot_u32 i;
    ot_u32 j;
    ot_u32 k;

    /* 仅对分类模型常见的 F32 [1, N] 输出做 top-k */
    if (tensor->dtype != OT_AVP_DTYPE_F32 || tensor->shape.dim_size != 2 ||
        tensor->shape.dims[0] != 1) {
        return;
    }

    n = (ot_u32)tensor->shape.dims[1];
    for (k = 0; k < TOP_K; k++) {
        val[k] = -FLT_MAX;
    }
    for (i = 0; i < n; i++) {
        ot_u32 min_k = 0;
        for (k = 1; k < TOP_K; k++) {
            if (val[k] < val[min_k]) {
                min_k = k;
            }
        }
        if (data[i] > val[min_k]) {
            val[min_k] = data[i];
            idx[min_k] = i;
        }
    }
    for (i = 1; i < TOP_K; i++) {
        float tv = val[i];
        ot_u32 ti = idx[i];
        for (j = i; j > 0 && val[j - 1] < tv; j--) {
            val[j] = val[j - 1];
            idx[j] = idx[j - 1];
        }
        val[j] = tv;
        idx[j] = ti;
    }

    printf("[%s] frame[%u] top-%d:", name, frame, TOP_K);
    for (k = 0; k < TOP_K; k++) {
        printf(" %u(%.4f)", idx[k], val[k]);
    }
    printf("\n");
}

static ot_s32 classification_postprocess(model_slot *slot, const char *output_dir,
                                         ot_u32 frame, bool save)
{
    ot_u32 i;

    if (!save) {
        return 0;
    }

    for (i = 0; i < slot->output_num; i++) {
        char path[FILE_PATH_MAX] = {0};
        ot_s32 ret;

        snprintf(path, sizeof(path), "%s%c%s_frame%u_out%u.bin",
                 output_dir, PATH_SEPARATOR, slot->name, frame, i);
        ret = dump_data_to_file(path, (ot_u8 *)slot->outputs[i].virt_addr,
                                slot->outputs[i].shape, slot->outputs[i].stride.dims[0],
                                slot->outputs[i].dtype);
        if (ret != 0) {
            printf("[%s] dump output[%u] fail: %s\n", slot->name, i, path);
            return ret;
        }
        printf("[%s] output[%u] dumped: %s\n", slot->name, i, path);
    }

    if (slot->output_num > 0) {
        print_topk(slot->name, &slot->outputs[0], frame);
    }
    return 0;
}

typedef struct {
    float x1;
    float y1;
    float x2;
    float y2;
    float score;
    ot_u32 class_id;
    bool suppressed;
} yolo_box;

static float yolo_sigmoid(float x)
{
    return 1.0f / (1.0f + expf(-x));
}

/* 输出 tensor 按行 stride 对齐，逐行拷到紧凑 F32 缓冲后再解码 */
static ot_s32 yolo_copy_output(const ot_avp_tensor *tensor, float *dst)
{
    ot_u32 row_elems;
    ot_u32 rows;
    ot_u32 i;

    if (tensor->dtype != OT_AVP_DTYPE_F32 || tensor->shape.dim_size != 4) {
        return -1;
    }
    row_elems = (ot_u32)tensor->shape.dims[3];
    rows = (ot_u32)tensor->shape.dims[0] * (ot_u32)tensor->shape.dims[1] *
           (ot_u32)tensor->shape.dims[2];
    for (i = 0; i < rows; i++) {
        memcpy(dst + (ot_u64)i * row_elems,
               (const ot_u8 *)tensor->virt_addr + (ot_u64)i * tensor->stride.dims[0],
               (size_t)row_elems * sizeof(float));
    }
    return 0;
}

static ot_u32 yolo_decode_output(const float *out, ot_s32 grid_h, ot_s32 grid_w,
                                 const ot_s32 mask[YOLO_ANCHOR_NUM],
                                 yolo_box *boxes, ot_u32 box_num)
{
    static const float anchors[6][2] = {
        {10.0f, 14.0f}, {23.0f, 27.0f}, {37.0f, 58.0f},
        {81.0f, 82.0f}, {135.0f, 169.0f}, {344.0f, 319.0f},
    };
    const ot_s32 feat_len = YOLO_ANCHOR_NUM * (4 + 1 + YOLO_CLASS_NUM);
    ot_u32 count = box_num;
    ot_s32 cell;

    for (cell = 0; cell < grid_h * grid_w; cell++) {
        const float *cell_data = out + (ot_u64)cell * feat_len;
        ot_s32 row = cell / grid_w;
        ot_s32 col = cell % grid_w;
        ot_s32 a;

        for (a = 0; a < YOLO_ANCHOR_NUM; a++) {
            const float *p = cell_data + a * (4 + 1 + YOLO_CLASS_NUM);
            float conf = yolo_sigmoid(p[4]);
            float best = 0.0f;
            float cx;
            float cy;
            ot_u32 best_id = 0;
            ot_s32 c;

            if (conf < YOLO_OBJ_THRESH) {
                continue;
            }
            for (c = 0; c < YOLO_CLASS_NUM; c++) {
                float cls = yolo_sigmoid(p[5 + c]);

                if (cls > best) {
                    best = cls;
                    best_id = (ot_u32)c;
                }
            }
            cx = (yolo_sigmoid(p[0]) + (float)col) * YOLO_INPUT_DIM / (float)grid_w;
            cy = (yolo_sigmoid(p[1]) + (float)row) * YOLO_INPUT_DIM / (float)grid_h;
            if (count >= YOLO_MAX_BOX) {
                return count;
            }
            boxes[count].x1 = cx - expf(p[2]) * anchors[mask[a]][0] * 0.5f;
            boxes[count].y1 = cy - expf(p[3]) * anchors[mask[a]][1] * 0.5f;
            boxes[count].x2 = cx + expf(p[2]) * anchors[mask[a]][0] * 0.5f;
            boxes[count].y2 = cy + expf(p[3]) * anchors[mask[a]][1] * 0.5f;
            boxes[count].score = conf * best;
            boxes[count].class_id = best_id;
            count++;
        }
    }
    return count;
}

static int yolo_score_desc(const void *lhs, const void *rhs)
{
    const yolo_box *a = (const yolo_box *)lhs;
    const yolo_box *b = (const yolo_box *)rhs;

    if (a->score > b->score) {
        return -1;
    }
    if (a->score < b->score) {
        return 1;
    }
    return 0;
}

static float yolo_iou(const yolo_box *a, const yolo_box *b)
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

/* tiny-yolov3：两路 NHWC [1,H,W,255] 输出做置信度阈值过滤 + NMS */
static ot_s32 tiny_yolov3_yuv420sp_postprocess(model_slot *slot, const char *output_dir,
                                              ot_u32 frame, bool save)
{
    static const ot_s32 masks[YOLO_OUTPUT_NUM][YOLO_ANCHOR_NUM] = {
        {0, 1, 2}, {3, 4, 5},
    };
    yolo_box *boxes;
    ot_u32 box_num = 0;
    ot_u32 kept = 0;
    ot_s32 o;

    if (!save) {
        return 0;
    }
    if (slot->output_num != YOLO_OUTPUT_NUM) {
        printf("[%s] tiny-yolov3 expects 2 outputs, got %u\n",
               slot->name, slot->output_num);
        return -1;
    }

    boxes = (yolo_box *)calloc(YOLO_MAX_BOX, sizeof(yolo_box));
    if (boxes == NULL) {
        printf("[%s] alloc detection box buffer fail\n", slot->name);
        return -1;
    }

    for (o = 0; o < slot->output_num; o++) {
        const ot_avp_tensor *tensor = &slot->outputs[o];
        ot_u64 elems = 1;
        float *compact;
        ot_s32 d;

        for (d = 0; d < tensor->shape.dim_size; d++) {
            elems *= (ot_u64)tensor->shape.dims[d];
        }
        compact = (float *)malloc((size_t)elems * sizeof(float));
        if (compact == NULL) {
            printf("[%s] alloc compact output[%d] fail\n", slot->name, o);
            free(boxes);
            return -1;
        }
        if (yolo_copy_output(tensor, compact) != 0) {
            printf("[%s] output[%d] is not F32 [1,H,W,255]\n", slot->name, o);
            free(compact);
            free(boxes);
            return -1;
        }
        box_num = yolo_decode_output(compact, tensor->shape.dims[1],
                                     tensor->shape.dims[2], masks[o], boxes, box_num);
        free(compact);

        if (save) {
            char path[FILE_PATH_MAX] = {0};

            snprintf(path, sizeof(path), "%s%c%s_frame%u_out%d.bin",
                     output_dir, PATH_SEPARATOR, slot->name, frame, o);
            if (dump_data_to_file(path, (ot_u8 *)tensor->virt_addr, tensor->shape,
                                  tensor->stride.dims[0], tensor->dtype) != 0) {
                printf("[%s] dump output[%d] fail: %s\n", slot->name, o, path);
                free(boxes);
                return -1;
            }
        }
    }

    qsort(boxes, box_num, sizeof(yolo_box), yolo_score_desc);
    for (o = 0; o < (ot_s32)box_num; o++) {
        ot_u32 j;

        if (boxes[o].suppressed) {
            continue;
        }
        for (j = o + 1; j < box_num; j++) {
            if (!boxes[j].suppressed && yolo_iou(&boxes[o], &boxes[j]) > YOLO_NMS_THRESH) {
                boxes[j].suppressed = true;
            }
        }
    }

    printf("[%s] frame[%u] detections after threshold+NMS:", slot->name, frame);
    for (o = 0; o < (ot_s32)box_num; o++) {
        if (boxes[o].suppressed) {
            continue;
        }
        printf("\n  class=%u score=%.3f box=(%.1f,%.1f)-(%.1f,%.1f)",
               boxes[o].class_id, boxes[o].score, boxes[o].x1, boxes[o].y1,
               boxes[o].x2, boxes[o].y2);
        kept++;
    }
    if (kept == 0) {
        printf(" none");
    }
    printf("\n");
    free(boxes);
    return 0;
}

static ot_s32 model_postprocess(model_slot *slot, const char *output_dir, ot_u32 frame,
                                bool save)
{
    if (slot->kind == MODEL_KIND_DETECT_YOLOV3) {
        return tiny_yolov3_yuv420sp_postprocess(slot, output_dir, frame, save);
    }
    return classification_postprocess(slot, output_dir, frame, save);
}

int main(int argc, char **argv)
{
    const char *model_a = (argc > 1) ? argv[1] : DEFAULT_MODEL_A;
    const char *input_a = (argc > 2) ? argv[2] : DEFAULT_INPUT_A;
    const char *model_b = (argc > 3) ? argv[3] : DEFAULT_MODEL_B;
    const char *input_b = (argc > 4) ? argv[4] : DEFAULT_INPUT_B;
    ot_u32 repeat = (argc > 5) ? (ot_u32)atoi(argv[5]) : 1;
    const char *output_dir = (argc > 6) ? argv[6] : NULL;
    bool save_output = (output_dir != NULL);
    model_slot models[MODEL_NUM] = {0};
    ot_avp_npu_config config = {0};
    ot_u32 frame;
    ot_s32 ret;

    if (repeat == 0) {
        repeat = 1;
    }

#ifndef SIMULATOR
#ifndef OT_AVP_NPU_V200
    ot_eis_media_init();
#endif
    ot_smr_init();
#endif

    config.core_ids = 0;
    config.dump_layer = false;
    config.perf_mode = false;
    config.log_level = 2;
    config.thread_num = 2; /* >0 才支持 trigger/wait 异步推理 */

    ret = ot_avp_npu_init(&config);
    if (ret != 0) {
        printf("npu init fail\n");
        usage(argv[0]);
        goto smr_out;
    }

    ret = model_init(&models[0], "A", model_a);
    if (ret != 0) {
        goto cleanup_models;
    }
    ret = model_init(&models[1], "B", model_b);
    if (ret != 0) {
        goto cleanup_models;
    }

    if (save_output) {
        mkdir_dir((char *)output_dir);
    }

    for (frame = 0; frame < repeat; frame++) {
        printf("\n===== frame %u =====\n", frame);

        /* 1. 预处理两个模型，输入来自 input 目录 */
        ret = model_preprocess(&models[0], input_a);
        if (ret != 0) {
            break;
        }
        ret = model_preprocess(&models[1], input_b);
        if (ret != 0) {
            break;
        }

        /* 2. ABAB 调度：trigger A -> trigger B */
        ret = ot_avp_npu_trigger(models[0].handle,
                                 models[0].input_dataset, models[0].output_dataset);
        if (ret != 0) {
            printf("[A] trigger fail\n");
            break;
        }
        ret = ot_avp_npu_trigger(models[1].handle,
                                 models[1].input_dataset, models[1].output_dataset);
        if (ret != 0) {
            printf("[B] trigger fail\n");
            break;
        }

        /* 3. wait A -> 后处理 A */
        ret = ot_avp_npu_wait(models[0].handle, -1);
        if (ret != 0) {
            printf("[A] wait fail: %d\n", ret);
            break;
        }
        ret = model_postprocess(&models[0], output_dir, frame, save_output);
        if (ret != 0) {
            break;
        }

        /* 4. wait B -> 后处理 B */
        ret = ot_avp_npu_wait(models[1].handle, -1);
        if (ret != 0) {
            printf("[B] wait fail: %d\n", ret);
            break;
        }
        ret = model_postprocess(&models[1], output_dir, frame, save_output);
        if (ret != 0) {
            break;
        }
    }

    printf("benchmark finished, frames run: %u\n", frame);

cleanup_models:
    model_destroy(&models[1]);
    model_destroy(&models[0]);

    ot_avp_npu_deinit();
smr_out:
#ifndef SIMULATOR
    ot_smr_deinit();
#ifndef OT_AVP_NPU_V200
    ot_eis_media_deinit();
#endif
#endif
    return ret;
}
