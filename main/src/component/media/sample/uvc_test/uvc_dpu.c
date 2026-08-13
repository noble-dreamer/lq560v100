/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */
#include "uvc_dpu.h"

#include <limits.h>
#include "securec.h"
#include "ot_media_pipe.h"
#include "ot_vrb.h"

ot_u32 sample_dpu_calc_stride(ot_u32 width, ot_u8 align)
{
    sample_avp_dpu_check_exps_return(align == 0, 0, "align can't be zero!\n");
    return (width + (align - width % align) % align);
}

/* function : create dpu rect memory info */
static ot_s32 sample_dpu_rect_create_mem_info(ot_avp_dpu_rect_mem_info *mem_info, ot_u32 size)
{
    ot_s32 ret;
    ot_smr_alloc_attr alloc_attr = {0};

    strcpy(alloc_attr.region_name, "anony");
    strcpy(alloc_attr.chunk_name, "dpu_rect");
    alloc_attr.len = size;
    alloc_attr.cached = OT_TRUE;

    mem_info->size = size;
    ret = ot_smr_alloc(&alloc_attr, (ot_phys_addr *)(&mem_info->phys_addr), (ot_void **)&mem_info->virt_addr);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_smr_alloc failed!\n", ret);
    return ret;
}

ot_s32 sample_dpu_get_lut_from_file(const ot_char *file_name, ot_avp_dpu_rect_mem_info *mem_info)
{
    ot_s32 ret;
    ot_u32 size;
    ot_void *virt_addr = OT_NULL;
    FILE *fp = OT_NULL;
    ot_char path[AVP_DPU_PATH_MAX + 1] = {0};

    sample_avp_dpu_check_exps_return(((strlen(file_name) > AVP_DPU_PATH_MAX) || (realpath(file_name, path) == OT_NULL)), OT_FAILURE, "Error, file_name is invalid!\n");
    fp = fopen(file_name, "rb");
    sample_avp_dpu_check_exps_return(fp == OT_NULL, OT_FAILURE, "Err, open lut file failed!\n");
    ret = fseek(fp, 0L, SEEK_END);
    sample_avp_dpu_check_exps_goto(ret == -1, fail_0, "Error, fseek failed!\n");
    size = ftell(fp);
    sample_avp_dpu_check_exps_goto(size <= 0, fail_0, "Error, ftell failed!\n");
    ret = fseek(fp, 0L, SEEK_SET);
    sample_avp_dpu_check_exps_goto(ret == -1, fail_0, "Error, fseek failed!\n");

    ret = memset_s(mem_info, sizeof(ot_avp_dpu_rect_mem_info), 0, sizeof(ot_avp_dpu_rect_mem_info));
    sample_avp_dpu_check_exps_goto(ret != EOK, fail_0, "Err, memset_s mem_info failed!\n");
    ret = sample_dpu_rect_create_mem_info(mem_info, size);
    sample_avp_dpu_check_exps_goto(ret != EOK, fail_0, "Err, dpu_rect_create_mem_info failed!\n");

    virt_addr = sample_avp_dpu_convert_addr_to_ptr(ot_void, mem_info->virt_addr);
    ret = fread(virt_addr, size, 1, fp);
    sample_avp_dpu_check_exps_goto(ret != 1, fail_1, "Error,read lut file failed!\n");
    if (fp != OT_NULL) {
        fclose(fp);
    }

    return OT_SUCCESS;
fail_1:
    sample_avp_dpu_smr_free(mem_info->phys_addr);
fail_0:
    if (fp != OT_NULL) {
        fclose(fp);
    }
    return OT_FAILURE;
}

