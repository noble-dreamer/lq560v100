/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
  Board-side security: device binding check + encrypted model decryption.
  Uses on-chip cipher engine (HMAC-SHA256 + AES-CTR-128) via ot_omi_cipher API.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stereo_sec.h"
#include "stereo_sec_key.h"
#include "stereo_types.h"
#include "ot_omi_sys.h"
#include "ot_omi_cipher.h"
#include "ot_omi_km.h"
#include "ot_omi_otp.h"
#include "ot_smr.h"
#include "ot_eis_errno.h"

/* -------------------------------------------------------------------------- */
/* Helper: read UID as raw bytes from hardware                                */
/* -------------------------------------------------------------------------- */
static ot_s32 sec_get_uid_bytes(ot_u8 *uid_bytes)
{
    ot_unique_id uid;
    ot_s32 ret;

    ret = ot_omi_sys_get_unique_id(&uid);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] ot_omi_sys_get_unique_id failed, ret:0x%x\n", ret);
        return ret;
    }

    /* ot_unique_id.id[6] → 24 bytes, little-endian per u32 */
    memcpy(uid_bytes, uid.id, STEREO_SEC_UID_BYTES);
    return OT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* Helper: check TEE enable bit (same pattern as sample code)                 */
/* -------------------------------------------------------------------------- */
static ot_bool sec_is_tee_enabled(void)
{
    ot_u8 tee_enable = 0;

    (void)ot_omi_otp_init();
    (void)ot_omi_otp_read_byte(0x12, &tee_enable);
    (void)ot_omi_otp_deinit();

    return (tee_enable == 0x42) ? OT_TRUE : OT_FALSE;
}

/* -------------------------------------------------------------------------- */
/* Helper: fill klad security config based on TEE status                      */
/* -------------------------------------------------------------------------- */
static void sec_fill_klad_sec(km_klad_key_secure_config *sec_cfg, ot_bool tee_en)
{
    if (tee_en) {
        sec_cfg->key_sec = KM_KLAD_SEC_ENABLE;
        sec_cfg->master_only_enable = OT_TRUE;
        sec_cfg->dest_buf_sec_support = OT_TRUE;
        sec_cfg->dest_buf_non_sec_support = OT_FALSE;
        sec_cfg->src_buf_sec_support = OT_TRUE;
        sec_cfg->src_buf_non_sec_support = OT_FALSE;
    } else {
        sec_cfg->key_sec = KM_KLAD_SEC_DISABLE;
        sec_cfg->master_only_enable = OT_FALSE;
        sec_cfg->dest_buf_sec_support = OT_FALSE;
        sec_cfg->dest_buf_non_sec_support = OT_TRUE;
        sec_cfg->src_buf_sec_support = OT_FALSE;
        sec_cfg->src_buf_non_sec_support = OT_TRUE;
    }
}

/* -------------------------------------------------------------------------- */
/* Public: compute HMAC-SHA256(uid, MASTER_KEY) via cipher engine             */
/* -------------------------------------------------------------------------- */
ot_s32 stereo_sec_compute_hmac(ot_u8 *digest)
{
    ot_s32 ret;
    ot_u8 uid_bytes[STEREO_SEC_UID_BYTES];
    ot_crypto_handle hash_handle;
    crypto_hash_attr hash_attr;
    crypto_buf_attr src_buf;
    ot_u32 result_len = 0;

    if (digest == NULL) {
        return OT_FAILURE;
    }

    ret = sec_get_uid_bytes(uid_bytes);
    if (ret != OT_SUCCESS) {
        return ret;
    }

    /* HMAC-SHA256 with soft key (no keyslot needed) */
    memset(&hash_attr, 0, sizeof(hash_attr));
    hash_attr.hash_type = CRYPTO_HASH_TYPE_HMAC_SHA256;
    hash_attr.is_keyslot = OT_FALSE;
    hash_attr.is_long_term = OT_FALSE;
    hash_attr.key = (ot_u8 *)STEREO_SEC_MASTER_KEY;
    hash_attr.key_len = STEREO_SEC_MASTER_KEY_LEN;

    ret = ot_omi_cipher_hash_init();
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] hash_init failed, ret:0x%x\n", ret);
        return ret;
    }

    ret = ot_omi_cipher_hash_create(&hash_handle, &hash_attr);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] hash_create failed, ret:0x%x\n", ret);
        goto hash_deinit;
    }

    memset(&src_buf, 0, sizeof(src_buf));
    src_buf.virt_addr = uid_bytes;
    ret = ot_omi_cipher_hash_update(hash_handle, &src_buf, STEREO_SEC_UID_BYTES);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] hash_update failed, ret:0x%x\n", ret);
        goto hash_destroy;
    }

    ret = ot_omi_cipher_hash_finish(hash_handle, digest, STEREO_SEC_HMAC_SIZE, &result_len);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] hash_finish failed, ret:0x%x\n", ret);
    }

