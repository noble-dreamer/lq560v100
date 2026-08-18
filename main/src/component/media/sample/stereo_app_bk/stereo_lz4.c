/*
 * stereo_lz4.c - Userspace LZ4 compression + decompression
 *
 * Adapted from Linux 5.10 kernel source:
 *   lib/lz4/lz4_compress.c
 *   lib/lz4/lz4_decompress.c
 *   lib/lz4/lz4defs.h
 *
 * Original: LZ4 - Fast LZ compression algorithm
 * Copyright (C) 2011-2016, Yann Collet.
 * BSD 2-Clause License
 */

#include "stereo_lz4.h"

/* ========================================================================== */
/* Internal definitions (adapted from lz4defs.h)                               */
/* ========================================================================== */

/* Force inline — GCC/Clang */
#define FORCE_INLINE inline __attribute__((always_inline))

/* Architecture: aarch64 is 64-bit little-endian */
#define LZ4_ARCH64 1
#define LZ4_LITTLE_ENDIAN 1

/* likely / unlikely */
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

/* Basic types */
typedef uint8_t  BYTE;
typedef uint16_t U16;
typedef uint32_t U32;
typedef int32_t  S32;
typedef uint64_t U64;
typedef uintptr_t uptrval;

/* Constants */
#define MINMATCH 4
#define WILDCOPYLENGTH 8
#define LASTLITERALS 5
#define MFLIMIT (WILDCOPYLENGTH + MINMATCH)
#define MATCH_SAFEGUARD_DISTANCE ((2 * WILDCOPYLENGTH) - MINMATCH)
#define LZ4_SKIPTRIGGER 6
#define HASH_UNIT sizeof(size_t)
#define KB (1 << 10)
#define MB (1 << 20)
#define GB (1U << 30)
#define MAXD_LOG 16
#define MAX_DISTANCE ((1 << MAXD_LOG) - 1)
#define STEPSIZE sizeof(size_t)
#define ML_BITS 4
#define ML_MASK ((1U << ML_BITS) - 1)
#define RUN_BITS (8 - ML_BITS)
#define RUN_MASK ((1U << RUN_BITS) - 1)

/* -------------------------------------------------------------------------- */
/* Memory access — on aarch64 unaligned access is natively supported           */
/* -------------------------------------------------------------------------- */
static FORCE_INLINE U16 LZ4_read16(const void *ptr) { return *(const U16 *)ptr; }
static FORCE_INLINE U32 LZ4_read32(const void *ptr) { return *(const U32 *)ptr; }
static FORCE_INLINE size_t LZ4_read_ARCH(const void *ptr) { return *(const size_t *)ptr; }
static FORCE_INLINE void LZ4_write16(void *p, U16 v) { *(U16 *)p = v; }
static FORCE_INLINE void LZ4_write32(void *p, U32 v) { *(U32 *)p = v; }

static FORCE_INLINE U16 LZ4_readLE16(const void *memPtr)
{
#if LZ4_LITTLE_ENDIAN
    return *(const U16 *)memPtr;
#else
    const BYTE *p = (const BYTE *)memPtr;
    return (U16)((U16)p[0] | ((U16)p[1] << 8));
#endif
}

static FORCE_INLINE void LZ4_writeLE16(void *memPtr, U16 value)
{
#if LZ4_LITTLE_ENDIAN
    *(U16 *)memPtr = value;
#else
    BYTE *p = (BYTE *)memPtr;
    p[0] = (BYTE)value;
    p[1] = (BYTE)(value >> 8);
#endif
}

/* memcpy/memmove with __builtin_ for optimal inlining */
#define LZ4_memcpy(dst, src, size)  __builtin_memcpy(dst, src, size)
#define LZ4_memmove(dst, src, size) __builtin_memmove(dst, src, size)

static FORCE_INLINE void LZ4_copy8(void *dst, const void *src)
{
#if LZ4_ARCH64
    U64 a = *(const U64 *)src;
    *(U64 *)dst = a;
#else
    U32 a = *(const U32 *)src;
    U32 b = *(const U32 *)((const BYTE *)src + 4);
    *(U32 *)dst = a;
    *(U32 *)((BYTE *)dst + 4) = b;
#endif
}

/* Wildcopy: can overwrite up to 7 bytes past dstEnd */
static FORCE_INLINE void LZ4_wildCopy(void *dstPtr, const void *srcPtr, void *dstEnd)
{
    BYTE *d = (BYTE *)dstPtr;
    const BYTE *s = (const BYTE *)srcPtr;
    BYTE *const e = (BYTE *)dstEnd;
    do {
        LZ4_copy8(d, s);
        d += 8;
        s += 8;
    } while (d < e);
}

