/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#include "uvc_media.h"
#include <stdint.h>
#include <unistd.h>
#include <sys/prctl.h>
#include "log.h"
#include "sample_comm.h"
#include "uvc.h"
#include "frame_cache.h"
#include "ot_eis_vi_api.h"
#include "uvc_scene_auto.h"

// #define USE_DEPTH_LUT

#ifdef USE_DEPTH_LUT
#include "depth_lut.h"
#endif

#define SAMPLE_DPU_IMG_H    480
#define SAMPLE_DPU_IMG_W    640

/* UVC dev_no should always equal to UVC chn, equal to X in /dev/videoX */
static uvc_media_ctx    gs_media_ctx[OT_UVC_MAX_CHN_NUM] = {0};
static encoder_property gs_encoder_prop[OT_UVC_MAX_CHN_NUM] = {0};

static ot_eis_handle    gs_media_pipe_hdl;
static ot_bool          gs_is_sys_init = OT_FALSE;

uvc_media_ctx *sample_uvc_get_media_ctx(int dev_no)
{
    if (dev_no < 0 || dev_no >= OT_UVC_MAX_CHN_NUM) {
        return OT_NULL;
    }

    return gs_media_ctx + dev_no;
}

static ot_void sample_uvc_get_default_media_cfg(uvc_media_ctx *ctx, int max_cnt)
{
    memset(ctx, 0, sizeof(uvc_media_ctx) * max_cnt);
    for (int i = 0; i < max_cnt; i++) {
        ctx[i].dev_no    = i;
        ctx[i].sns_type  = SMS_SC132GS_MIPI_1_5M_30FPS_RAW12_DIRECT_SLAVE;
        ctx[i].sns_num   = 2;
        ctx[i].dev_id[0] = 0;
        ctx[i].dev_id[1] = 2;
    }
}

static ot_s32 sample_uvc_get_vrb_cfg(ot_u32 pipe_num, ot_u32 width, ot_u32 height, ot_video_buffer_attr *buffer_attr)
{
    ot_eis_buf_size_calc_cfg calc_cfg;
    ot_eis_img_attr img_attr;

    if(buffer_attr == OT_NULL) {
        return -1;
    }

    buffer_attr->cnt = 2;

    img_attr.width         = width;
    img_attr.height        = height;
    img_attr.bit_depth     = OT_EIS_PIXEL_BIT_DEPTH_8;
    img_attr.pixel_fmt     = OT_EIS_IMAGE_FORMAT_BAYER_12BPP;
    img_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    img_attr.layout        = OT_EIS_IMAGE_LAYOUT_LINEAR;
    sample_common_get_pic_buf_cfg(&img_attr, &calc_cfg);
    buffer_attr->buf_blks[0].size = calc_cfg.blk_size;
    buffer_attr->buf_blks[0].cnt  = pipe_num * 3;
    strcpy(buffer_attr->buf_blks[0].region_name, "anony");

    img_attr.pixel_fmt     = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
    img_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    img_attr.layout        = OT_EIS_IMAGE_LAYOUT_LINEAR;
    sample_common_get_pic_buf_cfg(&img_attr, &calc_cfg);
    buffer_attr->buf_blks[1].size = calc_cfg.blk_size;
    buffer_attr->buf_blks[1].cnt  = pipe_num * 3;
    strcpy(buffer_attr->buf_blks[1].region_name, "anony");

    return 0;
}


static ot_s32 sample_uvc_user_mode_buf_pool_creat(ot_eis_img_attr *img_attr, ot_eis_handle *pool_hdl)
{
    ot_s32 ret;
    ot_u32 block_size;
    ot_video_buffer_attr pool_attr = { 0 };
    ot_eis_handle tmp;

    img_attr->bit_depth = OT_EIS_PIXEL_BIT_DEPTH_8;
    sample_common_get_buffer_pool_cfg(img_attr, &block_size);
    pool_attr.cnt = 1;
    pool_attr.buf_blks[0].cnt = 10;
    pool_attr.buf_blks[0].size = block_size;

    ret = ot_buffer_pool_create(&tmp, &pool_attr);
    if (ret != 0) {
        printf("buffer_pool create fail! ret[%d]\n", ret);
        goto pool_create_failed;
    }
    *pool_hdl = tmp;
    sample_print("create pool hdl 0x%llx \n", (ot_u64)tmp);
    return OT_SUCCESS;

pool_create_failed:
    ot_buffer_pool_deinit();
    return ret;
}