hash_destroy:
    ot_omi_cipher_hash_destroy(hash_handle);
hash_deinit:
    ot_omi_cipher_hash_deinit();
    return ret;
}

/* -------------------------------------------------------------------------- */
/* Public: get UID as hex string                                              */
/* -------------------------------------------------------------------------- */
ot_s32 stereo_sec_get_uid_hex(char *uid_hex)
{
    ot_u8 uid_bytes[STEREO_SEC_UID_BYTES];
    ot_s32 ret;

    if (uid_hex == NULL) {
        return OT_FAILURE;
    }

    ret = sec_get_uid_bytes(uid_bytes);
    if (ret != OT_SUCCESS) {
        return ret;
    }

    for (ot_s32 i = 0; i < STEREO_SEC_UID_BYTES; i++) {
        sprintf(uid_hex + i * 2, "%02x", uid_bytes[i]);
    }
    uid_hex[STEREO_SEC_UID_BYTES * 2] = '\0';
    return OT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* Public: verify device license                                              */
/* -------------------------------------------------------------------------- */
ot_s32 stereo_sec_check_device(void)
{
    ot_s32 ret;
    ot_u8 computed_hmac[STEREO_SEC_HMAC_SIZE];
    ot_u8 license_data[STEREO_SEC_LICENSE_SIZE];
    FILE *fp = NULL;
    size_t bytes_read;

    /* Compute expected HMAC from hardware UID */
    ret = stereo_sec_compute_hmac(computed_hmac);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] compute_hmac failed, ret:0x%x\n", ret);
        return ret;
    }

    /* Read license file */
    fp = fopen(STEREO_SEC_LICENSE_PATH, "rb");
    if (fp == NULL) {
        stereo_log_write("[stereo_sec] license file not found: %s\n", STEREO_SEC_LICENSE_PATH);
        return OT_FAILURE;
    }

    bytes_read = fread(license_data, 1, STEREO_SEC_LICENSE_SIZE, fp);
    fclose(fp);

    if (bytes_read < STEREO_SEC_HMAC_SIZE) {
        stereo_log_write("[stereo_sec] license file too small: %zu bytes\n", bytes_read);
        return OT_FAILURE;
    }

    /* Compare computed HMAC with stored digest (first 32 bytes) */
    if (memcmp(computed_hmac, license_data, STEREO_SEC_HMAC_SIZE) != 0) {
        stereo_log_write("[stereo_sec] license verification FAILED — device not authorized\n");
        return OT_FAILURE;
    }

    stereo_log_write("[stereo_sec] device license verified OK\n");
    return OT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* Helper: set clear AES key via klad (same pattern as sample_symc_clearkey)  */
/* -------------------------------------------------------------------------- */
static ot_s32 sec_set_clear_aes_key(crypto_handle keyslot_handle,
                                      const ot_u8 *key, ot_u32 key_len)
{
    ot_s32 ret;
    crypto_handle klad_handle = 0;
    ot_bool tee_en = sec_is_tee_enabled();

    km_klad_attr klad_attr;
    km_klad_clear_key clear_key;

    memset(&klad_attr, 0, sizeof(klad_attr));
    klad_attr.key_cfg.engine = KM_CRYPTO_ALG_AES;
    klad_attr.key_cfg.decrypt_support = OT_TRUE;
    klad_attr.key_cfg.encrypt_support = OT_TRUE;
    sec_fill_klad_sec(&klad_attr.key_sec_cfg, tee_en);

    memset(&clear_key, 0, sizeof(clear_key));
    clear_key.key = (ot_u8 *)key;
    clear_key.key_size = key_len;

    ret = ot_omi_klad_create(&klad_handle);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] klad_create failed, ret:0x%x\n", ret);
        return ret;
    }

    ret = ot_omi_klad_set_attr(klad_handle, &klad_attr);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] klad_set_attr failed, ret:0x%x\n", ret);
        goto klad_destroy;
    }

    ret = ot_omi_klad_attach(klad_handle, KM_KLAD_DEST_TYPE_MCIPHER, keyslot_handle);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] klad_attach failed, ret:0x%x\n", ret);
        goto klad_destroy;
    }

    ret = ot_omi_klad_set_clear_key(klad_handle, &clear_key);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] klad_set_clear_key failed, ret:0x%x\n", ret);
    }

    ot_omi_klad_detach(klad_handle, KM_KLAD_DEST_TYPE_MCIPHER, keyslot_handle);

