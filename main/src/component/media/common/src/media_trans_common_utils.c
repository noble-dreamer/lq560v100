
#include "media_trans_common_utils.h"
#include "string.h"
#include "ot_eis_errno.h"

// TODO 补充映射
ot_eis_module_id media_ot_mod_id_transform(ot_mod_id mod_id)
{
    switch (mod_id)
    {
    case OT_ID_VI:
        return OT_EIS_MODULE_VI;
        break;
    case OT_ID_VPROC:
        return OT_EIS_MODULE_VPROC;
        break;
    case OT_ID_VO:
        return OT_EIS_MODULE_VO;
        break;

    default:
        return OT_EIS_MODULE_INVALID;
        break;
    }
}

ot_mod_id media_ot_eis_module_id_transform(ot_eis_module_id mod_id)
{
    switch (mod_id)
    {
    case OT_EIS_MODULE_VI:
        return OT_ID_VI;
        break;
    case OT_EIS_MODULE_VPROC:
        return OT_ID_VPROC;
        break;
    case OT_EIS_MODULE_VO:
        return OT_ID_VO;
        break;

    default:
        return OT_ID_BUTT;
        break;
    }
}

// TODO 补充映射
ot_pixel_format media_ot_eis_img_pixel_format_transform(ot_eis_img_pixel_format pixel_format)
{
    switch(pixel_format) {
        case OT_EIS_IMAGE_FORMAT_BAYER_8BPP:
            return OT_PIXEL_FORMAT_RGB_BAYER_8BPP;
        case OT_EIS_IMAGE_FORMAT_BAYER_10BPP:
            return OT_PIXEL_FORMAT_RGB_BAYER_10BPP;
        case OT_EIS_IMAGE_FORMAT_BAYER_12BPP:
            return OT_PIXEL_FORMAT_RGB_BAYER_12BPP;
        case OT_EIS_IMAGE_FORMAT_BAYER_14BPP:
            return OT_PIXEL_FORMAT_RGB_BAYER_14BPP;
        case OT_EIS_IMAGE_FORMAT_BAYER_16BPP:
            return OT_PIXEL_FORMAT_RGB_BAYER_16BPP;
        case OT_EIS_IMAGE_FORMAT_RGB_444:
            return OT_PIXEL_FORMAT_RGB_444;
        case OT_EIS_IMAGE_FORMAT_RGB_555:
            return OT_PIXEL_FORMAT_RGB_555;
        case OT_EIS_IMAGE_FORMAT_RGB_565:
            return OT_PIXEL_FORMAT_RGB_565;
        case OT_EIS_IMAGE_FORMAT_RGB_888:
            return OT_PIXEL_FORMAT_RGB_888;

        case OT_EIS_IMAGE_FORMAT_BGR_444:
            return OT_PIXEL_FORMAT_BGR_444;
        case OT_EIS_IMAGE_FORMAT_BGR_555:
            return OT_PIXEL_FORMAT_BGR_555;
        case OT_EIS_IMAGE_FORMAT_BGR_565:
            return OT_PIXEL_FORMAT_BGR_565;
        case OT_EIS_IMAGE_FORMAT_BGR_888:
            return OT_PIXEL_FORMAT_BGR_888;

        case OT_EIS_IMAGE_FORMAT_ARGB_1555:
            return OT_PIXEL_FORMAT_ARGB_1555;
        case OT_EIS_IMAGE_FORMAT_ARGB_4444:
            return OT_PIXEL_FORMAT_ARGB_4444;
        case OT_EIS_IMAGE_FORMAT_ARGB_8888:
            return OT_PIXEL_FORMAT_ARGB_8888;

        case OT_EIS_IMAGE_FORMAT_ABGR_1555:
            return OT_PIXEL_FORMAT_ABGR_1555;
        case OT_EIS_IMAGE_FORMAT_ABGR_4444:
            return OT_PIXEL_FORMAT_ABGR_4444;
        case OT_EIS_IMAGE_FORMAT_ABGR_8888:
            return OT_PIXEL_FORMAT_ABGR_8888;

        case OT_EIS_IMAGE_FORMAT_YUV_422_SEMIPLANAR:
            return OT_PIXEL_FORMAT_YUV_SEMIPLANAR_422;
        case OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_YUYV:
            return OT_PIXEL_FORMAT_YUYV_PACKAGE_422;
        case OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_YVYU:
            return OT_PIXEL_FORMAT_YVYU_PACKAGE_422;
        case OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_YYUV:
            return OT_PIXEL_FORMAT_YYUV_PACKAGE_422;
        case OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_YYVU:
            return OT_PIXEL_FORMAT_YYVU_PACKAGE_422;
        case OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_UYVY:
            return OT_PIXEL_FORMAT_UYVY_PACKAGE_422;
        case OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_UVYY:
            return OT_PIXEL_FORMAT_UVYY_PACKAGE_422;
        case OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_VYUY:
            return OT_PIXEL_FORMAT_VYUY_PACKAGE_422;
        case OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_VUYY:
            return OT_PIXEL_FORMAT_VUYY_PACKAGE_422;

        case OT_EIS_IMAGE_FORMAT_YUV_420_SEMIPLANAR:
            return OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        case OT_EIS_IMAGE_FORMAT_YVU_422_PLANAR:
            return OT_PIXEL_FORMAT_YVU_PLANAR_422;
        case OT_EIS_IMAGE_FORMAT_YVU_422_SEMIPLANAR:
            return OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422;

        case OT_EIS_IMAGE_FORMAT_YVU_420_PLANAR:
            return OT_PIXEL_FORMAT_YVU_PLANAR_420;
        case OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR:
            return OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420;

        case OT_EIS_IMAGE_FORMAT_YUV_400:
            return OT_PIXEL_FORMAT_YUV_400;
        default:
            break;
    }
    return OT_PIXEL_FORMAT_BUTT;
}