static ot_s32 sample_uvc_stitch_images_to_fnode(frame_node_t *fnode, ot_eis_img_frame* img_l, ot_eis_img_frame *img_r)
{
    ot_s32 ret = OT_SUCCESS;

    ot_u32 tgt_size = img_l->attr.height * img_l->attr.width * 2 + img_r->attr.height * img_r->attr.width * 2;

    if (tgt_size > fnode->length) {
        return OT_FAILURE;
    }

    if (img_l->attr.height != img_r->attr.height || img_l->attr.width != img_r->attr.width) {
        return OT_FAILURE;
    }

    if (img_l->attr.pixel_fmt != OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR || img_r->attr.pixel_fmt != OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR) {
        return OT_FAILURE;
    }

    ot_u32 buf_size = img_l->buff.stride[0] * img_l->attr.height + img_l->buff.stride[1] * img_l->attr.height / 2;
    ot_u32 c_offset = img_l->buff.phys_addr[1] - img_l->buff.phys_addr[0];

    ot_u8 *y_l = OT_NULL, *c_l = OT_NULL, *y_r = OT_NULL, *c_r = OT_NULL;
    ot_u8 *dst = fnode->mem;

    ret = ot_smr_mmap(img_l->buff.phys_addr[0], buf_size, OT_TRUE, (ot_void *)&y_l);
    if (ret != OT_SUCCESS || y_l == OT_NULL) {
        sample_print("mmap phys 0x%llx size %d error \n", img_l->buff.phys_addr[0], buf_size);
        goto failed_exit;
    }

    ret = ot_smr_mmap(img_r->buff.phys_addr[0], buf_size, OT_TRUE, (ot_void *)&y_r);
    if (ret != OT_SUCCESS || y_r == OT_NULL) {
        sample_print("mmap phys 0x%llx size %d error \n", img_r->buff.phys_addr[0], buf_size);
        goto failed_exit;
    }

    c_l = y_l + c_offset;
    c_r = y_r + c_offset;

    for (ot_u32 i = 0; i < img_l->attr.height; i++) {
        ot_u8 *d = dst + i * img_l->attr.width * 2 + i * img_r->attr.width * 2;

        ot_u8 *y = y_l + i * img_l->buff.stride[0];
        ot_u8 *c = c_l + i / 2 * img_l->buff.stride[1];
        for (ot_u32 j = 0; j < img_l->attr.width / 2; j++) {
            *d++ = *y++; // Y
            *d++ = c[1]; // U
            *d++ = *y++; // Y
            *d++ = c[0]; // V
            c += 2;
        }

        y = y_r + i * img_r->buff.stride[0];
        c = c_r + i / 2 * img_r->buff.stride[1];
        for (ot_u32 j = 0; j < img_r->attr.width / 2; j++) {
            *d++ = *y++; // Y
            *d++ = c[1]; // U
            *d++ = *y++; // Y
            *d++ = c[0]; // V
            c += 2;
        }
    }

    fnode->used = tgt_size;

failed_exit:
    if (y_l != OT_NULL) {
        ot_smr_munmap(y_l, buf_size);
    }
    if (y_r != OT_NULL) {
        ot_smr_munmap(y_r, buf_size);
    }

    return ret;
}

