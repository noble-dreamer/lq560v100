/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
  NPU stereo matching: load model, setup datasets, run inference.
  Follows the pattern from media_ai/sample_ai.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stereo_npu.h"
#include "stereo_sec.h"
#include "ot_smr.h"
#include "ot_avp_npu_rts.h"
#include "ot_eis_media_sys.h"

/* -------------------------------------------------------------------------- */
/* NPU context (mirrors sample_ai.c's sample_npu_attr + model_info pattern)    */
/* Temporary serial mode: a single output buffer set is consumed immediately   */
/* by SubPixel in the same pipeline stage, so NPU/SubPixel ping-pong is off.   */
/* -------------------------------------------------------------------------- */

#define STEREO_NPU_BUF_SETS  1

typedef struct {
    ot_avp_handle       model_hdl;
    ot_u32              input_num;
    ot_u32              output_num;
    ot_avp_npu_dataset *input_dataset;
    ot_avp_npu_dataset *output_dataset[STEREO_NPU_BUF_SETS];
    ot_avp_npu_shape    input_dim[STEREO_NPU_MAX_INPUT];
    ot_avp_npu_shape    output_dim[STEREO_NPU_MAX_OUTPUT];
    ot_u32              input_stride[STEREO_NPU_MAX_INPUT];
    ot_u32              input_buffsize[STEREO_NPU_MAX_INPUT];
    ot_u32              output_stride[STEREO_NPU_MAX_OUTPUT];
    ot_u32              output_buffsize[STEREO_NPU_MAX_OUTPUT];
    ot_void            *input_data[STEREO_NPU_MAX_INPUT];    /* pre-allocated NPU input buf */
    ot_void            *output_data[STEREO_NPU_BUF_SETS][STEREO_NPU_MAX_OUTPUT];   /* cached virt addr */
    ot_phys_addr        output_phys[STEREO_NPU_BUF_SETS][STEREO_NPU_MAX_OUTPUT];   /* physical addr */
    ot_u32              cur_buf_set;    /* always 0 in temporary serial mode */
} stereo_npu_ctx_t;

static stereo_npu_ctx_t  g_npu_ctx;
static ot_bool           g_npu_inited = OT_FALSE;

/* -------------------------------------------------------------------------- */

