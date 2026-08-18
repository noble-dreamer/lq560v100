/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
  Confidential — shared master key for stereo_app device binding and model encryption.
 */

#ifndef __STEREO_SEC_KEY_H__
#define __STEREO_SEC_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* MASTER_KEY — 32-byte shared secret used for HMAC-SHA256.
   Both auth_gen (board) and encrypt_model (host) embed this key.
   The first 16 bytes of HMAC-SHA256(uid, MASTER_KEY) are used as AES-128 key.
   CHANGE THIS KEY BEFORE PRODUCTION DEPLOYMENT.                              */
/* -------------------------------------------------------------------------- */

#define STEREO_SEC_MASTER_KEY_LEN  32

static const unsigned char STEREO_SEC_MASTER_KEY[STEREO_SEC_MASTER_KEY_LEN] = {
    0x4F, 0x72, 0x69, 0x74, 0x65, 0x6B, 0x5F, 0x73,
    0x74, 0x65, 0x72, 0x65, 0x6F, 0x5F, 0x73, 0x65,
    0x63, 0x32, 0x30, 0x32, 0x34, 0x5F, 0x6B, 0x65,
    0x79, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5E, 0x26
};

/* License file: 64 bytes = 32-byte HMAC-SHA256 digest + 32-byte zero padding */
#define STEREO_SEC_LICENSE_SIZE     64
#define STEREO_SEC_LICENSE_PATH     "/opt/stereo/license.bin"

/* Encrypted model path on board */
#define STEREO_SEC_MODEL_ENC_PATH   "/data/model/stereo_match.ortm.enc"

/* UID size: OT_UNIQUE_ID_NUM=6, each u32 → 24 bytes */
#define STEREO_SEC_UID_BYTES        24

/* AES-CTR IV size */
#define STEREO_SEC_IV_SIZE          16

/* HMAC-SHA256 output size */
#define STEREO_SEC_HMAC_SIZE        32

/* AES-128 key derived from HMAC[:16] */
#define STEREO_SEC_AES_KEY_SIZE     16

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_SEC_KEY_H__ */
