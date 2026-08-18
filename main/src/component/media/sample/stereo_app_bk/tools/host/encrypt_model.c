/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.
  Host-side model encryption tool (x86_64, OpenSSL).
  Encrypts an ORTM model file bound to a specific board UID.

  Usage: ./encrypt_model <input.ortm> <output.ortm.enc> <uid_hex>

  Flow:
    1. Parse uid_hex (48 hex chars) → 24-byte uid
    2. HMAC-SHA256(uid, MASTER_KEY) → 32-byte digest
    3. AES-128 key = digest[:16]
    4. RAND_bytes → 16-byte IV
    5. OpenSSL AES-CTR-128 encrypt
    6. Output .ortm.enc = [IV(16B)][ciphertext(N B)]

  Build: gcc -o encrypt_model encrypt_model.c -lcrypto
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

/* Include the shared master key definition */
#include "stereo_sec_key.h"

static int hex_to_bytes(const char *hex, unsigned char *bytes, int max_len)
{
    int hex_len = strlen(hex);
    int byte_len = hex_len / 2;

    if (hex_len % 2 != 0 || byte_len > max_len) {
        return -1;
    }

    for (int i = 0; i < byte_len; i++) {
        unsigned int val;
        if (sscanf(hex + i * 2, "%02x", &val) != 1) {
            return -1;
        }
        bytes[i] = (unsigned char)val;
    }
    return byte_len;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input.ortm> <output.ortm.enc> <uid_hex>\n", argv[0]);
        fprintf(stderr, "  uid_hex: 48 hex chars (24 bytes) from gen_license\n");
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];
    const char *uid_hex = argv[3];

    /* Step 1: Parse UID hex string */
    unsigned char uid_bytes[STEREO_SEC_UID_BYTES];
    int uid_len = hex_to_bytes(uid_hex, uid_bytes, sizeof(uid_bytes));
    if (uid_len != STEREO_SEC_UID_BYTES) {
        fprintf(stderr, "ERROR: invalid uid_hex length, expected %d bytes (%d hex chars), got %d bytes\n",
                STEREO_SEC_UID_BYTES, STEREO_SEC_UID_BYTES * 2, uid_len);
        return 1;
    }

    /* Step 2: HMAC-SHA256(uid, MASTER_KEY) → 32-byte digest */
    unsigned char hmac_digest[STEREO_SEC_HMAC_SIZE];
    unsigned int hmac_len = 0;

    if (HMAC(EVP_sha256(),
             STEREO_SEC_MASTER_KEY, STEREO_SEC_MASTER_KEY_LEN,
             uid_bytes, STEREO_SEC_UID_BYTES,
             hmac_digest, &hmac_len) == NULL || hmac_len != STEREO_SEC_HMAC_SIZE) {
        fprintf(stderr, "ERROR: HMAC-SHA256 computation failed\n");
        return 1;
    }

    /* Step 3: AES-128 key = HMAC digest[:16] */
    unsigned char aes_key[STEREO_SEC_AES_KEY_SIZE];
    memcpy(aes_key, hmac_digest, STEREO_SEC_AES_KEY_SIZE);

    /* Step 4: Generate random 16-byte IV */
    unsigned char iv[STEREO_SEC_IV_SIZE];
    if (RAND_bytes(iv, STEREO_SEC_IV_SIZE) != 1) {
        fprintf(stderr, "ERROR: RAND_bytes for IV failed\n");
        return 1;
    }

    /* Step 5: Read input file */
    FILE *fp_in = fopen(input_path, "rb");
    if (fp_in == NULL) {
        fprintf(stderr, "ERROR: cannot open input file %s\n", input_path);
        return 1;
    }

    fseek(fp_in, 0, SEEK_END);
    long file_size = ftell(fp_in);
    fseek(fp_in, 0, SEEK_SET);

    if (file_size <= 0) {
        fprintf(stderr, "ERROR: input file is empty or unreadable\n");
        fclose(fp_in);
        return 1;
    }

    unsigned char *plaintext = (unsigned char *)malloc(file_size);
    if (plaintext == NULL) {
        fprintf(stderr, "ERROR: malloc for plaintext failed\n");
        fclose(fp_in);
        return 1;
    }

    if (fread(plaintext, 1, file_size, fp_in) != (size_t)file_size) {
        fprintf(stderr, "ERROR: failed to read input file\n");
        free(plaintext);
        fclose(fp_in);
        return 1;
    }
    fclose(fp_in);

    /* Step 6: AES-CTR-128 encrypt */
    unsigned char *ciphertext = (unsigned char *)malloc(file_size);
    if (ciphertext == NULL) {
        fprintf(stderr, "ERROR: malloc for ciphertext failed\n");
        free(plaintext);
        return 1;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        fprintf(stderr, "ERROR: EVP_CIPHER_CTX_new failed\n");
        free(plaintext);
        free(ciphertext);
        return 1;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, aes_key, iv) != 1) {
        fprintf(stderr, "ERROR: EVP_EncryptInit_ex failed\n");
        EVP_CIPHER_CTX_free(ctx);
        free(plaintext);
        free(ciphertext);
        return 1;
    }

    int out_len = 0;
    if (EVP_EncryptUpdate(ctx, ciphertext, &out_len, plaintext, (int)file_size) != 1) {
        fprintf(stderr, "ERROR: EVP_EncryptUpdate failed\n");
        EVP_CIPHER_CTX_free(ctx);
        free(plaintext);
        free(ciphertext);
        return 1;
    }

    int final_len = 0;
    if (EVP_EncryptFinal_ex(ctx, ciphertext + out_len, &final_len) != 1) {
        fprintf(stderr, "ERROR: EVP_EncryptFinal_ex failed\n");
        EVP_CIPHER_CTX_free(ctx);
        free(plaintext);
        free(ciphertext);
        return 1;
    }

    int total_enc = out_len + final_len;
    EVP_CIPHER_CTX_free(ctx);
    free(plaintext);

    if (total_enc != (int)file_size) {
        fprintf(stderr, "ERROR: ciphertext size mismatch: %d vs %ld\n", total_enc, file_size);
        free(ciphertext);
        return 1;
    }

    /* Step 7: Write output = [IV(16B)][ciphertext(N B)] */
    FILE *fp_out = fopen(output_path, "wb");
    if (fp_out == NULL) {
        fprintf(stderr, "ERROR: cannot open output file %s\n", output_path);
        free(ciphertext);
        return 1;
    }

    if (fwrite(iv, 1, STEREO_SEC_IV_SIZE, fp_out) != STEREO_SEC_IV_SIZE) {
        fprintf(stderr, "ERROR: failed to write IV\n");
        fclose(fp_out);
        free(ciphertext);
        return 1;
    }

    if (fwrite(ciphertext, 1, total_enc, fp_out) != (size_t)total_enc) {
        fprintf(stderr, "ERROR: failed to write ciphertext\n");
        fclose(fp_out);
        free(ciphertext);
        return 1;
    }

    fclose(fp_out);
    free(ciphertext);

    /* Securely wipe the AES key from memory */
    memset(aes_key, 0, sizeof(aes_key));
    memset(hmac_digest, 0, sizeof(hmac_digest));

    printf("OK: encrypted %ld bytes → %s (IV:%dB + cipher:%dB = %ldB)\n",
           file_size, output_path, STEREO_SEC_IV_SIZE, total_enc,
           (long)(STEREO_SEC_IV_SIZE + total_enc));
    return 0;
}