static ot_s32 sample_uvc_stitch_dpu_match_to_fnode(frame_node_t *fnode, sample_dpu_cfg *dpu_cfg, ot_avp_dpu_match_src_frame_info *match_src, ot_avp_dpu_match_dst_frame_info *match_dst)
{
    ot_s32 ret = OT_SUCCESS;
    ot_eis_img_frame *img_l = &match_src->rect_frame[0], *img_r = &match_src->rect_frame[1];
    ot_eis_img_frame *img_d = &match_dst->depth_frame;

    ot_u32 tgt_size = img_l->attr.height * img_l->attr.width * 2 + img_r->attr.height * img_r->attr.width * 2 + img_d->attr.height * img_d->attr.width * 2;

    if (tgt_size > fnode->length) {
        sample_print("fnode size not match!\n");
        return OT_FAILURE;
    }

    if (img_l->attr.height != img_r->attr.height || img_l->attr.width != img_r->attr.width) {
        sample_print("left-right size not match!\n");
        return OT_FAILURE;
    }

    if (img_l->attr.height != img_d->attr.height || img_l->attr.width != img_d->attr.width) {
        sample_print("depth size not match!\n");
        return OT_FAILURE;
    }

    ot_u8  *y_l = OT_NULL, *y_r = OT_NULL;
    ot_u16 *y_d = OT_NULL;    
    ot_u32 imgsz_l = 0, imgsz_r = 0, imgsz_d = 0, pts = match_src->src_frame[0].pts / 1000;

    ot_eis_img_frame *img_sl = &match_src->src_frame[0], *img_sr = &match_src->src_frame[1];
    ot_u8  *c_sl = OT_NULL, *c_sr = OT_NULL;
    ot_u16 *i_sl = (ot_u16 *)(uintptr_t)(dpu_cfg->dpu_rect_mem_info[0].virt_addr + 0x10);
    ot_u16 *i_sr = (ot_u16 *)(uintptr_t)(dpu_cfg->dpu_rect_mem_info[1].virt_addr + 0x10);
    ot_u32  imgsz_sl = 0, imgsz_sr = 0;

    ot_u8 *dst = fnode->mem;
    imgsz_l = img_l->buff.stride[0] * img_l->attr.height;
    ret = ot_smr_mmap(img_l->buff.phys_addr[0], imgsz_l, OT_TRUE, (ot_void **)&y_l);
    if (ret != OT_SUCCESS || y_l == OT_NULL) {
        sample_print("mmap phys 0x%llx size %d error \n", img_l->buff.phys_addr[0], imgsz_l);
        goto failed_exit;
    }

    imgsz_r = img_r->buff.stride[0] * img_r->attr.height;
    ret = ot_smr_mmap(img_r->buff.phys_addr[0], imgsz_r, OT_TRUE, (ot_void **)&y_r);
    if (ret != OT_SUCCESS || y_r == OT_NULL) {
        sample_print("mmap phys 0x%llx size %d error \n", img_r->buff.phys_addr[0], imgsz_r);
        goto failed_exit;
    }

    imgsz_d = img_d->buff.stride[0] * img_d->attr.height;
    ret = ot_smr_mmap(img_d->buff.phys_addr[0], imgsz_d, OT_TRUE, (ot_void **)&y_d);
    if (ret != OT_SUCCESS || y_d == OT_NULL) {
        sample_print("mmap phys 0x%llx size %d error \n", img_d->buff.phys_addr[0], imgsz_d);
        goto failed_exit;
    }

    imgsz_sl = img_sl->buff.stride[1] * img_sl->attr.height / 2;
    ret = ot_smr_mmap(img_sl->buff.phys_addr[1], imgsz_sl, OT_TRUE, (void **)&c_sl);
    if (ret != OT_SUCCESS || c_sl == OT_NULL) {
        sample_print("mmap phys 0x%llx size %d error \n", img_sl->buff.phys_addr[1], imgsz_sl);
        goto failed_exit;
    }

    imgsz_sr = img_sr->buff.stride[1] * img_sr->attr.height / 2;
    ret = ot_smr_mmap(img_sr->buff.phys_addr[1], imgsz_sr, OT_TRUE, (void **)&c_sr);
    if (ret != OT_SUCCESS || c_sr == OT_NULL) {
        sample_print("mmap phys 0x%llx size %d error \n", img_sr->buff.phys_addr[1], imgsz_sr);
        goto failed_exit;
    }
    
    for (ot_u32 i = 0; i < img_l->attr.height; i++) {

        ot_u8  *d = dst + i * (img_l->attr.width + img_r->attr.width + img_d->attr.width) * 2;
        ot_u8  *y = y_l + i * img_l->buff.stride[0];
        ot_u16 *c_i = i_sl + i * img_l->attr.width * 2, xi  = 0, yi = 0;
        ot_u32   ss = img_l->buff.stride[1];
        for (ot_u32 j = 0; j < img_l->attr.width / 2; j++) {
            *d++ = *y++; // Y

            xi = ((*c_i++) >> 5) | 0x01;
            yi = (*c_i++) >> 6;
            *d++ = c_sl[yi * ss + xi];  // U

            *d++ = *y++; // Y

            xi = ((*c_i++) >> 5) & (~0x01);
            yi = (*c_i++) >> 6;
            *d++ = c_sl[yi * ss + xi];  // V
        }

        y = y_r + i * img_r->buff.stride[0];
        c_i = i_sr + i * img_r->attr.width * 2;
        ss  = img_r->buff.stride[1];
        for (ot_u32 j = 0; j < img_r->attr.width / 2; j++) {
            *d++ = *y++; // Y

            xi = ((*c_i++) >> 5) | 0x01;
            yi = (*c_i++) >> 6;
            *d++ = c_sr[yi * ss + xi];  // U

            *d++ = *y++; // Y

            xi = ((*c_i++) >> 5) & (~0x01);
            yi = (*c_i++) >> 6;
            *d++ = c_sr[yi * ss + xi];  // V
        }

        ot_u16 *m = (ot_u16 *)((ot_u8 *)y_d + i * img_d->buff.stride[0]);
#ifdef USE_DEPTH_LUT
        YUVColor *yuv = NULL;
        for (ot_u32 j = 0; j < img_d->attr.width / 2; j++) {
            yuv = depth_to_yuv((uint8_t)((*m >> 8) & 0xFF));
            *d++ = yuv->Y; // Y
            *d++ = yuv->U; // U
            m++;
            yuv = depth_to_yuv((uint8_t)((*m >> 8) & 0xFF));
            *d++ = yuv->Y; // Y
            *d++ = yuv->V; // V
            m++;
        }
#else
        for (ot_u32 j = 0; j < img_d->attr.width; j++) {
            *d++ = (*m >> 8) & 0xFF; // Y
            *d++ = (*m++ & 0xFF);    // UV
        }
#endif
    }

    // add timestamp to the head of frame
    dst[0] = (pts >> 24) & 0xFF;
    dst[1] = (pts >> 16) & 0xFF;
    dst[2] = (pts >>  8) & 0xFF;
    dst[3] = (pts      ) & 0xFF;

    fnode->used = tgt_size;

failed_exit:
    if (c_sl != OT_NULL) {
        ot_smr_munmap(c_sl, imgsz_sl);
    }
    if (c_sr != OT_NULL) {
        ot_smr_munmap(c_sr, imgsz_sr);
    }

    if (y_l != OT_NULL) {
        ot_smr_munmap(y_l, imgsz_l);
    }
    
    if (y_r != OT_NULL) {
        ot_smr_munmap(y_r, imgsz_r);
    }

    if (y_d != OT_NULL) {
        ot_smr_munmap(y_d, imgsz_d);
    }
    return ret;
}