//TODO： 补充映射
ot_eis_img_pixel_format media_ot_pixel_format_transform(ot_pixel_format pixel_format)
{
    switch (pixel_format) {
        case OT_PIXEL_FORMAT_RGB_444:
            return OT_EIS_IMAGE_FORMAT_RGB_444;
        case OT_PIXEL_FORMAT_RGB_555:
            return OT_EIS_IMAGE_FORMAT_RGB_555;
        case OT_PIXEL_FORMAT_RGB_565:
            return OT_EIS_IMAGE_FORMAT_RGB_565;
        case OT_PIXEL_FORMAT_RGB_888:
            return OT_EIS_IMAGE_FORMAT_RGB_888;

        case OT_PIXEL_FORMAT_BGR_444:
            return OT_EIS_IMAGE_FORMAT_BGR_444;
        case OT_PIXEL_FORMAT_BGR_555:
            return OT_EIS_IMAGE_FORMAT_BGR_555;
        case OT_PIXEL_FORMAT_BGR_565:
            return OT_EIS_IMAGE_FORMAT_BGR_565;
        case OT_PIXEL_FORMAT_BGR_888:
            return OT_EIS_IMAGE_FORMAT_BGR_888;

        case OT_PIXEL_FORMAT_ARGB_1555:
            return OT_EIS_IMAGE_FORMAT_ARGB_1555;
        case OT_PIXEL_FORMAT_ARGB_4444:
            return OT_EIS_IMAGE_FORMAT_ARGB_4444;
        case OT_PIXEL_FORMAT_ARGB_8888:
            return OT_EIS_IMAGE_FORMAT_ARGB_8888;

        case OT_PIXEL_FORMAT_ABGR_1555:
            return OT_EIS_IMAGE_FORMAT_ABGR_1555;
        case OT_PIXEL_FORMAT_ABGR_4444:
            return OT_EIS_IMAGE_FORMAT_ABGR_4444;
        case OT_PIXEL_FORMAT_ABGR_8888:
            return OT_EIS_IMAGE_FORMAT_ABGR_8888;

        case OT_PIXEL_FORMAT_RGB_BAYER_8BPP:
            return OT_EIS_IMAGE_FORMAT_BAYER_8BPP;
        case OT_PIXEL_FORMAT_RGB_BAYER_10BPP:
            return OT_EIS_IMAGE_FORMAT_BAYER_10BPP;
        case OT_PIXEL_FORMAT_RGB_BAYER_12BPP:
            return OT_EIS_IMAGE_FORMAT_BAYER_12BPP;
        case OT_PIXEL_FORMAT_RGB_BAYER_14BPP:
            return OT_EIS_IMAGE_FORMAT_BAYER_14BPP;
        case OT_PIXEL_FORMAT_RGB_BAYER_16BPP:
            return OT_EIS_IMAGE_FORMAT_BAYER_16BPP;

        case OT_PIXEL_FORMAT_YVU_PLANAR_422:
            return OT_EIS_IMAGE_FORMAT_YVU_422_PLANAR;
        case OT_PIXEL_FORMAT_YVU_PLANAR_420:
            return OT_EIS_IMAGE_FORMAT_YVU_420_PLANAR;

        case OT_PIXEL_FORMAT_YVU_SEMIPLANAR_422:
            return OT_EIS_IMAGE_FORMAT_YVU_422_SEMIPLANAR;
        case OT_PIXEL_FORMAT_YVU_SEMIPLANAR_420:
            return OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR;
        case OT_PIXEL_FORMAT_YUV_SEMIPLANAR_422:
            return OT_EIS_IMAGE_FORMAT_YUV_422_SEMIPLANAR;
        case OT_PIXEL_FORMAT_YUV_SEMIPLANAR_420:
            return OT_EIS_IMAGE_FORMAT_YUV_420_SEMIPLANAR;

        case OT_PIXEL_FORMAT_YUYV_PACKAGE_422:
            return OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_YUYV;
        case OT_PIXEL_FORMAT_YVYU_PACKAGE_422:
            return OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_YVYU;
        case OT_PIXEL_FORMAT_UYVY_PACKAGE_422:
            return OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_UYVY;
        case OT_PIXEL_FORMAT_VYUY_PACKAGE_422:
            return OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_VYUY;
        case OT_PIXEL_FORMAT_YYUV_PACKAGE_422:
            return OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_YYUV;
        case OT_PIXEL_FORMAT_YYVU_PACKAGE_422:
            return OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_YYVU;
        case OT_PIXEL_FORMAT_UVYY_PACKAGE_422:
            return OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_UVYY;
        case OT_PIXEL_FORMAT_VUYY_PACKAGE_422:
            return OT_EIS_IMAGE_FORMAT_YUV_422_PACKED_VUYY;

        case OT_PIXEL_FORMAT_YUV_400:
            return OT_EIS_IMAGE_FORMAT_YUV_400;

        default:
            break;
    }

    return OT_EIS_IMAGE_FORMAT_BUTT;
}

