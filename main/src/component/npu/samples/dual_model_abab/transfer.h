/*
 * transfer.h - frame protocol for streaming NPU results over SSH stdio.
 *
 * The board sample runs inside an SSH session. stdout (fd 1) carries only
 * framed protocol records and stdin (fd 0) carries host->board control frames.
 * SSH supplies transport, encryption and authentication, so no virtual-USB
 * (RNDIS) resource is consumed; that channel stays free for the upgrade
 * handshake.
 *
 * Frame layout (little-endian, header + payload):
 *   magic u32 | version u8 | flags u8 | type u8 | model_id u8 | seq u32 |
 *   timestamp_us u64 | orig_len u32 | payload_len u32 | payload_crc u32 |
 *   payload[payload_len]
 */
#ifndef __NPU_SAMPLE_TRANSFER_H__
#define __NPU_SAMPLE_TRANSFER_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRANSFER_MAGIC          (0x5453504eu)  /* "NPST" */
#define TRANSFER_VERSION        (1u)
#define TRANSFER_HEADER_SIZE    (32u)
#define TRANSFER_COMPRESS_MIN   (256u)         /* try compression >= this size */
#define TRANSFER_RESYNC_BUDGET  (1024u * 1024u)

typedef enum {
    TRANSFER_TYPE_SYNC = 0x01,    /* stream hello / checkpoint */
    TRANSFER_TYPE_RESULT = 0x02,  /* per-frame inference result */
    TRANSFER_TYPE_TENSOR = 0x03,  /* raw output tensor */
    TRANSFER_TYPE_ACK = 0x04,     /* acknowledgement */
    TRANSFER_TYPE_CONTROL = 0x05, /* host -> board control */
    TRANSFER_TYPE_ERROR = 0x06,
} transfer_frame_type;

typedef enum {
    TRANSFER_CTRL_STOP = 0x01,
    TRANSFER_CTRL_RESUME = 0x02,
} transfer_control_cmd;

#define TRANSFER_FLAG_COMPRESSED    (0x01u)
#define TRANSFER_FLAG_HAS_TIMESTAMP (0x02u)

typedef struct {
    uint32_t magic;
    uint8_t version;
    uint8_t flags;
    uint8_t type;
    uint8_t model_id;
    uint32_t seq;
    uint64_t timestamp_us;
    uint32_t orig_len;    /* payload size before compression */
    uint32_t payload_len; /* payload size on the wire */
    uint32_t payload_crc; /* CRC32 of the wire payload */
} transfer_header;

typedef struct {
    int fd_in;
    int fd_out;
    bool has_timestamp;
    uint32_t last_rx_seq;
    bool rx_seen_frame;
    uint32_t bad_frames;
} transfer_ctx;

#define TRANSFER_RECV_OK      (0)
#define TRANSFER_RECV_TIMEOUT (1)
#define TRANSFER_RECV_ERROR   (-1)

/* Explicit little-endian codecs shared with the host-side Python decoder. */
static inline void transfer_put_u8(uint8_t *p, uint8_t v)
{
    p[0] = v;
}

static inline void transfer_put_u16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v & 0xffu);
    p[1] = (uint8_t)((v >> 8) & 0xffu);
}

static inline void transfer_put_u32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v & 0xffu);
    p[1] = (uint8_t)((v >> 8) & 0xffu);
    p[2] = (uint8_t)((v >> 16) & 0xffu);
    p[3] = (uint8_t)((v >> 24) & 0xffu);
}

static inline void transfer_put_u64(uint8_t *p, uint64_t v)
{
    transfer_put_u32(p, (uint32_t)(v & 0xffffffffu));
    transfer_put_u32(p + 4, (uint32_t)(v >> 32));
}

static inline void transfer_put_f32(uint8_t *p, float v)
{
    uint32_t bits;

    memcpy(&bits, &v, sizeof(bits));
    transfer_put_u32(p, bits);
}

static inline uint8_t transfer_get_u8(const uint8_t *p)
{
    return p[0];
}

static inline uint16_t transfer_get_u16(const uint8_t *p)
{
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static inline uint32_t transfer_get_u32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static inline uint64_t transfer_get_u64(const uint8_t *p)
{
    return (uint64_t)transfer_get_u32(p) |
           ((uint64_t)transfer_get_u32(p + 4) << 32);
}

/* IEEE CRC32, identical to Python's binascii.crc32. 查表实现：大 tensor 载荷
 * 下比逐位计算快约 8 倍，是 stream=2 模式的 CPU 热点之一。 */
static inline uint32_t transfer_crc32(const void *data, size_t len)
{
    static uint32_t table[256];
    static bool ready = false;
    const uint8_t *p = (const uint8_t *)data;
    uint32_t crc = 0xffffffffu;
    size_t i;

    if (!ready) {
        uint32_t j;

        for (i = 0; i < 256; i++) {
            uint32_t c = i;

            for (j = 0; j < 8; j++) {
                c = (c & 1u) ? (0xedb88320u ^ (c >> 1)) : (c >> 1);
            }
            table[i] = c;
        }
        ready = true;
    }
    for (i = 0; i < len; i++) {
        crc = table[(crc ^ p[i]) & 0xffu] ^ (crc >> 8);
    }
    return crc ^ 0xffffffffu;
}

/* Sample-local internals shared between transfer*.c; not for pipeline use. */
int transfer_compress_buf(const uint8_t *in, size_t in_len, uint8_t **out,
                          size_t *out_len);
int transfer_decompress_buf(const uint8_t *in, size_t in_len, uint8_t *out,
                            size_t out_len);
void transfer_header_encode(const transfer_header *h,
                            uint8_t out[TRANSFER_HEADER_SIZE]);
void transfer_header_decode(const uint8_t in[TRANSFER_HEADER_SIZE],
                            transfer_header *h);
int transfer_read_full(int fd, void *buf, size_t len, int timeout_ms);
int transfer_write_full(int fd, const void *buf, size_t len);
int transfer_read_header(transfer_ctx *ctx, int timeout_ms, transfer_header *h);

int transfer_init(transfer_ctx *ctx, int fd_in, int fd_out);

int transfer_send(transfer_ctx *ctx, uint8_t type, uint8_t model_id, uint32_t seq,
                  const void *payload, size_t payload_len, bool compress);

/* On TRANSFER_RECV_OK, *payload is malloc'd (orig_len bytes); caller frees it.
 * timeout_ms < 0 blocks, 0 polls. */
int transfer_recv(transfer_ctx *ctx, int timeout_ms, uint8_t *type, uint8_t *model_id,
                  uint32_t *seq, void **payload, size_t *payload_len);

#ifdef __cplusplus
}
#endif

#endif /* __NPU_SAMPLE_TRANSFER_H__ */
