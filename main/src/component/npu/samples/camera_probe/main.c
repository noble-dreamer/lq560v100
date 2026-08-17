/*
 * camera_probe: 验证 sc132gs 双目媒体管线（不创建 UVC gadget）与检测通道参数。
 */
#include "sample_comm.h"
#include <stdio.h>
#include <unistd.h>

#define PROBE_DET_W     416
#define PROBE_DET_H     312
#define PROBE_DET_CHN   2
#define PROBE_PIPE_NUM  4

static ot_bool        gs_sys_init = OT_FALSE;
static ot_eis_handle  gs_media_pipe_hdl = OT_NULL;

/* 与 uvc_app 一致的双缓冲块 vrb 配置。 */
static ot_s32 probe_get_vrb_cfg(ot_u32 pipe_num, ot_u32 width, ot_u32 height,
                                ot_video_buffer_attr *buffer_attr)
{
    ot_eis_buf_size_calc_cfg calc_cfg;
    ot_eis_img_attr img_attr;

    if (buffer_attr == OT_NULL) {
        return -1;
    }

    buffer_attr->cnt = 2;

    img_attr.width = width;
    img_attr.height = height;
    img_attr.bit_depth = OT_EIS_PIXEL_BIT_DEPTH_8;
    img_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_BAYER_12BPP;
    img_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    img_attr.layout = OT_EIS_IMAGE_LAYOUT_LINEAR;
    sample_common_get_pic_buf_cfg(&img_attr, &calc_cfg);
    buffer_attr->buf_blks[0].size = calc_cfg.blk_size;
    buffer_attr->buf_blks[0].cnt = pipe_num * 3;
    strcpy(buffer_attr->buf_blks[0].region_name, "anony");

    img_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
    sample_common_get_pic_buf_cfg(&img_attr, &calc_cfg);
    buffer_attr->buf_blks[1].size = calc_cfg.blk_size;
    buffer_attr->buf_blks[1].cnt = pipe_num * 3;
    strcpy(buffer_attr->buf_blks[1].region_name, "anony");

    return 0;
}

static ot_s32 probe_sys_init(void)
{
    ot_s32 ret;
    ot_u32 max_w = 0;
    ot_u32 max_h = 0;
    ot_eis_img_size img_size = {0};
    ot_video_buffer_attr vrb_cfg;

    sample_comm_vi_get_size_by_sns_type(SMS_SC132GS_MIPI_1_5M_30FPS_RAW12_DIRECT_SLAVE,
                                        &img_size);
    max_w = img_size.width;
    max_h = img_size.height;

    probe_get_vrb_cfg(PROBE_PIPE_NUM, max_w, max_h, &vrb_cfg);
    ret = sample_comm_sys_init(&vrb_cfg);
    if (ret != OT_SUCCESS) {
        printf("[probe] sample_comm_sys_init fail, ret=%d\n", ret);
        return ret;
    }

    ret = sample_comm_sys_set_vi_vproc_init_cfg(OT_EIS_VI_OFFLINE_VPROC_OFFLINE);
    if (ret != OT_SUCCESS) {
        printf("[probe] set vi/vproc offline fail, ret=%d\n", ret);
        sample_comm_sys_exit();
        return ret;
    }

    ret = sample_comm_media_pipe_init(&gs_media_pipe_hdl);
    if (ret != OT_SUCCESS) {
        printf("[probe] media pipe init fail, ret=%d\n", ret);
        sample_comm_sys_exit();
        return ret;
    }

    ret = ot_buffer_pool_init();
    if (ret != OT_SUCCESS && ret != OT_ERR_BUFPOOL_ALREADY_INIT) {
        printf("[probe] buffer pool init fail, ret=%d\n", ret);
        sample_comm_media_pipe_stop(gs_media_pipe_hdl);
        sample_comm_sys_exit();
        return ret;
    }

    gs_sys_init = OT_TRUE;
    printf("[probe] media system init ok (sensor %ux%u)\n", max_w, max_h);
    return OT_SUCCESS;
}

static void probe_sys_exit(void)
{
    if (gs_sys_init != OT_TRUE) {
        return;
    }
    ot_buffer_pool_deinit();
    sample_comm_media_pipe_stop(gs_media_pipe_hdl);
    sample_comm_sys_exit();
    gs_sys_init = OT_FALSE;
    printf("[probe] media system exit ok\n");
}

int main(void)
{
    if (probe_sys_init() != OT_SUCCESS) {
        printf("[probe] init fail\n");
        return -1;
    }
    probe_sys_exit();
    return 0;
}
