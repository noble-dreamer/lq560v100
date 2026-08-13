#ifndef __MEDIA_TRANS_COMMON_UTILS_H__
#define __MEDIA_TRANS_COMMON_UTILS_H__


#include "ot_common_video.h"
#include "ot_eis_video.h"

#ifdef __cplusplus
extern "C" {
#endif
ot_eis_module_id media_ot_mod_id_transform(ot_mod_id mod_id);
ot_mod_id media_ot_eis_module_id_transform(ot_eis_module_id mod_id);

ot_pixel_format media_ot_eis_img_pixel_format_transform(ot_eis_img_pixel_format pixel_format);
ot_eis_img_pixel_format media_ot_pixel_format_transform(ot_pixel_format pixel_format);

ot_video_format media_ot_eis_img_layout_transform(ot_eis_img_layout video_format);
ot_eis_img_layout media_ot_video_format_transform(ot_video_format video_format);

ot_compress_mode media_ot_eis_img_compress_mode_transform(ot_eis_img_compress_mode compress_mode);
ot_eis_img_compress_mode media_ot_compress_mode_transform(ot_compress_mode compress_mode);

ot_color_gamut media_ot_eis_img_color_space_transform(ot_eis_img_color_space color_gamut);
ot_eis_img_color_space media_ot_color_gamut_transform(ot_color_gamut color_gamut);

ot_coord media_ot_eis_coord_type_transform(ot_eis_coord_type key);
ot_eis_coord_type media_ot_coord_transform(ot_coord key);

ot_s32 media_ot_video_frame_info_transform(const ot_video_frame_info *frame_src, ot_eis_img_frame *frame_dst);
ot_s32 media_ot_eis_img_frame_transform(const ot_eis_img_frame *frame_src, ot_video_frame_info *frame_dst);

ot_eis_img_point media_ot_eis_img_point_transform(const ot_point point);
ot_point media_ot_point_transform(const ot_eis_img_point point);

ot_eis_img_rect media_ot_eis_img_rect_transform(const ot_rect rect);
ot_rect media_ot_rect_transform(const ot_eis_img_rect rect);

#ifdef __cplusplus
}
#endif

#endif