ot_s32 sample_dpu_rect_load_lut(const ot_char *file_name, ot_avp_dpu_rect_mem_info *mem_info, ot_s32 *lut_id)
{
    ot_s32 ret;
    ot_char path[AVP_DPU_PATH_MAX + 1] = {0};

    sample_avp_dpu_check_exps_return(((file_name == OT_NULL) || (mem_info == OT_NULL) || (lut_id == OT_NULL)), OT_FAILURE, "Error, model_file or nnie_model or lut_id is NULL!\n");
    sample_avp_dpu_check_exps_return(((strlen(file_name) > AVP_DPU_PATH_MAX) || (realpath(file_name, path) == OT_NULL)), OT_FAILURE, "Error, file_name is invalid!\n");

    ret = sample_dpu_get_lut_from_file(file_name, mem_info);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, OT_FAILURE, "Err, get_lut_from_file failed!\n");

    ret = ot_avp_dpu_rect_load_lut(mem_info, lut_id);
    sample_avp_dpu_check_exps_goto(ret != OT_SUCCESS, fail, "Error(%#x), ot_avp_dpu_rect_load_lut failed!\n", ret);

    return ret;
fail:
    sample_avp_dpu_smr_free(mem_info->phys_addr);
    return ret;
}

ot_void sample_dpu_rect_unload_lut(ot_avp_dpu_rect_mem_info *mem_info, ot_s32 lut_id)
{
    if (mem_info == OT_NULL) {
        sample_avp_dpu_trace_err("Error, mem_info can't be OT_NULL!\n");
        return;
    }
    (ot_void)ot_avp_dpu_rect_unload_lut(lut_id);
    sample_avp_dpu_smr_free(mem_info->phys_addr);
}

/* function : start dpu rect grp. */
ot_s32 sample_dpu_rect_start(ot_eis_handle* rect_grp_hdl, ot_avp_dpu_rect_grp_attr *grp_attr, ot_avp_dpu_rect_chn_attr *chn_attr, ot_eis_handle rect_chn_hdl[OT_AVP_DPU_RECT_MAX_CHN_NUM])
{
    ot_eis_handle dpu_rect_chn_hdl;
    ot_s32 ret;
    ot_s32 j;
    ot_s32 dpu_rect_chn_num;

    sample_avp_dpu_check_exps_return(((grp_attr == OT_NULL) || (chn_attr == OT_NULL) || (rect_grp_hdl == OT_NULL)), OT_FAILURE, "Error, grp_attr or chn_attr or rect_grp_hdl is OT_NULL!\n");

    ret = ot_avp_dpu_rect_create_grp(rect_grp_hdl, grp_attr);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_rect_create_grp failed!\n", ret);

    if (grp_attr->rect_mode == OT_AVP_DPU_RECT_MODE_SINGLE) {
        dpu_rect_chn_num = 1;
    }
    else  {
        dpu_rect_chn_num = OT_AVP_DPU_RECT_MAX_CHN_NUM;
    }

    for (j = 0; j < dpu_rect_chn_num; j++) {

        ret = ot_avp_dpu_rect_create_chn(*rect_grp_hdl, &dpu_rect_chn_hdl);
        sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_rect_create_chn failed!\n", ret);

        ret = ot_avp_dpu_rect_set_chn_attr(dpu_rect_chn_hdl, &chn_attr[j]);
        sample_avp_dpu_check_exps_goto(ret != OT_SUCCESS, rect_start_fail, "Err(%#x), ot_avp_dpu_rect_set_chn_attr failed!\n", ret);

        rect_chn_hdl[j] = dpu_rect_chn_hdl;
        ret = ot_avp_dpu_rect_enable_chn(dpu_rect_chn_hdl);
        sample_avp_dpu_check_exps_goto(ret != OT_SUCCESS, rect_start_fail, "Err(%#x), ot_avp_dpu_rect_enable_chn failed!\n", ret);
    }

    ret = ot_avp_dpu_rect_start_grp(*rect_grp_hdl);
    sample_avp_dpu_check_exps_goto(ret != OT_SUCCESS, rect_start_fail, "Err(%#x), ot_avp_dpu_rect_start_grp failed!\n", ret);

    return OT_SUCCESS;
rect_start_fail:
    ot_avp_dpu_rect_destroy_chn(dpu_rect_chn_hdl);
    return OT_FAILURE;
}

