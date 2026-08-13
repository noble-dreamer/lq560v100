/*
 * TinyCrypt ECDSA implementation for embedded systems
 * Uses micro-ecc library for secp256r1 (NIST P-256) curve verification
 */

#include <string.h>
#include "ecdsa.h"
#include "uECC.h"

#define TC_CRYPTO_SUCCESS (1)
#define TC_CRYPTO_FAIL    (0)

/* ECDSA-P256 signature verification using micro-ecc
 *
 * @param hash      SHA-256 hash of signed data (32 bytes)
 * @param hash_len  Length of hash (must be 32 for SHA256)
 * @param signature ECDSA signature (64 bytes: R + S, big-endian)
 * @param key       ECDSA public key (64 bytes: x + y, big-endian)
 * @return TC_CRYPTO_SUCCESS (1) if verification passes, TC_CRYPTO_FAIL (0) otherwise
 */
int tc_ecdsa_verify(const uint8_t *hash, size_t hash_len,
                    const uint8_t *signature, const tc_ecdsa_key_t *key) {
    if (hash == NULL || signature == NULL || key == NULL) {
        return TC_CRYPTO_FAIL;
    }

    if (hash_len != 32) {
        return TC_CRYPTO_FAIL;
    }

    /* Reject all-0xFF (uninitialized NAND/flash) */
    int all_ff = 1, all_00 = 1;
    for (int i = 0; i < 64; i++) {
        if (signature[i] != 0xFF) all_ff = 0;
        if (signature[i] != 0x00) all_00 = 0;
    }
    if (all_ff || all_00) {
        return TC_CRYPTO_FAIL;
    }

    /* Check for invalid public key (all zeros) */
    int key_valid = 0;
    for (int i = 0; i < 32; i++) {
        if (key->x[i] != 0 || key->y[i] != 0) {
            key_valid = 1;
            break;
        }
    }
    if (!key_valid) {
        return TC_CRYPTO_FAIL;
    }

    /* Use micro-ecc for real ECDSA-P256 verification.
     * tc_ecdsa_key_t layout is x[32] + y[32], which matches
     * micro-ecc's uncompressed public key format.
     */
    int ret = uECC_verify((const uint8_t *)key,
                          hash,
                          (unsigned)hash_len,
                          signature,
                          uECC_secp256r1());

    return ret ? TC_CRYPTO_SUCCESS : TC_CRYPTO_FAIL;
}
