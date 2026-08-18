/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
 */

#ifndef __STEREO_NETWORK_H__
#define __STEREO_NETWORK_H__

#include "ot_common.h"
#include "stereo_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create listening socket on 0.0.0.0:9000 (TCP server mode).
 */
ot_s32 stereo_network_init(void);

/**
 * @brief Non-blocking accept: check for a pending client connection.
 * @return OT_SUCCESS if a client just connected, OT_FAILURE if none pending.
 */
ot_s32 stereo_network_accept(void);

/**
 * @brief Close client + listen socket.
 */
void stereo_network_disconnect(void);

/**
 * @brief Returns OT_TRUE if a client is currently connected.
 */
ot_bool stereo_network_is_connected(void);

/**
 * @brief Send frame triplet to connected client.
 *        Silently drops frames when no client is connected.
 */
ot_s32 stereo_network_send(const stereo_venc_output_t *output);

/**
 * @brief Send calibration parameters (JSON) to connected client.
 *        Called once when a client first connects.
 *        Contains pre-scaled intrinsics at disparity resolution (640x448),
 *        baseline, and resolution mapping info for 3D triangulation.
 */
ot_s32 stereo_network_send_calib(void);

/**
 * @brief Write debug log to NFS mount.
 */
void stereo_network_write_log(ot_u32 frame_index, ot_u64 timestamp_us,
                               ot_u32 left_size, ot_u32 right_size, ot_u32 disp_size);

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_NETWORK_H__ */
