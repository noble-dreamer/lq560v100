/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stereo_venc.h"
#include "ot_eis_venc.h"
#include "ot_smr.h"
#include "ot_buffer_pool.h"
#include "sample_comm.h"
#include "stereo_depth_lut.h"
#include "stereo_lz4.h"

/* -------------------------------------------------------------------------- */
/* VENC channel descriptors                                                    */
/* -------------------------------------------------------------------------- */

typedef struct {
    ot_eis_venc_chn_handle  hdl;
    ot_eis_venc_chn_attr    attr;
    ot_u32                  width;
    ot_u32                  height;
} stereo_venc_chn_t;

static stereo_venc_chn_t   g_venc_ch[STEREO_VENC_CH_NUM];
static ot_bool             g_venc_inited = OT_FALSE;
static ot_bool             g_venc_module_inited = OT_FALSE;
/* Number of channels successfully created (for precise error cleanup) */
static ot_s32              g_venc_created_cnt = 0;
/* Number of channels successfully started (for precise error cleanup) */
static ot_s32              g_venc_started_cnt = 0;

/* Disparity YVU420SP frame buffer pool — ring-buffer approach.
   VENC validates that the input frame memory belongs to a registered buffer
   pool. Furthermore, VENC may hold an internal reference to a submitted blk
   beyond release_stream, so a per-frame acquire/release pattern causes frame 1+
   to fail with ILLEGAL_PARAM. Solution: pre-allocate N blks at init, rotate
   through them (triple-buffer), and release all only at deinit. */
#define STEREO_DISP_BLK_CNT   3

static ot_eis_handle       g_disp_pool       = OT_NULL;
static ot_buffer_pool_blk  g_disp_blks[STEREO_DISP_BLK_CNT];
static ot_u8              *g_disp_virts[STEREO_DISP_BLK_CNT];
static ot_phys_addr        g_disp_phys[STEREO_DISP_BLK_CNT];
static ot_u32              g_disp_blk_idx    = 0;  /* current ring index */
static ot_u32              g_disp_frame_seq  = 0;  /* frame sequence for pts/index */
static ot_u32              g_disp_yuv_size   = 0;  /* used image size (Y+VU) */
static ot_u32              g_disp_block_size = 0;  /* pool blk size for mmap/munmap */
static ot_u32              g_disp_y_size     = 0;  /* luma plane size (stride*H) */
static ot_u32              g_disp_stride     = 0;  /* aligned main stride */

/* LZ4 compression working memory (allocated once, reused every frame) */
static void              *g_lz4_wrkmem      = NULL;

/* -------------------------------------------------------------------------- */
/* Fill one JPEG channel attribute. The mjpeg_attr union member is initialized */
/* with a sane CQP qfactor so the attr is fully valid for the encoder.         */
/* -------------------------------------------------------------------------- */
static void stereo_venc_fill_jpeg_attr(ot_eis_venc_chn_attr *attr,
                                        ot_u32 width, ot_u32 height)
{
    memset(attr, 0, sizeof(*attr));
    attr->codecFormat = OT_EIS_VENC_CODEC_JPEG;
    attr->common_attr.max_pic_width   = width;
    attr->common_attr.max_pic_height  = height;
    attr->common_attr.out_width       = width;
    attr->common_attr.out_height      = height;
    attr->common_attr.stream_buf_size = width * height * 2;

    /* JPEG uses the mjpeg_attr union member; CQP with a fixed qfactor. */
    attr->mjpeg_attr.rc_attr.output_fps = 20;
    attr->mjpeg_attr.rc_attr.input_fps  = 20;
    attr->mjpeg_attr.rc_attr.rc_mode    = OT_EIS_VENC_RC_MODE_CQP;
    attr->mjpeg_attr.rc_attr.cqp_attr.qfactor = 75;
}

/* -------------------------------------------------------------------------- */