/* function : stop dpu rect grp */
ot_s32 sample_dpu_rect_stop(ot_eis_handle rect_grp_hdl, ot_eis_handle rect_chn_hdl[OT_AVP_DPU_RECT_MAX_CHN_NUM], ot_avp_dpu_rect_mode rect_mode)
{
    ot_s32 j;
    ot_s32 ret;
    ot_s32 dpu_rect_chn_num;

    ret = ot_avp_dpu_rect_stop_grp(rect_grp_hdl);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_rect_stop_grp failed!\n", ret);

    if (rect_mode == OT_AVP_DPU_RECT_MODE_SINGLE) {
        dpu_rect_chn_num = 1;
    }
    else {
        dpu_rect_chn_num = OT_AVP_DPU_RECT_MAX_CHN_NUM;
    }

    for (j = 0; j < dpu_rect_chn_num; j++) {
        ret = ot_avp_dpu_rect_disable_chn(rect_chn_hdl[j]);
        sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_rect_disable_chn failed!\n", ret);

        ret = ot_avp_dpu_rect_destroy_chn(rect_chn_hdl[j]);
        sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_rect_destroy_chn failed!\n", ret);
    }

    ret = ot_avp_dpu_rect_destroy_grp(rect_grp_hdl);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_rect_destroy_grp failed!\n", ret);

    return OT_SUCCESS;
}

/* function : write  image to binary file */
ot_s32 sample_avp_dpu_write_to_bin_file(FILE *fp, ot_u8 *src, const ot_eis_img_frame *frame_info, ot_u32 ele_size)
{
    ot_u32 i;
    ot_s32 ret;

    for (i = 0; i < frame_info->attr.height; i++) {
        ret = fwrite(src, frame_info->attr.width * ele_size, 1, fp);
        sample_avp_dpu_check_exps_return(ret != 1, OT_FAILURE, "fwrite file failed!\n");
        src += frame_info->buff.stride[0];
    }

    (void)fflush(fp);
    return  OT_SUCCESS;
}

/* function : write frame to file. */
ot_s32 sample_dpu_write_frame_to_file(FILE *fp, ot_u32 ele_size, ot_eis_img_frame *frame_info)
{
    ot_u32 stride;
    ot_u32 height;
    ot_void *virt_addr = OT_NULL;
    ot_u32 size;
    ot_s32 ret;

    height = frame_info->attr.height;
    stride = frame_info->buff.stride[0];
    size = stride * height;

    ret = ot_smr_mmap(frame_info->buff.phys_addr[0], size, OT_TRUE, &virt_addr);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, OT_FAILURE, "ot_smr_mmap failed!\n");

    ret = sample_avp_dpu_write_to_bin_file(fp, (ot_u8*)virt_addr, frame_info, ele_size);
    sample_avp_dpu_check_exps_goto(ret != OT_SUCCESS, fail, "write_to_bin_filp failed!\n");
fail:
    (ot_void)ot_smr_munmap(virt_addr, size);
    virt_addr = OT_NULL;

    return ret;
}

/* function : create dpu match memory info */
ot_s32 sample_dpu_match_create_mem_info(ot_avp_dpu_match_mem_info *mem_info, ot_char *mmb, ot_char *zone, ot_u32 size)
{
    ot_s32 ret;
    ot_smr_alloc_attr alloc_attr = {0};

    sample_avp_dpu_check_exps_return(mem_info == OT_NULL, OT_FAILURE, "mem_info can't be NULL\n");
    ret = memset_s(mem_info, sizeof(ot_avp_dpu_match_mem_info), 0, sizeof(ot_avp_dpu_match_mem_info));
    sample_avp_dpu_check_exps_return(ret != EOK, OT_FAILURE, "Err, memset_s mem_info failed!\n");

    strcpy(alloc_attr.region_name, "anony");
    strcpy(alloc_attr.chunk_name,  "dpu_match");
    alloc_attr.len = size;
    alloc_attr.cached = OT_FALSE;

    mem_info->size = size;
    ret = ot_smr_alloc(&alloc_attr, (ot_phys_addr*)&mem_info->phys_addr, (ot_void **)&mem_info->virt_addr);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, OT_FAILURE, "Err(%#x), ot_smr_alloc failed\n", ret);

    return OT_SUCCESS;
}

