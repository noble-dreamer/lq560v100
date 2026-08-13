/*
 * TinyCrypt SHA256 implementation for embedded systems
 * Compatible with RFC 6234
 */

#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TC_SHA256_BLOCK_SIZE (64U)
#define TC_SHA256_DIGEST_SIZE (32U)

typedef struct tc_sha256_state_struct {
    uint32_t iv[8];
    uint64_t bits_hashed;
    uint8_t  leftover[TC_SHA256_BLOCK_SIZE];
    size_t   leftover_offset;
} tc_sha256_state_t;

/**
 * @brief SHA256 initialization procedure
 *
 * Initializes the SHA256 state.
 *
 * @param s SHA256 state structure
 * @return returns TC_CRYPTO_SUCCESS (1)
 */
int tc_sha256_init(tc_sha256_state_t *s);

/**
 * @brief SHA256 update procedure
 *
 * Updates the SHA256 state with the given data.
 *
 * @param s SHA256 state structure
 * @param data Incoming data
 * @param data_length Length of incoming data
 * @return returns TC_CRYPTO_SUCCESS (1)
 */
int tc_sha256_update(tc_sha256_state_t *s, const uint8_t *data, size_t data_length);

/**
 * @brief SHA256 final procedure
 *
 * Returns the digest of the data processed.
 *
 * @param digest digest
 * @param s SHA256 state structure
 * @return returns TC_CRYPTO_SUCCESS (1)
 */
int tc_sha256_final(uint8_t *digest, tc_sha256_state_t *s);

#ifdef __cplusplus
}
#endif

#endif /* SHA256_H */