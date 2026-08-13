
#include "stereo_vo.h"
#include "ot_eis_mipi_tx.h"

#define VO_FPS_MIN   30

static stereo_vo_attr gs_stereo_vo_1920x480_attr  = {
    // in port param
    .in_port_enable[0] = OT_TRUE,
    .in_port_enable[1] = OT_FALSE,
    .in_port_attr[0] = {
        .pixel_attr=  {
            .format = OT_EIS_IMAGE_FORMAT_YVU_422_SEMIPLANAR,
            .layout = OT_EIS_IMAGE_LAYOUT_LINEAR,
            .width = 1920,
            .height = 480,
        },
    },
    
    .surface_bind_attr[0] = {
        .x = 0,
        .y = 0,
        .show = OT_TRUE,
    },
    // surface param
    .surface_enable[0] = OT_TRUE,
    .surface[0] = OT_EIS_VO_VIDEO_SURFACE_0,
    .surface_attr[0] = {
        .width = 1920,
        .height = 480,
        .buf_len = 3,
    },
    .display_bind_attr[0] = {
        .x = 0,
        .y = 0,
        .order = 0,
    },
    // display param
    .display[0] = OT_EIS_VO_DISPLAY_0,
    .display_attr[0] = {
        .vtth = 0,
        .interface = OT_EIS_VO_INTF_MIPI_0,
        .size = {
            .type = OT_EIS_VO_DISPLAY_CUSTOMIZED,
            .frame_rate = OT_VO_OUTPUT_FRAME_RATE_30,
        },
        .sync_info = {
            .syncm          = 0,
            .iop            = 1,
            .intfb          = 1,

            .vact           = 480,
            .vbb            = 6,
            .vfb            = 12,

            .hact           = 1920,
            .hbb            = 10,
            .hfb            = 134,
            .hmid           = 0,

            .bvact          = 0,
            .bvbb           = 0,
            .bvfb           = 0,

            .hpw            = 2,
            .vpw            = 2,

            .idv            = 0,
            .ihs            = 0,
            .ivs            = 0,
        },
    },
};

void stereo_vo_get_attr(stereo_vo_attr* vo_attr)
{
    if (vo_attr != OT_NULL) {
        memcpy_s(vo_attr, sizeof(stereo_vo_attr), &gs_stereo_vo_1920x480_attr, sizeof(stereo_vo_attr));
    }
}

static ot_void stereo_vo_get_mipitx_combo_dev_attr(stereo_vo_attr *vo_attr, ot_eis_vo_if_attr *if_attr)
{
    if (vo_attr->display_attr[0].size.type != OT_EIS_VO_DISPLAY_CUSTOMIZED) {
        return;
    }

    if_attr->dev_attr.devno = 0;
    for (int i = 0 ; i < OT_MIPITX_MAX_LANE_NUM; i++) {
        if_attr->dev_attr.lane_id[i] = i;
    }
    if_attr->dev_attr.out_mode              = OT_MIPITX_OUT_MODE_CSI;
    if_attr->dev_attr.out_format            = OT_MIPITX_OUT_FORMAT_YUV422_8BIT;
    if_attr->dev_attr.clklane_continue_mode = OT_MIPITX_CLK_LANE_CONTINUE;

    if_attr->dev_attr.sync_info.hpw = vo_attr->display_attr[0].sync_info.hpw;
    if_attr->dev_attr.sync_info.hbp = vo_attr->display_attr[0].sync_info.hbb - vo_attr->display_attr[0].sync_info.hpw;
    if_attr->dev_attr.sync_info.hact = vo_attr->display_attr[0].sync_info.hact;
    if_attr->dev_attr.sync_info.hfp = vo_attr->display_attr[0].sync_info.hfb;
    if_attr->dev_attr.sync_info.vpw = vo_attr->display_attr[0].sync_info.vpw;
    if_attr->dev_attr.sync_info.vbp = vo_attr->display_attr[0].sync_info.vbb - vo_attr->display_attr[0].sync_info.vpw;
    if_attr->dev_attr.sync_info.vact = vo_attr->display_attr[0].sync_info.vact;
    if_attr->dev_attr.sync_info.vfp = vo_attr->display_attr[0].sync_info.vfb;
    if_attr->dev_attr.pixel_clk = vo_attr->display_attr[0].clk_info.auto_user_sync_info.pixel_clk / 1000 - 300; // KHz
    if_attr->dev_attr.phy_data_rate = (if_attr->dev_attr.pixel_clk * 24 + 4000) / 4 / 1000; // Mbps
    if_attr->dev_attr.video_mode = OT_MIPITX_BURST_MODE;
}