ot_video_format media_ot_eis_img_layout_transform(ot_eis_img_layout video_format)
{
    switch (video_format)
    {
    case OT_EIS_IMAGE_LAYOUT_LINEAR:
        return OT_VIDEO_FORMAT_LINEAR;
        break;
    case OT_EIS_IMAGE_LAYOUT_TILE_64x16:
        return OT_VIDEO_FORMAT_TILE_64x16;
        break;

    default:
        return OT_VIDEO_FORMAT_BUTT;
        break;
    }
}

//只取同时支持的LINEAR格式
ot_eis_img_layout media_ot_video_format_transform(ot_video_format video_format)
{
    switch (video_format)
    {
    case OT_VIDEO_FORMAT_LINEAR:
        return OT_EIS_IMAGE_LAYOUT_LINEAR;
        break;
    case OT_VIDEO_FORMAT_TILE_64x16:
        return OT_EIS_IMAGE_LAYOUT_TILE_64x16;
        break;
    default:
        return OT_EIS_IMAGE_LAYOUT_BUTT;
        break;
    }
}

ot_compress_mode media_ot_eis_img_compress_mode_transform(ot_eis_img_compress_mode compress_mode)
{
    switch (compress_mode)
    {
    case OT_EIS_IMAGE_COMPRESS_MODE_NONE:
        return OT_COMPRESS_MODE_NONE;
        break;
    case OT_EIS_IMAGE_COMPRESS_MODE_SEG:
        return OT_COMPRESS_MODE_SEG;
        break;
    case OT_EIS_IMAGE_COMPRESS_MODE_LINE:
        return OT_COMPRESS_MODE_LINE;
        break;
    case OT_EIS_IMAGE_COMPRESS_MODE_FRAME:
        return OT_COMPRESS_MODE_FRAME;
        break;

    default:
        return OT_COMPRESS_MODE_BUTT;
        break;
    }
}

