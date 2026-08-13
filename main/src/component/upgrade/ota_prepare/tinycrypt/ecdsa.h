/*
 * TinyCrypt ECDSA implementation for embedded systems
 * Supports secp256r1 (NIST P-256) curve
 * Fast software implementation optimized for ARM
 */

#ifndef ECDSA_H
#define ECDSA_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TC_ECDSA_P256_KEY_SIZE (64U)  /* x (32 bytes) + y (32 bytes) */
#define TC_ECDSA_P256_SIG_SIZE (64U)  /* R (32 bytes) + S (32 bytes) */

typedef struct tc_ecdsa_key_struct {
    uint8_t x[32];  /* x coordinate of public key (big-endian) */
    uint8_t y[32];  /* y coordinate of public key (big-endian) */
} tc_ecdsa_key_t;

/**
 * @brief ECDSA-P256 signature verification
 * 
 * @param hash      Hash of the message (SHA256, 32 bytes)
 * @param hash_len  Length of hash (32 for SHA256)
 * @param signature ECDSA signature (64 bytes: R + S, big-endian)
 * @param key       ECDSA public key
 * @return returns 1 if verification passes, 0 otherwise
 */
int tc_ecdsa_verify(const uint8_t *hash, size_t hash_len,
                    const uint8_t *signature, const tc_ecdsa_key_t *key);

#ifdef __cplusplus
}
#endif

#endif /* ECDSA_H */