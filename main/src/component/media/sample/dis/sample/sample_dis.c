/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "sample_gyro_dis.h"

#include "sample_comm.h"
#include "ot_eis_vi_api.h"
#include "ot_eis_vi_alg_api.h"

typedef enum {
    SAMPLE_DIS_GME_TYPE_4DOF = 0,
    SAMPLE_DIS_GME_TYPE_6DOF,
    SAMPLE_DIS_GME_TYPE_BUTT
} dis_gme_type;

ot_vi_pipe_id g_vi_pipe_id = 0;
ot_vi_pipe_handle g_vi_pipe_hdl = OT_NULL;
ot_vi_chn_id  g_vi_chn_id = 0;
ot_vi_chn_handle g_vi_chn_hdl = OT_NULL;

ot_vi_pipe_id g_send_vi_pipe = 3;

ot_s32 g_vproc_grp_id = 0;
ot_eis_handle g_vproc_grp_hdl = OT_NULL;
ot_s32 g_vproc_chn_id = 0;
ot_eis_handle g_vproc_chn_hdl = OT_NULL;

ot_s32 g_vo_chn = 0;
ot_s32 g_venc_chn = 0;
static ot_u32 g_dis_sample_signal_flag = 0;
ot_bool g_dis_save_stream = OT_TRUE;
ot_bool g_dis_send_data = OT_FALSE;

ot_void sample_dis_set_send_data_statue(ot_bool send_data)
{
    g_dis_send_data = send_data;
}

ot_bool sample_dis_get_send_data_statue(ot_void)
{
    return g_dis_send_data;
}

ot_bool sample_dis_get_save_stream(void)
{
    return g_dis_save_stream;
}

ot_void sample_dis_set_save_stream(ot_bool save_stream)
{
    g_dis_save_stream = save_stream;
}

/* function : show usage */
static ot_void sample_dis_usage(ot_char *argv_name)
{
    printf("Usage : %s <index> <intf> \n", argv_name);
    printf("index:\n");
    printf("\t 0)DIS-4DOF_GME.VI-VPROC-VO VENC.\n");
    printf("\t 1)DIS_GYRO and DIS_LDC SWITCH. VI-VPROC-VO VENC. DIS_LDC+DIS_GYRO -> DIS_LDC -> DIS_LDC+DIS_GYRO.\n");
    printf("intf:\n");
    printf("\t 0) vo BT1120 output, default.\n");
    return;
}

/* function : Get param by different sensor */
static ot_s32 sample_dis_get_param_by_sensor(sample_sns_type sns_type, ot_eis_dis_cfg *dis_cfg, ot_eis_dis_attr *dis_attr)
{
    ot_s32 ret = OT_SUCCESS;

    if (dis_cfg == NULL  || dis_attr == NULL) {
        return OT_FAILURE;
    }

    dis_cfg->frame_rate = 30; /* 30 fps default frame_rate */
    dis_attr->timelag  = 1000; /* 1000 default timelag */

    return ret;
}

/* function : to process abnormal case */
static void sample_dis_handle_sig(ot_s32 signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        g_dis_sample_signal_flag = 1;
    }
}

static ot_s32 sample_dis_get_dis_cfg_and_attr(dis_gme_type gme_type, ot_eis_dis_cfg *dis_cfg, ot_eis_dis_attr *dis_attr)
{
    ot_s32 ret = OT_SUCCESS;

    if (dis_cfg == NULL || dis_attr == NULL) {
        return OT_FAILURE;
    }

    dis_cfg->motion_level = OT_EIS_DIS_MOTION_LEVEL_NORM;
    dis_cfg->crop_ratio = 80; /* 80 sample crop ratio */
    dis_cfg->buf_num = 10;    /* 10 sample buf num */
    dis_cfg->frame_rate = 30; /* 30 sample frame rate */
    dis_cfg->camera_steady = OT_FALSE;

    if (gme_type == SAMPLE_DIS_GME_TYPE_4DOF) {
        dis_cfg->scale = OT_FALSE;
        dis_cfg->pdt_type = OT_EIS_DIS_PDT_TYPE_DV;
        dis_cfg->mode = OT_EIS_DIS_MODE_4_DOF_GME;
    } else {
        dis_cfg->scale = OT_TRUE;
        dis_cfg->pdt_type = OT_EIS_DIS_PDT_TYPE_RECORDER;
        dis_cfg->mode = OT_EIS_DIS_MODE_6_DOF_GME;
    }

    dis_attr->enable = OT_TRUE;
    dis_attr->moving_subject_level = 0;
    dis_attr->rolling_shutter_coef = 0;
    dis_attr->timelag = 0;
    dis_attr->still_crop = OT_FALSE;
    dis_attr->hor_limit = 512; /* 512 sample hor_limit */
    dis_attr->ver_limit = 512; /* 512 sample ver_limit */
    dis_attr->gdc_bypass = OT_FALSE;
    dis_attr->strength = 1024; /* 1024 sample strength */
    dis_attr->dis_ldc_attr.focal_len_x = 6400;
    dis_attr->dis_ldc_attr.focal_len_y = 6400;
    dis_attr->dis_ldc_attr.coord_shift_x = 35 * 3840;
    dis_attr->dis_ldc_attr.coord_shift_y = 35 * 2160;
    for (int i = 0; i < OT_EIS_SRC_LENS_COEF_NUM; i++) {
        dis_attr->dis_ldc_attr.src_calibration_ratio[i] = 100000;
    }
    return ret;
}