ot_eis_img_compress_mode media_ot_compress_mode_transform(ot_compress_mode compress_mode)
{
    switch (compress_mode)
    {
    case OT_COMPRESS_MODE_NONE:
        return OT_EIS_IMAGE_COMPRESS_MODE_NONE;
        break;
    case OT_COMPRESS_MODE_SEG:
        return OT_EIS_IMAGE_COMPRESS_MODE_SEG;
        break;
    case OT_COMPRESS_MODE_LINE:
        return OT_EIS_IMAGE_COMPRESS_MODE_LINE;
        break;
    case OT_COMPRESS_MODE_FRAME:
        return OT_EIS_IMAGE_COMPRESS_MODE_FRAME;
        break;

    default:
        return OT_EIS_IMAGE_COMPRESS_MODE_BUTT;
        break;
    }
}

ot_color_gamut media_ot_eis_img_color_space_transform(ot_eis_img_color_space color_gamut)
{

    switch (color_gamut)
    {
    case OT_EIS_IMAGE_COLOR_SPACE_BT601:
        return OT_COLOR_GAMUT_BT601;
        break;
    case OT_EIS_IMAGE_COLOR_SPACE_BT709:
        return OT_COLOR_GAMUT_BT709;
        break;
    case OT_EIS_IMAGE_COLOR_SPACE_USER:
        return OT_COLOR_GAMUT_USER;
        break;
    default:
        return OT_COLOR_GAMUT_BUTT;
        break;
    }
}

ot_eis_img_color_space media_ot_color_gamut_transform(ot_color_gamut color_gamut)
{

    switch (color_gamut)
    {
    case OT_COLOR_GAMUT_BT601:
        return OT_EIS_IMAGE_COLOR_SPACE_BT601;
        break;
    case OT_COLOR_GAMUT_BT709:
        return OT_EIS_IMAGE_COLOR_SPACE_BT709;
        break;
    case OT_COLOR_GAMUT_USER:
        return OT_EIS_IMAGE_COLOR_SPACE_USER;
        break;
    default:
        return OT_EIS_IMAGE_COLOR_SPACE_BUTT;
        break;
    }
}

ot_coord media_ot_eis_coord_type_transform(ot_eis_coord_type key)
{
    switch (key)
    {
        case OT_EIS_COORD_ABS:
            return OT_COORD_ABS;

        case OT_EIS_COORD_RATIO:
            return OT_COORD_RATIO;

        default:
            break;
    }

    return OT_COORD_BUTT;
}

ot_eis_coord_type media_ot_coord_transform(ot_coord key)
{
    switch (key)
    {
        case OT_COORD_ABS:
            return OT_EIS_COORD_ABS;

        case OT_COORD_RATIO:
            return OT_EIS_COORD_RATIO;

        default:
            break;
    }

    return OT_EIS_COORD_BUTT;
}