/* function : start dpu match grp. */
ot_s32 sample_dpu_match_start(ot_eis_handle* dpu_match_grp, ot_eis_handle match_chn_hdl[OT_AVP_DPU_MATCH_MAX_CHN_NUM], ot_avp_dpu_match_grp_attr *grp_attr, ot_avp_dpu_match_chn_attr *chn_attr)
{
    ot_eis_handle chn_hdl;
    ot_s32 ret;
    ot_s32 j;

    sample_avp_dpu_check_exps_return((grp_attr == OT_NULL) || (chn_attr == OT_NULL), OT_FAILURE, "grp_attr or chn_attr can't be NULL\n");

    ret = ot_avp_dpu_match_create_grp(dpu_match_grp, grp_attr);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_match_create_grp failed\n", ret);

    for (j = 0; j < OT_AVP_DPU_MATCH_MAX_CHN_NUM; j++) {
        ret = ot_avp_dpu_match_create_chn(*dpu_match_grp, &chn_hdl);
        sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_match_create_chn failed\n", ret);

        match_chn_hdl[j] = chn_hdl;
        
        ret = ot_avp_dpu_match_set_chn_attr(chn_hdl, chn_attr);
        sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_match_set_chn_attr failed\n", ret);

        ret = ot_avp_dpu_match_enable_chn(chn_hdl);
        sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_match_enable_chn failed\n", ret);
    }

    ret = ot_avp_dpu_match_start_grp(*dpu_match_grp);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_match_start_grp failed\n", ret);

    return OT_SUCCESS;
}

/* function : stop dpu match grp */
ot_s32 sample_dpu_match_stop(ot_eis_handle dpu_match_grp, ot_eis_handle match_chn_hdl[OT_AVP_DPU_MATCH_MAX_CHN_NUM])
{
    ot_s32 j;
    ot_s32 ret;

    ret = ot_avp_dpu_match_stop_grp(dpu_match_grp);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_match_stop_grp failed\n", ret);

    for (j = 0; j < OT_AVP_DPU_MATCH_MAX_CHN_NUM; j++) {
        ret = ot_avp_dpu_match_disable_chn(match_chn_hdl[j]);
        sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_match_disable_chn failed\n", ret);

        ret = ot_avp_dpu_match_destroy_chn(match_chn_hdl[j]);
        sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_match_destroy_chn failed\n", ret);
    }

    ret = ot_avp_dpu_match_destroy_grp(dpu_match_grp);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "Err(%#x), ot_avp_dpu_match_destroy_grp failed\n", ret);

    return OT_SUCCESS;
}

ot_s32 sample_dpu_rect_bind_match(ot_eis_handle pip_hdl, ot_eis_handle rect_chn_handle, ot_eis_handle match_chn_handle)
{
    ot_s32 ret;
    ot_eis_media_node_port src_port;
    ot_eis_media_node_port sink_port;

    src_port.node_type = OT_EIS_MODULE_DPU_RECT;
    src_port.handle = rect_chn_handle;

    sink_port.node_type = OT_EIS_MODULE_DPU_MATCH;
    sink_port.handle = match_chn_handle;

    ret = ot_media_pipe_attach(pip_hdl, &src_port, &sink_port);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, OT_FAILURE, "Err(%#x), ot_media_pipe_attach(RECT-MATCH) failed!\n", ret);

    return OT_SUCCESS;
}