ot_s32 stereo_venc_init(void)
{
    ot_s32 ret;
    ot_s32 i;

    memset(g_venc_ch, 0, sizeof(g_venc_ch));
    g_venc_created_cnt = 0;
    g_venc_started_cnt = 0;

    /* Initialize the VENC module before creating channels.
       If a previous run crashed without calling ot_eis_venc_exit(), the
       kernel-side VENC state lingers and ot_eis_venc_init() returns
       OT_EIS_ERR_VENC_NOT_PERM (0x80080054). Recover by exiting the stale
       module once and retrying init. */
    ret = ot_eis_venc_init();
    if (ret == OT_EIS_ERR_VENC_NOT_PERM) {
        stereo_log_write("[stereo_venc] venc already inited (stale state 0x%x), "
                         "recovering: exit + re-init\n", ret);
        ot_eis_venc_exit();
        ret = ot_eis_venc_init();
    }
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_venc] ot_eis_venc_init failed, ret:0x%x\n", ret);
        return ret;
    }
    g_venc_module_inited = OT_TRUE;

    /* Channel geometry: ch0/ch1 full-res left/right only.
       Disparity channel (ch2) is disabled — was causing buffer pool contention
       (0x80080000) and frame drops. */
    g_venc_ch[STEREO_VENC_CH_LEFT].width   = STEREO_SENSOR_WIDTH;
    g_venc_ch[STEREO_VENC_CH_LEFT].height  = STEREO_SENSOR_HEIGHT;
    g_venc_ch[STEREO_VENC_CH_RIGHT].width  = STEREO_SENSOR_WIDTH;
    g_venc_ch[STEREO_VENC_CH_RIGHT].height = STEREO_SENSOR_HEIGHT;

    /* Create L+R channels only (skip disparity ch2).
       If a previous process was killed (e.g. kill -9) without calling
       stereo_venc_deinit(), the kernel-side VENC channels linger and
       ot_eis_venc_chn_create() returns OT_EIS_ERR_VENC_EXIST (0x80080052).
       Recover by exiting the entire VENC module (which destroys all channels)
       and retrying init + channel creation. */
    for (i = 0; i < 2; i++) {
        stereo_venc_fill_jpeg_attr(&g_venc_ch[i].attr,
                                    g_venc_ch[i].width, g_venc_ch[i].height);

        ret = ot_eis_venc_chn_create(&g_venc_ch[i].hdl, &g_venc_ch[i].attr);
        if (ret != OT_SUCCESS && i == 0) {
            stereo_log_write("[stereo_venc] create chn[%d] failed (0x%x), recovering: "
                             "venc_exit + venc_init + retry\n", i, ret);
            ot_eis_venc_exit();
            g_venc_module_inited = OT_FALSE;
            ret = ot_eis_venc_init();
            if (ret != OT_SUCCESS) {
                stereo_log_write("[stereo_venc] venc_init retry failed, ret:0x%x\n", ret);
                return ret;
            }
            g_venc_module_inited = OT_TRUE;
            ret = ot_eis_venc_chn_create(&g_venc_ch[i].hdl, &g_venc_ch[i].attr);
        }
        if (ret != OT_SUCCESS) {
            stereo_log_write("[stereo_venc] create chn[%d] %ux%u failed, ret:0x%x\n",
                             i, g_venc_ch[i].width, g_venc_ch[i].height, ret);
            goto err_cleanup;
        }
        g_venc_created_cnt = i + 1;
        stereo_log_write("[stereo_venc] create chn[%d] ok, hdl=0x%llx (%ux%u)\n",
                         i, (ot_u64)g_venc_ch[i].hdl, g_venc_ch[i].width, g_venc_ch[i].height);
    }

    /* Start L+R channels */
    for (i = 0; i < 2; i++) {
        ret = ot_eis_venc_chn_start(g_venc_ch[i].hdl);
        if (ret != OT_SUCCESS) {
            stereo_log_write("[stereo_venc] start chn[%d] failed, ret:0x%x\n", i, ret);
            goto err_cleanup;
        }
        g_venc_started_cnt = i + 1;
        stereo_log_write("[stereo_venc] start chn[%d] ok\n", i);
    }

    g_venc_inited = OT_TRUE;

    /* Allocate LZ4 working memory (16KB for HASH_SIZE_U32 * 4 bytes) */
    g_lz4_wrkmem = malloc(LZ4_MEM_COMPRESS);
    if (!g_lz4_wrkmem) {
        stereo_log_write("[stereo_venc] WARNING: LZ4 wrkmem alloc failed, disparity will be raw\n");
    } else {
        stereo_log_write("[stereo_venc] LZ4 wrkmem allocated (%d bytes)\n", LZ4_MEM_COMPRESS);
    }

    stereo_log_write("[stereo_venc] init done: ch0=%ux%u, ch1=%ux%u (disparity ch disabled)\n",
                     g_venc_ch[STEREO_VENC_CH_LEFT].width,
                     g_venc_ch[STEREO_VENC_CH_LEFT].height,
                     g_venc_ch[STEREO_VENC_CH_RIGHT].width,
                     g_venc_ch[STEREO_VENC_CH_RIGHT].height);
    return OT_SUCCESS;

