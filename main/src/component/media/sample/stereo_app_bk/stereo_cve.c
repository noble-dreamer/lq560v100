/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
  CVE preprocessing: downsample 1280x1080→640x540, center crop 640x540→640x448
  Uses the ot_avp_cve_alg_lib helper functions for image creation/destruction.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stereo_cve.h"
#include "ot_avp_cve_alg_lib.h"
#include "ot_avp_cve.h"
#include "ot_smr.h"

/* Pre-allocated CVE images (reused per frame) */
static ot_avp_cve_img     g_cve_down_left;
static ot_avp_cve_img     g_cve_down_right;
static ot_avp_cve_img     g_cve_crop_left;
static ot_avp_cve_img     g_cve_crop_right;

/* CSC output images (RGB888 planar, intermediate — overwritten each frame) */
static ot_avp_cve_img     g_cve_rgb_left;
static ot_avp_cve_img     g_cve_rgb_right;

/* NPU input images (double-buffered: map writes XOR'd data here, NPU reads
   from the other set. Eliminates CVE/NPU buffer race condition.) */
#define STEREO_CVE_NPU_BUF_NUM  2
static ot_avp_cve_img     g_cve_npu_left[STEREO_CVE_NPU_BUF_NUM];
static ot_avp_cve_img     g_cve_npu_right[STEREO_CVE_NPU_BUF_NUM];
static ot_u32             g_cve_buf_idx = 0;

/* CVE resize auxiliary memory */
static ot_mem_info        g_cve_resize_aux_mem;

/* CVE operation handles (used for async ops, resolved as void*) */
static ot_avp_handle      g_resize_handle_l;
static ot_avp_handle      g_resize_handle_r;
static ot_avp_handle      g_dma_handle_l_y;
static ot_avp_handle      g_dma_handle_l_uv;
static ot_avp_handle      g_dma_handle_r_y;
static ot_avp_handle      g_dma_handle_r_uv;
static ot_avp_handle      g_csc_handle_l;
static ot_avp_handle      g_csc_handle_r;
static ot_avp_handle      g_map_handle_l;
static ot_avp_handle      g_map_handle_r;

/* XOR 0x80 LUT for uint8->int8 sign-bit flip (RGB-128, hardware-accelerated via CVE map) */
static ot_mem_info        g_xor_lut;

static ot_bool g_cve_inited        = OT_FALSE;
static ot_bool g_cve_engine_inited = OT_FALSE;

/* -------------------------------------------------------------------------- */
/* Forward declarations                                                       */
/* -------------------------------------------------------------------------- */

static ot_s32 stereo_cve_downsample(const ot_eis_img_frame *src_frame,
                                     ot_avp_cve_img *dst_img,
                                     ot_avp_handle *handle);

static ot_s32 stereo_cve_center_crop(ot_avp_cve_img *src_img,
                                      ot_avp_cve_img *dst_img,
                                      ot_avp_handle *h_y,
                                      ot_avp_handle *h_uv);

static void stereo_frame_to_cve_img(ot_avp_cve_img *dst, const ot_eis_img_frame *frame);

static ot_s32 stereo_cve_create_rgb888_planar(ot_avp_cve_img *img, ot_u32 width, ot_u32 height);

static ot_s32 stereo_cve_csc_to_rgb(ot_avp_cve_img *src_img,
                                     ot_avp_cve_img *dst_img,
                                     ot_avp_handle *handle);

/* -------------------------------------------------------------------------- */
/* Auxiliary memory size for CVE bilinear resize (YUV420SP)                    */
/* -------------------------------------------------------------------------- */

static ot_u32 stereo_cve_resize_aux_mem_size(void)
{
    /* Conservative estimate based on CVE tile processing requirements.
       The CVE resize needs ~16KB for tile alignment plus per-batch buffer. */
    return 64 * 1024;  /* 64KB — sufficient for bilinear resize of YUV420SP */
}

/* -------------------------------------------------------------------------- */