ot_s32 sample_dpu_rect_unbind_match(ot_eis_handle pip_hdl, ot_eis_handle rect_chn_handle, ot_eis_handle match_chn_handle)
{
    ot_s32 ret;
    ot_eis_media_node_port src_port;
    ot_eis_media_node_port sink_port;

    src_port.node_type = OT_EIS_MODULE_DPU_RECT;
    src_port.handle    = rect_chn_handle;

    sink_port.node_type = OT_EIS_MODULE_DPU_MATCH;
    sink_port.handle    = match_chn_handle;

    ret = ot_media_pipe_detach(pip_hdl, &src_port, &sink_port);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, OT_FAILURE, "Err(%#x), ot_media_pipe_detach(RECT-MATCH) failed!\n", ret);

    return OT_SUCCESS;
}

ot_s32 sample_dpu_get_fg_aggregate_coef_and_unique_ratio(ot_avp_dpu_match_density_accuracy_mode density_accuracy_mode, ot_u8 *aggregate_coef, ot_u8 *unique_ratio)
{
    sample_avp_dpu_check_exps_return((aggregate_coef == OT_NULL) || (unique_ratio == OT_NULL), OT_FAILURE, "aggregate_coef or unique_ratio can't be NULL\n");

    switch (density_accuracy_mode) {
        case OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D0_A11: {
            *aggregate_coef = SAMPLE_DPU_MATCH_D0_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_DPU_MATCH_A0_UNIQ_RATIO;
            break;
        }
        case OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D1_A10: {
            *aggregate_coef = SAMPLE_DPU_MATCH_D1_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_DPU_MATCH_A0_UNIQ_RATIO;
             break;
        }
        case OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D2_A9: {
            *aggregate_coef = SAMPLE_DPU_MATCH_D0_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_DPU_MATCH_A1_UNIQ_RATIO;
            break;
        }
        case OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D3_A8: {
            *aggregate_coef = SAMPLE_DPU_MATCH_D1_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_DPU_MATCH_A1_UNIQ_RATIO;
            break;
        }
        case OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D4_A7: {
            *aggregate_coef = SAMPLE_DPU_MATCH_D0_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_DPU_MATCH_A2_UNIQ_RATIO;
            break;
        }
        case OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D5_A6: {
            *aggregate_coef = SAMPLE_DPU_MATCH_D2_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_DPU_MATCH_A0_UNIQ_RATIO;
            break;
        }
        case OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D6_A5: {
            *aggregate_coef = SAMPLE_DPU_MATCH_D1_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_DPU_MATCH_A2_UNIQ_RATIO;
            break;
        }
        case OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D7_A4: {
            *aggregate_coef = SAMPLE_DPU_MATCH_D2_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_DPU_MATCH_A1_UNIQ_RATIO;
            break;
        }
        case OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D8_A3: {
            *aggregate_coef = SAMPLE_DPU_MATCH_D2_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_DPU_MATCH_A2_UNIQ_RATIO;
            break;
        }
        case OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D9_A2: {
            *aggregate_coef = SAMPLE_DPU_MATCH_D3_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_DPU_MATCH_A0_UNIQ_RATIO;
            break;
        }
        case OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D10_A1: {
            *aggregate_coef = SAMPLE_DPU_MATCH_D3_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_DPU_MATCH_A1_UNIQ_RATIO;
            break;
        }
        case OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D11_A0: {
            *aggregate_coef = SAMPLE_DPU_MATCH_D3_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_DPU_MATCH_A2_UNIQ_RATIO;
            break;
        }
        default: {
            sample_avp_dpu_trace_err("Err, density_accuracy_mode(%d) must be [%d, %d)\n", density_accuracy_mode, OT_DPU_MATCH_DENSITY_ACCURACY_MODE_D0_A11, OT_DPU_MATCH_DENSITY_ACCURACY_MODE_BUTT);
            return OT_FAILURE;
        }
    }
    return OT_SUCCESS;
}