err_cleanup:
    for (ot_s32 k = 0; k < STEREO_DISP_BLK_CNT; k++) {
        if (g_disp_virts[k]) {
            ot_smr_munmap(g_disp_virts[k], g_disp_block_size);
            g_disp_virts[k] = OT_NULL;
        }
        if (g_disp_blks[k].phys_addr) {
            ot_buffer_pool_blk_release(g_disp_pool, &g_disp_blks[k]);
            memset(&g_disp_blks[k], 0, sizeof(g_disp_blks[k]));
        }
    }
    if (g_disp_pool) {
        ot_buffer_pool_destroy(g_disp_pool);
        g_disp_pool = OT_NULL;
    }
    /* Stop only channels that were actually started */
    for (ot_s32 j = 0; j < g_venc_started_cnt; j++) {
        ot_eis_venc_chn_stop(g_venc_ch[j].hdl);
    }
    /* Destroy only channels that were actually created */
    for (ot_s32 j = 0; j < g_venc_created_cnt; j++) {
        ot_eis_venc_chn_destroy(g_venc_ch[j].hdl);
    }
    g_venc_created_cnt = 0;
    g_venc_started_cnt = 0;
    if (g_venc_module_inited == OT_TRUE) {
        ot_eis_venc_exit();
        g_venc_module_inited = OT_FALSE;
    }
    return ret;
}

void stereo_venc_deinit(void)
{
    if (g_venc_inited != OT_TRUE) {
        /* Even if full init did not complete, release the module if it was
           initialized (defensive; normally err_cleanup already handled it). */
        if (g_venc_module_inited == OT_TRUE) {
            ot_eis_venc_exit();
            g_venc_module_inited = OT_FALSE;
        }
        return;
    }

    for (ot_s32 i = 0; i < STEREO_VENC_CH_NUM; i++) {
        ot_eis_venc_chn_stop(g_venc_ch[i].hdl);
        ot_eis_venc_chn_destroy(g_venc_ch[i].hdl);
    }

    for (ot_s32 k = 0; k < STEREO_DISP_BLK_CNT; k++) {
        if (g_disp_virts[k]) {
            ot_smr_munmap(g_disp_virts[k], g_disp_block_size);
            g_disp_virts[k] = OT_NULL;
        }
        if (g_disp_blks[k].phys_addr) {
            ot_buffer_pool_blk_release(g_disp_pool, &g_disp_blks[k]);
            memset(&g_disp_blks[k], 0, sizeof(g_disp_blks[k]));
        }
    }
    if (g_disp_pool) {
        ot_buffer_pool_destroy(g_disp_pool);
        g_disp_pool = OT_NULL;
    }

    if (g_venc_module_inited == OT_TRUE) {
        ot_eis_venc_exit();
        g_venc_module_inited = OT_FALSE;
    }

    if (g_lz4_wrkmem) {
        free(g_lz4_wrkmem);
        g_lz4_wrkmem = NULL;
    }

    g_venc_inited = OT_FALSE;
    stereo_log_write("[stereo_venc] deinit done\n");
}