static ot_s32 sample_dis_set_dis_attr(const ot_eis_dis_attr *dis_attr)
{
    ot_s32 ret;
    ot_eis_vproc_chn_attr vproc_chn_attr = { 0 };
    ot_eis_img_size dis_out_size = { 0 };

    ret = ot_eis_vi_set_chn_dis_attr(g_vi_pipe_hdl, g_vi_chn_hdl, dis_attr);
    if (ret != OT_SUCCESS) {
        sample_print("set dis attr failed.ret:0x%x !\n", ret);
        return OT_FAILURE;
    }

    ret = ot_eis_vproc_chn_get_attr(g_vproc_chn_hdl, &vproc_chn_attr);
    if (ret != OT_SUCCESS) {
        sample_print("get vproc chn attr failed.ret:0x%x !\n", ret);
        return OT_FAILURE;
    }

    ret = ot_eis_vi_get_chn_dis_out_size(g_vi_pipe_hdl, g_vi_chn_hdl, &dis_out_size);
    if (ret != OT_SUCCESS) {
        sample_print("get dis out size failed.ret:0x%x !\n", ret);
        return OT_FAILURE;
    }
    vproc_chn_attr.image_attr.width = dis_out_size.width;
    vproc_chn_attr.image_attr.height = dis_out_size.height;
    ret = ot_eis_vproc_chn_set_attr(g_vproc_chn_hdl, &vproc_chn_attr);
    if (ret != OT_SUCCESS) {
        sample_print("set vproc chn attr failed.ret:0x%x !\n", ret);
        return OT_FAILURE;
    }
    return OT_SUCCESS;
}