// proc functions
static ot_s32 sample_dpu_set_match_grp_common_attr(sample_dpu_cfg *dpu_cfg, ot_u32 width, ot_u32 height)
{
    ot_s32 ret;
    ot_u32 size;

    ret = ot_avp_dpu_match_get_assist_buf_size(SAMPLE_DPU_DISPARITY_NUM, height, &size);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "get dpu match assist buffer size failed!\n");
    ret = sample_dpu_match_create_mem_info(&dpu_cfg->dpu_match_grp_attr.assist_buf, "sampe_dpu_match_assist_buf", OT_NULL, size);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "create dpu match assist buffer failed!\n");
    dpu_cfg->match_grp_hdl = OT_NULL;
    dpu_cfg->match_chn_hdl[0] = OT_NULL;
    dpu_cfg->dpu_match_grp_attr.left_image_size.width = width;
    dpu_cfg->dpu_match_grp_attr.left_image_size.height = height;
    dpu_cfg->dpu_match_grp_attr.right_image_size.width = width;
    dpu_cfg->dpu_match_grp_attr.right_image_size.height = height;
    dpu_cfg->dpu_match_grp_attr.invalid_disp = SAMPLE_DPU_INVALID_DISP;
    dpu_cfg->dpu_match_grp_attr.depth = 1;
    dpu_cfg->dpu_match_grp_attr.is_need_src_frame = OT_TRUE;
    dpu_cfg->dpu_match_grp_attr.frame_rate.src_frame_rate = SAMPLE_DPU_NO_RATE_CTRL;
    dpu_cfg->dpu_match_grp_attr.frame_rate.dst_frame_rate = SAMPLE_DPU_NO_RATE_CTRL;

    return OT_SUCCESS;
}

static ot_s32 sample_dpu_set_match_grp_disp_attr(sample_dpu_cfg *dpu_cfg)
{
    dpu_cfg->dpu_match_grp_attr.disp_param.version = OT_AVP_DPU_MATCH_DISP_VERSION_V1;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.is_proc_disp = OT_TRUE;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.is_output_disp = OT_TRUE;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.is_output_conf = OT_TRUE;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.is_post_proc = OT_TRUE;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.is_filter_disp_by_conf = OT_TRUE;//OT_TRUE;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.is_time_domain_filter = OT_TRUE;//OT_TRUE;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.is_enable_disp_subpixel = OT_TRUE;//OT_TRUE;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.mask_mode = OT_AVP_DPU_MATCH_MASK_DEFAULT_MODE;//OT_AVP_DPU_MATCH_MASK_11X11_MODE;//OT_AVP_DPU_MATCH_MASK_9X9_MODE;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.speed_accuracy_mode = OT_AVP_DPU_MATCH_SPEED_ACCURACY_MODE_ACCURACY;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.bg_mode = OT_AVP_DPU_MATCH_BG_MODE_NORMAL_LIGHT;//OT_AVP_DPU_MATCH_BG_MODE_LOW_LIGHT OT_AVP_DPU_MATCH_BG_MODE_NONE OT_AVP_DPU_MATCH_BG_MODE_HIGH_LIGHT
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.opti_mode = OT_AVP_DPU_MATCH_OPTI_MODE_ADAPTIVE;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.disp_num = SAMPLE_DPU_DISPARITY_NUM;  // 64
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.disp_start_pos = SAMPLE_UVC_DEPU_START_POS;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.first_penalty_coef = 4;  // 4  SAMPLE_AVP_DPU_MATCH_FIRST_PENALTY_COEF
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.second_penalty_coef = 64; // 64;  SAMPLE_AVP_DPU_MATCH_SECOND_PENALTY_COEF
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.fg_init_cost_zero_thr = 8;  // 8;
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.local_feature = 2;  // 2
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.conf_filter_disp_thr = 2;  // 2;  10
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.aggregate_coef = 8;   // 8;  // 7; //
    dpu_cfg->dpu_match_grp_attr.disp_param.v1.unique_ratio = 31; // 31; //
    dpu_cfg->dpu_match_grp_attr.is_need_src_frame = OT_TRUE;

    return OT_SUCCESS;
}

