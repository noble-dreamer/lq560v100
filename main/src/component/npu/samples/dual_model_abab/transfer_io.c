/*
 * transfer_io.c - byte-level IO, wire codec and header re-sync.
 *
 * Shared by transfer.c for both directions of the SSH stdio stream. Header
 * reads re-synchronize on the magic word so a stray log line (or a receiver
 * that attaches mid-stream) cannot permanently derail the decoder.
 */
#include "transfer.h"

#include <errno.h>
#include <sys/select.h>
#include <unistd.h>

static int wait_readable(int fd, int timeout_ms)
{
    struct timeval tv;
    fd_set rfds;

    if (timeout_ms < 0) {
        return 0;
    }
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    return select(fd + 1, &rfds, NULL, NULL, &tv);
}

int transfer_read_full(int fd, void *buf, size_t len, int timeout_ms)
{
    uint8_t *p = (uint8_t *)buf;
    size_t done = 0;

    while (done < len) {
        ssize_t n;
        int rc = wait_readable(fd, timeout_ms);

        if (rc < 0) {
            if (errno == EINTR) {
                continue;
            }
            return TRANSFER_RECV_ERROR;
        }
        if (rc == 0) {
            return TRANSFER_RECV_TIMEOUT;
        }
        n = read(fd, p + done, len - done);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return TRANSFER_RECV_ERROR;
        }
        if (n == 0) {
            return TRANSFER_RECV_ERROR; /* EOF mid-frame */
        }
        done += (size_t)n;
    }
    return TRANSFER_RECV_OK;
}

int transfer_write_full(int fd, const void *buf, size_t len)
{
    const uint8_t *p = (const uint8_t *)buf;
    size_t done = 0;

    while (done < len) {
        ssize_t n = write(fd, p + done, len - done);

        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        done += (size_t)n;
    }
    return 0;
}

void transfer_header_encode(const transfer_header *h, uint8_t out[TRANSFER_HEADER_SIZE])
{
    size_t off = 0;

    transfer_put_u32(out + off, h->magic);
    off += 4;
    out[off++] = h->version;
    out[off++] = h->flags;
    out[off++] = h->type;
    out[off++] = h->model_id;
    transfer_put_u32(out + off, h->seq);
    off += 4;
    transfer_put_u64(out + off, h->timestamp_us);
    off += 8;
    transfer_put_u32(out + off, h->orig_len);
    off += 4;
    transfer_put_u32(out + off, h->payload_len);
    off += 4;
    transfer_put_u32(out + off, h->payload_crc);
}

void transfer_header_decode(const uint8_t in[TRANSFER_HEADER_SIZE], transfer_header *h)
{
    size_t off = 0;

    h->magic = transfer_get_u32(in + off);
    off += 4;
    h->version = in[off++];
    h->flags = in[off++];
    h->type = in[off++];
    h->model_id = in[off++];
    h->seq = transfer_get_u32(in + off);
    off += 4;
    h->timestamp_us = transfer_get_u64(in + off);
    off += 8;
    h->orig_len = transfer_get_u32(in + off);
    off += 4;
    h->payload_len = transfer_get_u32(in + off);
    off += 4;
    h->payload_crc = transfer_get_u32(in + off);
}

static int scan_magic(int fd, int timeout_ms)
{
    static const uint8_t magic[4] = {'N', 'P', 'S', 'T'};
    size_t got = 0;
    size_t budget = TRANSFER_RESYNC_BUDGET;

    while (got < 4 && budget-- > 0) {
        uint8_t c;

        if (transfer_read_full(fd, &c, 1, timeout_ms) != TRANSFER_RECV_OK) {
            return TRANSFER_RECV_ERROR;
        }
        if (c == magic[got]) {
            got++;
        } else {
            got = (c == magic[0]) ? 1u : 0u;
        }
    }
    return (got == 4) ? TRANSFER_RECV_OK : TRANSFER_RECV_ERROR;
}

int transfer_read_header(transfer_ctx *ctx, int timeout_ms, transfer_header *h)
{
    uint8_t raw[TRANSFER_HEADER_SIZE];
    int rc = transfer_read_full(ctx->fd_in, raw, sizeof(raw), timeout_ms);

    if (rc != TRANSFER_RECV_OK) {
        return rc;
    }
    transfer_header_decode(raw, h);
    if (h->magic == TRANSFER_MAGIC && h->version <= TRANSFER_VERSION && h->type != 0) {
        return TRANSFER_RECV_OK;
    }

    /* Skip forward to the next magic word, then read the rest of its header. */
    ctx->bad_frames++;
    if (scan_magic(ctx->fd_in, timeout_ms) != TRANSFER_RECV_OK) {
        return TRANSFER_RECV_ERROR;
    }
    if (transfer_read_full(ctx->fd_in, raw + 4, sizeof(raw) - 4, timeout_ms) !=
        TRANSFER_RECV_OK) {
        return TRANSFER_RECV_ERROR;
    }
    transfer_header_decode(raw, h);
    if (h->magic != TRANSFER_MAGIC || h->version > TRANSFER_VERSION) {
        return TRANSFER_RECV_ERROR;
    }
    return TRANSFER_RECV_OK;
}
