/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
  Standalone board-side authorization generator.
  Deployed to /tmp/auth_gen by gen_license.sh, run once, then deleted.

  Flow:
    1. ot_omi_sys_get_unique_id() → 24B uid
    2. HMAC-SHA256(uid, MASTER_KEY) → 32B digest (cipher engine)
    3. Write /opt/stereo/license.bin (64B = 32B digest + 32B zero pad)
    4. stdout: "UID:<48 hex chars>"

  Build: cross-compiled with SDK toolchain + libot_cipher.a + libot_km.a
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "stereo_sec_key.h"
#include "ot_omi_sys.h"
#include "ot_omi_cipher.h"

static ot_s32 get_uid_bytes(unsigned char *uid_bytes)
{
    ot_unique_id uid;
    ot_s32 ret;

    ret = ot_omi_sys_get_unique_id(&uid);
    if (ret != 0) {
        fprintf(stderr, "ERROR: ot_omi_sys_get_unique_id failed, ret:0x%x\n", ret);
        return ret;
    }

    memcpy(uid_bytes, uid.id, STEREO_SEC_UID_BYTES);
    return 0;
}

static ot_s32 compute_hmac(const unsigned char *uid_bytes, unsigned char *digest)
{
    ot_s32 ret;
    ot_crypto_handle hash_handle;
    crypto_hash_attr hash_attr;
    crypto_buf_attr src_buf;
    ot_u32 result_len = 0;

    memset(&hash_attr, 0, sizeof(hash_attr));
    hash_attr.hash_type = CRYPTO_HASH_TYPE_HMAC_SHA256;
    hash_attr.is_keyslot = OT_FALSE;
    hash_attr.is_long_term = OT_FALSE;
    hash_attr.key = (ot_u8 *)STEREO_SEC_MASTER_KEY;
    hash_attr.key_len = STEREO_SEC_MASTER_KEY_LEN;

    ret = ot_omi_cipher_hash_init();
    if (ret != 0) {
        fprintf(stderr, "ERROR: hash_init failed, ret:0x%x\n", ret);
        return ret;
    }

    ret = ot_omi_cipher_hash_create(&hash_handle, &hash_attr);
    if (ret != 0) {
        fprintf(stderr, "ERROR: hash_create failed, ret:0x%x\n", ret);
        goto hash_deinit;
    }

    memset(&src_buf, 0, sizeof(src_buf));
    src_buf.virt_addr = (void *)uid_bytes;
    ret = ot_omi_cipher_hash_update(hash_handle, &src_buf, STEREO_SEC_UID_BYTES);
    if (ret != 0) {
        fprintf(stderr, "ERROR: hash_update failed, ret:0x%x\n", ret);
        goto hash_destroy;
    }

    ret = ot_omi_cipher_hash_finish(hash_handle, digest, STEREO_SEC_HMAC_SIZE, &result_len);
    if (ret != 0) {
        fprintf(stderr, "ERROR: hash_finish failed, ret:0x%x\n", ret);
    }

hash_destroy:
    ot_omi_cipher_hash_destroy(hash_handle);
hash_deinit:
    ot_omi_cipher_hash_deinit();
    return ret;
}

int main(int argc, char *argv[])
{
    unsigned char uid_bytes[STEREO_SEC_UID_BYTES];
    unsigned char hmac_digest[STEREO_SEC_HMAC_SIZE];
    unsigned char license_data[STEREO_SEC_LICENSE_SIZE];
    char uid_hex[STEREO_SEC_UID_BYTES * 2 + 1];
    ot_s32 ret;
    FILE *fp;
    int i;

    (void)argc;
    (void)argv;

    /* Step 1: Get hardware unique ID */
    ret = get_uid_bytes(uid_bytes);
    if (ret != 0) {
        return 1;
    }

    /* Convert UID to hex string for stdout output */
    for (i = 0; i < STEREO_SEC_UID_BYTES; i++) {
        sprintf(uid_hex + i * 2, "%02x", uid_bytes[i]);
    }
    uid_hex[STEREO_SEC_UID_BYTES * 2] = '\0';

    /* Step 2: Compute HMAC-SHA256(uid, MASTER_KEY) */
    ret = compute_hmac(uid_bytes, hmac_digest);
    if (ret != 0) {
        return 1;
    }

    /* Step 3: Build license data = [HMAC(32B)][zero pad(32B)] */
    memset(license_data, 0, STEREO_SEC_LICENSE_SIZE);
    memcpy(license_data, hmac_digest, STEREO_SEC_HMAC_SIZE);

    /* Step 4: Write license.bin */
    /* Ensure /opt/stereo exists */
    mkdir("/opt/stereo", 0755);

    fp = fopen(STEREO_SEC_LICENSE_PATH, "wb");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: cannot write %s\n", STEREO_SEC_LICENSE_PATH);
        return 1;
    }

    if (fwrite(license_data, 1, STEREO_SEC_LICENSE_SIZE, fp) != STEREO_SEC_LICENSE_SIZE) {
        fprintf(stderr, "ERROR: short write to license file\n");
        fclose(fp);
        return 1;
    }
    fclose(fp);

    /* Step 5: Output UID hex for the host to capture */
    printf("UID:%s\n", uid_hex);
    fflush(stdout);

    return 0;
}
