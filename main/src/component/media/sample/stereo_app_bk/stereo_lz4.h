/*
 * stereo_lz4.h - Userspace LZ4 compression (adapted from Linux kernel source)
 *
 * Original: LZ4 - Fast LZ compression algorithm
 * Copyright (C) 2011-2016, Yann Collet.
 * BSD 2-Clause License
 *
 * Kernel adaptation: Sven Schmidt
 * Userspace adaptation for stereo_app: 2024
 */

#ifndef __STEREO_LZ4_H__
#define __STEREO_LZ4_H__

#include <stdint.h>
#include <string.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* Constants                                                                  */
/* -------------------------------------------------------------------------- */
#define LZ4_MEMORY_USAGE 14
#define LZ4_MAX_INPUT_SIZE 0x7E000000

#define LZ4_COMPRESSBOUND(isize) \
    ((unsigned int)(isize) > (unsigned int)LZ4_MAX_INPUT_SIZE \
     ? 0 \
     : (isize) + ((isize) / 255) + 16)

#define LZ4_ACCELERATION_DEFAULT 1
#define LZ4_HASHLOG      (LZ4_MEMORY_USAGE - 2)
#define LZ4_HASHTABLESIZE (1 << LZ4_MEMORY_USAGE)
#define LZ4_HASH_SIZE_U32 (1 << LZ4_HASHLOG)

#define LZ4_STREAMSIZE_U64 ((1 << (LZ4_MEMORY_USAGE - 3)) + 4)
#define LZ4_STREAMSIZE    (LZ4_STREAMSIZE_U64 * sizeof(unsigned long long))
#define LZ4_MEM_COMPRESS  LZ4_STREAMSIZE

/* -------------------------------------------------------------------------- */
/* Stream structures                                                          */
/* -------------------------------------------------------------------------- */
typedef struct {
    uint32_t hashTable[LZ4_HASH_SIZE_U32];
    uint32_t currentOffset;
    uint32_t initCheck;
    const uint8_t *dictionary;
    uint8_t *bufferStart;
    uint32_t dictSize;
} LZ4_stream_t_internal;

typedef union {
    unsigned long long table[LZ4_STREAMSIZE_U64];
    LZ4_stream_t_internal internal_donotuse;
} LZ4_stream_t;

/* -------------------------------------------------------------------------- */
/* API                                                                        */
/* -------------------------------------------------------------------------- */

static inline int LZ4_compressBound(size_t isize)
{
    return LZ4_COMPRESSBOUND(isize);
}

void LZ4_resetStream(LZ4_stream_t *stream);

int LZ4_compress_default(const char *source, char *dest, int inputSize,
                          int maxOutputSize, void *wrkmem);

int LZ4_compress_fast(const char *source, char *dest, int inputSize,
                       int maxOutputSize, int acceleration, void *wrkmem);

int LZ4_decompress_safe(const char *source, char *dest, int compressedSize,
                         int maxDecompressedSize);

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_LZ4_H__ */