ot_s32 stereo_cve_init(void)
{
    ot_s32 ret;
    ot_u32 aux_size;

    memset(&g_cve_down_left,  0, sizeof(g_cve_down_left));
    memset(&g_cve_down_right, 0, sizeof(g_cve_down_right));
    memset(&g_cve_crop_left,  0, sizeof(g_cve_crop_left));
    memset(&g_cve_crop_right, 0, sizeof(g_cve_crop_right));
    memset(&g_cve_resize_aux_mem, 0, sizeof(g_cve_resize_aux_mem));

    /* Initialize the CVE engine before issuing any CVE operation. Without this
       the CVE device/driver is not opened and every ot_avp_cve_* op (resize,
       dma_2d, ...) returns the generic failure 0xffffffff. All official CVE
       samples (sample_ai.c, sample_cve_resize.c) call this first. */
    ret = ot_avp_cve_init();
    if (ret != OT_AVP_CVE_SUCCESS) {
        stereo_log_write("[stereo_cve] ot_avp_cve_init failed, ret:0x%x\n", ret);
        return ret;
    }
    g_cve_engine_inited = OT_TRUE;
    stereo_log_write("[stereo_cve] cve engine init ok\n");

    /* Create downsample output images (640x540 YUV420SP).
       NOTE: CVE resize only accepts OT_IMAGE_FORMAT_YUV_420_SEMIPLANAR; using
       the YVU variant makes ot_avp_cve_resize return 0xffffffff. The resize is
       a per-plane bilinear interpolation and does not reorder the interleaved
       U/V bytes, so tagging the NV21 source as YUV420SP is safe. */
    g_cve_down_left.type   = OT_IMAGE_FORMAT_YUV_420_SEMIPLANAR;
    g_cve_down_left.width  = STEREO_CVE_DOWNSAMPLE_W;
    g_cve_down_left.height = STEREO_CVE_DOWNSAMPLE_H;
    ret = ot_avp_cve_create_semi_planar_img(&g_cve_down_left);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_cve] create down_left image failed, ret:0x%x\n", ret);
        return ret;
    }

    g_cve_down_right.type   = OT_IMAGE_FORMAT_YUV_420_SEMIPLANAR;
    g_cve_down_right.width  = STEREO_CVE_DOWNSAMPLE_W;
    g_cve_down_right.height = STEREO_CVE_DOWNSAMPLE_H;
    ret = ot_avp_cve_create_semi_planar_img(&g_cve_down_right);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_cve] create down_right image failed, ret:0x%x\n", ret);
        goto err1;
    }

    /* Create center crop output images (640x448 YUV420SP) */
    g_cve_crop_left.type   = OT_IMAGE_FORMAT_YUV_420_SEMIPLANAR;
    g_cve_crop_left.width  = STEREO_CVE_CROP_W;
    g_cve_crop_left.height = STEREO_CVE_CROP_H;
    ret = ot_avp_cve_create_semi_planar_img(&g_cve_crop_left);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_cve] create crop_left image failed, ret:0x%x\n", ret);
        goto err2;
    }

    g_cve_crop_right.type   = OT_IMAGE_FORMAT_YUV_420_SEMIPLANAR;
    g_cve_crop_right.width  = STEREO_CVE_CROP_W;
    g_cve_crop_right.height = STEREO_CVE_CROP_H;
    ret = ot_avp_cve_create_semi_planar_img(&g_cve_crop_right);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_cve] create crop_right image failed, ret:0x%x\n", ret);
        goto err3;
    }

    /* Allocate auxiliary memory for resize */
    aux_size = stereo_cve_resize_aux_mem_size();
    ret = ot_avp_cve_create_mem_info(&g_cve_resize_aux_mem, aux_size);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_cve] create resize aux mem failed, ret:0x%x\n", ret);
        goto err4;
    }

    /* Create CSC output images (640x448 RGB888 planar). Intermediate buffers
       overwritten each frame — only used as map input within the same call.
       The per-pixel -128 bias (uint8->int8) is applied via CVE map
       (XOR 0x80 LUT) writing to separate double-buffered NPU input buffers. */
    ret = stereo_cve_create_rgb888_planar(&g_cve_rgb_left, STEREO_CVE_CROP_W, STEREO_CVE_CROP_H);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_cve] create rgb_left image failed, ret:0x%x\n", ret);
        goto err5;
    }

    ret = stereo_cve_create_rgb888_planar(&g_cve_rgb_right, STEREO_CVE_CROP_W, STEREO_CVE_CROP_H);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_cve] create rgb_right image failed, ret:0x%x\n", ret);
        goto err6;
    }

    /* Create double-buffered NPU input images (RGB888 planar, XOR'd output).
       Buffer 0 and 1 alternate per frame: CVE writes to one while NPU reads
       from the other. Requires NPU queue depth ≤ 1 for correctness. */
    ot_u32 npu_i;
    for (npu_i = 0; npu_i < STEREO_CVE_NPU_BUF_NUM; npu_i++) {
        ret = stereo_cve_create_rgb888_planar(&g_cve_npu_left[npu_i], STEREO_CVE_CROP_W, STEREO_CVE_CROP_H);
        if (ret != OT_SUCCESS) {
            stereo_log_write("[stereo_cve] create npu_left[%u] failed, ret:0x%x\n", npu_i, ret);
            goto err7;
        }
        ret = stereo_cve_create_rgb888_planar(&g_cve_npu_right[npu_i], STEREO_CVE_CROP_W, STEREO_CVE_CROP_H);
        if (ret != OT_SUCCESS) {
            stereo_log_write("[stereo_cve] create npu_right[%u] failed, ret:0x%x\n", npu_i, ret);
            goto err8;
        }
    }

    /* Allocate identity LUT (256 bytes in SMR) for CVE map operation.
       The ORTM model's Preprocess node handles subtract-128 internally,
       so CVE outputs raw uint8 RGB888 planar data without sign-bit flip.
       lut[i] = i  (identity copy via CVE map, maintains double-buffering). */
    ret = ot_avp_cve_create_mem_info(&g_xor_lut, 256);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_cve] create identity lut failed, ret:0x%x\n", ret);
        goto err9;
    }
    {
        ot_u8 *lut_ptr = (ot_u8 *)(uintptr_t)g_xor_lut.virt_addr;
        for (ot_u32 i = 0; i < 256; i++) {
            lut_ptr[i] = (ot_u8)i;
        }
    }

    g_cve_inited = OT_TRUE;
    stereo_log_write("[stereo_cve] init done. down=%ux%u, crop=%ux%u, csc=RGB888planar, xor_lut=ok, npu_buf=%u\n",
                     STEREO_CVE_DOWNSAMPLE_W, STEREO_CVE_DOWNSAMPLE_H,
                     STEREO_CVE_CROP_W, STEREO_CVE_CROP_H, STEREO_CVE_NPU_BUF_NUM);
    return OT_SUCCESS;