static ot_s32 sample_uvc_depth_to_fnode(frame_node_t *fnode, ot_eis_img_frame *img_d)
{
    ot_s32 ret = OT_SUCCESS;

    ot_u32 tgt_size = img_d->attr.height * img_d->attr.width * 2;

    if (tgt_size > fnode->length) {
        return OT_FAILURE;
    }

    ot_u8 *dst = fnode->mem;

    ot_u16 *y_d = OT_NULL;
    ot_u32 imgsz_d = img_d->buff.stride[0] * img_d->attr.height;
    ret = ot_smr_mmap(img_d->buff.phys_addr[0], imgsz_d, OT_TRUE, (ot_void **)&y_d);
    if (ret != OT_SUCCESS || y_d == OT_NULL) {
        sample_print("mmap phys 0x%llx size %d error \n", img_d->buff.phys_addr[0], imgsz_d);
        goto failed_exit;
    }
    
    for (ot_u32 i = 0; i < img_d->attr.height; i++) {
        ot_u8  *d = dst + i * (img_d->attr.width) * 2;
        ot_u16 *m = (ot_u16 *)((ot_u8 *)y_d + i * img_d->buff.stride[0]);
#ifdef USE_DEPTH_LUT
        YUVColor *yuv = NULL;
        for (ot_u32 j = 0; j < img_d->attr.width / 2; j++) {
            yuv = depth_to_yuv((uint8_t)((*m >> 8) & 0xFF));
            *d++ = yuv->Y; // Y
            *d++ = yuv->U; // U
            m++;
            yuv = depth_to_yuv((uint8_t)((*m >> 8) & 0xFF));
            *d++ = yuv->Y; // Y
            *d++ = yuv->V; // V
            m++;
        }
#else
        for (ot_u32 j = 0; j < img_d->attr.width; j++) {
            *d++ = (*m >> 8) & 0xFF; // Y
            *d++ = (*m++ & 0xFF);    // UV
        }
#endif
    }

    fnode->used = tgt_size;
    
failed_exit:
    if (y_d != OT_NULL) {
        ot_smr_munmap(y_d, imgsz_d);
    }
    return ret;
}

static ot_s32 sample_uvc_stitch_dpu_match_to_cache(int dev_no, sample_dpu_cfg *dpu_cfg, ot_avp_dpu_match_src_frame_info *match_src, ot_avp_dpu_match_dst_frame_info *match_dst)
{
    // Attention, attention, attention: No time-consuming operations are allowed here, otherwise the image delay and stalling will occur.
    // Suggestion: Use ringBuffer to save the stream.  The user needs to copy the stream into the ringBuffer and then manage the ringbuffer
    ot_s32 ret = OT_SUCCESS;

    frame_node_t *fnode = OT_NULL;
    uvc_cache_t  *cache = uvc_cache_get(dev_no);
    if (cache != OT_NULL) {
        get_node_from_queue(cache->free_queue, &fnode);
    } else {
        printf("(%s %d) no uvc cache!!!\n", __FUNCTION__,__LINE__);
        return OT_SUCCESS;
    }

    if (fnode != OT_NULL) {
        ret = sample_uvc_stitch_dpu_match_to_fnode(fnode, dpu_cfg, match_src, match_dst);
        put_node_to_queue(cache->ok_queue, fnode);
    }

    return ret;
}


static ot_s32 sample_uvc_stitch_yuv420sp_to_cache(int dev_no, ot_eis_img_frame* img_l, ot_eis_img_frame *img_r)
{
    // Attention, attention, attention: No time-consuming operations are allowed here, otherwise the image delay and stalling will occur.
    // Suggestion: Use ringBuffer to save the stream.  The user needs to copy the stream into the ringBuffer and then manage the ringbuffer
    ot_s32 ret = OT_SUCCESS;

    frame_node_t *fnode = OT_NULL;
    uvc_cache_t  *cache = uvc_cache_get(dev_no);
    if (cache != OT_NULL) {
        get_node_from_queue(cache->free_queue, &fnode);
    } else {
        printf("(%s %d) no uvc cache!!!\n", __FUNCTION__,__LINE__);
        return OT_SUCCESS;
    }

    if (fnode != OT_NULL) {
        ret = sample_uvc_stitch_images_to_fnode(fnode, img_l, img_r);
        put_node_to_queue(cache->ok_queue, fnode);
    }

    return ret;
}

static ot_s32 sample_uvc_send_depth_to_cache(int dev_no, ot_eis_img_frame* img_d)
{
    // Attention, attention, attention: No time-consuming operations are allowed here, otherwise the image delay and stalling will occur.
    // Suggestion: Use ringBuffer to save the stream.  The user needs to copy the stream into the ringBuffer and then manage the ringbuffer
    ot_s32 ret = OT_SUCCESS;

    frame_node_t *fnode = OT_NULL;
    uvc_cache_t  *cache = uvc_cache_get(dev_no);
    if (cache != OT_NULL) {
        get_node_from_queue(cache->free_queue, &fnode);
    } else {
        printf("(%s %d) no uvc cache!!!\n", __FUNCTION__,__LINE__);
        return OT_SUCCESS;
    }

    if (fnode != OT_NULL) {
        sample_uvc_depth_to_fnode(fnode, img_d);
        put_node_to_queue(cache->ok_queue, fnode);
    }

    return ret;
}