/* Count common bytes — use ctzll (count trailing zeros, 0-indexed) for correct
   byte index. The original kernel code used __builtin_ffsl (1-indexed) >> 3,
   which is off-by-one when the first differing bit is at the highest position
   of a byte (bit 7,15,...,63), causing match length over-count by 1 and
   introducing XOR 0x80 corruption in the decompressed output. */
static FORCE_INLINE unsigned int LZ4_NbCommonBytes(register size_t val)
{
#if LZ4_LITTLE_ENDIAN
    return __builtin_ctzll(val) >> 3;
#else
    return __builtin_clzll(val) >> 3;
#endif
}

static FORCE_INLINE unsigned int LZ4_count(
    const BYTE *pIn, const BYTE *pMatch, const BYTE *pInLimit)
{
    const BYTE *const pStart = pIn;

    while (likely(pIn < pInLimit - (STEPSIZE - 1))) {
        size_t const diff = LZ4_read_ARCH(pMatch) ^ LZ4_read_ARCH(pIn);
        if (!diff) {
            pIn += STEPSIZE;
            pMatch += STEPSIZE;
            continue;
        }
        pIn += LZ4_NbCommonBytes(diff);
        return (unsigned int)(pIn - pStart);
    }

#if LZ4_ARCH64
    if ((pIn < (pInLimit - 3)) && (LZ4_read32(pMatch) == LZ4_read32(pIn))) {
        pIn += 4;
        pMatch += 4;
    }
#endif
    if ((pIn < (pInLimit - 1)) && (LZ4_read16(pMatch) == LZ4_read16(pIn))) {
        pIn += 2;
        pMatch += 2;
    }
    if ((pIn < pInLimit) && (*pMatch == *pIn))
        pIn++;

    return (unsigned int)(pIn - pStart);
}

/* Enums for compression */
typedef enum { noLimit = 0, limitedOutput = 1 } limitedOutput_directive;
typedef enum { byPtr, byU32, byU16 } tableType_t;
typedef enum { noDict = 0, withPrefix64k, usingExtDict } dict_directive;
typedef enum { noDictIssue = 0, dictSmall } dictIssue_directive;

/* Enums for decompression */
typedef enum { endOnOutputSize = 0, endOnInputSize = 1 } endCondition_directive;
typedef enum { decode_full_block = 0, partial_decode = 1 } earlyEnd_directive;

/* Static assert */
#define LZ4_STATIC_ASSERT(c) _Static_assert(c, #c)

/* ========================================================================== */
/* Compression (adapted from lz4_compress.c)                                   */
/* ========================================================================== */

static const int LZ4_minLength = (MFLIMIT + 1);
static const int LZ4_64Klimit = ((64 * KB) + (MFLIMIT - 1));

static FORCE_INLINE U32 LZ4_hash4(U32 sequence, tableType_t const tableType)
{
    if (tableType == byU16)
        return ((sequence * 2654435761U) >> ((MINMATCH * 8) - (LZ4_HASHLOG + 1)));
    else
        return ((sequence * 2654435761U) >> ((MINMATCH * 8) - LZ4_HASHLOG));
}

static FORCE_INLINE U32 LZ4_hash5(U64 sequence, tableType_t const tableType)
{
    const U32 hashLog = (tableType == byU16) ? LZ4_HASHLOG + 1 : LZ4_HASHLOG;

#if LZ4_LITTLE_ENDIAN
    static const U64 prime5bytes = 889523592379ULL;
    return (U32)(((sequence << 24) * prime5bytes) >> (64 - hashLog));
#else
    static const U64 prime8bytes = 11400714785074694791ULL;
    return (U32)(((sequence >> 24) * prime8bytes) >> (64 - hashLog));
#endif
}

static FORCE_INLINE U32 LZ4_hashPosition(const void *p, tableType_t const tableType)
{
#if LZ4_ARCH64
    if (tableType == byU32)
        return LZ4_hash5(LZ4_read_ARCH(p), tableType);
#endif
    return LZ4_hash4(LZ4_read32(p), tableType);
}

static void LZ4_putPositionOnHash(const BYTE *p, U32 h, void *tableBase,
                                   tableType_t const tableType,
                                   const BYTE *srcBase)
{
    switch (tableType) {
    case byPtr: {
        const BYTE **hashTable = (const BYTE **)tableBase;
        hashTable[h] = p;
        return;
    }
    case byU32: {
        U32 *hashTable = (U32 *)tableBase;
        hashTable[h] = (U32)(p - srcBase);
        return;
    }
    case byU16: {
        U16 *hashTable = (U16 *)tableBase;
        hashTable[h] = (U16)(p - srcBase);
        return;
    }
    }
}

