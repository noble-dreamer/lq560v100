/*
 * transfer.c - public API of the SSH stream transport: framing, sync, CRC.
 *
 * Every frame is checksummed and sequence-numbered. The wire codec and
 * re-sync live in transfer_io.c, zlib support in transfer_zlib.c.
 */
#include "transfer.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>

static uint64_t now_us(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000ull + (uint64_t)tv.tv_usec;
}

/* 一次 writev 把帧头和载荷写入通道：SSH 按写次数组包，减少小包数量能
 * 明显降低 USB/RNDIS 链路上的传输延迟。 */
static int write_frame(int fd, const void *header, const void *payload, size_t payload_len)
{
    struct iovec iov[2];
    struct iovec *cur = iov;
    int iovcnt = 1;
    size_t total = TRANSFER_HEADER_SIZE + payload_len;
    size_t done = 0;

    cur[0].iov_base = (void *)header;
    cur[0].iov_len = TRANSFER_HEADER_SIZE;
    if (payload_len > 0) {
        cur[1].iov_base = (void *)payload;
        cur[1].iov_len = payload_len;
        iovcnt = 2;
    }
    while (done < total) {
        ssize_t n = writev(fd, cur, iovcnt);

        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        done += (size_t)n;
        /* 按已写字节数推进 iovec，处理部分写 */
        while (iovcnt > 0 && (size_t)n >= cur[0].iov_len) {
            n -= (ssize_t)cur[0].iov_len;
            cur++;
            iovcnt--;
        }
        if (iovcnt > 0 && n > 0) {
            cur[0].iov_base = (uint8_t *)cur[0].iov_base + n;
            cur[0].iov_len -= (size_t)n;
        }
    }
    return 0;
}

int transfer_init(transfer_ctx *ctx, int fd_in, int fd_out)
{
    if (ctx == NULL || fd_in < 0 || fd_out < 0) {
        return -1;
    }
    memset(ctx, 0, sizeof(*ctx));
    ctx->fd_in = fd_in;
    ctx->fd_out = fd_out;
    ctx->has_timestamp = true;
    return 0;
}

int transfer_send(transfer_ctx *ctx, uint8_t type, uint8_t model_id, uint32_t seq,
                  const void *payload, size_t payload_len, bool compress)
{
    uint8_t header[TRANSFER_HEADER_SIZE];
    transfer_header h;
    uint8_t *wire = NULL;
    size_t wire_len = payload_len;
    int ret = -1;

    if (ctx == NULL || (payload == NULL && payload_len != 0)) {
        return -1;
    }
    memset(&h, 0, sizeof(h));
    if (compress && payload_len >= TRANSFER_COMPRESS_MIN &&
        transfer_compress_buf((const uint8_t *)payload, payload_len, &wire, &wire_len) == 0) {
        h.flags |= TRANSFER_FLAG_COMPRESSED;
    }
    h.magic = TRANSFER_MAGIC;
    h.version = TRANSFER_VERSION;
    h.type = type;
    h.model_id = model_id;
    h.seq = seq;
    if (ctx->has_timestamp) {
        h.flags |= TRANSFER_FLAG_HAS_TIMESTAMP;
        h.timestamp_us = now_us();
    }
    h.orig_len = (uint32_t)payload_len;
    h.payload_len = (uint32_t)wire_len;
    h.payload_crc = transfer_crc32(wire != NULL ? wire : payload, wire_len);

    transfer_header_encode(&h, header);
    if (write_frame(ctx->fd_out, header, wire != NULL ? wire : payload, wire_len) != 0) {
        goto out;
    }
    ret = 0;
out:
    free(wire);
    return ret;
}

int transfer_recv(transfer_ctx *ctx, int timeout_ms, uint8_t *type, uint8_t *model_id,
                  uint32_t *seq, void **payload, size_t *payload_len)
{
    transfer_header h;
    uint8_t *wire;
    uint8_t *data;
    int rc;

    if (ctx == NULL || type == NULL || model_id == NULL || seq == NULL ||
        payload == NULL || payload_len == NULL) {
        return TRANSFER_RECV_ERROR;
    }
    *payload = NULL;
    *payload_len = 0;

    rc = transfer_read_header(ctx, timeout_ms, &h);
    if (rc != TRANSFER_RECV_OK) {
        return rc;
    }
    wire = (uint8_t *)malloc(h.payload_len != 0 ? h.payload_len : 1);
    if (wire == NULL) {
        return TRANSFER_RECV_ERROR;
    }
    if (h.payload_len > 0 &&
        transfer_read_full(ctx->fd_in, wire, h.payload_len, timeout_ms) != TRANSFER_RECV_OK) {
        free(wire);
        return TRANSFER_RECV_ERROR;
    }
    if (transfer_crc32(wire, h.payload_len) != h.payload_crc) {
        ctx->bad_frames++;
        free(wire);
        return TRANSFER_RECV_ERROR;
    }
    if ((h.flags & TRANSFER_FLAG_COMPRESSED) != 0) {
        data = (uint8_t *)malloc(h.orig_len != 0 ? h.orig_len : 1);
        if (data == NULL ||
            transfer_decompress_buf(wire, h.payload_len, data, h.orig_len) != 0) {
            free(data);
            free(wire);
            ctx->bad_frames++;
            return TRANSFER_RECV_ERROR;
        }
        free(wire);
    } else {
        if (h.orig_len != h.payload_len) {
            free(wire);
            return TRANSFER_RECV_ERROR;
        }
        data = wire;
    }
    *type = h.type;
    *model_id = h.model_id;
    *seq = h.seq;
    *payload = data;
    *payload_len = h.orig_len;
    ctx->last_rx_seq = h.seq;
    ctx->rx_seen_frame = true;
    return TRANSFER_RECV_OK;
}