/* -------------------------------------------------------------------------- */
/* Encode a single frame to JPEG. Encoded stream is copied to output packet.   */
/* -------------------------------------------------------------------------- */

static ot_s32 stereo_venc_encode_one(ot_eis_venc_chn_handle hdl,
                                      const ot_eis_img_frame *frame,
                                      stereo_jpeg_packet_t *pkt)
{
    ot_s32 ret;
    ot_eis_venc_image_frame venc_frame;
    ot_eis_venc_stream stream;
    ot_s32 retry = 0;
    ot_s32 send_retry = 0;

    memset(&venc_frame, 0, sizeof(venc_frame));
    memset(&stream, 0, sizeof(stream));

    venc_frame.frame = (ot_eis_img_frame *)frame;

    /* VENC may need an internal cleanup cycle after the previous encode on this
       channel completes (observed as alternating success/fail pattern). Retry
       with a short delay if send_frame returns ILLEGAL_PARAM. */
    for (send_retry = 0; send_retry < 10; send_retry++) {
        ret = ot_eis_venc_send_frame(hdl, &venc_frame, 1000);
        if (ret == OT_SUCCESS) {
            break;
        }
        if (ret != (ot_s32)0x80080000) {
            /* Non-retryable error */
            stereo_log_write("[stereo_venc] send_frame failed, ret:0x%x\n", ret);
            return ret;
        }
        usleep(5000); /* 5ms wait for VENC internal release */
    }
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_venc] send_frame failed after %d retries, ret:0x%x\n",
                         send_retry, ret);
        return ret;
    }

    /* Poll for encoded stream */
    while (retry < 100) {
        ot_eis_venc_chn_status status;
        ot_eis_venc_chn_query_status(hdl, &status);
        if (status.unTaken_streams > 0) {
            break;
        }
        usleep(1000); /* 1ms */
        retry++;
    }

    if (retry >= 100) {
        stereo_log_write("[stereo_venc] acquire stream timeout\n");
        return OT_FAILURE;
    }

    ret = ot_eis_venc_chn_acquire_stream(hdl, &stream, 500);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_venc] acquire_stream failed, ret:0x%x\n", ret);
        return ret;
    }

    /* Copy encoded JPEG data to output packet */
    pkt->size = stream.stream_lens;
    pkt->data = (ot_u8 *)malloc(pkt->size);
    if (pkt->data) {
        memcpy(pkt->data, stream.stream_virt_addr, pkt->size);
    } else {
        pkt->size = 0;
    }

    ot_eis_venc_chn_release_stream(hdl, &stream);

    return (pkt->data && pkt->size > 0) ? OT_SUCCESS : OT_FAILURE;
}

/* -------------------------------------------------------------------------- */
/* Pack disparity uint8 data into YVU420SP with a pseudo-color colormap.        */
/* Both luma and chroma come from the disparity->YUV LUT (jet-style: near=red,   */
/* far=blue, invalid 0=black), so the color is faithful. YVU420SP stores chroma */
/* interleaved as V,U and every 2x2 luma block shares one (V,U) pair; the        */
/* block's top-left disparity is used to pick the shared color.                 */
/* -------------------------------------------------------------------------- */

