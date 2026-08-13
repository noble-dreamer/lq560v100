/*
 * TinyCrypt SHA256 implementation for embedded systems
 * Compatible with RFC 6234
 */

#include "sha256.h"

#define TC_CRYPTO_SUCCESS (1)
#define TC_CRYPTO_FAIL (0)

/* SHA256 constants */
static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/* Rotate right */
#define ROR32(a, n) (((a) >> (n)) | ((a) << (32 - (n))))

/* SHA256 round function */
#define Ch(x, y, z)  (((x) & (y)) ^ (~(x) & (z)))
#define Maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define Sigma0(x)    (ROR32(x, 2) ^ ROR32(x, 13) ^ ROR32(x, 22))
#define Sigma1(x)    (ROR32(x, 6) ^ ROR32(x, 11) ^ ROR32(x, 25))
#define sigma0(x)    (ROR32(x, 7) ^ ROR32(x, 18) ^ ((x) >> 3))
#define sigma1(x)    (ROR32(x, 17) ^ ROR32(x, 19) ^ ((x) >> 10))

/* Endian swap for 32-bit */
static inline uint32_t swap32(uint32_t x)
{
    return ((x >> 24) & 0xff) |
           ((x >> 8) & 0xff00) |
           ((x << 8) & 0xff0000) |
           ((x << 24) & 0xff000000);
}

int tc_sha256_init(tc_sha256_state_t *s)
{
    if (s == NULL) {
        return TC_CRYPTO_FAIL;
    }

    s->iv[0] = 0x6a09e667;
    s->iv[1] = 0xbb67ae85;
    s->iv[2] = 0x3c6ef372;
    s->iv[3] = 0xa54ff53a;
    s->iv[4] = 0x510e527f;
    s->iv[5] = 0x9b05688c;
    s->iv[6] = 0x1f83d9ab;
    s->iv[7] = 0x5be0cd19;
    s->bits_hashed = 0ULL;
    s->leftover_offset = 0U;

    return TC_CRYPTO_SUCCESS;
}

static void sha256_compress(uint32_t *state, const uint8_t *block)
{
    uint32_t w[64];
    uint32_t a, b, c, d, e, f, g, h;
    int i;

    /* Message schedule */
    for (i = 0; i < 16; i++) {
        w[i] = swap32(*((uint32_t *)(block + i * 4)));
    }
    for (i = 16; i < 64; i++) {
        w[i] = sigma1(w[i-2]) + w[i-7] + sigma0(w[i-15]) + w[i-16];
    }

    /* Initialize working variables */
    a = state[0]; b = state[1]; c = state[2]; d = state[3];
    e = state[4]; f = state[5]; g = state[6]; h = state[7];

    /* Compression function */
    for (i = 0; i < 64; i++) {
        uint32_t t1 = h + Sigma1(e) + Ch(e, f, g) + k[i] + w[i];
        uint32_t t2 = Sigma0(a) + Maj(a, b, c);
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }

    /* Update state */
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

int tc_sha256_update(tc_sha256_state_t *s, const uint8_t *data, size_t data_length)
{
    if ((s == NULL) || (data == NULL)) {
        return TC_CRYPTO_FAIL;
    }

    /* Process leftover from previous call */
    if (s->leftover_offset != 0U) {
        size_t needed = TC_SHA256_BLOCK_SIZE - s->leftover_offset;
        size_t to_copy = (data_length < needed) ? data_length : needed;

        for (size_t i = 0; i < to_copy; i++) {
            s->leftover[s->leftover_offset + i] = data[i];
        }
        s->leftover_offset += to_copy;
        data += to_copy;
        data_length -= to_copy;

        if (s->leftover_offset == TC_SHA256_BLOCK_SIZE) {
            sha256_compress(s->iv, s->leftover);
            s->bits_hashed += (uint64_t)TC_SHA256_BLOCK_SIZE * 8U;
            s->leftover_offset = 0U;
        }
    }

    /* Process full blocks */
    while (data_length >= TC_SHA256_BLOCK_SIZE) {
        sha256_compress(s->iv, data);
        s->bits_hashed += (uint64_t)TC_SHA256_BLOCK_SIZE * 8U;
        data += TC_SHA256_BLOCK_SIZE;
        data_length -= TC_SHA256_BLOCK_SIZE;
    }

    /* Store remaining data */
    if (data_length > 0U) {
        for (size_t i = 0; i < data_length; i++) {
            s->leftover[s->leftover_offset + i] = data[i];
        }
        s->leftover_offset += data_length;
    }

    return TC_CRYPTO_SUCCESS;
}

int tc_sha256_final(uint8_t *digest, tc_sha256_state_t *s)
{
    if ((digest == NULL) || (s == NULL)) {
        return TC_CRYPTO_FAIL;
    }

    /* Add padding */
    uint64_t total_bits = s->bits_hashed + ((uint64_t)s->leftover_offset * 8U);
    size_t padding_len = TC_SHA256_BLOCK_SIZE - ((s->leftover_offset + 9U) % TC_SHA256_BLOCK_SIZE);

    s->leftover[s->leftover_offset] = 0x80;
    for (size_t i = s->leftover_offset + 1U; i < s->leftover_offset + 1U + padding_len; i++) {
        s->leftover[i] = 0x00;
    }

    /* Append length (big-endian) */
    uint8_t *len_ptr = s->leftover + s->leftover_offset + 1U + padding_len;
    for (int i = 0; i < 8; i++) {
        len_ptr[i] = (uint8_t)(total_bits >> (56 - 8 * i));
    }

    /* Process final block(s) */
    sha256_compress(s->iv, s->leftover);
    if (s->leftover_offset + 1U + padding_len + 8U > TC_SHA256_BLOCK_SIZE) {
        sha256_compress(s->iv, s->leftover + TC_SHA256_BLOCK_SIZE);
    }

    /* Output digest (big-endian).
     * iv[] already holds big-endian uint32_t values from sha256_compress,
     * so no swap32 needed here. */
    for (int i = 0; i < 8; i++) {
        uint32_t val = s->iv[i];
        digest[i * 4]     = (uint8_t)(val >> 24);
        digest[i * 4 + 1] = (uint8_t)(val >> 16);
        digest[i * 4 + 2] = (uint8_t)(val >> 8);
        digest[i * 4 + 3] = (uint8_t)val;
    }

    return TC_CRYPTO_SUCCESS;
}