static void * sample_uvc_dpu_proc(void *p)
{
    uvc_media_ctx    *ctx = (uvc_media_ctx *)p;
    encoder_property *ep  = &gs_encoder_prop[ctx->dev_no];

    ot_avp_dpu_match_src_frame_info match_src;
    ot_avp_dpu_match_dst_frame_info match_dst;

    while (ctx->dpu_run) {
        ot_s32 ret = ot_avp_dpu_match_get_frame(ctx->dpu_cfg.match_grp_hdl, SAMPLE_DPU_TIME_OUT, &match_src, &match_dst);

        if (ret == OT_SUCCESS) {
            if (ep->width == 1920) {
                sample_uvc_stitch_dpu_match_to_cache(ctx->dev_no, &ctx->dpu_cfg, &match_src, &match_dst);
            }
            else {
                sample_uvc_send_depth_to_cache(ctx->dev_no, &match_dst.depth_frame);
            }
            ot_avp_dpu_match_release_frame(ctx->dpu_cfg.match_grp_hdl, &match_src, &match_dst);
        }
    }

    ctx->dpu_run = OT_FALSE;

    return NULL;
}


static ot_s32 sample_dpu_start(uvc_media_ctx *ctx)
{
    ot_eis_img_size dpu_imgsz =  {SAMPLE_DPU_IN_WIDTH, SAMPLE_DPU_IN_HEIGHT};
    const char *lut_file[3] = {
        "lut-L.bin",
        "lut-R.bin",
        "bxf.bin",
    };

    ot_s32 ret = sample_dpu_rect_init(&ctx->dpu_cfg, &dpu_imgsz, &dpu_imgsz, lut_file, 2);
    if (ret != OT_SUCCESS) {
        sample_print("dpu rect init failed\n");
        return ret;
    }
    ret = sample_dpu_match_init(&ctx->dpu_cfg, &dpu_imgsz);
    if (ret != OT_SUCCESS) {
        sample_print("dpu match init failed\n");
        goto dpu_match_init_failed;
    }

    ret = sample_dpu_rect_bind_match(gs_media_pipe_hdl, ctx->dpu_cfg.rect_chn_hdl[0], ctx->dpu_cfg.match_chn_hdl[0]);
    if (ret != OT_SUCCESS) {
        sample_print("dpu rect->match bind failed\n");
        goto dpu_bind_failed;
    }

    return OT_SUCCESS;

dpu_bind_failed:
    sample_dpu_match_deinit(&ctx->dpu_cfg);
dpu_match_init_failed:
    sample_dpu_rect_deinit(&ctx->dpu_cfg);

    return ret;
}

static ot_void sample_dpu_stop(uvc_media_ctx *ctx)
{
    sample_dpu_rect_unbind_match(gs_media_pipe_hdl, ctx->dpu_cfg.rect_chn_hdl[0], ctx->dpu_cfg.match_chn_hdl[0]);
    sample_dpu_match_deinit(&ctx->dpu_cfg);
    sample_dpu_rect_deinit(&ctx->dpu_cfg);
}

static void * sample_uvc_get_frame_proc(void *p)
{
    ot_s32 ret = OT_SUCCESS;
    uvc_media_ctx *ctx = (uvc_media_ctx *)p;
    ot_eis_handle vp_chn_hdl[2] = {ctx->vp_cfg[0].chn_hdl[1], ctx->vp_cfg[1].chn_hdl[1]};
    ot_eis_img_frame img_l = {0}, img_r = {0};

    encoder_property *ep = &gs_encoder_prop[ctx->dev_no];

    prctl(PR_SET_NAME, "get-frame", 0, 0, 0);

    ot_u32 frame_cnt = 0;

    if (ep->width == 640 || ep->width == 1920) {
        ret = sample_dpu_start(ctx);
        if (ret == OT_SUCCESS) {  
            ctx->dpu_run = OT_TRUE;
            pthread_create(&ctx->dpu_tid, NULL, sample_uvc_dpu_proc, ctx);
        }
    }

    // clear buffer to sync
    while (ot_eis_vproc_chn_acquire_frame(vp_chn_hdl[0], &img_l, 0) == OT_SUCCESS) {
        ot_eis_vproc_chn_release_frame(vp_chn_hdl[0], &img_l);
    }
    while (ot_eis_vproc_chn_acquire_frame(vp_chn_hdl[1], &img_r, 0) == OT_SUCCESS) {
        ot_eis_vproc_chn_release_frame(vp_chn_hdl[1], &img_r);
    }

    while (ctx->get_frm_run == OT_TRUE) {
        ret = ot_eis_vproc_chn_acquire_frame(vp_chn_hdl[0], &img_l, 500);
        if (ret != OT_SUCCESS) {
            usleep(100 * 1000);
            sample_print("ot_eis_vproc_chn_acquire_frame from left sensor failed with %#x!\n", ret);
            continue;
        }

        ret = ot_eis_vproc_chn_acquire_frame(vp_chn_hdl[1], &img_r, 500);
        if (ret != OT_SUCCESS) {
            ot_eis_vproc_chn_release_frame(vp_chn_hdl[0], &img_l);
            sample_print("ot_eis_vproc_chn_acquire_frame from right sensor failed with %#x!\n", ret);
            continue;
        }

        if (ctx->dpu_run == OT_TRUE) {
            ot_avp_dpu_rect_send_frame(ctx->dpu_cfg.rect_grp_hdl, &img_l, &img_r, SAMPLE_DPU_TIME_OUT);
        }
        else {
            sample_uvc_stitch_yuv420sp_to_cache(ctx->dev_no, &img_l, &img_r);
        }
        
        ot_eis_vproc_chn_release_frame(vp_chn_hdl[0], &img_l);
        ot_eis_vproc_chn_release_frame(vp_chn_hdl[1], &img_r);
    }

    if (ctx->dpu_run == OT_TRUE) {
        ctx->dpu_run = OT_FALSE;
        pthread_join(ctx->dpu_tid, NULL);
        sample_dpu_stop(ctx);
    }

    ctx->get_frm_run = OT_FALSE;

    return OT_NULL;
}