err9:
    ot_avp_cve_destory_mem_info(&g_xor_lut);
    /* fall through: free NPU buffers too */

err8:
    /* npu_right[npu_i] failed: free npu_right[0..npu_i-1] and npu_left[0..npu_i] */
    for (ot_u32 j = 0; j < npu_i; j++) {
        ot_avp_cve_destory_img(&g_cve_npu_right[j]);
    }
    for (ot_u32 j = 0; j <= npu_i; j++) {
        ot_avp_cve_destory_img(&g_cve_npu_left[j]);
    }
    goto err_rgb;

err7:
    /* npu_left[npu_i] failed: free npu_right[0..npu_i-1] and npu_left[0..npu_i-1] */
    for (ot_u32 j = 0; j < npu_i; j++) {
        ot_avp_cve_destory_img(&g_cve_npu_right[j]);
        ot_avp_cve_destory_img(&g_cve_npu_left[j]);
    }

err_rgb:
    ot_avp_cve_destory_img(&g_cve_rgb_right);

err6:
    ot_avp_cve_destory_img(&g_cve_rgb_left);
err5:
    ot_avp_cve_destory_mem_info(&g_cve_resize_aux_mem);
err4:
    ot_avp_cve_destory_img(&g_cve_crop_right);
err3:
    ot_avp_cve_destory_img(&g_cve_crop_left);
err2:
    ot_avp_cve_destory_img(&g_cve_down_right);
err1:
    ot_avp_cve_destory_img(&g_cve_down_left);
    if (g_cve_engine_inited == OT_TRUE) {
        ot_avp_cve_deinit();
        g_cve_engine_inited = OT_FALSE;
    }
    return ret;
}

