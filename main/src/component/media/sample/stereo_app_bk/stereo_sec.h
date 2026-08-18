/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
  Board-side security module: device binding check + encrypted model loading.
 */

#ifndef __STEREO_SEC_H__
#define __STEREO_SEC_H__

#include "ot_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Verify device authorization.
 *        Reads the hardware unique ID, computes HMAC-SHA256(uid, MASTER_KEY)
 *        using the on-chip cipher engine, and compares the result against
 *        the digest stored in /opt/stereo/license.bin.
 *
 * @return OT_SUCCESS if the device is authorized, error code otherwise.
 */
ot_s32 stereo_sec_check_device(void);

/**
 * @brief Load and decrypt the encrypted ORTM model file into memory.
 *        Reads /data/model/stereo_match.ortm.enc (= IV[16B] + ciphertext),
 *        derives the AES-128 key from HMAC-SHA256(uid, MASTER_KEY)[:16],
 *        and decrypts via on-chip AES-CTR-128 hardware.
 *
 * @param[out] model_buf  Pointer to decrypted model memory (malloc'd, caller frees).
 * @param[out] model_len  Decrypted model length in bytes.
 * @return OT_SUCCESS on success, error code otherwise.
 */
ot_s32 stereo_sec_load_decrypt_model(ot_u8 **model_buf, ot_u32 *model_len);

/**
 * @brief Get the board unique ID as a hex string (48 chars for 24 bytes).
 *        Uses ot_omi_sys_get_unique_id().
 *
 * @param[out] uid_hex  Buffer of at least 49 bytes (48 hex + NUL).
 * @return OT_SUCCESS on success, error code otherwise.
 */
ot_s32 stereo_sec_get_uid_hex(char *uid_hex);

/**
 * @brief Compute HMAC-SHA256(uid, MASTER_KEY) using on-chip cipher engine.
 *
 * @param[out] digest  Output buffer of at least STEREO_SEC_HMAC_SIZE bytes.
 * @return OT_SUCCESS on success, error code otherwise.
 */
ot_s32 stereo_sec_compute_hmac(ot_u8 *digest);

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_SEC_H__ */