ot_s32 sample_uvc_sc_init(ot_void)
{
    ot_s32 ret = OT_SUCCESS;
    ot_u32 max_h = 0, max_w = 0;
    ot_eis_vi_vproc_mode_type vi_vp_mode = OT_EIS_VI_OFFLINE_VPROC_OFFLINE;
    ot_eis_img_size img_size = { 0 };
    ot_video_buffer_attr vrb_cfg;

    memset(gs_encoder_prop, 0, sizeof(gs_encoder_prop));
    sample_uvc_get_default_media_cfg(gs_media_ctx, OT_UVC_MAX_CHN_NUM);

    for (ot_u32 i = 0; i < OT_UVC_MAX_CHN_NUM; i++) {
        uvc_media_ctx *ctx = gs_media_ctx + i;
        sample_comm_vi_get_size_by_sns_type(ctx->sns_type, &img_size);
        if (max_w < img_size.width) {
            max_w = img_size.width;
        }
        if (max_h < img_size.height) {
            max_h = img_size.height;
        }
    }
    
    sample_uvc_get_vrb_cfg(4, max_w, max_h, &vrb_cfg);
    ret = sample_comm_sys_init(&vrb_cfg);
    if (ret != OT_SUCCESS) {
        sample_print("sample_comm_sys_init failed.\n");
        return ret;
    }

    ret = sample_comm_sys_set_vi_vproc_init_cfg(vi_vp_mode);
    if (ret != OT_SUCCESS) {
        sample_print("set vi vp mode failed.\n");
        goto sys_exit;
    }

    ret = sample_comm_media_pipe_init(&gs_media_pipe_hdl);
    if (ret != OT_SUCCESS) {
        sample_print("media pipe init failed.\n");
        goto sys_exit;
    }

    ret = ot_buffer_pool_init();
    if (ret != OT_SUCCESS && ret != OT_ERR_BUFPOOL_ALREADY_INIT) {
        printf("buffer_pool init fail! ret[%d]\n", ret);
        goto bf_init_failed;
    }

    gs_is_sys_init = OT_TRUE;

    return OT_SUCCESS;

bf_init_failed:
    sample_comm_media_pipe_stop(gs_media_pipe_hdl);
sys_exit:
    sample_comm_sys_exit();

    return ret;
}

ot_s32 sample_uvc_sc_deinit(ot_void)
{
    if (gs_is_sys_init != OT_TRUE) {
        return OT_SUCCESS;
    }
    ot_buffer_pool_deinit();
    sample_comm_media_pipe_stop(gs_media_pipe_hdl);
    sample_comm_sys_exit();
    
    gs_is_sys_init = OT_FALSE;

    return OT_SUCCESS;
}