static ot_void stereo_vo_calculate_clock_info(ot_eis_vo_display_attr *display_attr)
{
    ot_u32 pixel_clk, h_total, v_total;
    ot_float fps;
    if (display_attr->size.type != OT_EIS_VO_DISPLAY_CUSTOMIZED) {
        return;
    }

    if (display_attr->interface == OT_EIS_VO_INTF_BT1120) {
        sample_comm_vo_set_fps(VO_FPS_MIN);
    }

    h_total = display_attr->sync_info.hact + display_attr->sync_info.hbb + display_attr->sync_info.hfb;
    v_total = display_attr->sync_info.vact + display_attr->sync_info.vbb + display_attr->sync_info.vfb;
    fps = sample_comm_vo_get_fps();

    pixel_clk = (h_total * v_total * fps + 300000) / 1000 * 1000;

    printf("h_total[%d], v_total[%d], pixel clock[%d] fps[%.1f]\n", h_total, v_total, pixel_clk, fps);

    display_attr->clk_info.op_mode = OT_EIS_OP_MODE_AUTO;
    display_attr->clk_info.auto_user_sync_info.pixel_clk = pixel_clk;
}


static ot_s32 stereo_vo_start_mipi_tx(stereo_vo_attr *vo_attr)
{
    ot_s32 ret = OT_SUCCESS;
    ot_eis_handle vo_intf_hdl;
    ot_eis_vo_if_attr if_attr = {0};

    ret = ot_eis_vo_if_init();
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vo_if_init ret err: %08x\n", ret);
        return ret;
    }

    ret = ot_eis_vo_if_create(&vo_intf_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vo_if_create ret err: %08x\n", ret);
        goto IF_DEINIT;
    }

    stereo_vo_get_mipitx_combo_dev_attr(vo_attr, &if_attr);

    ret = ot_eis_vo_if_config(vo_intf_hdl, &if_attr);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vo_if_config_attr ret err: %08x\n", ret);
        goto IF_DESTROY;
    }

    ret = ot_eis_vo_if_start(vo_intf_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vo_if_start ret err: %08x\n", ret);
        goto IF_DESTROY;
    }
    vo_attr->mipi_tx_hdl = vo_intf_hdl;
    return OT_SUCCESS;
IF_DESTROY:
    ot_eis_vo_if_destroy(vo_intf_hdl);
IF_DEINIT:
    ot_eis_vo_if_deinit();
    return ret;
}

static ot_void stereo_vo_stop_mipi_tx(stereo_vo_attr *vo_attr)
{
    ot_s32 ret = OT_SUCCESS;

    ret = ot_eis_vo_if_stop(vo_attr->mipi_tx_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vo_if_stop ret err: %08x\n", ret);
        return;
    }

    ret = ot_eis_vo_if_destroy(vo_attr->mipi_tx_hdl);
    if (ret != OT_SUCCESS) {
        printf("ot_eis_vo_if_destroy ret err: %08x\n", ret);
        return;
    }

    (ot_void)ot_eis_vo_if_deinit();
}