static FORCE_INLINE void LZ4_putPosition(const BYTE *p, void *tableBase,
                                          tableType_t tableType,
                                          const BYTE *srcBase)
{
    U32 const h = LZ4_hashPosition(p, tableType);
    LZ4_putPositionOnHash(p, h, tableBase, tableType, srcBase);
}

static const BYTE *LZ4_getPositionOnHash(U32 h, void *tableBase,
                                           tableType_t tableType,
                                           const BYTE *srcBase)
{
    if (tableType == byPtr) {
        const BYTE **hashTable = (const BYTE **)tableBase;
        return hashTable[h];
    }
    if (tableType == byU32) {
        const U32 *const hashTable = (const U32 *)tableBase;
        return hashTable[h] + srcBase;
    }
    {
        const U16 *const hashTable = (const U16 *)tableBase;
        return hashTable[h] + srcBase;
    }
}

static FORCE_INLINE const BYTE *LZ4_getPosition(const BYTE *p, void *tableBase,
                                                  tableType_t tableType,
                                                  const BYTE *srcBase)
{
    U32 const h = LZ4_hashPosition(p, tableType);
    return LZ4_getPositionOnHash(h, tableBase, tableType, srcBase);
}

static FORCE_INLINE int LZ4_compress_generic(
    LZ4_stream_t_internal *const dictPtr,
    const char *const source, char *const dest,
    const int inputSize, const int maxOutputSize,
    const limitedOutput_directive outputLimited,
    const tableType_t tableType,
    const dict_directive dict,
    const dictIssue_directive dictIssue,
    const U32 acceleration)
{
    const BYTE *ip = (const BYTE *)source;
    const BYTE *base;
    const BYTE *lowLimit;
    const BYTE *const lowRefLimit = ip - dictPtr->dictSize;
    const BYTE *const dictionary = dictPtr->dictionary;
    const BYTE *const dictEnd = dictionary + dictPtr->dictSize;
    const size_t dictDelta = dictEnd - (const BYTE *)source;
    const BYTE *anchor = (const BYTE *)source;
    const BYTE *const iend = ip + inputSize;
    const BYTE *const mflimit = iend - MFLIMIT;
    const BYTE *const matchlimit = iend - LASTLITERALS;

    BYTE *op = (BYTE *)dest;
    BYTE *const olimit = op + maxOutputSize;

    U32 forwardH;
    size_t refDelta = 0;

    if ((U32)inputSize > (U32)LZ4_MAX_INPUT_SIZE)
        return 0;

    switch (dict) {
    case noDict:
    default:
        base = (const BYTE *)source;
        lowLimit = (const BYTE *)source;
        break;
    case withPrefix64k:
        base = (const BYTE *)source - dictPtr->currentOffset;
        lowLimit = (const BYTE *)source - dictPtr->dictSize;
        break;
    case usingExtDict:
        base = (const BYTE *)source - dictPtr->currentOffset;
        lowLimit = (const BYTE *)source;
        break;
    }

    if ((tableType == byU16) && (inputSize >= LZ4_64Klimit))
        return 0;

    if (inputSize < LZ4_minLength)
        goto _last_literals;

    LZ4_putPosition(ip, dictPtr->hashTable, tableType, base);
    ip++;
    forwardH = LZ4_hashPosition(ip, tableType);

    for (;;) {
        const BYTE *match;
        BYTE *token;

        {
            const BYTE *forwardIp = ip;
            unsigned int step = 1;
            unsigned int searchMatchNb = acceleration << LZ4_SKIPTRIGGER;

            do {
                U32 const h = forwardH;
                ip = forwardIp;
                forwardIp += step;
                step = (searchMatchNb++ >> LZ4_SKIPTRIGGER);

                if (unlikely(forwardIp > mflimit))
                    goto _last_literals;

                match = LZ4_getPositionOnHash(h, dictPtr->hashTable,
                                               tableType, base);

                if (dict == usingExtDict) {
                    if (match < (const BYTE *)source) {
                        refDelta = dictDelta;
                        lowLimit = dictionary;
                    } else {
                        refDelta = 0;
                        lowLimit = (const BYTE *)source;
                    }
                }

                forwardH = LZ4_hashPosition(forwardIp, tableType);
                LZ4_putPositionOnHash(ip, h, dictPtr->hashTable,
                                       tableType, base);
            } while (((dictIssue == dictSmall) ? (match < lowRefLimit) : 0)
                     || ((tableType == byU16) ? 0 : (match + MAX_DISTANCE < ip))
                     || (LZ4_read32(match + refDelta) != LZ4_read32(ip)));
        }

        while (((ip > anchor) & (match + refDelta > lowLimit))
               && (unlikely(ip[-1] == match[refDelta - 1]))) {
            ip--;
            match--;
        }

        {
            unsigned const int litLength = (unsigned int)(ip - anchor);
            token = op++;

            if ((outputLimited) && (unlikely(op + litLength +
                (2 + 1 + LASTLITERALS) + (litLength / 255) > olimit)))
                return 0;

            if (litLength >= RUN_MASK) {
                int len = (int)litLength - RUN_MASK;
                *token = (RUN_MASK << ML_BITS);
                for (; len >= 255; len -= 255)
                    *op++ = 255;
                *op++ = (BYTE)len;
            } else
                *token = (BYTE)(litLength << ML_BITS);

            LZ4_wildCopy(op, anchor, op + litLength);
            op += litLength;
        }

_next_match:
        LZ4_writeLE16(op, (U16)(ip - match));
        op += 2;

        {
            unsigned int matchCode;

            if ((dict == usingExtDict) && (lowLimit == dictionary)) {
                const BYTE *limit;
                match += refDelta;
                limit = ip + (dictEnd - match);
                if (limit > matchlimit)
                    limit = matchlimit;
                matchCode = LZ4_count(ip + MINMATCH, match + MINMATCH, limit);
                ip += MINMATCH + matchCode;
                if (ip == limit) {
                    unsigned const int more = LZ4_count(ip, (const BYTE *)source,
                                                        matchlimit);
                    matchCode += more;
                    ip += more;
                }
            } else {
                matchCode = LZ4_count(ip + MINMATCH, match + MINMATCH,
                                       matchlimit);
                ip += MINMATCH + matchCode;
            }

            if (outputLimited && (unlikely(op + (1 + LASTLITERALS) +
                (matchCode >> 8) > olimit)))
                return 0;

            if (matchCode >= ML_MASK) {
                *token += ML_MASK;
                matchCode -= ML_MASK;
                LZ4_write32(op, 0xFFFFFFFF);
                while (matchCode >= 4 * 255) {
                    op += 4;
                    LZ4_write32(op, 0xFFFFFFFF);
                    matchCode -= 4 * 255;
                }
                op += matchCode / 255;
                *op++ = (BYTE)(matchCode % 255);
            } else
                *token += (BYTE)(matchCode);
        }

        anchor = ip;

        if (ip > mflimit)
            break;

        LZ4_putPosition(ip - 2, dictPtr->hashTable, tableType, base);

        match = LZ4_getPosition(ip, dictPtr->hashTable, tableType, base);

        if (dict == usingExtDict) {
            if (match < (const BYTE *)source) {
                refDelta = dictDelta;
                lowLimit = dictionary;
            } else {
                refDelta = 0;
                lowLimit = (const BYTE *)source;
            }
        }

        LZ4_putPosition(ip, dictPtr->hashTable, tableType, base);

        if (((dictIssue == dictSmall) ? (match >= lowRefLimit) : 1)
            && (match + MAX_DISTANCE >= ip)
            && (LZ4_read32(match + refDelta) == LZ4_read32(ip))) {
            token = op++;
            *token = 0;
            goto _next_match;
        }

        forwardH = LZ4_hashPosition(++ip, tableType);
    }

_last_literals:
    {
        size_t const lastRun = (size_t)(iend - anchor);

        if ((outputLimited) &&
            ((op - (BYTE *)dest) + lastRun + 1 +
            ((lastRun + 255 - RUN_MASK) / 255) > (U32)maxOutputSize))
            return 0;

        if (lastRun >= RUN_MASK) {
            size_t accumulator = lastRun - RUN_MASK;
            *op++ = RUN_MASK << ML_BITS;
            for (; accumulator >= 255; accumulator -= 255)
                *op++ = 255;
            *op++ = (BYTE)accumulator;
        } else {
            *op++ = (BYTE)(lastRun << ML_BITS);
        }

        LZ4_memcpy(op, anchor, lastRun);
        op += lastRun;
    }

    return (int)(((char *)op) - dest);
}

