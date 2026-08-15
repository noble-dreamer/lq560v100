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

int main(int argc, char **argv)
{
    ot_avp_npu_config config = {0};
    ot_s32 ret;
    (void)argc;

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