ot_s32 media_ot_video_frame_info_transform(const ot_video_frame_info *frame_src, ot_eis_img_frame *frame_dst)
{
    frame_dst->index = frame_src->video_frame.time_ref;
    frame_dst->pts = frame_src->video_frame.pts;

    frame_dst->attr.width = frame_src->video_frame.width;
    frame_dst->attr.height = frame_src->video_frame.height;
    frame_dst->attr.pixel_fmt = media_ot_pixel_format_transform(frame_src->video_frame.pixel_format);
    frame_dst->attr.layout = media_ot_video_format_transform(frame_src->video_frame.video_format);
    frame_dst->attr.compress_mode = media_ot_compress_mode_transform(frame_src->video_frame.compress_mode);

    memcpy(frame_dst->buff.phys_addr, frame_src->video_frame.phys_addr, OT_MAX_COLOR_COMPONENT*sizeof(ot_phys_addr_t));
    memcpy(frame_dst->buff.virt_addr, frame_src->video_frame.virt_addr, OT_MAX_COLOR_COMPONENT*sizeof(ot_void*));
    memcpy(frame_dst->buff.stride, frame_src->video_frame.stride, OT_MAX_COLOR_COMPONENT*sizeof(ot_u32));

    memcpy(frame_dst->header.phys_addr, frame_src->video_frame.header_phys_addr, OT_MAX_COLOR_COMPONENT*sizeof(ot_phys_addr_t));
    memcpy(frame_dst->header.virt_addr, frame_src->video_frame.header_virt_addr, OT_MAX_COLOR_COMPONENT*sizeof(ot_void*));
    memcpy(frame_dst->header.stride, frame_src->video_frame.header_stride, OT_MAX_COLOR_COMPONENT*sizeof(ot_u32));

    frame_dst->mod_id = media_ot_mod_id_transform(frame_src->mod_id);
    frame_dst->pool_hdl = (ot_eis_handle)((uintptr_t)frame_src->pool_id);
    frame_dst->color_space = media_ot_color_gamut_transform(frame_src->video_frame.color_gamut);

    frame_dst->extra_attr.misc_attr_phys_addr = frame_src->video_frame.supplement.misc_info_phys_addr;
    frame_dst->extra_attr.jpeg_dcf_phys_addr = frame_src->video_frame.supplement.jpeg_dcf_phys_addr;
    frame_dst->extra_attr.isp_attr_phys_addr = frame_src->video_frame.supplement.isp_info_phys_addr;
    frame_dst->extra_attr.low_delay_phys_addr = frame_src->video_frame.supplement.low_delay_phys_addr;
    frame_dst->extra_attr.bnr_mot_phys_addr = frame_src->video_frame.supplement.bnr_mot_phys_addr;
    frame_dst->extra_attr.rsv1_phys_addr = frame_src->video_frame.supplement.motion_data_phys_addr;
    frame_dst->extra_attr.rsv2_phys_addr = frame_src->video_frame.supplement.frame_dng_phys_addr;
    frame_dst->extra_attr.isp_phys_addr = frame_src->video_frame.supplement.aiisp_phys_addr;

    frame_dst->extra_attr.misc_attr_virt_addr = frame_src->video_frame.supplement.misc_info_virt_addr;
    frame_dst->extra_attr.jpeg_dcf_virt_addr = frame_src->video_frame.supplement.jpeg_dcf_virt_addr;
    frame_dst->extra_attr.isp_attr_virt_addr = frame_src->video_frame.supplement.isp_info_virt_addr;
    frame_dst->extra_attr.low_delay_virt_addr = frame_src->video_frame.supplement.low_delay_virt_addr;
    frame_dst->extra_attr.bnr_mot_virt_addr = frame_src->video_frame.supplement.bnr_mot_virt_addr;
    frame_dst->extra_attr.rsv1_virt_addr = frame_src->video_frame.supplement.motion_data_virt_addr;
    frame_dst->extra_attr.rsv2_virt_addr = frame_src->video_frame.supplement.frame_dng_virt_addr;
    frame_dst->extra_attr.isp_virt_addr = frame_src->video_frame.supplement.aiisp_virt_addr;

    frame_dst->frame_flag = frame_src->video_frame.frame_flag;

    return OT_SUCCESS;
}