klad_destroy:
    ot_omi_klad_destroy(klad_handle);
    return ret;
}

/* Decrypt the encrypted model to a plaintext file (async NPU cannot run
   models loaded from memory, so the plaintext must be a file). */
ot_s32 stereo_sec_decrypt_model_to_file(const char *out_path)
{
    ot_s32 ret;
    ot_u8 hmac_digest[STEREO_SEC_HMAC_SIZE];
    ot_u8 aes_key[STEREO_SEC_AES_KEY_SIZE];
    ot_u8 iv[STEREO_SEC_IV_SIZE];
    FILE *fp = NULL;
    FILE *fp_out = NULL;
    long file_size;
    ot_u32 enc_data_len;

    /* Cipher handles and buffers */
    ot_crypto_handle symc_handle = 0;
    crypto_handle keyslot_handle = 0;
    crypto_symc_attr symc_attr;
    crypto_symc_ctrl_t symc_ctrl;
    crypto_buf_attr src_buf;
    crypto_buf_attr dst_buf;
    ot_phys_addr src_phys = 0;
    ot_phys_addr dst_phys = 0;
    void *src_virt = NULL;
    void *dst_virt = NULL;
    ot_smr_alloc_attr smr_attr;

    if (out_path == NULL) {
        return OT_FAILURE;
    }

    /* Step 1: Derive AES key from HMAC-SHA256(uid, MASTER_KEY)[:16] */
    ret = stereo_sec_compute_hmac(hmac_digest);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] compute HMAC for key derivation failed\n");
        return ret;
    }
    memcpy(aes_key, hmac_digest, STEREO_SEC_AES_KEY_SIZE);

    /* Step 2: Read encrypted model file = [IV(16B)][ciphertext(N B)] */
    fp = fopen(STEREO_SEC_MODEL_ENC_PATH, "rb");
    if (fp == NULL) {
        stereo_log_write("[stereo_sec] encrypted model not found: %s\n", STEREO_SEC_MODEL_ENC_PATH);
        return OT_FAILURE;
    }

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size <= STEREO_SEC_IV_SIZE) {
        stereo_log_write("[stereo_sec] encrypted model too small: %ld bytes\n", file_size);
        fclose(fp);
        return OT_FAILURE;
    }

    enc_data_len = (ot_u32)(file_size - STEREO_SEC_IV_SIZE);

    /* Read IV (first 16 bytes) */
    if (fread(iv, 1, STEREO_SEC_IV_SIZE, fp) != STEREO_SEC_IV_SIZE) {
        stereo_log_write("[stereo_sec] failed to read IV\n");
        fclose(fp);
        return OT_FAILURE;
    }

    stereo_log_write("[stereo_sec] encrypted model: %u bytes ciphertext + 16B IV\n", enc_data_len);

    /* Step 3: Allocate SMR buffers for cipher hardware DMA */
    memset(&smr_attr, 0, sizeof(smr_attr));
    snprintf((char *)smr_attr.region_name, OT_SMR_REGION_NAME_LEN_MAX, "anony");
    snprintf((char *)smr_attr.chunk_name, OT_SMR_CHUNK_NAME_MAX, "sec_src");
    smr_attr.len = enc_data_len;
    smr_attr.align = 64;
    smr_attr.cached = OT_FALSE;

    ret = ot_smr_alloc(&smr_attr, &src_phys, &src_virt);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] smr_alloc src failed, ret:0x%x\n", ret);
        goto close_fp;
    }

    snprintf((char *)smr_attr.chunk_name, OT_SMR_CHUNK_NAME_MAX, "sec_dst");
    ret = ot_smr_alloc(&smr_attr, &dst_phys, &dst_virt);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] smr_alloc dst failed, ret:0x%x\n", ret);
        goto free_src_smr;
    }

    /* Read ciphertext directly into the SMR source buffer (no heap staging) */
    if (fread(src_virt, 1, enc_data_len, fp) != enc_data_len) {
        stereo_log_write("[stereo_sec] failed to read ciphertext\n");
        ret = OT_FAILURE;
        goto free_src_smr;
    }
    fclose(fp);
    fp = NULL;

    /* Step 4: Setup AES-CTR-128 cipher with clear key */
    ret = ot_omi_cipher_symc_init();
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] symc_init failed, ret:0x%x\n", ret);
        goto free_dst_smr;
    }

    ret = ot_omi_km_init();
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] km_init failed, ret:0x%x\n", ret);
        goto symc_deinit;
    }

    memset(&symc_attr, 0, sizeof(symc_attr));
    symc_attr.symc_type = CRYPTO_SYMC_TYPE_NORMAL;
    symc_attr.is_long_term = OT_FALSE;

    ret = ot_omi_cipher_symc_create(&symc_handle, &symc_attr);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] symc_create failed, ret:0x%x\n", ret);
        goto km_deinit;
    }

    ret = ot_omi_keyslot_create(&keyslot_handle, KM_KEYSLOT_TYPE_MCIPHER);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] keyslot_create failed, ret:0x%x\n", ret);
        goto symc_destroy;
    }

    ret = ot_omi_cipher_symc_attach(symc_handle, (ot_crypto_handle)keyslot_handle);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] symc_attach failed, ret:0x%x\n", ret);
        goto keyslot_destroy;
    }

    ret = sec_set_clear_aes_key(keyslot_handle, aes_key, STEREO_SEC_AES_KEY_SIZE);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] set_clear_key failed, ret:0x%x\n", ret);
        goto keyslot_destroy;
    }

    /* Configure AES-CTR-128 with IV from file */
    memset(&symc_ctrl, 0, sizeof(symc_ctrl));
    symc_ctrl.symc_alg = CRYPTO_SYMC_ALG_AES;
    symc_ctrl.work_mode = CRYPTO_SYMC_WORK_MODE_CTR;
    symc_ctrl.symc_key_length = CRYPTO_SYMC_KEY_128BIT;
    symc_ctrl.symc_bit_width = CRYPTO_SYMC_BIT_WIDTH_128BIT;
    symc_ctrl.iv_change_flag = CRYPTO_SYMC_IV_CHANGE_ONE_PKG;
    symc_ctrl.iv_length = STEREO_SEC_IV_SIZE;
    memcpy(symc_ctrl.iv, iv, STEREO_SEC_IV_SIZE);

    ret = ot_omi_cipher_symc_set_config(symc_handle, &symc_ctrl);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] symc_set_config failed, ret:0x%x\n", ret);
        goto keyslot_destroy;
    }

    /* Step 5: Decrypt — hardware DMA from src SMR to dst SMR */
    memset(&src_buf, 0, sizeof(src_buf));
    memset(&dst_buf, 0, sizeof(dst_buf));
    src_buf.phys_addr = (unsigned long)src_phys;
    src_buf.virt_addr = src_virt;
    src_buf.buf_sec = CRYPTO_BUF_NONSECURE;
    dst_buf.phys_addr = (unsigned long)dst_phys;
    dst_buf.virt_addr = dst_virt;
    dst_buf.buf_sec = CRYPTO_BUF_NONSECURE;

    ret = ot_omi_cipher_symc_decrypt(symc_handle, &src_buf, &dst_buf, enc_data_len);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[stereo_sec] symc_decrypt failed, ret:0x%x\n", ret);
        goto keyslot_destroy;
    }

    /* Step 6: Write decrypted model to the plaintext staging file */
    fp_out = fopen(out_path, "wb");
    if (fp_out == NULL) {
        stereo_log_write("[stereo_sec] open plaintext out failed: %s\n", out_path);
        ret = OT_FAILURE;
        goto keyslot_destroy;
    }
    if (fwrite(dst_virt, 1, enc_data_len, fp_out) != enc_data_len) {
        stereo_log_write("[stereo_sec] write plaintext out failed\n");
        ret = OT_FAILURE;
        goto close_fp_out;
    }
    fclose(fp_out);
    fp_out = NULL;

    stereo_log_write("[stereo_sec] model decrypted to %s: %u bytes\n", out_path, enc_data_len);

close_fp_out:
    if (fp_out != NULL) fclose(fp_out);
keyslot_destroy:
    ot_omi_keyslot_destroy(keyslot_handle);
symc_destroy:
    ot_omi_cipher_symc_destroy(symc_handle);
km_deinit:
    ot_omi_km_deinit();
symc_deinit:
    ot_omi_cipher_symc_deinit();
free_dst_smr:
    if (dst_phys != 0) ot_smr_free(dst_phys);
free_src_smr:
    if (src_phys != 0) ot_smr_free(src_phys);
close_fp:
    if (fp) fclose(fp);
    return ret;
}
