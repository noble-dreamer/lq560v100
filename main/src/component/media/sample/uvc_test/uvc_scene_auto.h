#ifndef UVC_SCENE_AUTO_H__
#define UVC_SCENE_AUTO_H__

#include "sample_comm.h"
#include "ot_scene.h"

ot_s32 sample_uvc_scene_auto_start(const char *ini_path, ot_eis_handle vi_pipe_hdl[OT_SCENE_PIPE_MAX_NUM], ot_eis_handle vp_grp_hdl[OT_SCENE_PIPE_MAX_NUM], ot_u32 max_hdl_num);
void   sample_uvc_scene_auto_stop();

#endif