ot_s32 stereo_vo_start(stereo_vo_attr *vo_attr)
{    
    ot_eis_vo_init();
    for (int i = 0; i < OT_VO_MAX_BIND_PORT_NUM; i++) {
        if (vo_attr->in_port_enable[i] == OT_TRUE) {
            ot_eis_vo_in_port_create(&vo_attr->in_port_handle[i], &vo_attr->in_port_attr[i]);
        }
    }

    // create surface
    for (int i = 0; i < OT_VO_MAX_BIND_SURF_NUM; i++) {
        if (vo_attr->surface_enable[i] == OT_TRUE) {
            ot_eis_vo_surface_create(vo_attr->surface[i], &vo_attr->surface_handle[i], &vo_attr->surface_attr[i]);
        }
    }

    // bind in_port
    for (int i = 0; i < OT_VO_MAX_BIND_PORT_NUM; i++) {
        if (vo_attr->in_port_enable[i] == OT_TRUE) {
            ot_eis_vo_surface_bind(vo_attr->surface_handle[0],
            vo_attr->in_port_handle[i], &vo_attr->surface_bind_attr[i]);
        }
    }

    /* surface crop */
    if (vo_attr->crop_attr[0].enable == OT_TRUE) {
        printf("crop: enable[%d], x[%d], y[%d], w[%d], h[%d]\n", vo_attr->crop_attr[0].enable, vo_attr->crop_attr[0].rect.x,
        vo_attr->crop_attr[0].rect.y,
        vo_attr->crop_attr[0].rect.w,
        vo_attr->crop_attr[0].rect.h);
        ot_eis_vo_surface_set_crop(vo_attr->surface_handle[0], &vo_attr->crop_attr[0]);
    }

    // create display
    stereo_vo_calculate_clock_info(&vo_attr->display_attr[0]);
    
    ot_eis_vo_display_create(vo_attr->display[0], &vo_attr->display_handle[0], &vo_attr->display_attr[0]);

    // bind surface
    ot_eis_vo_display_bind(vo_attr->display_handle[0], vo_attr->surface_handle[0], &vo_attr->display_bind_attr[0]);

    if (vo_attr->display_attr[0].interface == OT_EIS_VO_INTF_MIPI_0) {
        /* set defalut csc to BT709 FULL */
        ot_eis_vo_mipi_param param;
        param.csc.contrast = 50;
        param.csc.ex_csc_en = 0;
        param.csc.hue = 50;
        param.csc.luma = 50;
        param.csc.saturation = 50;
        param.csc.csc_matrix = OT_EIS_VO_CSC_MATRIX_BT709FULL_TO_BT709FULL;
        param.gamma_info.gamma_enable = OT_FALSE;
        ot_eis_vo_display_set_mipi_param(vo_attr->display_handle[0], &param);
    }
    else if (vo_attr->display_attr[0].interface == OT_EIS_VO_INTF_BT1120) {
        ot_eis_vo_bt_param bt_param;
        bt_param.yc_inverted_en = 0;
        bt_param.bit_inverted_en = 0;
        bt_param.clk_edge = OT_EIS_VO_CLK_EDGE_SINGLE;
        ot_eis_vo_display_set_bt_param(vo_attr->display_handle[0], &bt_param);
    }
    // display start
    ot_eis_vo_display_start(vo_attr->display_handle[0]);

    if (vo_attr->display_attr[0].interface == OT_EIS_VO_INTF_MIPI_0) {
        stereo_vo_start_mipi_tx(vo_attr);
    }

    return OT_SUCCESS;
}

ot_s32 stereo_vo_stop(stereo_vo_attr *vo_attr)
{
    // display stop
    ot_eis_vo_display_stop(vo_attr->display_handle[0]);
    // unbind surface and in port
    for (int i = 0; i < OT_VO_MAX_BIND_PORT_NUM; i++) {
        if (vo_attr->in_port_enable[i] == OT_TRUE) {
            ot_eis_vo_surface_unbind(vo_attr->surface_handle[0], vo_attr->in_port_handle[i]);
            // in port destroy
            ot_eis_vo_in_port_destroy(vo_attr->in_port_handle[i]);
        }
    }
    // surface destroy
    for (int i = 0; i < OT_VO_MAX_BIND_SURF_NUM; i++) {
        if (vo_attr->surface_enable[i] == OT_TRUE) {
            // unbind display and surface
            ot_eis_vo_display_unbind(vo_attr->display_handle[0], vo_attr->surface_handle[i]);
            ot_eis_vo_surface_destroy(vo_attr->surface_handle[i]);
        }
    }
    // display destroy
    ot_eis_vo_display_destroy(vo_attr->display_handle[0]);
    if (vo_attr->display_attr[0].interface == OT_EIS_VO_INTF_MIPI_0) {
        stereo_vo_stop_mipi_tx(vo_attr);
    }
    // deinit vo
    ot_eis_vo_deinit();

    return OT_SUCCESS;
}