void stereo_cve_deinit(void)
{
    if (g_cve_inited != OT_TRUE) {
        return;
    }

    ot_avp_cve_destory_mem_info(&g_xor_lut);
    for (ot_u32 i = 0; i < STEREO_CVE_NPU_BUF_NUM; i++) {
        ot_avp_cve_destory_img(&g_cve_npu_right[i]);
        ot_avp_cve_destory_img(&g_cve_npu_left[i]);
    }
    ot_avp_cve_destory_img(&g_cve_rgb_right);
    ot_avp_cve_destory_img(&g_cve_rgb_left);
    ot_avp_cve_destory_mem_info(&g_cve_resize_aux_mem);
    ot_avp_cve_destory_img(&g_cve_crop_right);
    ot_avp_cve_destory_img(&g_cve_crop_left);
    ot_avp_cve_destory_img(&g_cve_down_right);
    ot_avp_cve_destory_img(&g_cve_down_left);

    if (g_cve_engine_inited == OT_TRUE) {
        ot_avp_cve_deinit();
        g_cve_engine_inited = OT_FALSE;
    }

    g_cve_inited = OT_FALSE;
    stereo_log_write("[stereo_cve] deinit done\n");
}

const ot_avp_cve_img *stereo_cve_get_left_crop(void)
{
    return &g_cve_crop_left;
}

const ot_avp_cve_img *stereo_cve_get_right_crop(void)
{
    return &g_cve_crop_right;
}

/* -------------------------------------------------------------------------- */
/* Convert VPROC video frame to CVE image (zero-copy: reference phys addr)     */
/* -------------------------------------------------------------------------- */

static void stereo_frame_to_cve_img(ot_avp_cve_img *dst, const ot_eis_img_frame *frame)
{
    memset(dst, 0, sizeof(*dst));
    dst->virt_addr[0] = (ot_u64)frame->buff.virt_addr[0];
    dst->virt_addr[1] = (ot_u64)frame->buff.virt_addr[1];
    dst->phys_addr[0] = (ot_u64)frame->buff.phys_addr[0];
    dst->phys_addr[1] = (ot_u64)frame->buff.phys_addr[1];
    dst->stride[0]    = frame->buff.stride[0];
    dst->stride[1]    = frame->buff.stride[1];
    dst->width        = frame->attr.width;
    dst->height       = frame->attr.height;
    dst->type         = OT_IMAGE_FORMAT_YUV_420_SEMIPLANAR;
}

/* -------------------------------------------------------------------------- */
/* Create a 3-channel planar 8-bit image (RGB stored as U8C3 planar).          */
/* The CVE CSC operator only accepts YUV420SP/YUV422SP/U8C3_PACKED/U8C3_PLANAR */
/* as src/dst formats (per CVE API doc). RGB output must therefore use         */
/* OT_IMAGE_FORMAT_U8C3_PLANAR, whose byte layout (3 planes, stride*height     */
/* each) is identical to RGB888 planar and matches the NPU input.             */
/* -------------------------------------------------------------------------- */

