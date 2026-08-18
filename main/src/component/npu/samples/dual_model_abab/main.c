/*
 * dual_model_abab: two board offline models run in an ABAB async pipeline.
 *
 * usage:
 *   sample_dual_model_abab [modelA] [inputA] [modelB] [inputB] [repeat] [output_dir]
 */
#include "ot_avp_npu_rts.h"
#include "camera.h"
#include "file_utils.h"
#include "transfer.h"
#include <float.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

/* SSH stream payload kinds shared with the host-side receiver. */
#define STREAM_KIND_CLASSIFY (1u)
#define STREAM_KIND_DETECT   (2u)
#define STREAM_SYNC_SIZE     (48u)
#define STREAM_RESULT_HEAD   (16u)  /* kind + pad + count u32 + duration_us u64 */
#define STREAM_TENSOR_HEAD   (40u)
#define STREAM_KIND_IMAGE_NV12 (1u)
#define STREAM_IMAGE_HEAD    (12u)  /* kind + pad3 + width u32 + height u32 */

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
    ot_u32 model_id;
} model_slot;

static void usage(const char *prog)
{
    printf("Usage: %s [modelA] [inputA] [modelB] [inputB] [repeat] [output_dir] [stream] [camera_fps]\n", prog);
    printf("inputB may be \"camera\" to feed model B from the sc132gs detection channel.\n");
    printf("camera_fps is the detection channel FRC target (1..30, default 10).\n");
    printf("--dump-frame writes one camera frame to /tmp/camera_frame.yuv420sp (debug).\n");
    printf("output_dir is optional; omit it to run in perf mode without saving results.\n");
    printf("stream: 0=off, 1=results, 2=results+tensors, 3=results+camera image.\n");
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

static ot_s32 model_init(model_slot *slot, const char *name, ot_u32 model_id,
                         const char *model_path)
{
    ot_avp_npu_mdl_config mdl_cfg = {
        .priority_level = OT_AVP_MDL_PRI_MEDIUM,
        .priority_preemp = true,
    };
    ot_u32 runtime_size = 0;
    ot_s32 ret;

    snprintf(slot->name, sizeof(slot->name), "%s", name);
    slot->model_id = model_id;
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

/* 对分类模型常见的 F32 [1,N] 输出计算 top-k；形状不匹配时返回 false。 */
static bool compute_topk(const ot_avp_tensor *tensor, ot_u32 idx[TOP_K], float val[TOP_K])
{
    /* NPU 输出内存按字节读很慢：先整块拷进普通内存再扫描，比逐元素读快约 4 倍。 */
    float *copy;
    const float *data;
    ot_u32 n;
    ot_u32 i;
    ot_u32 j;
    ot_u32 k;

    if (tensor->dtype != OT_AVP_DTYPE_F32 || tensor->shape.dim_size != 2 ||
        tensor->shape.dims[0] != 1) {
        return false;
    }

    n = (ot_u32)tensor->shape.dims[1];
    copy = (float *)malloc((size_t)n * sizeof(float));
    if (copy == NULL) {
        return false;
    }
    memcpy(copy, (const void *)tensor->virt_addr, (size_t)n * sizeof(float));
    data = copy;
    for (k = 0; k < TOP_K; k++) {
        val[k] = -FLT_MAX;
        idx[k] = 0;
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
    free(copy);
    return true;
}

static void print_topk(const char *name, const ot_avp_tensor *tensor, ot_u32 frame)
{
    ot_u32 idx[TOP_K];
    float val[TOP_K];
    ot_u32 k;

    if (!compute_topk(tensor, idx, val)) {
        return;
    }

    printf("[%s] frame[%u] top-%d:", name, frame, TOP_K);
    for (k = 0; k < TOP_K; k++) {
        printf(" %u(%.4f)", idx[k], val[k]);
    }
    printf("\n");
}

/* 流起始同步帧：协议版本、tensor 开关、总帧数与两模型名/类型，供主机端校验。 */
static ot_s32 stream_send_sync(transfer_ctx *tx, const model_slot *a, const model_slot *b,
                               ot_u32 total_frames, bool stream_tensors)
{
    uint8_t payload[STREAM_SYNC_SIZE] = {0};

    transfer_put_u8(payload + 0, TRANSFER_VERSION);
    transfer_put_u8(payload + 1, stream_tensors ? 1u : 0u);
    transfer_put_u16(payload + 2, 0);
    transfer_put_u32(payload + 4, total_frames);
    memcpy(payload + 8, a->name, NAME_MAX);
    transfer_put_u8(payload + 24, (uint8_t)a->kind);
    memcpy(payload + 28, b->name, NAME_MAX);
    transfer_put_u8(payload + 44, (uint8_t)b->kind);
    return transfer_send(tx, TRANSFER_TYPE_SYNC, 0, 0, payload, sizeof(payload), false);
}

/* 分类结果载荷：kind=1 + count + duration_us + (idx u32, score f32) x count。
 * duration_us 为从本帧循环开始到这条结果发出的耗时。 */
static ot_s32 stream_send_classify_result(transfer_ctx *tx, const model_slot *slot,
                                          ot_u32 seq, ot_u64 frame_start_us,
                                          const ot_u32 idx[TOP_K], const float val[TOP_K])
{
    uint8_t payload[STREAM_RESULT_HEAD + TOP_K * 8];
    size_t off = STREAM_RESULT_HEAD;
    ot_u32 k;

    transfer_put_u8(payload, STREAM_KIND_CLASSIFY);
    transfer_put_u32(payload + 4, TOP_K);
    transfer_put_u64(payload + 8, transfer_now_us() - frame_start_us);
    for (k = 0; k < TOP_K; k++) {
        transfer_put_u32(payload + off, idx[k]);
        transfer_put_f32(payload + off + 4, val[k]);
        off += 8;
    }
    return transfer_send(tx, TRANSFER_TYPE_RESULT, (uint8_t)slot->model_id, seq,
                         payload, off, false);
}

static ot_s32 classification_postprocess(model_slot *slot, const char *output_dir,
                                         ot_u32 frame, bool save, transfer_ctx *tx,
                                         ot_u32 seq, ot_u64 frame_start_us)
{
    ot_u32 i;

    if (tx != NULL && slot->output_num > 0) {
        ot_u32 idx[TOP_K];
        float val[TOP_K];

        if (compute_topk(&slot->outputs[0], idx, val)) {
            return stream_send_classify_result(tx, slot, seq, frame_start_us, idx, val);
        }
        return 0;
    }
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

/* 检测结果载荷：kind=2 + count + duration_us + (x1,y1,x2,y2,score f32 +
 * class_id u32) x count + src_w u32 + src_h u32。 */
static ot_s32 stream_send_detect_result(transfer_ctx *tx, const model_slot *slot,
                                        ot_u32 seq, ot_u64 frame_start_us,
                                        const yolo_box *boxes, ot_u32 box_num,
                                        ot_u32 src_w, ot_u32 src_h)
{
    uint8_t *payload;
    size_t off = STREAM_RESULT_HEAD;
    ot_u32 kept = 0;
    ot_u32 o;
    ot_s32 ret;

    for (o = 0; o < box_num; o++) {
        if (!boxes[o].suppressed) {
            kept++;
        }
    }
    payload = (uint8_t *)malloc(STREAM_RESULT_HEAD + (size_t)kept * 24u + 8u);
    if (payload == NULL) {
        return -1;
    }
    transfer_put_u8(payload, STREAM_KIND_DETECT);
    transfer_put_u32(payload + 4, kept);
    transfer_put_u64(payload + 8, transfer_now_us() - frame_start_us);
    for (o = 0; o < box_num; o++) {
        const yolo_box *b = &boxes[o];

        if (b->suppressed) {
            continue;
        }
        transfer_put_f32(payload + off, b->x1);
        transfer_put_f32(payload + off + 4, b->y1);
        transfer_put_f32(payload + off + 8, b->x2);
        transfer_put_f32(payload + off + 12, b->y2);
        transfer_put_f32(payload + off + 16, b->score);
        transfer_put_u32(payload + off + 20, b->class_id);
        off += 24;
    }
    transfer_put_u32(payload + off, src_w);
    transfer_put_u32(payload + off + 4, src_h);
    off += 8;
    ret = transfer_send(tx, TRANSFER_TYPE_RESULT, (uint8_t)slot->model_id, seq,
                        payload, off, false);
    free(payload);
    return ret;
}

static ot_u32 stream_dtype_bytes(ot_avp_data_type dtype)
{
    switch (dtype) {
    case OT_AVP_DTYPE_UINT8:
    case OT_AVP_DTYPE_INT8:
    case OT_AVP_DTYPE_BOOL:
        return 1;
    case OT_AVP_DTYPE_F16:
    case OT_AVP_DTYPE_UINT16:
    case OT_AVP_DTYPE_INT16:
        return 2;
    case OT_AVP_DTYPE_F32:
    case OT_AVP_DTYPE_UINT32:
    case OT_AVP_DTYPE_INT32:
        return 4;
    case OT_AVP_DTYPE_F64:
    case OT_AVP_DTYPE_UINT64:
    case OT_AVP_DTYPE_INT64:
        return 8;
    default:
        return 0;
    }
}

/* tensor 载荷：index/dtype/dim_size/dims[8]/stride 元信息 + 按行紧凑后的原始字节。
 * 只在 stream=2 时使用：整块拷贝 NPU 输出会明显增加 CPU 开销，性能模式不启用。 */
static ot_s32 stream_send_tensors(transfer_ctx *tx, const model_slot *slot, ot_u32 seq)
{
    ot_u32 o;

    for (o = 0; o < slot->output_num; o++) {
        const ot_avp_tensor *t = &slot->outputs[o];
        ot_u32 elem = stream_dtype_bytes(t->dtype);
        ot_u64 rows = 1;
        ot_u32 line_size;
        size_t compact;
        uint8_t *payload;
        uint8_t *raw;
        ot_u32 d;
        ot_u32 i;
        ot_s32 ret;

        if (elem == 0) {
            return -1;
        }
        for (d = 0; d + 1 < t->shape.dim_size; d++) {
            rows *= (ot_u64)(ot_u32)t->shape.dims[d];
        }
        line_size = (ot_u32)t->shape.dims[t->shape.dim_size - 1] * elem;
        /* NPU 输出缓冲按行 stride 对齐，tensor.len 含行间 padding；只发送
         * 按行紧凑后的有效字节，与落盘模式 dump_data_to_file 的语义一致。 */
        compact = (size_t)rows * (size_t)line_size;
        payload = (uint8_t *)malloc(STREAM_TENSOR_HEAD + compact);
        if (payload == NULL) {
            return -1;
        }
        raw = payload + STREAM_TENSOR_HEAD;
        for (i = 0; i < (ot_u32)rows; i++) {
            memcpy(raw + (size_t)i * line_size,
                   (const uint8_t *)t->virt_addr + (ot_u64)i * t->stride.dims[0],
                   line_size);
        }
        payload[0] = (uint8_t)o;
        payload[1] = (uint8_t)t->dtype;
        payload[2] = t->shape.dim_size;
        payload[3] = 0;
        for (d = 0; d < 8; d++) {
            uint32_t dim = (d < t->shape.dim_size) ? (uint32_t)t->shape.dims[d] : 0;

            transfer_put_u32(payload + 4 + d * 4, dim);
        }
        transfer_put_u32(payload + 36, (uint32_t)t->stride.dims[0]);
        ret = transfer_send(tx, TRANSFER_TYPE_TENSOR, (uint8_t)slot->model_id, seq,
                            payload, STREAM_TENSOR_HEAD + compact, true);
        free(payload);
        if (ret != 0) {
            return ret;
        }
    }
    return 0;
}

/* IMAGE 帧（相机模式 stream>=3）：payload = kind(1=NV12) + pad3 + w u32 + h u32
 * + w*h*3/2 紧凑 NV12。内容为模型 B 实际看到的 416x312 有效区，从 416x416
 * letterbox 输入张量直接截取（Y 行 52..363、UV 行 26..181），零额外相机拷贝。
 * 图像不做 zlib：板端压缩 ~194KB 需数十 ms，得不偿失。 */
static ot_s32 stream_send_image(transfer_ctx *tx, const model_slot *slot, ot_u32 seq)
{
    static uint8_t payload[STREAM_IMAGE_HEAD + CAMERA_NPU_IN_LEN];
    const uint8_t *src = (const uint8_t *)slot->inputs[0].virt_addr;
    uint32_t top_pad = (CAMERA_NPU_IN_H - CAMERA_DET_OUT_H) / 2;
    uint32_t y_size = (uint32_t)CAMERA_NPU_IN_W * CAMERA_NPU_IN_H;
    uint32_t y_off = top_pad * CAMERA_NPU_IN_W;
    uint32_t uv_off = y_size + (top_pad / 2) * CAMERA_NPU_IN_W;
    uint32_t y_len = (uint32_t)CAMERA_DET_OUT_H * CAMERA_NPU_IN_W;
    uint32_t uv_len = (uint32_t)(CAMERA_DET_OUT_H / 2) * CAMERA_NPU_IN_W;

    if (slot->inputs[0].len < CAMERA_NPU_IN_LEN) {
        return -1;
    }
    transfer_put_u8(payload, STREAM_KIND_IMAGE_NV12);
    transfer_put_u32(payload + 4, CAMERA_DET_OUT_W);
    transfer_put_u32(payload + 8, CAMERA_DET_OUT_H);
    memcpy(payload + STREAM_IMAGE_HEAD, src + y_off, y_len);
    memcpy(payload + STREAM_IMAGE_HEAD + y_len, src + uv_off, uv_len);
    return transfer_send(tx, TRANSFER_TYPE_IMAGE, (uint8_t)slot->model_id, seq,
                         payload, STREAM_IMAGE_HEAD + y_len + uv_len, false);
}

/* 非阻塞轮询 stdin：收到 STOP 控制帧时回 ACK 并返回 true。 */
static bool stream_poll_stop(transfer_ctx *tx)
{
    uint8_t type;
    uint8_t model;
    uint32_t seq;
    void *payload = NULL;
    size_t len = 0;
    bool stop = false;
    int rc = transfer_recv(tx, 0, &type, &model, &seq, &payload, &len);

    if (rc == TRANSFER_RECV_OK) {
        if (type == TRANSFER_TYPE_CONTROL && len >= 1 &&
            transfer_get_u8((const uint8_t *)payload) == TRANSFER_CTRL_STOP) {
            uint8_t ack[1] = {TRANSFER_CTRL_STOP};

            transfer_send(tx, TRANSFER_TYPE_ACK, 0, seq, ack, sizeof(ack), false);
            stop = true;
        }
        free(payload);
    }
    return stop;
}

static float yolo_sigmoid(float x)
{
    return 1.0f / (1.0f + expf(-x));
}

/* 直接在 stride 对齐的 NHWC [1,H,W,255] 输出上解码，避免先把整块输出
 * 拷到紧凑缓冲（NPU 输出内存的读带宽是主要瓶颈）。每个 anchor 先只读
 * objectness，过阈值才读该 anchor 的 85 个 float。 */
static ot_u32 yolo_decode_output(const ot_avp_tensor *tensor, ot_s32 grid_h, ot_s32 grid_w,
                                 const ot_s32 mask[YOLO_ANCHOR_NUM],
                                 yolo_box *boxes, ot_u32 box_num)
{
    static const float anchors[6][2] = {
        {10.0f, 14.0f}, {23.0f, 27.0f}, {37.0f, 58.0f},
        {81.0f, 82.0f}, {135.0f, 169.0f}, {344.0f, 319.0f},
    };
    const ot_u8 *base = (const ot_u8 *)tensor->virt_addr;
    const ot_u64 cell_stride = tensor->stride.dims[0];
    ot_u32 count = box_num;
    ot_s32 cell;

    for (cell = 0; cell < grid_h * grid_w; cell++) {
        const float *cell_data =
            (const float *)(base + (ot_u64)cell * cell_stride);
        ot_s32 row = cell / grid_w;
        ot_s32 col = cell % grid_w;
        ot_s32 a;

        for (a = 0; a < YOLO_ANCHOR_NUM; a++) {
            const float *p = cell_data + a * (4 + 1 + YOLO_CLASS_NUM);
            float conf = yolo_sigmoid(p[4]);
            float best = 0.0f;
            float cx;
            float cy;
            float bw;
            float bh;
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
            bw = expf(p[2]) * anchors[mask[a]][0];
            bh = expf(p[3]) * anchors[mask[a]][1];
            if (count >= YOLO_MAX_BOX) {
                return count;
            }
            boxes[count].x1 = cx - bw * 0.5f;
            boxes[count].y1 = cy - bh * 0.5f;
            boxes[count].x2 = cx + bw * 0.5f;
            boxes[count].y2 = cy + bh * 0.5f;
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

/* 把 416 模型空间的检测框映射回源分辨率（参考工具链样例 rescale） */
static void yolo_rescale(yolo_box *b, ot_u32 src_w, ot_u32 src_h)
{
    float max_side;
    float pad_x;
    float pad_y;
    float unpad_w;
    float unpad_h;

    if (src_w == 0 || src_h == 0) {
        return;
    }
    max_side = (src_w > src_h) ? (float)src_w : (float)src_h;
    pad_x = ((src_h > src_w) ? (float)(src_h - src_w) : 0.0f) *
            ((float)YOLO_INPUT_DIM / max_side);
    pad_y = ((src_w > src_h) ? (float)(src_w - src_h) : 0.0f) *
            ((float)YOLO_INPUT_DIM / max_side);
    unpad_w = (float)YOLO_INPUT_DIM - pad_x;
    unpad_h = (float)YOLO_INPUT_DIM - pad_y;
    b->x1 = (b->x1 - pad_x * 0.5f) / unpad_w * (float)src_w;
    b->y1 = (b->y1 - pad_y * 0.5f) / unpad_h * (float)src_h;
    b->x2 = (b->x2 - pad_x * 0.5f) / unpad_w * (float)src_w;
    b->y2 = (b->y2 - pad_y * 0.5f) / unpad_h * (float)src_h;
}

/* tiny-yolov3：两路 NHWC [1,H,W,255] 输出做置信度阈值过滤 + NMS */
static ot_s32 tiny_yolov3_yuv420sp_postprocess(model_slot *slot, const char *output_dir,
                                              ot_u32 frame, bool save, transfer_ctx *tx,
                                              ot_u32 seq, ot_u64 frame_start_us,
                                              ot_u32 src_w, ot_u32 src_h)
{
    static const ot_s32 masks[YOLO_OUTPUT_NUM][YOLO_ANCHOR_NUM] = {
        {0, 1, 2}, {3, 4, 5},
    };
    yolo_box *boxes;
    ot_u32 box_num = 0;
    ot_u32 kept = 0;
    ot_s32 o;

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
        box_num = yolo_decode_output(tensor, tensor->shape.dims[1],
                                     tensor->shape.dims[2], masks[o], boxes, box_num);

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
    for (o = 0; o < (ot_s32)box_num; o++) {
        if (!boxes[o].suppressed) {
            yolo_rescale(&boxes[o], src_w, src_h);
        }
    }

    /* 性能模式也执行解码+NMS，只是不落盘、不打印检测结果；流模式把
     * NMS 后的保留框序列化发送到主机。 */
    if (!save) {
        ot_s32 sret = 0;

        if (tx != NULL) {
            sret = stream_send_detect_result(tx, slot, seq, frame_start_us, boxes,
                                             box_num, src_w, src_h);
        }
        free(boxes);
        return sret;
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
                                bool save, transfer_ctx *tx, ot_u32 seq,
                                ot_u64 frame_start_us, ot_u32 src_w, ot_u32 src_h)
{
    if (slot->kind == MODEL_KIND_DETECT_YOLOV3) {
        return tiny_yolov3_yuv420sp_postprocess(slot, output_dir, frame, save, tx, seq,
                                               frame_start_us, src_w, src_h);
    }
    return classification_postprocess(slot, output_dir, frame, save, tx, seq,
                                      frame_start_us);
}

int main(int argc, char **argv)
{
    const char *model_a = (argc > 1) ? argv[1] : DEFAULT_MODEL_A;
    const char *input_a = (argc > 2) ? argv[2] : DEFAULT_INPUT_A;
    const char *model_b = (argc > 3) ? argv[3] : DEFAULT_MODEL_B;
    const char *input_b = (argc > 4) ? argv[4] : DEFAULT_INPUT_B;
    ot_u32 repeat = (argc > 5) ? (ot_u32)atoi(argv[5]) : 1;
    const char *output_dir = (argc > 6) ? argv[6] : NULL;
    ot_u32 stream_level = (argc > 7) ? (ot_u32)atoi(argv[7]) : 0;
    ot_u32 camera_fps = (argc > 8) ? (ot_u32)atoi(argv[8]) : CAMERA_FPS_DEFAULT;
    bool stream_mode = (stream_level > 0);
    bool stream_tensors = (stream_level == 2);
    bool stream_image = (stream_level >= 3);
    bool save_output = (output_dir != NULL) && !stream_mode;
    bool camera_mode = (strcmp(input_b, "camera") == 0);
    bool dump_frame = false;
    model_slot models[MODEL_NUM] = {0};
    ot_avp_npu_config config = {0};
    transfer_ctx tx;
    int stream_fd = -1;
    ot_u32 frame;
    ot_s32 ret;

    /* SSH 通道被主机关闭时写 stdout 会收到 EPIPE；忽略 SIGPIPE，让发送函数
     * 返回错误走正常清理路径（camera_stop/model_destroy），避免进程被信号
     * 杀掉后遗留失效媒体句柄，需要 load_lq560v100 -a 才能恢复。 */
    signal(SIGPIPE, SIG_IGN);

    for (ot_s32 a = 1; a < argc; a++) {
        if (strcmp(argv[a], "--dump-frame") == 0) {
            dump_frame = true;
        }
    }

    if (repeat == 0) {
        repeat = 1;
    }

#ifndef SIMULATOR
#ifndef OT_AVP_NPU_V200
    if (camera_mode) {
        if (camera_init() != 0) {
            printf("camera init fail\n");
            ret = -1;
            goto smr_out;
        }
    } else {
        ot_eis_media_init();
    }
#endif
    if (!camera_mode) {
        ot_smr_init();
    }
#endif

    config.core_ids = 0;
    config.dump_layer = false;
    config.perf_mode = false;
    config.log_level = 2;
    config.thread_num = 2; /* >0 才支持 trigger/wait 异步推理 */

    if (stream_mode) {
        /* 流模式下 stdout 只承载协议帧，printf 日志改走 stderr。提前到
         * npu 初始化之前，避免 NPU 库的启动日志混进帧流。 */
        stream_fd = dup(STDOUT_FILENO);
        if (stream_fd < 0) {
            fprintf(stderr, "stream: duplicate stdout fail\n");
            ret = -1;
            goto smr_out;
        }
        dup2(STDERR_FILENO, STDOUT_FILENO);
        transfer_init(&tx, STDIN_FILENO, stream_fd);
    }

    ret = ot_avp_npu_init(&config);
    if (ret != 0) {
        printf("npu init fail\n");
        usage(argv[0]);
        goto smr_out;
    }

    ret = model_init(&models[0], "A", 0, model_a);
    if (ret != 0) {
        goto cleanup_models;
    }
    ret = model_init(&models[1], "B", 1, model_b);
    if (ret != 0) {
        goto cleanup_models;
    }

    if (camera_mode) {
        if (models[1].kind != MODEL_KIND_DETECT_YOLOV3) {
            printf("[B] camera input requires a tiny-yolov3 detection model\n");
            goto cleanup_models;
        }
        ret = camera_start(camera_fps);
        if (ret != 0) {
            printf("camera start fail\n");
            goto cleanup_models;
        }
    }

    if (stream_mode) {
        ret = stream_send_sync(&tx, &models[0], &models[1], repeat, stream_tensors);
        if (ret != 0) {
            printf("[stream] send sync fail\n");
            goto cleanup_models;
        }
    }

    if (save_output) {
        mkdir_dir((char *)output_dir);
    }

    for (frame = 0; frame < repeat; frame++) {
        ot_u64 frame_start_us = transfer_now_us();

        if (!stream_mode) {
            printf("\n===== frame %u =====\n", frame);
        }

        /* 1. 预处理两个模型；B 在相机模式下从检测通道拷贝最新帧 */
        ret = model_preprocess(&models[0], input_a);
        if (ret != 0) {
            break;
        }
        if (camera_mode) {
            ret = camera_copy_latest_to_input(
                (uint8_t *)models[1].inputs[0].virt_addr,
                (uint32_t)models[1].inputs[0].len);
            if (ret == 0 && stream_image) {
                ret = stream_send_image(&tx, &models[1], frame);
            }
            if (ret == 0 && dump_frame) {
                FILE *fp = fopen("/tmp/camera_frame.yuv420sp", "wb");

                if (fp != NULL) {
                    if (fwrite((const void *)models[1].inputs[0].virt_addr, 1,
                               models[1].inputs[0].len, fp) !=
                        models[1].inputs[0].len) {
                        printf("[camera] dump frame write fail\n");
                    }
                    fclose(fp);
                    printf("[camera] dumped frame to /tmp/camera_frame.yuv420sp\n");
                }
                dump_frame = false;
            }
        } else {
            ret = model_preprocess(&models[1], input_b);
        }
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
        ret = model_postprocess(&models[0], output_dir, frame, save_output,
                                stream_mode ? &tx : NULL, frame, frame_start_us, 0, 0);
        if (ret != 0) {
            break;
        }

        /* 4. wait B -> 后处理 B */
        ret = ot_avp_npu_wait(models[1].handle, -1);
        if (ret != 0) {
            printf("[B] wait fail: %d\n", ret);
            break;
        }
        ret = model_postprocess(&models[1], output_dir, frame, save_output,
                                stream_mode ? &tx : NULL, frame, frame_start_us,
                                camera_mode ? CAMERA_SRC_W : 0,
                                camera_mode ? CAMERA_SRC_H : 0);
        if (ret != 0) {
            break;
        }

        /* 5. 流模式：按需发送输出 tensor（压缩后），并轮询主机控制帧 */
        if (stream_mode) {
            if (stream_tensors) {
                ret = stream_send_tensors(&tx, &models[0], frame);
                if (ret == 0) {
                    ret = stream_send_tensors(&tx, &models[1], frame);
                }
                if (ret != 0) {
                    break;
                }
            }
            /* 控制帧轮询每次约 20us，每 8 帧轮询一次，STOP 延迟最多约 50ms */
            if ((frame & 7u) == 0 && stream_poll_stop(&tx)) {
                printf("[stream] host requested stop\n");
                break;
            }
        }
    }

    printf("benchmark finished, frames run: %u\n", frame);

    if (camera_mode) {
        camera_stats stats;

        if (camera_get_stats(&stats) == 0) {
            printf("[camera] stats: det=%u lr_pairs=%u dpts min/avg/max=%llu/%llu/%llu us\n",
                   stats.det_frames, stats.lr_pairs,
                   (unsigned long long)stats.dpts_min_us,
                   (unsigned long long)stats.dpts_avg_us,
                   (unsigned long long)stats.dpts_max_us);
        }
    }

cleanup_models:
    if (camera_mode) {
        camera_stop();
    }
    model_destroy(&models[1]);
    model_destroy(&models[0]);

    if (stream_fd >= 0) {
        close(stream_fd);
    }
    ot_avp_npu_deinit();
smr_out:
#ifndef SIMULATOR
    if (camera_mode) {
        camera_deinit();
    } else {
        ot_smr_deinit();
#ifndef OT_AVP_NPU_V200
        ot_eis_media_deinit();
#endif
    }
#endif
    return ret;
}