static void stereo_pack_disparity_yuv(const ot_u8 *disparity,
                                       ot_eis_img_frame *out_frame)
{
    ot_u32 w = STEREO_NPU_OUTPUT_W;
    ot_u32 h = STEREO_NPU_OUTPUT_H;
    ot_u32 stride = g_disp_stride;
    ot_u32 row, col;
    ot_u32 idx;
    ot_u8 *base;
    ot_u8 *vu;

    memset(out_frame, 0, sizeof(*out_frame));

    if (!g_disp_pool) {
        return;
    }

    /* Pick the next pre-allocated blk from the ring (triple-buffer). */
    idx = g_disp_blk_idx;
    g_disp_blk_idx = (g_disp_blk_idx + 1) % STEREO_DISP_BLK_CNT;
    base = g_disp_virts[idx];

    memset(base, 0, g_disp_yuv_size);

    /* Y plane: per-pixel luma from the LUT, written stride-aligned. */
    for (row = 0; row < h; row++) {
        ot_u8 *line = base + row * stride;
        for (col = 0; col < w; col++) {
            ot_u8 d = disparity[row * w + col];
            line[col] = stereo_depth_to_yuv(d)->y;
        }
    }

    /* VU plane: one (V,U) per 2x2 luma block, V first then U (NV21 order). */
    vu = base + g_disp_y_size;
    for (row = 0; row < h / 2; row++) {
        ot_u8 *line = vu + row * stride;
        for (col = 0; col < w / 2; col++) {
            ot_u8 d = disparity[(row * 2) * w + (col * 2)];
            const stereo_yuv_color *c = stereo_depth_to_yuv(d);
            line[col * 2]     = c->v;
            line[col * 2 + 1] = c->u;
        }
    }

    /* Flush CPU cache so VENC DMA sees the data */
    ot_smr_flush_cache(g_disp_phys[idx], base, g_disp_yuv_size);

    /* Frame sequence and timestamp — VENC requires monotonically increasing PTS
       per channel, otherwise it rejects subsequent frames as ILLEGAL_PARAM. */
    out_frame->index = g_disp_frame_seq;
    out_frame->pts   = (ot_u64)g_disp_frame_seq * 50000;  /* ~20fps interval in us */
    g_disp_frame_seq++;

    out_frame->attr.width      = STEREO_NPU_OUTPUT_W;
    out_frame->attr.height     = STEREO_NPU_OUTPUT_H;
    out_frame->attr.pixel_fmt  = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
    out_frame->attr.bit_depth  = OT_EIS_PIXEL_BIT_DEPTH_8;
    out_frame->attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    out_frame->attr.layout     = OT_EIS_IMAGE_LAYOUT_LINEAR;
    out_frame->buff.virt_addr[0]  = base;
    out_frame->buff.virt_addr[1]  = base + g_disp_y_size;
    out_frame->buff.phys_addr[0]  = (ot_u64)g_disp_phys[idx];
    out_frame->buff.phys_addr[1]  = (ot_u64)g_disp_phys[idx] + g_disp_y_size;
    out_frame->buff.stride[0]     = stride;
    out_frame->buff.stride[1]     = stride;
    out_frame->pool_hdl    = g_disp_pool;
    out_frame->mod_id      = OT_EIS_MODULE_VPROC;
    out_frame->color_space = OT_EIS_IMAGE_COLOR_SPACE_BT601;

    stereo_log_write("[stereo_venc] disp pack: seq=%u blk=%u phys=0x%llx pool=0x%llx\n",
                     g_disp_frame_seq - 1, idx, (ot_u64)g_disp_phys[idx], (ot_u64)g_disp_pool);
}

/* -------------------------------------------------------------------------- */