static ot_s32 stereo_cve_create_rgb888_planar(ot_avp_cve_img *img, ot_u32 width, ot_u32 height)
{
    ot_smr_alloc_attr alloc_attr;
    ot_phys_addr phys_addr = 0;
    ot_void *virt_addr = OT_NULL;
    ot_u32 stride, img_size;

    memset(img, 0, sizeof(*img));
    memset(&alloc_attr, 0, sizeof(alloc_attr));

    img->type   = OT_IMAGE_FORMAT_U8C3_PLANAR;
    img->width  = (ot_u16)width;
    img->height = (ot_u16)height;

    /* CVE requires 16-byte aligned stride; 640 is already aligned. */
    stride = ((width + 15) / 16) * 16;
    img->stride[0] = stride;
    img->stride[1] = stride;
    img->stride[2] = stride;

    img_size = height * stride * 3;

    alloc_attr.len   = img_size;
    alloc_attr.align = 1;
    strcpy((char *)alloc_attr.region_name, "anony");

    if (ot_smr_alloc(&alloc_attr, &phys_addr, &virt_addr) != OT_SUCCESS) {
        stereo_log_write("[stereo_cve] smr_alloc rgb888 planar (%u bytes) failed\n", img_size);
        return OT_FAILURE;
    }

    memset(virt_addr, 0, img_size);
    img->virt_addr[0] = (ot_u64)virt_addr;
    img->virt_addr[1] = img->virt_addr[0] + (ot_u64)stride * height;
    img->virt_addr[2] = img->virt_addr[1] + (ot_u64)stride * height;
    img->phys_addr[0] = (ot_u64)phys_addr;
    img->phys_addr[1] = img->phys_addr[0] + (ot_u64)stride * height;
    img->phys_addr[2] = img->phys_addr[1] + (ot_u64)stride * height;
    img->capacity     = img_size;

    return OT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* CSC: YUV420SP -> RGB888 planar (BT.601 limited range)                       */
/* -------------------------------------------------------------------------- */

static ot_s32 stereo_cve_csc_to_rgb(ot_avp_cve_img *src_img,
                                     ot_avp_cve_img *dst_img,
                                     ot_avp_handle *handle)
{
    ot_s32 ret;
    ot_avp_cve_csc_ctrl ctrl;

    memset(&ctrl, 0, sizeof(ctrl));
    ctrl.csc_mode = OT_CSC_BT601_YCbCr_LR2RGB_LR;

    ret = ot_avp_cve_csc(handle, src_img, dst_img, &ctrl, OT_AVP_CVE_INVOKE_SYNC);
    if (ret != OT_AVP_CVE_SUCCESS) {
        stereo_log_write("[stereo_cve] csc YUV->RGB failed, ret:0x%x\n", ret);
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* Bilinear downsample: 1280x1080 -> 640x540                                   */
/* -------------------------------------------------------------------------- */

static ot_s32 stereo_cve_downsample(const ot_eis_img_frame *src_frame,
                                     ot_avp_cve_img *dst_img,
                                     ot_avp_handle *handle)
{
    ot_s32 ret;
    ot_avp_cve_img src_img;
    ot_avp_cve_resize_ctrl ctrl;

    memset(&src_img, 0, sizeof(src_img));
    memset(&ctrl,    0, sizeof(ctrl));

    /* Wrap source frame as CVE image (zero-copy) */
    stereo_frame_to_cve_img(&src_img, src_frame);

    /* Configure bilinear resize, batch=1 */
    ctrl.alg_mode  = OT_AVP_CVE_RESIZE_MODE_BILINEAR;
    ctrl.batch_num = 1;
    ctrl.mem       = g_cve_resize_aux_mem;

    /* Cast to the expected parameter types (src_img / dst_img are single-element arrays) */
    ret = ot_avp_cve_resize(handle, &src_img, dst_img, &ctrl, OT_AVP_CVE_INVOKE_SYNC);
    if (ret != OT_AVP_CVE_SUCCESS) {
        stereo_log_write("[stereo_cve] resize %dx%d -> %dx%d failed, ret:0x%x\n",
               src_frame->attr.width, src_frame->attr.height,
               dst_img->width, dst_img->height, ret);
    }

    return (ret == OT_AVP_CVE_SUCCESS) ? OT_SUCCESS : OT_FAILURE;
}

/* -------------------------------------------------------------------------- */
/* DMA 2D center crop: 640x540 -> 640x448                                     */
/*   Source ROI: x=0(STEREO_CROP_LEFT), y=46(STEREO_CROP_TOP), w=640, h=448   */
/* -------------------------------------------------------------------------- */

static ot_s32 stereo_cve_center_crop(ot_avp_cve_img *src_img,
                                      ot_avp_cve_img *dst_img,
                                      ot_avp_handle *h_y,
                                      ot_avp_handle *h_uv)
{
    ot_s32 ret;
    ot_data_info y_src, uv_src, y_dst, uv_dst;
    ot_avp_cve_dma_2d_ctrl ctrl;

    memset(&y_src,  0, sizeof(y_src));
    memset(&uv_src, 0, sizeof(uv_src));
    memset(&y_dst,  0, sizeof(y_dst));
    memset(&uv_dst, 0, sizeof(uv_dst));
    memset(&ctrl,   0, sizeof(ctrl));

    /* Y plane: offset by (STEREO_CROP_TOP * stride + STEREO_CROP_LEFT) bytes */
    y_src.phys_addr = src_img->phys_addr[0]
                    + STEREO_CROP_TOP * src_img->stride[0]
                    + STEREO_CROP_LEFT;
    y_src.stride    = src_img->stride[0];
    y_src.width     = STEREO_CVE_CROP_W;
    y_src.height    = STEREO_CVE_CROP_H;

    y_dst.phys_addr = dst_img->phys_addr[0];
    y_dst.stride    = dst_img->stride[0];
    y_dst.width     = STEREO_CVE_CROP_W;
    y_dst.height    = STEREO_CVE_CROP_H;

    /* UV plane: offset by ((STEREO_CROP_TOP/2) * stride + STEREO_CROP_LEFT)
       UV plane height = image height / 2 */
    uv_src.phys_addr = src_img->phys_addr[1]
                     + (STEREO_CROP_TOP / 2) * src_img->stride[1]
                     + STEREO_CROP_LEFT;
    uv_src.stride    = src_img->stride[1];
    uv_src.width     = STEREO_CVE_CROP_W;
    uv_src.height    = STEREO_CVE_CROP_H / 2;

    uv_dst.phys_addr = dst_img->phys_addr[1];
    uv_dst.stride    = dst_img->stride[1];
    uv_dst.width     = STEREO_CVE_CROP_W;
    uv_dst.height    = STEREO_CVE_CROP_H / 2;

    ctrl.mode = OT_AVP_CVE_DMA_2D_MODE_DIRECT_CP;

    /* Copy Y plane */
    ret = ot_avp_cve_dma_2d(h_y, &y_src, &y_dst, &ctrl, OT_AVP_CVE_INVOKE_SYNC);
    if (ret != OT_AVP_CVE_SUCCESS) {
        stereo_log_write("[stereo_cve] dma_2d Y plane failed, ret:0x%x\n", ret);
        return OT_FAILURE;
    }

    /* Copy UV plane */
    ret = ot_avp_cve_dma_2d(h_uv, &uv_src, &uv_dst, &ctrl, OT_AVP_CVE_INVOKE_SYNC);
    if (ret != OT_AVP_CVE_SUCCESS) {
        stereo_log_write("[stereo_cve] dma_2d UV plane failed, ret:0x%x\n", ret);
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* XOR 0x80 sign-bit flip via CVE map (hardware-accelerated uint8->int8)       */
/* Applies a 256-entry LUT to each of the 3 planes of a U8C3_PLANAR image.    */
/* lut[i] = i ^ 0x80  <=>  (i - 128) mod 256  <=>  RGB-128                     */
/* The NPU reinterprets the resulting bytes as int8, yielding [-128, 127].     */
/* -------------------------------------------------------------------------- */

static ot_s32 stereo_cve_xor_sign_bit(const ot_avp_cve_img *src_img,
                                       ot_avp_cve_img *dst_img,
                                       ot_avp_handle *handle)
{
    ot_s32 ret;
    ot_avp_cve_img src_plane, dst_plane;
    ot_avp_cve_src_lut_data lut_data;

    memset(&src_plane, 0, sizeof(src_plane));
    memset(&dst_plane, 0, sizeof(dst_plane));
    memset(&lut_data, 0, sizeof(lut_data));

    /* LUT is pre-filled in init: lut[i] = i (identity, no XOR) */
    lut_data.phys_addr = g_xor_lut.phys_addr;
    lut_data.virt_addr = g_xor_lut.virt_addr;
    lut_data.size      = g_xor_lut.size;

    /* Process each of the 3 planes (R, G, B) as U8C1 via CVE map.
       src and dst are separate buffers (not in-place) to avoid races. */
    src_plane.type      = OT_IMAGE_FORMAT_U8C1;
    src_plane.width     = src_img->width;
    src_plane.height    = src_img->height;
    src_plane.stride[0] = src_img->stride[0];

    dst_plane.type      = OT_IMAGE_FORMAT_U8C1;
    dst_plane.width     = dst_img->width;
    dst_plane.height    = dst_img->height;
    dst_plane.stride[0] = dst_img->stride[0];

    for (ot_s32 ch = 0; ch < 3; ch++) {
        src_plane.virt_addr[0] = src_img->virt_addr[ch];
        src_plane.phys_addr[0] = src_img->phys_addr[ch];
        dst_plane.virt_addr[0] = dst_img->virt_addr[ch];
        dst_plane.phys_addr[0] = dst_img->phys_addr[ch];

        ret = ot_avp_cve_map(handle, &src_plane, &lut_data, &dst_plane, OT_AVP_CVE_INVOKE_SYNC);
        if (ret != OT_AVP_CVE_SUCCESS) {
            stereo_log_write("[stereo_cve] map XOR 0x80 plane %d failed, ret:0x%x\n", ch, ret);
            return OT_FAILURE;
        }
    }

    return OT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* Full CVE processing: downsample + crop + CSC + XOR 0x80 for both left/right */
/* -------------------------------------------------------------------------- */

ot_s32 stereo_cve_process(const stereo_frame_pair_t *frame_pair,
                           ot_avp_cve_img *left_crop,
                           ot_avp_cve_img *right_crop)
{
    ot_s32 ret;

    if (!frame_pair || !left_crop || !right_crop) {
        return OT_FAILURE;
    }

    /* Step 1: Bilinear downsample 1280x1080 → 640x540 for left and right */
    ret = stereo_cve_downsample(&frame_pair->left_full,  &g_cve_down_left,  &g_resize_handle_l);
    if (ret != OT_SUCCESS) {
        return ret;
    }

    ret = stereo_cve_downsample(&frame_pair->right_full, &g_cve_down_right, &g_resize_handle_r);
    if (ret != OT_SUCCESS) {
        return ret;
    }

    /* Step 2: DMA 2D center crop 640x540 → 640x448 for left and right */
    ret = stereo_cve_center_crop(&g_cve_down_left,  &g_cve_crop_left,
                                  &g_dma_handle_l_y, &g_dma_handle_l_uv);
    if (ret != OT_SUCCESS) {
        return ret;
    }

    ret = stereo_cve_center_crop(&g_cve_down_right, &g_cve_crop_right,
                                  &g_dma_handle_r_y, &g_dma_handle_r_uv);
    if (ret != OT_SUCCESS) {
        return ret;
    }

    /* Step 3: CSC YUV420SP -> RGB888 planar (BT.601 limited range) for both eyes.
       This produces the 640x448x3 layout the NPU model expects. */
    ret = stereo_cve_csc_to_rgb(&g_cve_crop_left, &g_cve_rgb_left, &g_csc_handle_l);
    if (ret != OT_SUCCESS) {
        return ret;
    }

    ret = stereo_cve_csc_to_rgb(&g_cve_crop_right, &g_cve_rgb_right, &g_csc_handle_r);
    if (ret != OT_SUCCESS) {
        return ret;
    }

    /* Step 4: Identity copy (raw uint8 RGB -> NPU input buffer) via CVE map.
       The ORTM model's Preprocess node handles subtract-128 internally,
       so no sign-bit flip is needed. CVE map with identity LUT copies
       RGB888 planar data to double-buffered g_cve_npu_*[buf_idx]. */
    ot_u32 buf_idx = g_cve_buf_idx;
    ret = stereo_cve_xor_sign_bit(&g_cve_rgb_left, &g_cve_npu_left[buf_idx], &g_map_handle_l);
    if (ret != OT_SUCCESS) {
        return ret;
    }

    ret = stereo_cve_xor_sign_bit(&g_cve_rgb_right, &g_cve_npu_right[buf_idx], &g_map_handle_r);
    if (ret != OT_SUCCESS) {
        return ret;
    }

    /* Output: XOR'd RGB888 planar from the current double-buffer slot */
    *left_crop  = g_cve_npu_left[buf_idx];
    *right_crop = g_cve_npu_right[buf_idx];
    g_cve_buf_idx = (g_cve_buf_idx + 1) % STEREO_CVE_NPU_BUF_NUM;

    return OT_SUCCESS;
}