ot_s32 stereo_vo_acquire_frame(ot_eis_handle pool, ot_eis_video_frame *frame)
{
    ot_s32 ret = 0;
    ot_eis_buf_size_calc_cfg calc_cfg = { 0 };
    ot_phys_addr phys_addr;
    ot_void *virt_addr = OT_NULL;
    ot_buffer_pool_blk blk = { 0 };

    sample_common_get_pic_buf_cfg(&frame->attr, &calc_cfg);
    ret = ot_buffer_pool_blk_acquire(pool, calc_cfg.blk_size, &blk);
    if (ret != OT_SUCCESS) {
        sample_print("ot_buffer_pool_blk_acquire err, size:%u\n", calc_cfg.blk_size);
        return OT_FAILURE;
    }

    phys_addr = blk.phys_addr;
    ret = ot_smr_mmap(phys_addr, calc_cfg.blk_size, OT_TRUE, &virt_addr);
    if (ret != OT_SUCCESS) {
        sample_print("ot_smr_mmap err!\n");
        ot_buffer_pool_blk_release(pool, &blk);
        return OT_FAILURE;
    }
    frame->pool_hdl           = pool;
    frame->mod_id             = OT_EIS_MODULE_VPROC;
    frame->color_space        = OT_EIS_IMAGE_COLOR_SPACE_BT709;
    frame->buff.phys_addr[0]  = phys_addr;
    frame->buff.phys_addr[1]  = frame->buff.phys_addr[0] + calc_cfg.main_y_size;
    frame->buff.virt_addr[0]  = virt_addr;
    frame->buff.virt_addr[1]  = frame->buff.virt_addr[0] + calc_cfg.main_y_size;
    frame->buff.stride[0]     = calc_cfg.main_stride;
    frame->buff.stride[1]     = calc_cfg.main_stride;

    return ret;
}

ot_s32 stereo_vo_flush_frame(ot_eis_video_frame *frame)
{
    if (frame == OT_NULL || frame->buff.virt_addr[0] == OT_NULL) {
        return OT_FAILURE;
    }
    if (frame->attr.pixel_fmt == OT_EIS_IMAGE_FORMAT_YVU_422_SEMIPLANAR || frame->attr.pixel_fmt == OT_EIS_IMAGE_FORMAT_YUV_422_SEMIPLANAR) {
        return ot_smr_flush_cache(frame->buff.phys_addr[0], frame->buff.virt_addr[0], frame->attr.height * frame->buff.stride[0] + frame->attr.height * frame->buff.stride[1]);
    }
    else if (frame->attr.pixel_fmt == OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR || frame->attr.pixel_fmt == OT_EIS_IMAGE_FORMAT_YUV_420_SEMIPLANAR) {
        return ot_smr_flush_cache(frame->buff.phys_addr[0], frame->buff.virt_addr[0], frame->attr.height * frame->buff.stride[0] + frame->attr.height / 2 * frame->buff.stride[1]);
    }
    
    return OT_FAILURE;
}   

ot_s32 stereo_vo_release_frame(ot_eis_handle pool, ot_eis_video_frame *frame)
{
    ot_buffer_pool_blk blk;
    ot_s32 ret = 0;
    ot_eis_buf_size_calc_cfg calc_cfg = { 0 };

    sample_common_get_pic_buf_cfg(&frame->attr, &calc_cfg);
    blk.phys_addr  = frame->buff.phys_addr[0];
    blk.block_size = calc_cfg.blk_size;
    ot_smr_munmap(frame->buff.virt_addr[0], calc_cfg.blk_size);
    
    ret = ot_buffer_pool_blk_release(pool, &blk);
    return ret;
}