ot_s32 stereo_venc_encode(const ot_eis_img_frame *left_frame,
                           const ot_eis_img_frame *right_frame,
                           const ot_u8 *disparity_data,
                           ot_u32 disparity_bytes,
                           stereo_venc_output_t *output)
{
    ot_s32 ret;

    if (!left_frame || !right_frame || !output) {
        return OT_FAILURE;
    }

    /* NOTE: caller already zeroed output via memset; do NOT re-zero here
       as it would destroy frame_index/timestamp_us set by caller. */

    /* --- Encode left --- */
    ret = stereo_venc_encode_one(g_venc_ch[STEREO_VENC_CH_LEFT].hdl,
                                   left_frame,
                                   &output->packets[STEREO_VENC_CH_LEFT]);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_venc] encode left failed\n");
        goto cleanup;
    }
    output->packets[STEREO_VENC_CH_LEFT].type = STEREO_FRAME_TYPE_LEFT;

    /* --- Encode right --- */
    ret = stereo_venc_encode_one(g_venc_ch[STEREO_VENC_CH_RIGHT].hdl,
                                   right_frame,
                                   &output->packets[STEREO_VENC_CH_RIGHT]);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_venc] encode right failed\n");
        goto cleanup;
    }
    output->packets[STEREO_VENC_CH_RIGHT].type = STEREO_FRAME_TYPE_RIGHT;

    /* --- Disparity: LZ4 compress uint16 sub-pixel (640x448x2 bytes) --- */
    {
        ot_u32 disp_bytes = disparity_bytes;
        ot_u32 max_compressed = LZ4_compressBound(disp_bytes);

        if (g_lz4_wrkmem && disparity_data && max_compressed > 0) {
            ot_u8 *comp_buf = (ot_u8 *)malloc(max_compressed);
            if (comp_buf) {
                int comp_size = LZ4_compress_default(
                    (const char *)disparity_data,
                    (char *)comp_buf,
                    (int)disp_bytes,
                    (int)max_compressed,
                    g_lz4_wrkmem);

                if (comp_size > 0 && (ot_u32)comp_size < disp_bytes) {
                    /* Compression succeeded and is smaller than raw */
                    output->packets[STEREO_VENC_CH_DISPARITY].data = comp_buf;
                    output->packets[STEREO_VENC_CH_DISPARITY].size = comp_size;
                    output->packets[STEREO_VENC_CH_DISPARITY].type = STEREO_FRAME_TYPE_DISPARITY_LZ4;
                } else {
                    /* Compression not beneficial (incompressible data) — send raw */
                    free(comp_buf);
                    output->packets[STEREO_VENC_CH_DISPARITY].data =
                        (ot_u8 *)malloc(disp_bytes);
                    if (output->packets[STEREO_VENC_CH_DISPARITY].data) {
                        memcpy(output->packets[STEREO_VENC_CH_DISPARITY].data,
                               disparity_data, disp_bytes);
                        output->packets[STEREO_VENC_CH_DISPARITY].size = disp_bytes;
                        output->packets[STEREO_VENC_CH_DISPARITY].type = STEREO_FRAME_TYPE_DISPARITY;
                    }

                }
            } else {
                /* malloc failure — fallback to raw */
                output->packets[STEREO_VENC_CH_DISPARITY].data =
                    (ot_u8 *)malloc(disp_bytes);
                if (output->packets[STEREO_VENC_CH_DISPARITY].data) {
                    memcpy(output->packets[STEREO_VENC_CH_DISPARITY].data,
                           disparity_data, disp_bytes);
                    output->packets[STEREO_VENC_CH_DISPARITY].size = disp_bytes;
                    output->packets[STEREO_VENC_CH_DISPARITY].type = STEREO_FRAME_TYPE_DISPARITY;
                }
            }
        } else {
            /* No LZ4 wrkmem or no disparity data — send raw */
            output->packets[STEREO_VENC_CH_DISPARITY].data =
                (ot_u8 *)malloc(disp_bytes);
            if (output->packets[STEREO_VENC_CH_DISPARITY].data && disparity_data) {
                memcpy(output->packets[STEREO_VENC_CH_DISPARITY].data,
                       disparity_data, disp_bytes);
                output->packets[STEREO_VENC_CH_DISPARITY].size = disp_bytes;
            } else {
                output->packets[STEREO_VENC_CH_DISPARITY].size = 0;
            }
            output->packets[STEREO_VENC_CH_DISPARITY].type = STEREO_FRAME_TYPE_DISPARITY;
        }
    }

    /* Set timestamps and frame index on all packets */
    for (ot_s32 i = 0; i < STEREO_VENC_CH_NUM; i++) {
        output->packets[i].frame_index  = output->frame_index;
        output->packets[i].timestamp_us = output->timestamp_us;
    }
    output->packet_count = STEREO_VENC_CH_NUM;

    return OT_SUCCESS;

cleanup:
    /* Free any partially allocated data */
    for (ot_s32 i = 0; i < STEREO_VENC_CH_NUM; i++) {
        if (output->packets[i].data) {
            free(output->packets[i].data);
            output->packets[i].data = OT_NULL;
        }
    }
    return ret;
}