ot_s32 stereo_npu_init(void)
{
    ot_s32 ret;
    ot_s32 i;
    stereo_npu_ctx_t *ctx = &g_npu_ctx;
    ot_avp_npu_config config;

    memset(ctx, 0, sizeof(*ctx));
    memset(&config, 0, sizeof(config));

    ret = ot_avp_npu_init(&config);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_npu] ot_avp_npu_init failed, ret:0x%x\n", ret);
        return ret;
    }

    /* Load stereo matching model — decrypt from encrypted file to memory, then load */
    ot_u8 *model_buf = NULL;
    ot_u32 model_len = 0;
    ret = stereo_sec_load_decrypt_model(&model_buf, &model_len);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_npu] stereo_sec_load_decrypt_model failed, ret:0x%x\n", ret);
        goto npu_deinit;
    }

    ret = ot_avp_npu_load_model_from_mem(model_buf, model_len, &ctx->model_hdl);
    /* Free plaintext model buffer — NPU has copied what it needs */
    free(model_buf);
    model_buf = NULL;
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_npu] load_model_from_mem failed, ret:0x%x\n", ret);
        goto npu_deinit;
    }

    /* Query IO counts */
    ot_avp_npu_get_input_num(ctx->model_hdl, &ctx->input_num);
    ot_avp_npu_get_output_num(ctx->model_hdl, &ctx->output_num);
    stereo_log_write("[stereo_npu] model loaded: %d inputs, %d outputs\n",
           ctx->input_num, ctx->output_num);

    /* Create datasets: 1 input + 1 output set (serial mode) */
    ctx->input_dataset  = ot_avp_npu_create_dataset();
    ctx->output_dataset[0] = ot_avp_npu_create_dataset();
    ctx->cur_buf_set = 0;

    /* Setup one output buffer set using SMR cached allocation.
       NPU DMA writes to physical address; CPU reads from cached virtual address.
       SubPixel runs synchronously after inference, before the next NPU execute. */
    for (ot_s32 s = 0; s < STEREO_NPU_BUF_SETS; s++) {
        for (i = 0; i < ctx->output_num; i++) {
            if (s == 0) {
                ot_avp_npu_get_output_shape_by_index(ctx->model_hdl, i, &ctx->output_dim[i]);
                ctx->output_stride[i]   = ot_avp_npu_get_output_default_stride(ctx->model_hdl, i);
                ctx->output_buffsize[i] = ot_avp_npu_get_output_size_by_index(ctx->model_hdl, i);
            }

            /* Allocate cached SMR buffer for output */
            ot_smr_alloc_attr smr_attr;
            memset(&smr_attr, 0, sizeof(smr_attr));
            snprintf((char *)smr_attr.region_name, OT_SMR_REGION_NAME_LEN_MAX, "anony");
            snprintf((char *)smr_attr.chunk_name, OT_SMR_CHUNK_NAME_MAX, "npu_out%d_s%d", i, s);
            smr_attr.len    = ctx->output_buffsize[i];
            smr_attr.align  = 64;
            smr_attr.cached = OT_TRUE;

            ret = ot_smr_alloc(&smr_attr, &ctx->output_phys[s][i], &ctx->output_data[s][i]);
            if (ret != OT_SUCCESS) {
                stereo_log_write("[stereo_npu] smr_alloc output[%d] set%d failed, ret:0x%x\n", i, s, ret);
                goto destroy_output;
            }

            ret = ot_avp_npu_add_buffer(ctx->output_dataset[s],
                                         (ot_u8 *)ctx->output_data[s][i],
                                         (ot_u64)ctx->output_phys[s][i],
                                         ctx->output_buffsize[i], ctx->output_stride[i]);
            if (ret != OT_SUCCESS) {
                stereo_log_write("[stereo_npu] add output[%d] set%d buffer failed\n", i, s);
                goto destroy_output;
            }

            if (s == 0) {
                stereo_log_write("[stereo_npu] output[%d]: size=%u stride=%u (SMR cached serial)\n",
                       i, ctx->output_buffsize[i], ctx->output_stride[i]);
            }
        }
    }

    /* Setup input buffers (pre-allocated; will be rebound per frame) */
    for (i = 0; i < ctx->input_num; i++) {
        ot_avp_npu_get_input_shape_by_index(ctx->model_hdl, i, &ctx->input_dim[i]);
        ctx->input_stride[i]   = ot_avp_npu_get_input_default_stride(ctx->model_hdl, i);
        ctx->input_buffsize[i] = ot_avp_npu_get_input_size_by_index(ctx->model_hdl, i);

        ret = ot_avp_npu_malloc(&ctx->input_data[i], ctx->input_buffsize[i]);
        if (ret != OT_SUCCESS) {
            stereo_log_write("[stereo_npu] malloc input[%d] failed\n", i);
            goto destroy_input;
        }

        ret = ot_avp_npu_add_buffer(ctx->input_dataset,
                                     (ot_u8 *)ctx->input_data[i], 0,
                                     ctx->input_buffsize[i], ctx->input_stride[i]);
        if (ret != OT_SUCCESS) {
            stereo_log_write("[stereo_npu] add input[%d] buffer failed\n", i);
            goto destroy_input;
        }

        stereo_log_write("[stereo_npu] input[%d]: size=%u stride=%u\n",
               i, ctx->input_buffsize[i], ctx->input_stride[i]);
    }

    g_npu_inited = OT_TRUE;
    stereo_log_write("[stereo_npu] init done\n");
    return OT_SUCCESS;

destroy_input:
    for (ot_s32 j = 0; j < i; j++) {
        if (ctx->input_data[j]) {
            ot_avp_npu_free(ctx->input_data[j]);
            ctx->input_data[j] = OT_NULL;
        }
    }
    ot_avp_npu_destroy_dataset(ctx->input_dataset);
destroy_output:
    for (ot_s32 s = 0; s < STEREO_NPU_BUF_SETS; s++) {
        for (ot_s32 j = 0; j < ctx->output_num; j++) {
            if (ctx->output_data[s][j]) {
                ot_smr_free(ctx->output_phys[s][j]);
                ctx->output_data[s][j] = OT_NULL;
            }
        }
        ot_avp_npu_destroy_dataset(ctx->output_dataset[s]);
    }
    ot_avp_npu_unload_model(ctx->model_hdl);
npu_deinit:
    ot_avp_npu_deinit();
    return ret;
}

void stereo_npu_deinit(void)
{
    stereo_npu_ctx_t *ctx = &g_npu_ctx;

    if (g_npu_inited != OT_TRUE) {
        return;
    }

    for (ot_s32 j = 0; j < ctx->input_num; j++) {
        if (ctx->input_data[j]) ot_avp_npu_free(ctx->input_data[j]);
    }
    for (ot_s32 s = 0; s < STEREO_NPU_BUF_SETS; s++) {
        for (ot_s32 j = 0; j < ctx->output_num; j++) {
            if (ctx->output_data[s][j]) ot_smr_free(ctx->output_phys[s][j]);
        }
    }
    ot_avp_npu_destroy_dataset(ctx->input_dataset);
    for (ot_s32 s = 0; s < STEREO_NPU_BUF_SETS; s++) {
        ot_avp_npu_destroy_dataset(ctx->output_dataset[s]);
    }
    ot_avp_npu_unload_model(ctx->model_hdl);
    ot_avp_npu_deinit();

    memset(ctx, 0, sizeof(*ctx));
    g_npu_inited = OT_FALSE;
    stereo_log_write("[stereo_npu] deinit done\n");
}