static ot_void sample_dpu_set_match_grp_depth_attr(sample_dpu_cfg *dpu_cfg)
{
    dpu_cfg->dpu_match_grp_attr.depth_param.version = OT_AVP_DPU_MATCH_DEPTH_VERSION_V1;
    dpu_cfg->dpu_match_grp_attr.depth_param.v1.is_output_depth = OT_TRUE;
    dpu_cfg->dpu_match_grp_attr.depth_param.v1.disp_from = OT_AVP_DPU_MATCH_DISP_FROM_MATCH;
    dpu_cfg->dpu_match_grp_attr.depth_param.v1.fb_norm_bits = SAMPLE_DPU_MATCH_FB_NORM_BITS;
    dpu_cfg->dpu_match_grp_attr.depth_param.v1.fb = (ot_u32)(1142 * 159.1 * (1 << SAMPLE_DPU_MATCH_FB_NORM_BITS) + 0.5);
    dpu_cfg->dpu_match_grp_attr.depth_param.v1.z0 = SAMPLE_DPU_MATCH_Z0;
    dpu_cfg->dpu_match_grp_attr.depth_param.v1.min_depth_thr = 100;
    dpu_cfg->dpu_match_grp_attr.depth_param.v1.max_depth_thr = 10000;
    dpu_cfg->dpu_match_grp_attr.depth_param.v1.invalid_depth = SAMPLE_DPU_MATCH_INVALID_DEPTH;
    dpu_cfg->dpu_match_grp_attr.depth_param.v1.disp_coef = 1;
    dpu_cfg->dpu_match_grp_attr.depth_param.v1.depth_prec = dpu_cfg->dpu_match_grp_attr.depth_param.v1.fb_norm_bits - SAMPLE_DPU_MATCH_FB_NORM_BITS_8;
}

ot_s32 sample_dpu_rect_init(sample_dpu_cfg *dpu_cfg, const ot_eis_img_size *src_size, const ot_eis_img_size *dst_size, const ot_char *lut_file[], ot_u32 file_num)
{
    ot_s32 ret;
    ot_u32 i;
    ot_u32 rect_pipe_num = 0;
    ot_avp_dpu_rect_chn_attr rect_chn_attr[OT_AVP_DPU_RECT_MAX_CHN_NUM] = {0};
    ot_char path[AVP_DPU_PATH_MAX + 1] = {0};

    for (i = 0; i < file_num; i++) {
        sample_avp_dpu_check_exps_return(((strlen(lut_file[i]) > AVP_DPU_PATH_MAX) || (realpath(lut_file[i], path) == OT_NULL)), OT_FAILURE, "Error, file_name is invalid!\n");

        ret = sample_dpu_rect_load_lut(lut_file[i], &dpu_cfg->dpu_rect_mem_info[i], &dpu_cfg->lut_id[i]);
        sample_avp_dpu_check_exps_goto(ret != OT_SUCCESS, fail_0, "load lut failed!\n");
        rect_pipe_num++;
    }

    dpu_cfg->dpu_rect_grp_attr.left_attr.lut_id = dpu_cfg->lut_id[0];
    dpu_cfg->dpu_rect_grp_attr.right_attr.lut_id = dpu_cfg->lut_id[1];
    dpu_cfg->rect_grp_hdl = OT_NULL;
    dpu_cfg->dpu_rect_grp_attr.left_attr.image_size.width = src_size->width;
    dpu_cfg->dpu_rect_grp_attr.left_attr.image_size.height = src_size->height;
    dpu_cfg->dpu_rect_grp_attr.right_attr.image_size.width = src_size->width;
    dpu_cfg->dpu_rect_grp_attr.right_attr.image_size.height = src_size->height;
    dpu_cfg->dpu_rect_grp_attr.rect_mode = OT_AVP_DPU_RECT_MODE_DOUBLE;
    dpu_cfg->dpu_rect_grp_attr.depth = 1;
    dpu_cfg->dpu_rect_grp_attr.is_need_src_frame = OT_TRUE;
    dpu_cfg->dpu_rect_grp_attr.frame_rate.src_frame_rate = SAMPLE_DPU_NO_RATE_CTRL;
    dpu_cfg->dpu_rect_grp_attr.frame_rate.dst_frame_rate = SAMPLE_DPU_NO_RATE_CTRL;

    for (i = 0; i < OT_AVP_DPU_RECT_MAX_CHN_NUM; i++) {
        rect_chn_attr[i].image_size.width = dst_size->width;
        rect_chn_attr[i].image_size.height = dst_size->height;
        dpu_cfg->rect_chn_hdl[i] = OT_NULL;
    }

    ret = sample_dpu_rect_start(&dpu_cfg->rect_grp_hdl, &dpu_cfg->dpu_rect_grp_attr, rect_chn_attr, dpu_cfg->rect_chn_hdl);
    sample_avp_dpu_check_exps_goto(ret != OT_SUCCESS, fail_1, "start dpu rect failed!\n");

    return OT_SUCCESS;
fail_1:
    (ot_void)sample_dpu_rect_stop(dpu_cfg->rect_grp_hdl, dpu_cfg->rect_chn_hdl, dpu_cfg->dpu_rect_grp_attr.rect_mode);
fail_0:
    for (i = 0; i < rect_pipe_num; i++) {
        (ot_void)sample_dpu_rect_unload_lut(&dpu_cfg->dpu_rect_mem_info[i], dpu_cfg->lut_id[i]);
    }
    return ret;
}

