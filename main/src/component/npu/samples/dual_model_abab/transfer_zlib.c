/*
 * transfer_zlib.c - zlib compression glue for the SSH stream transport.
 *
 * Built without NPU_TRANSFER_ZLIB, payloads are sent uncompressed and
 * decompression is rejected, so the module stays self-contained.
 */
#include "transfer.h"

#include <stdlib.h>

#ifdef NPU_TRANSFER_ZLIB
#include <zlib.h>
#endif

/* Returns 0 and fills the output pointers when compression helps, 1 when it
 * does not (send uncompressed), -1 on error. */
int transfer_compress_buf(const uint8_t *in, size_t in_len, uint8_t **out,
                          size_t *out_len)
{
#ifdef NPU_TRANSFER_ZLIB
    uLongf bound = compressBound((uLong)in_len);
    uint8_t *buf = (uint8_t *)malloc((size_t)bound);

    if (buf == NULL) {
        return -1;
    }
    if (compress2(buf, &bound, in, (uLong)in_len, Z_BEST_SPEED) != Z_OK) {
        free(buf);
        return -1;
    }
    if ((size_t)bound >= in_len) {
        free(buf);
        return 1;
    }
    *out = buf;
    *out_len = (size_t)bound;
    return 0;
#else
    (void)in;
    (void)in_len;
    (void)out;
    (void)out_len;
    return 1;
#endif
}

int transfer_decompress_buf(const uint8_t *in, size_t in_len, uint8_t *out,
                            size_t out_len)
{
#ifdef NPU_TRANSFER_ZLIB
    uLongf dest = (uLongf)out_len;

    if (uncompress(out, &dest, in, (uLong)in_len) != Z_OK || dest != out_len) {
        return -1;
    }
    return 0;
#else
    (void)in;
    (void)in_len;
    (void)out;
    (void)out_len;
    return -1;
#endif
}