ot_s32 media_ot_eis_img_frame_transform(const ot_eis_img_frame *frame_src, ot_video_frame_info *frame_dst)
{
    frame_dst->pool_id = (ot_u32)((uintptr_t)frame_src->pool_hdl);
    frame_dst->mod_id = media_ot_eis_module_id_transform(frame_src->mod_id);

    frame_dst->video_frame.width = frame_src->attr.width;
    frame_dst->video_frame.height = frame_src->attr.height;
    frame_dst->video_frame.field = OT_VIDEO_FIELD_FRAME;
    frame_dst->video_frame.pixel_format = media_ot_eis_img_pixel_format_transform(frame_src->attr.pixel_fmt);
    frame_dst->video_frame.video_format = media_ot_eis_img_layout_transform(frame_src->attr.layout);
    frame_dst->video_frame.compress_mode = media_ot_eis_img_compress_mode_transform(frame_src->attr.compress_mode);
    frame_dst->video_frame.dynamic_range = OT_DYNAMIC_RANGE_SDR8;
    frame_dst->video_frame.color_gamut = media_ot_eis_img_color_space_transform(frame_src->color_space);

    memcpy(frame_dst->video_frame.phys_addr, frame_src->buff.phys_addr, OT_MAX_COLOR_COMPONENT*sizeof(ot_phys_addr_t));
    memcpy(frame_dst->video_frame.virt_addr, frame_src->buff.virt_addr, OT_MAX_COLOR_COMPONENT*sizeof(ot_void*));
    memcpy(frame_dst->video_frame.stride, frame_src->buff.stride, OT_MAX_COLOR_COMPONENT*sizeof(ot_u32));

    memcpy(frame_dst->video_frame.header_phys_addr, frame_src->header.phys_addr, OT_MAX_COLOR_COMPONENT*sizeof(ot_phys_addr_t));
    memcpy(frame_dst->video_frame.header_virt_addr, frame_src->header.virt_addr, OT_MAX_COLOR_COMPONENT*sizeof(ot_void*));
    memcpy(frame_dst->video_frame.header_stride, frame_src->header.stride, OT_MAX_COLOR_COMPONENT*sizeof(ot_u32));

    frame_dst->video_frame.time_ref = frame_src->index;
    frame_dst->video_frame.pts = frame_src->pts;

    // 不赋值字段
    // frame_dst->video_frame.user_data

    frame_dst->video_frame.frame_flag = frame_src->frame_flag;

    //supplement
    frame_dst->video_frame.supplement.misc_info_phys_addr = frame_src->extra_attr.misc_attr_phys_addr;
    frame_dst->video_frame.supplement.jpeg_dcf_phys_addr = frame_src->extra_attr.jpeg_dcf_phys_addr;
    frame_dst->video_frame.supplement.isp_info_phys_addr = frame_src->extra_attr.isp_attr_phys_addr;
    frame_dst->video_frame.supplement.low_delay_phys_addr = frame_src->extra_attr.low_delay_phys_addr;
    frame_dst->video_frame.supplement.bnr_mot_phys_addr = frame_src->extra_attr.bnr_mot_phys_addr;
    frame_dst->video_frame.supplement.motion_data_phys_addr = frame_src->extra_attr.rsv1_phys_addr;
    frame_dst->video_frame.supplement.frame_dng_phys_addr = frame_src->extra_attr.rsv2_phys_addr ;
    frame_dst->video_frame.supplement.aiisp_phys_addr = frame_src->extra_attr.isp_phys_addr ;

    frame_dst->video_frame.supplement.misc_info_virt_addr = frame_src->extra_attr.misc_attr_virt_addr;
    frame_dst->video_frame.supplement.jpeg_dcf_virt_addr = frame_src->extra_attr.jpeg_dcf_virt_addr;
    frame_dst->video_frame.supplement.isp_info_virt_addr = frame_src->extra_attr.isp_attr_virt_addr;
    frame_dst->video_frame.supplement.low_delay_virt_addr = frame_src->extra_attr.low_delay_virt_addr;
    frame_dst->video_frame.supplement.bnr_mot_virt_addr	= frame_src->extra_attr.bnr_mot_virt_addr;
    frame_dst->video_frame.supplement.motion_data_virt_addr	= frame_src->extra_attr.rsv1_virt_addr;
    frame_dst->video_frame.supplement.frame_dng_virt_addr = frame_src->extra_attr.rsv2_virt_addr;
    frame_dst->video_frame.supplement.aiisp_virt_addr = frame_src->extra_attr.isp_virt_addr;

    return OT_SUCCESS;
}
ot_eis_img_point media_ot_eis_img_point_transform(const ot_point point)
{
    ot_eis_img_point ret;

    ret.x = point.x;
    ret.y = point.y;

    return ret;
}

ot_point media_ot_point_transform(const ot_eis_img_point point)
{
    ot_point ret;

    ret.x = point.x;
    ret.y = point.y;

    return ret;
}

ot_eis_img_rect media_ot_eis_img_rect_transform(const ot_rect rect)
{
    ot_eis_img_rect ret;

    ret.x = rect.x;
    ret.y = rect.y;
    ret.width = rect.width;
    ret.height = rect.height;

    return ret;
}

ot_rect media_ot_rect_transform(const ot_eis_img_rect rect)
{
    ot_rect ret;

    ret.x = rect.x;
    ret.y = rect.y;
    ret.width = rect.width;
    ret.height = rect.height;

    return ret;
}

