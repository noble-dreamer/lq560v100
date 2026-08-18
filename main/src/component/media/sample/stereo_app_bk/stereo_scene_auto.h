/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef __STEREO_SCENE_AUTO_H__
#define __STEREO_SCENE_AUTO_H__

#include "ot_common.h"
#include "ot_eis_vi.h"
#include "ot_eis_vproc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STEREO_SCENE_PIPE_MAX   2

/**
 * @brief Start ISP scene auto tuning for stereo cameras.
 *        Reuses the scene_auto library with SC132GS ini parameters.
 *
 * @param ini_path       Path to ISP parameter ini file (e.g. "./param/sc132gs")
 * @param vi_pipe_hdl    VI pipe handles (array of 2)
 * @param vp_grp_hdl     VPROC group handles (array of 2)
 * @param max_hdl_num    Number of handles (2 for stereo)
 * @return OT_SUCCESS or failure code.
 */
ot_s32 stereo_scene_auto_start(const ot_s8 *ini_path,
                                ot_eis_handle vi_pipe_hdl[STEREO_SCENE_PIPE_MAX],
                                ot_eis_handle vp_grp_hdl[STEREO_SCENE_PIPE_MAX],
                                ot_u32 max_hdl_num);

/**
 * @brief Stop all scene auto threads and deinit.
 */
void stereo_scene_auto_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_SCENE_AUTO_H__ */