static int LZ4_compress_fast_extState(void *state, const char *source,
    char *dest, int inputSize, int maxOutputSize, int acceleration)
{
    LZ4_stream_t_internal *ctx = &((LZ4_stream_t *)state)->internal_donotuse;
#if LZ4_ARCH64
    const tableType_t tableType = byU32;
#else
    const tableType_t tableType = byPtr;
#endif

    LZ4_resetStream((LZ4_stream_t *)state);

    if (acceleration < 1)
        acceleration = LZ4_ACCELERATION_DEFAULT;

    if (maxOutputSize >= LZ4_COMPRESSBOUND(inputSize)) {
        if (inputSize < LZ4_64Klimit)
            return LZ4_compress_generic(ctx, source, dest, inputSize, 0,
                noLimit, byU16, noDict, noDictIssue, acceleration);
        else
            return LZ4_compress_generic(ctx, source, dest, inputSize, 0,
                noLimit, tableType, noDict, noDictIssue, acceleration);
    } else {
        if (inputSize < LZ4_64Klimit)
            return LZ4_compress_generic(ctx, source, dest, inputSize,
                maxOutputSize, limitedOutput, byU16, noDict,
                noDictIssue, acceleration);
        else
            return LZ4_compress_generic(ctx, source, dest, inputSize,
                maxOutputSize, limitedOutput, tableType, noDict,
                noDictIssue, acceleration);
    }
}

