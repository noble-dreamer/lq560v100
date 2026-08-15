/*
 * dual_model_abab: two board offline models run in an ABAB async pipeline.
 *
 * usage:
 *   sample_dual_model_abab [modelA] [inputA] [modelB] [inputB] [repeat] [output_dir]
 */
#include "ot_avp_npu_rts.h"
#include "file_utils.h"
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

#define DEFAULT_MODEL_A  "../data/model/classification/resnet50_binary_b.ortm"
#define DEFAULT_INPUT_A  "../data/ImageNet/binary/ILSVRC2012_val_00024327.bin"
#define DEFAULT_MODEL_B  "../data/model/classification/mobilenetv2_rgbplanar_b.ortm"
#define DEFAULT_INPUT_B  "../data/ImageNet/rgbplanar/ILSVRC2012_val_00024327.rgb"

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
} model_slot;

static void usage(const char *prog)
{
    printf("Usage: %s [modelA] [inputA] [modelB] [inputB] [repeat] [output_dir]\n", prog);
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
        ot_u32 size;
        ot_u32 stride;
        ot_s32 ret;

        if (is_output) {
            ot_avp_npu_get_output_shape_by_index(slot->handle, i, &shape);
            ot_avp_npu_get_output_dtype_by_index(slot->handle, i, &dtype);
            size = ot_avp_npu_get_output_size_by_index(slot->handle, i);
            stride = ot_avp_npu_get_output_default_stride(slot->handle, i);
        } else {
            ot_avp_npu_get_input_shape_by_index(slot->handle, i, &shape);
            ot_avp_npu_get_input_dtype_by_index(slot->handle, i, &dtype);
            size = ot_avp_npu_get_input_size_by_index(slot->handle, i);
            stride = ot_avp_npu_get_input_default_stride(slot->handle, i);
        }

        tensors[i].len = size;
        tensors[i].shape = shape;
        tensors[i].dtype = dtype;
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
        printf("[%s] set config fail\n", slot->name);
        return ret;
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

    printf("[%s] model ready: %s (in=%u out=%u)\n",
           slot->name, model_path, slot->input_num, slot->output_num);
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

int main(int argc, char **argv)
{
    const char *model_a = (argc > 1) ? argv[1] : DEFAULT_MODEL_A;
    const char *model_b = (argc > 3) ? argv[3] : DEFAULT_MODEL_B;
    model_slot models[MODEL_NUM] = {0};
    ot_avp_npu_config config = {0};
    ot_s32 ret;

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
        goto npu_out;
    }
    ret = model_init(&models[1], "B", model_b);
    if (ret != 0) {
        goto npu_out;
    }

    model_destroy(&models[1]);
    model_destroy(&models[0]);

npu_out:
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