static ot_s32 sample_dis_gme_enable(sample_sns_type sns_type, dis_gme_type gme_type)
{
    ot_s32 ret;
    ot_eis_dis_cfg dis_cfg = { 0 };
    ot_eis_dis_attr dis_attr = { 0 };

    ret = sample_dis_get_dis_cfg_and_attr(gme_type, &dis_cfg, &dis_attr);
    if (ret != OT_SUCCESS) {
        sample_print("sample dis get dis_cfg and dis_attr failed.ret:0x%x !\n", ret);
        return OT_FAILURE;
    }

    ret = sample_dis_get_param_by_sensor(sns_type, &dis_cfg, &dis_attr);
    if (ret != OT_SUCCESS) {
        sample_print("sample_dis_get_param_by_sensor failed.ret:0x%x !\n", ret);
        return OT_FAILURE;
    }

    ret = ot_eis_vi_set_chn_dis_cfg(g_vi_pipe_hdl, g_vi_chn_hdl, &dis_cfg);
    if (ret != OT_SUCCESS) {
        sample_print("set dis config failed.ret:0x%x !\n", ret);
        return OT_FAILURE;
    }

    ret = sample_dis_set_dis_attr(&dis_attr);
    if (ret != OT_SUCCESS) {
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

ot_void sample_dis_pause(ot_char *s)
{
    if (g_dis_sample_signal_flag == 1) {
        return;
    }

    printf("please hit the Enter key to %s!\n", s);
    (ot_void)getchar();
}

static ot_s32 sample_dis_gme_change()
{
    ot_s32 ret;
    ot_eis_dis_attr dis_attr = {0};

    sample_dis_pause("Disable DIS");

    ret = ot_eis_vi_get_chn_dis_attr(g_vi_pipe_hdl, g_vi_chn_hdl, &dis_attr);
    if (ret != OT_SUCCESS) {
        sample_print("get dis attr failed.ret:0x%x !\n", ret);
        return OT_FAILURE;
    }

    dis_attr.enable = OT_FALSE;
    ret = sample_dis_set_dis_attr(&dis_attr);
    if (ret != OT_SUCCESS) {
        return OT_FAILURE;
    }

    sample_dis_pause("enable DIS");

    dis_attr.enable = OT_TRUE;
    ret = sample_dis_set_dis_attr(&dis_attr);
    if (ret != OT_SUCCESS) {
        return OT_FAILURE;
    }

    sample_dis_pause("exit");
    return OT_SUCCESS;
}

static ot_s32 sample_dis_init_sys_vrb(const ot_eis_img_size *img_size)
{
    ot_s32 ret;
    ot_video_buffer_attr vrb_cfg;
    ot_bool save_stream = sample_dis_get_save_stream();

    if (img_size == OT_NULL) {
        return -1;
    }

    sample_common_get_vrb_cfg(1, img_size->width, img_size->height, &vrb_cfg);

    if (save_stream == OT_FALSE) {
        vrb_cfg.buf_blks[0].cnt = 20; /* 20 normal blk cnt */
        vrb_cfg.buf_blks[1].cnt = 12; /* 12 bayer 16bpp blk cnt */
    }

    ret = sample_comm_sys_init_with_vrb_supplement(&vrb_cfg,
        OT_VRB_SUPP_BNR_MOT_MASK | OT_VRB_SUPP_MOTION_DATA_MASK);
    if (ret != OT_SUCCESS) {
        sample_print("init sys fail.ret:0x%x !\n", ret);
        return OT_FAILURE;
    }

    return 0;
}

static ot_void sample_dis_reset_vproc_cfg(ot_void)
{
    ot_eis_vproc_grp_cfg grp_cfg = { 0 };
    ot_s32 ret;

    ret = ot_eis_vproc_get_grp_cfg(g_vproc_grp_id, &grp_cfg);
    if (ret != OT_SUCCESS) {
        sample_print("get vproc grp cfg failed. ret: 0x%x !\n", ret);
        return;
    }
    grp_cfg.is_dis_gyro_support = OT_FALSE;
    ret = ot_eis_vproc_set_grp_cfg(g_vproc_grp_id, &grp_cfg);
    if (ret != OT_SUCCESS) {
        sample_print("set vproc grp cfg failed. ret: 0x%x !\n", ret);
    }
}

static ot_void sample_dis_vproc_stop(sample_vproc_attr *vproc_attr)
{
    sample_comm_stop_vproc(vproc_attr);
    sample_dis_reset_vproc_cfg();
}

static ot_s32 sample_dis_start_vproc(sample_vproc_attr *vproc_attr, const ot_eis_img_size *img_size)
{
    ot_s32 ret;
    // ot_eis_vproc_grp_attr   vproc_grp_attr = { 0 };
    ot_eis_vproc_gdc_cfg    gdc = { 0 };
    ot_eis_vproc_gdc_param  gdc_param = { 0 };
    ot_bool pipe_sw[OT_EIS_VPROC_GRP_PIPE_MAX_NUM] = {OT_TRUE, OT_FALSE, OT_FALSE, OT_FALSE};
    ot_bool chn_sw[OT_EIS_VPROC_GRP_CHN_MAX_NUM] = {OT_TRUE, OT_FALSE, OT_FALSE, OT_FALSE};

    vproc_attr->chn_attr[0].image_attr.width = img_size->width;
    vproc_attr->chn_attr[0].image_attr.height = img_size->height;
    vproc_attr->chn_attr[0].mode = OT_EIS_VPROC_WORK_MODE_USER;
    vproc_attr->chn_attr[0].image_attr.compress_mode = OT_EIS_IMAGE_COMPRESS_MODE_NONE;
    vproc_attr->chn_attr[0].image_attr.pixel_fmt = OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
    vproc_attr->chn_attr[0].frc.src_frame_rate = -1;
    vproc_attr->chn_attr[0].frc.dst_frame_rate = -1;
    vproc_attr->chn_attr[0].frame_queue_depth = 1;

    ret = sample_comm_start_vproc(vproc_attr, pipe_sw, chn_sw);
    if (ret != OT_SUCCESS) {
        sample_print("start vproc failed. ret: 0x%x !\n", ret);
        return OT_FAILURE;
    }

    gdc_param.cfg = &gdc;
    gdc_param.enable = OT_TRUE;
    gdc_param.cfg->src.width  = img_size->width;
    gdc_param.cfg->src.height = img_size->height;
    gdc_param.cfg->cell_size  = OT_EIS_LUT_CELL_SIZE_16;
    ret = ot_eis_vproc_grp_set_gdc(vproc_attr->grp_hdl, &gdc_param);
    if (ret != OT_SUCCESS) {
        return OT_FAILURE;
    }

    return OT_SUCCESS;
}

static ot_s32 sample_dis_start_vi(sample_vi_cfg *vi_cfg)
{
    ot_s32 ret;
    ot_eis_vi_vproc_mode_type mode_type = OT_EIS_VI_OFFLINE_VPROC_OFFLINE;
    ot_eis_vi_nr_attr nr_attr = { 0 };

    ret = sample_comm_sys_set_vi_vproc_mode(mode_type);
    if (ret != OT_SUCCESS) {
        sample_print("set vi failed.ret:0x%x !\n", ret);
        return OT_FAILURE;
    }

    ret = sample_comm_vi_start_vi(vi_cfg);
    if (ret != OT_SUCCESS) {
        sample_print("start vi failed.ret:0x%x !\n", ret);
        return OT_FAILURE;
    }
    if (g_dis_send_data == OT_FALSE) {
        ret = ot_eis_vi_nr_get_attr(vi_cfg->pipe_info[0].pipe_hdl, &nr_attr);
        if (ret != OT_SUCCESS) {
            sample_print("get nr attr failed.ret:0x%x !\n", ret);
            return OT_FAILURE;
        }

        nr_attr.enable = OT_FALSE;
        ret = ot_eis_vi_nr_set_attr(vi_cfg->pipe_info[0].pipe_hdl, &nr_attr);
        if (ret != OT_SUCCESS) {
            sample_print("set nr attr failed.ret:0x%x !\n", ret);
            return OT_FAILURE;
        }
    }

    return OT_SUCCESS;
}

static ot_s32 sample_dis_bind(sample_vi_cfg *vi_cfg, const sample_vproc_attr *vproc_attr, const sample_vo_attr *vo_attr)
{
    ot_s32 ret;
    ot_eis_handle media_pipe_hdl;
    // ot_bool save_stream = sample_dis_get_save_stream();

    ret = sample_comm_media_pipe_init(&media_pipe_hdl);
    if (ret != OT_SUCCESS) {
        return OT_FAILURE;
    }

    ret = sample_comm_vproc_bind_vo(vproc_attr->chn_hdl[0], vo_attr->in_port_handle[0], media_pipe_hdl);
    if (ret != OT_SUCCESS) {
        sample_print("vo bind vproc failed. ret: 0x%x !\n", ret);
        return OT_FAILURE;
    }

    if (g_dis_send_data == OT_TRUE) {
        ret = sample_comm_vi_bind_vproc(vi_cfg->pipe_info[0].chn_info.chn_hdl, vproc_attr->pipe_hdl[0], media_pipe_hdl);
    } else {
        ret = sample_comm_vi_bind_vproc(vi_cfg->pipe_info[0].chn_info.chn_hdl, vproc_attr->pipe_hdl[0], media_pipe_hdl);
    }
    if (ret != OT_SUCCESS) {
        sample_print("vi bind vproc failed. ret: 0x%x !\n", ret);
        return OT_FAILURE;
    }

    vi_cfg->media_pipe_hdl = media_pipe_hdl;

    return OT_SUCCESS;
}

static ot_void sample_dis_unbind(const sample_vi_cfg *vi_cfg, const sample_vproc_attr *vproc_attr, const sample_vo_attr *vo_attr)
{
    ot_eis_handle media_pipe_hdl = vi_cfg->media_pipe_hdl;

    if (g_dis_send_data == OT_TRUE) {
        sample_comm_vi_un_bind_vproc(vi_cfg->pipe_info[0].chn_info.chn_hdl, vproc_attr->pipe_hdl[0], media_pipe_hdl);
    } else {
        sample_comm_vi_un_bind_vproc(vi_cfg->pipe_info[0].chn_info.chn_hdl, vproc_attr->pipe_hdl[0], media_pipe_hdl);
    }
    sample_comm_vproc_un_bind_vo(vproc_attr->chn_hdl[0], vo_attr->in_port_handle[0], media_pipe_hdl);
}

/* define SAMPLE_MEM_SHARE_ENABLE, when use tools to dump YUV/RAW. */
#ifdef SAMPLE_MEM_SHARE_ENABLE
ot_void sample_dis_init_mem_share(ot_void)
{
    ot_u32 i;
    ot_vrb_common_pools_id pools_id = {0};

    if (ot_omi_vrb_get_common_pool_id(&pools_id) != OT_SUCCESS) {
        sample_print("get common pool_id failed!\n");
        return;
    }
    for (i = 0; i < pools_id.pool_cnt; ++i) {
        ot_omi_vrb_pool_share_all(pools_id.pool[i]);
    }
}
#endif

ot_s32 sample_dis_start_sample(sample_vi_cfg *vi_cfg, sample_vproc_attr *vproc_attr, sample_vo_attr *vo_attr, ot_eis_img_size *img_size)
{
    /* step 1: init SYS and common VRB */
    if (sample_dis_init_sys_vrb(img_size) != OT_SUCCESS) {
        return OT_FAILURE;
    }

    /* step 2: set vproc pos */
    if (ot_eis_media_set_3dnr_pos(OT_EIS_3DNR_POS_VPROC) != OT_SUCCESS) {
        goto sys_exit;
    }

    /* step 3: start VI */
    if (sample_dis_start_vi(vi_cfg) != OT_SUCCESS) {
        goto sys_exit;
    }

    /* step 4:  start VPROC */
    if (sample_dis_start_vproc(vproc_attr, img_size) != OT_SUCCESS) {
        goto vi_stop;
    }

    // /* step 5:  start VO */
    if (sample_comm_start_vo(vo_attr) != OT_SUCCESS) {
        goto vproc_stop;
    }

    /* step 7:  start bind */
    if (sample_dis_bind(vi_cfg, vproc_attr, vo_attr) != OT_SUCCESS) {
        goto vo_stop;
    }

    return OT_SUCCESS;

vo_stop:
    sample_comm_stop_vo(vo_attr);
vproc_stop:
    sample_dis_vproc_stop(vproc_attr);
vi_stop:
    sample_comm_vi_stop_vi(vi_cfg);
sys_exit:
    sample_comm_sys_exit();
    return OT_FAILURE;
}

ot_void sample_dis_stop_sample_without_sys_exit(sample_vi_cfg *vi_cfg, sample_vproc_attr *vproc_attr, sample_vo_attr *vo_attr)
{
    sample_dis_unbind(vi_cfg, vproc_attr, vo_attr);

    sample_comm_stop_vo(vo_attr);
    sample_dis_vproc_stop(vproc_attr);
    sample_comm_vi_stop_vi(vi_cfg);
}

ot_void sample_dis_stop_sample(sample_vi_cfg *vi_cfg, sample_vproc_attr *vproc_attr, sample_vo_attr *vo_attr)
{
    sample_dis_stop_sample_without_sys_exit(vi_cfg, vproc_attr, vo_attr);
    sample_comm_sys_exit();
}

static ot_s32 sample_dis_gme(ot_u32 vo_intf_type, dis_gme_type gme_type)
{
    ot_s32 ret;
    ot_eis_img_size img_size;
    sample_vi_cfg vi_cfg = { 0 };
    sample_vo_attr *vo_attr = OT_NULL;
    sample_vproc_attr vproc_attr = { 0 };
    sample_sns_type sns_type = SONY_IMX623_MIPI_3M_60FPS_16BIT;

    sample_dis_set_save_stream(OT_TRUE);

    if (gme_type != SAMPLE_DIS_GME_TYPE_4DOF && gme_type != SAMPLE_DIS_GME_TYPE_6DOF) {
        sample_print("wrong gme_type %d!\n", gme_type);
        return OT_FAILURE;
    }

    /* step 1:  get sensors information and vo config */
    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);
    sample_media_vproc_get_default_attr_by_snsor(sns_type, &vproc_attr);
    vo_attr = sample_comm_args_get_vo_attr();

    g_vi_pipe_id = vi_cfg.pipe_info[0].pipe_id[0];
    g_vi_chn_id  = vi_cfg.pipe_info[0].chn_info.chn_id;
    g_vi_pipe_hdl = vi_cfg.pipe_info[0].pipe_hdl;
    g_vi_chn_hdl = vi_cfg.pipe_info[0].chn_info.chn_hdl;
    g_vproc_grp_hdl = vproc_attr.grp_hdl;
    g_vproc_chn_hdl = vproc_attr.chn_hdl[0];

    /* step 2:  get input size */
    sample_comm_vi_get_size_by_sns_type(sns_type, &img_size);

    /* step 3: start VI-VPROC-VO-VENC */
    vi_cfg.pipe_info[g_vi_pipe_id].nr_attr.enable = OT_FALSE;
    ret = sample_dis_start_sample(&vi_cfg, &vproc_attr, vo_attr, &img_size);
    if (ret != OT_SUCCESS) {
        return OT_FAILURE;
    }

    /* step 4: set DIS config & attribute */
    if (sample_dis_gme_enable(vi_cfg.sns_info.sns_type, gme_type) != OT_SUCCESS) {
        goto exit;
    }

    /* step 5: dis enable disable */
    if (sample_dis_gme_change() != OT_SUCCESS) {
        goto exit;
    }

exit:
    /* exit process */
    sample_dis_stop_sample(&vi_cfg, &vproc_attr, vo_attr);
    return OT_SUCCESS;
}

static ot_s32 sample_dis_proc(ot_char *argv_name, ot_u32 case_index, ot_u32 vo_intf_type, ot_eis_img_size* input_size)
{
    ot_s32 ret = OT_FAILURE;
    switch (case_index) {
        case 0: /* 0 index */
            ret = sample_dis_gme(vo_intf_type, SAMPLE_DIS_GME_TYPE_4DOF);
            break;
        case 1: /* 1 index */
            ret = sample_dis_gyro_ldc_switch(vo_intf_type);
            break;
        case 2:
            ret = sample_dis_send(input_size);
            break;
        default:
            sample_print("the index is invalid!\n");
            sample_dis_usage(argv_name);
            return OT_FAILURE;
    }
    return ret;
}

#ifdef __LITEOS__
int app_main(ot_s32 argc, ot_char *argv[])
#else
int main(ot_s32 argc, ot_char *argv[])
#endif
{
    ot_s32 ret;
    ot_eis_img_size input_size = {3840, 2160};
    ot_u32 vo_intf_type = OT_EIS_VO_INTF_BT1120;
    ot_u32 index;
    ot_char *end_ptr = OT_NULL;
    if ((argc < 2) || (argc > 4) || (strlen(argv[1]) > 2) || /* 2 4 argv num */
        (strlen(argv[1]) == 2 && (!check_digit(argv[1][1]) || argv[1][0] == '0'))) { /* 2:arg len */
        sample_dis_usage(argv[0]);
        return OT_FAILURE;
    }
    g_dis_sample_signal_flag = 0;
#ifndef __LITEOS__
    sample_sys_signal(&sample_dis_handle_sig);
#endif
    index = (ot_u32)strtol(argv[1], &end_ptr, 10); /* arg index, 10 : decimal */
    if (argc == 4) { /* 4 : argv num */
        input_size.width = (ot_u32)strtol(argv[2], &end_ptr, 10); /* 2 : arg index, 10 : decimal */
        input_size.height = (ot_u32)strtol(argv[3], &end_ptr, 10); /* 3 : arg index, 10 : decimal */
    }
    if (argc == 3) { /* 3 argv num */
        if ((strlen(argv[2]) != 1)) { /* 2 intf */
            sample_dis_usage(argv[0]);
            return OT_FAILURE;
        }
        switch (*argv[2]) { /* 2 intf */
            case '0':
                break;
            default:
                sample_print("the index is invalid!\n");
                sample_dis_usage(argv[0]);
                return OT_FAILURE;
        }
    }
    ret = sample_dis_proc(argv[0], index, vo_intf_type, &input_size);
    if (g_dis_sample_signal_flag == 1) {
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        exit(-1);
    }
    if (ret == OT_SUCCESS) {
        sample_print("program exit normally!\n");
    } else {
        sample_print("program exit abnormally!\n");
    }
    return ret;
}