/* -------------------------------------------------------------------------- */
/* Run inference: directly bind CVE output buffer to NPU input via set_buffer.
   CVE buffers are ot_smr_alloc'd — already have valid virt/phys addresses.
   Raw uint8 RGB data is fed to NPU; the model's Preprocess node handles subtract-128.
   Single-output: returns float32 disparity directly (cost_data=NULL, cost_size=0).
   Dual-output: returns pointers to cost volume and integer disparity in-place. */
/* -------------------------------------------------------------------------- */

ot_s32 stereo_npu_infer(const ot_avp_cve_img *left_crop,
                         const ot_avp_cve_img *right_crop,
                         void **cost_data, ot_u32 *cost_size,
                         void **disp_data, ot_u32 *disp_size,
                         ot_u32 *buf_set_idx)
{
    ot_s32 ret;
    stereo_npu_ctx_t *ctx = &g_npu_ctx;

    if (!left_crop || !right_crop || !cost_data || !disp_data || !buf_set_idx) {
        return OT_FAILURE;
    }

    /* Serial mode always writes into set 0. SubPixel consumes the output before
       the next inference starts, so no buffer-set wait or in-use flag is needed. */
    ot_u32 cur_set = 0;

    /* Bind CVE output buffers directly to NPU input dataset. */
    ret = ot_avp_npu_set_buffer(ctx->input_dataset, 0,
                                 (ot_u8 *)(uintptr_t)left_crop->virt_addr[0],
                                 (ot_u64)left_crop->phys_addr[0],
                                 ctx->input_buffsize[0], ctx->input_stride[0]);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_npu] set input[0] buffer failed\n");
        return ret;
    }

    if (ctx->input_num > 1) {
        ret = ot_avp_npu_set_buffer(ctx->input_dataset, 1,
                                     (ot_u8 *)(uintptr_t)right_crop->virt_addr[0],
                                     (ot_u64)right_crop->phys_addr[0],
                                     ctx->input_buffsize[1], ctx->input_stride[1]);
        if (ret != OT_SUCCESS) {
            stereo_log_write("[stereo_npu] set input[1] buffer failed\n");
            return ret;
        }
    }

    /* Execute inference using current buffer set */
    ret = ot_avp_npu_execute(ctx->model_hdl, ctx->input_dataset, ctx->output_dataset[cur_set]);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_npu] execute failed, ret:0x%x\n", ret);
        return ret;
    }

    /* Invalidate CPU cache for output buffers so CPU sees fresh NPU DMA data.
       ot_smr_flush_cache on ARM does clean+invalidate (DC CIVAC). */
    for (ot_s32 j = 0; j < ctx->output_num; j++) {
        ot_smr_flush_cache(ctx->output_phys[cur_set][j], ctx->output_data[cur_set][j],
                           ctx->output_buffsize[j]);
    }

    /* Dual-output: auto-detect cost volume (larger) vs integer disparity (smaller) */
    if (ctx->output_num >= 2) {
        ot_s32 cost_idx = 0, disp_idx = 1;
        if (ctx->output_buffsize[1] > ctx->output_buffsize[0]) {
            cost_idx = 1;
            disp_idx = 0;
        }
        *cost_data = ctx->output_data[cur_set][cost_idx];
        *cost_size = ctx->output_buffsize[cost_idx];
        *disp_data = ctx->output_data[cur_set][disp_idx];
        *disp_size = ctx->output_buffsize[disp_idx];
    } else {
        /* Fallback: single output treated as disparity */
        *cost_data = NULL;
        *cost_size = 0;
        *disp_data = ctx->output_data[cur_set][0];
        *disp_size = ctx->output_buffsize[0];
    }

    *buf_set_idx = cur_set;
    ctx->cur_buf_set = 0;

    return ret;
}

/* -------------------------------------------------------------------------- */
/* Release an NPU output buffer set after SubPixel processing is complete.     */
/* Called by the SubPixel thread to signal that cost_data/disp_data pointers   */
/* are no longer being read, and the NPU can safely reuse this buffer set.     */
/* -------------------------------------------------------------------------- */

void stereo_npu_release_buf_set(ot_u32 buf_set_idx)
{
    (void)buf_set_idx;
}
