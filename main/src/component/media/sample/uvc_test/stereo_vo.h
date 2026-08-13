#ifndef STEREO_VO_H
#define STEREO_VO_H

#include "sample_comm.h"

typedef struct {
    ot_bool in_port_enable[OT_VO_MAX_BIND_PORT_NUM];
    ot_eis_handle in_port_handle[OT_VO_MAX_BIND_PORT_NUM];
    ot_eis_vo_in_port_attr in_port_attr[OT_VO_MAX_BIND_PORT_NUM];
    ot_eis_vo_surface_bind_attr surface_bind_attr[OT_VO_MAX_BIND_PORT_NUM];
    ot_eis_vo_mirror_attr mirror_attr[OT_VO_MAX_BIND_PORT_NUM];
    ot_eis_vo_rot_attr rot_attr[OT_VO_MAX_BIND_PORT_NUM];

    ot_bool surface_enable[OT_VO_MAX_BIND_SURF_NUM];
    ot_eis_handle surface_handle[OT_VO_MAX_BIND_SURF_NUM];
    ot_eis_vo_surface surface[OT_VO_MAX_BIND_SURF_NUM];
    ot_eis_vo_surface_attr surface_attr[OT_VO_MAX_BIND_SURF_NUM];
    ot_eis_vo_crop_attr crop_attr[OT_VO_MAX_BIND_SURF_NUM];
    ot_eis_vo_mirror_attr surface_mirror_attr[OT_VO_MAX_BIND_SURF_NUM];
    ot_eis_vo_csc_attr csc_attr[OT_VO_MAX_BIND_SURF_NUM];
    ot_eis_vo_display_bind_attr display_bind_attr[OT_VO_MAX_BIND_SURF_NUM];

    ot_bool display_enable[OT_VO_MAX_DISP_NUM];
    ot_eis_handle display_handle[OT_VO_MAX_DISP_NUM];
    ot_eis_vo_display display[OT_VO_MAX_DISP_NUM];
    ot_eis_vo_display_attr display_attr[OT_VO_MAX_DISP_NUM];
    ot_eis_handle mipi_tx_hdl;
} stereo_vo_attr;


void stereo_vo_get_attr(stereo_vo_attr* vo_attr);
ot_s32 stereo_vo_start(stereo_vo_attr *vo_attr);
ot_s32 stereo_vo_stop(stereo_vo_attr *vo_attr);

ot_s32 stereo_vo_acquire_frame(ot_eis_handle pool, ot_eis_video_frame *frame);
ot_s32 stereo_vo_flush_frame(ot_eis_video_frame *frame);
ot_s32 stereo_vo_release_frame(ot_eis_handle pool, ot_eis_video_frame *frame);

#endif /* STEREO_VO_H */