int LZ4_compress_fast(const char *source, char *dest, int inputSize,
    int maxOutputSize, int acceleration, void *wrkmem)
{
    return LZ4_compress_fast_extState(wrkmem, source, dest, inputSize,
        maxOutputSize, acceleration);
}

int LZ4_compress_default(const char *source, char *dest, int inputSize,
    int maxOutputSize, void *wrkmem)
{
    return LZ4_compress_fast(source, dest, inputSize, maxOutputSize,
        LZ4_ACCELERATION_DEFAULT, wrkmem);
}

void LZ4_resetStream(LZ4_stream_t *stream)
{
    memset(stream, 0, sizeof(LZ4_stream_t));
}

/* ========================================================================== */
/* Decompression — clean implementation of LZ4 block format                     */
/* Board-side use for round-trip testing; PC side should use Python lz4 lib    */
/* ========================================================================== */

int LZ4_decompress_safe(const char *source, char *dest,
    int compressedSize, int maxDecompressedSize)
{
    const BYTE *ip = (const BYTE *)source;
    const BYTE *const iend = ip + compressedSize;
    BYTE *op = (BYTE *)dest;
    BYTE *const oend = op + maxDecompressedSize;

    if (ip >= iend)
        return (maxDecompressedSize == 0) ? 0 : -1;

    while (ip < iend) {
        unsigned int token;
        unsigned int lit_len;
        unsigned int match_len;
        unsigned int offset;
        const BYTE *match;

        /* --- Token: high 4 bits = literal length, low 4 bits = match length --- */
        token = *ip++;
        lit_len = token >> ML_BITS;

        /* --- Extended literal length --- */
        if (lit_len == RUN_MASK) {
            unsigned int s;
            do {
                if (ip >= iend)
                    return -1;
                s = *ip++;
                lit_len += s;
            } while (s == 255);
        }

        /* --- Bounds check literals --- */
        if ((size_t)(iend - ip) < lit_len)
            return -1;
        if ((size_t)(oend - op) < lit_len)
            return -1;

        /* --- Copy literals --- */
        if (lit_len > 0) {
            LZ4_memcpy(op, ip, lit_len);
            op += lit_len;
            ip += lit_len;
        }

        /* --- End of input? Last sequence is literals only --- */
        if (ip >= iend)
            break;

        /* --- Need at least 2 bytes for offset --- */
        if ((size_t)(iend - ip) < 2)
            return -1;

        /* --- Read match offset (16-bit LE) --- */
        offset = (unsigned int)LZ4_readLE16(ip);
        ip += 2;
        if (offset == 0)
            return -1;  /* invalid offset */

        /* --- Match starts 'offset' bytes before current output position --- */
        match = op - offset;
        if (match < (const BYTE *)dest)
            return -1;  /* offset too large */

        /* --- Match length from token --- */
        match_len = token & ML_MASK;
        if (match_len == ML_MASK) {
            unsigned int s;
            do {
                if (ip >= iend)
                    return -1;
                s = *ip++;
                match_len += s;
            } while (s == 255);
        }
        match_len += MINMATCH;  /* match length includes the 4-byte minimum match */

        /* --- Bounds check match output --- */
        if ((size_t)(oend - op) < match_len)
            return -1;

        /* --- Copy match (byte-by-byte to handle overlap) --- */
        {
            unsigned int i;
            for (i = 0; i < match_len; i++)
                op[i] = match[i];
            op += match_len;
        }
    }

    return (int)(op - (BYTE *)dest);
}