ot_s32 sample_uvc_sc_startup(ot_u32 dev_no)
{
    ot_s32 ret = OT_SUCCESS;
    uvc_media_ctx  *ctx = gs_media_ctx + dev_no;
    ot_s32 i_vi = 0, i_ub0 = 0, i_ub1 = 0, i_vp = 0, i_vi2vp = 0;

    ot_bool pipe_sw[OT_EIS_VPROC_GRP_PIPE_MAX_NUM] = {OT_TRUE, OT_FALSE, OT_FALSE, OT_FALSE};
    ot_bool chnl_sw[OT_EIS_VPROC_GRP_CHN_MAX_NUM]  = {OT_TRUE, OT_TRUE,  OT_FALSE, OT_FALSE};

    // scene auto handles
    ot_eis_handle vi_pipe_hdl[OT_EIS_VI_MAX_PIPE_NUM], vp_grp_hdl[OT_EIS_VI_MAX_PIPE_NUM];

    if (gs_is_sys_init != OT_TRUE) {
        return OT_FAILURE;
    }

    for (i_vi = 0; i_vi < ctx->sns_num; i_vi++) {
        sample_comm_vi_get_default_vi_cfg_by_dev_id(ctx->sns_type, &ctx->vi_cfg[i_vi], ctx->dev_id[i_vi]);
        for (int j = 0; j < 2; j++) {
            ctx->vi_cfg[i_vi].pipe_info[0].pipe_id[j] = 2 * i_vi + j;
        }
        ret = sample_comm_vi_start_vi(&ctx->vi_cfg[i_vi]);
        if (ret != OT_SUCCESS) {
            sample_print("sample_comm_vi_start_vi failed, ret:0x%x\n", ret);
            goto start_vi_failed;
        }
        ctx->vi_cfg[i_vi].media_pipe_hdl = gs_media_pipe_hdl;
    }

    for (ot_s32 i = 0; i < ctx->sns_num; i++) {
        ot_eis_img_size img_size = { 0 };
        sample_comm_vi_get_size_by_sns_type(ctx->sns_type, &img_size);
        sample_media_vproc_get_default_attr_by_snsor(ctx->sns_type, &ctx->vp_cfg[i]);
        ctx->vp_cfg[i].chn_attr[0].mode                 = OT_EIS_VPROC_WORK_MODE_USER;
        ctx->vp_cfg[i].chn_attr[0].frame_queue_depth    = VPROC_FRAME_QUEUE_DEPTH;
        ctx->vp_cfg[i].chn_attr[0].image_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
        ctx->vp_cfg[i].chn_attr[0].image_attr.width     = 960;
        ctx->vp_cfg[i].chn_attr[0].image_attr.height    = 1280;
        ctx->vp_cfg[i].chn_attr[1].image_attr.width     = SAMPLE_DPU_IMG_H;
        ctx->vp_cfg[i].chn_attr[1].image_attr.height    = SAMPLE_DPU_IMG_W;
    }

    for (i_ub0 = 0; i_ub0 < ctx->sns_num; i_ub0++) {
        ot_eis_handle user_pool_hdl = OT_NULL;
        ret = sample_uvc_user_mode_buf_pool_creat(&ctx->vp_cfg[i_ub0].chn_attr[0].image_attr, &user_pool_hdl);
        if (ret != OT_SUCCESS) {
            sample_print("create user buffer pool failed, ret:0x%x\n", ret);
            goto create_ub0_failed;
        }
        ctx->vp_cfg[i_ub0].chn_attr[0].pool_handle = user_pool_hdl;
    }

    for (i_ub1 = 0; i_ub1 < ctx->sns_num; i_ub1++) {
        ot_eis_handle user_pool_hdl = OT_NULL;
        ret = sample_uvc_user_mode_buf_pool_creat(&ctx->vp_cfg[i_ub1].chn_attr[1].image_attr, &user_pool_hdl);
        if (ret != OT_SUCCESS) {
            sample_print("create user buffer pool failed, ret:0x%x\n", ret);
            goto create_ub1_failed;
        }
        ctx->vp_cfg[i_ub1].chn_attr[1].pool_handle = user_pool_hdl;
    }

    for (i_vp = 0; i_vp < ctx->sns_num; i_vp++) {
        ot_eis_vproc_chn_rotation ro_param = { 0 };

        ret = sample_comm_start_vproc(&ctx->vp_cfg[i_vp], pipe_sw, chnl_sw);
        if (ret != OT_SUCCESS) {
            sample_print("sample_comm_start_vproc[grp%d] failed, ret:0x%x\n", i_vp, ret);
            goto start_vp_failed;
        }
        
        memset(&ctx->vp_cfg[i_vp].set_attr, 0, sizeof(sample_vproc_set_attr));
        ctx->vp_cfg[i_vp].is_set_attr = OT_TRUE;
        ctx->vp_cfg[i_vp].set_attr.crop_param.enable    = OT_TRUE;
        ctx->vp_cfg[i_vp].set_attr.crop_param.crop_type = OT_EIS_COORD_ABS;
        ctx->vp_cfg[i_vp].set_attr.crop_param.crop_rect.x = 60;
        ctx->vp_cfg[i_vp].set_attr.crop_param.crop_rect.y = 0;
        ctx->vp_cfg[i_vp].set_attr.crop_param.crop_rect.width = 960;
        ctx->vp_cfg[i_vp].set_attr.crop_param.crop_rect.height = 1280;
        ot_eis_vproc_grp_set_crop(ctx->vp_cfg[i_vp].grp_hdl, &ctx->vp_cfg[i_vp].set_attr.crop_param);

        ro_param.enable = OT_TRUE;
        ro_param.angle  = OT_EIS_RTT_270;
        ot_eis_vproc_chn_set_rotation(ctx->vp_cfg[i_vp].chn_hdl[0], &ro_param);
        ot_eis_vproc_chn_set_rotation(ctx->vp_cfg[i_vp].chn_hdl[1], &ro_param);
    }

    for (i_vi2vp = 0; i_vi2vp < ctx->sns_num; i_vi2vp++) {
        ret = sample_comm_vi_bind_vproc(ctx->vi_cfg[i_vi2vp].pipe_info[0].chn_info.chn_hdl, ctx->vp_cfg[i_vi2vp].pipe_hdl[0], gs_media_pipe_hdl);
        if (ret != OT_SUCCESS) {
            sample_print("vi bind vp failed, ret:0x%x\n", ret);
            goto vi_bind_vp_failed;
        }
    }
    for (ot_u32 i = 0; i < ctx->sns_num; i++) {
        vi_pipe_hdl[i] = ctx->vi_cfg[i].pipe_info[0].pipe_hdl;
        vp_grp_hdl[i]  = ctx->vp_cfg[i].grp_hdl;
    }
    ret = sample_uvc_scene_auto_start("./param/sc132gs", vi_pipe_hdl, vp_grp_hdl, ctx->sns_num);
    if (ret != OT_SUCCESS) {
        sample_print("scene auto start failed, ret:0x%x\n", ret);
        goto scene_auto_start_failed;
    }

    ctx->get_frm_run = OT_TRUE;
    ret = pthread_create(&ctx->get_frm_tid, NULL, sample_uvc_get_frame_proc, (void *)ctx);
    if (ret != 0) {
        sample_print("start stereo proc failed, ret:0x%x\n", ret);
        ctx->get_frm_run = OT_FALSE;
        goto start_stereo_proc_failed;
    }

    ctx->is_stream_run = OT_TRUE;

    return OT_SUCCESS;

start_stereo_proc_failed:
    sample_uvc_scene_auto_stop();

scene_auto_start_failed:
vi_bind_vp_failed:
    for (ot_s32 i = 0; i < i_vi2vp; i++) {
        sample_comm_vi_un_bind_vproc(ctx->vi_cfg[i].pipe_info[0].chn_info.chn_hdl, ctx->vp_cfg[i].pipe_hdl[0], gs_media_pipe_hdl);
    }
start_vp_failed:
    for (ot_s32 i = 0; i < i_vi; i++) {
        sample_comm_stop_vproc(&ctx->vp_cfg[i]);
    }
create_ub1_failed:
    for (ot_s32 i = 0; i < i_ub1; i++) {
        ot_buffer_pool_destroy(ctx->vp_cfg[i].chn_attr[1].pool_handle);
    }
create_ub0_failed:
    for (ot_s32 i = 0; i < i_ub0; i++) {
        ot_buffer_pool_destroy(ctx->vp_cfg[i].chn_attr[0].pool_handle);
    }
start_vi_failed:
    for (ot_s32 i = 0; i < i_vi; i++) {
        sample_comm_vi_stop_vi(&ctx->vi_cfg[i]);
    }

    return OT_SUCCESS;
}