ot_void sample_dpu_rect_deinit(sample_dpu_cfg *dpu_cfg)
{
    ot_u32 i;

    (ot_void)sample_dpu_rect_stop(dpu_cfg->rect_grp_hdl, dpu_cfg->rect_chn_hdl, dpu_cfg->dpu_rect_grp_attr.rect_mode);
    for (i = 0; i < OT_AVP_DPU_RECT_MAX_PIPE_NUM; i++) {
        (ot_void)sample_dpu_rect_unload_lut(&dpu_cfg->dpu_rect_mem_info[i], dpu_cfg->lut_id[i]);
    }
    return;
}

ot_s32 sample_dpu_match_init(sample_dpu_cfg *dpu_cfg, ot_eis_img_size *match_size)
{
    ot_s32 ret;
    ot_avp_dpu_match_chn_attr match_chn_attr = {0};
    ret = sample_dpu_set_match_grp_common_attr(dpu_cfg, match_size->width, match_size->height);
    sample_avp_dpu_check_exps_return(ret != OT_SUCCESS, ret, "set match grp common attr failed!\n");

    ret = sample_dpu_set_match_grp_disp_attr(dpu_cfg);
    sample_avp_dpu_check_exps_goto(ret != OT_SUCCESS, fail, "set match grp disp attr failed!\n");
    sample_dpu_set_match_grp_depth_attr(dpu_cfg);

    match_chn_attr.image_size.width  = match_size->width;
    match_chn_attr.image_size.height = match_size->height;
    ret = sample_dpu_match_start(&dpu_cfg->match_grp_hdl, dpu_cfg->match_chn_hdl, &dpu_cfg->dpu_match_grp_attr, &match_chn_attr);
    sample_avp_dpu_check_exps_goto(ret != OT_SUCCESS, fail, "dpu match start failed!\n");
    return OT_SUCCESS;

fail:
    (ot_void)sample_dpu_match_stop(dpu_cfg->match_grp_hdl, dpu_cfg->match_chn_hdl);
    sample_avp_dpu_smr_free(dpu_cfg->dpu_match_grp_attr.assist_buf.phys_addr);
    return ret;
}

ot_void sample_dpu_match_deinit(sample_dpu_cfg *dpu_cfg)
{
    (ot_void)sample_dpu_match_stop(dpu_cfg->match_grp_hdl, dpu_cfg->match_chn_hdl);
    sample_avp_dpu_smr_free(dpu_cfg->dpu_match_grp_attr.assist_buf.phys_addr);
}