ot_s32 sample_uvc_sc_shutdown(ot_u32 dev_no)
{
    uvc_media_ctx  *ctx = gs_media_ctx + dev_no;
    if (ctx->is_stream_run != OT_TRUE) {
        return OT_SUCCESS;
    }

    if (ctx->get_frm_run == OT_TRUE) {
        ctx->get_frm_run = OT_FALSE;
        pthread_join(ctx->get_frm_tid, NULL);
    }
    
    sample_uvc_scene_auto_stop();

    for (ot_s32 i = 0; i < ctx->sns_num; i++) {
        sample_comm_vi_un_bind_vproc(ctx->vi_cfg[i].pipe_info[0].chn_info.chn_hdl, ctx->vp_cfg[i].pipe_hdl[0], gs_media_pipe_hdl);
    }
    for (ot_s32 i = 0; i < ctx->sns_num; i++) {
        sample_comm_stop_vproc(&ctx->vp_cfg[i]);
        ot_buffer_pool_destroy(ctx->vp_cfg[i].chn_attr[0].pool_handle);
        ot_buffer_pool_destroy(ctx->vp_cfg[i].chn_attr[1].pool_handle);
    }
    
    for (ot_s32 i = 0; i < ctx->sns_num; i++) {
        sample_comm_vi_stop_vi(&ctx->vi_cfg[i]);
    }

    ctx->is_stream_run = OT_FALSE;

    return OT_SUCCESS;
}

ot_s32 sample_uvc_sc_set_property(ot_u32 dev_no, encoder_property *ep)
{
    if (ep == NULL) {
        err("p is NULL\n");
        return OT_FAILURE;
    }
    (ot_void)memcpy_s(&gs_encoder_prop[dev_no], sizeof(encoder_property), ep, sizeof(encoder_property));
    return 0;
}

ot_void sample_uvc_sc_get_property(ot_u32 dev_no, encoder_property *ep)
{
    if (ep == NULL) {
        err("ep is NULL\n");
        return;
    }
    (ot_void)memcpy_s(ep, sizeof(encoder_property), &gs_encoder_prop[dev_no], sizeof(encoder_property));
}

ot_s32 sample_uvc_sc_set_idr(ot_u32 dev_no)
{
    return OT_SUCCESS;
}

ot_s32 sample_uvc_sc_get_send(ot_u32 dev_no)
{    
    return OT_SUCCESS;
}
