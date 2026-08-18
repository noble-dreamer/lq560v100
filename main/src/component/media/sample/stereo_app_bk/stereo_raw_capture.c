#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include "stereo_raw_capture.h"
#include "stereo_media.h"
#include "stereo_types.h"
#include "ot_omi_sys_mem.h"

#define STEREO_RAW_CAPTURE_CMD_MAGIC 0x53545243u
#define STEREO_RAW_CAPTURE_CMD_GRAB  0x01u
#define STEREO_RAW_CAPTURE_MASK_LEFT 0x01u
#define STEREO_RAW_CAPTURE_MASK_RIGHT 0x02u
#define STEREO_RAW_META_SIZE 24u
#define STEREO_RAW_DUMP_DEPTH 1u
#define STEREO_RAW_CAPTURE_TIMEOUT_MS 3000

static ot_vi_chn_handle g_raw_chn[STEREO_SNS_NUM];
static pthread_t g_raw_tid;
static volatile ot_bool g_raw_run = OT_FALSE;
static ot_bool g_raw_started = OT_FALSE;
static ot_s32 g_raw_listen_fd = -1;

static void raw_put_u16(ot_u8 *p, ot_u16 v)
{
    p[0] = (ot_u8)((v >> 8) & 0xFF);
    p[1] = (ot_u8)(v & 0xFF);
}

static void raw_put_u32(ot_u8 *p, ot_u32 v)
{
    p[0] = (ot_u8)((v >> 24) & 0xFF);
    p[1] = (ot_u8)((v >> 16) & 0xFF);
    p[2] = (ot_u8)((v >> 8) & 0xFF);
    p[3] = (ot_u8)(v & 0xFF);
}

static ot_u64 raw_htonll(ot_u64 v)
{
    return (((ot_u64)htonl((ot_u32)(v & 0xFFFFFFFF))) << 32) |
           ((ot_u64)htonl((ot_u32)(v >> 32)));
}

static ot_s32 raw_send_all(ot_s32 fd, const ot_u8 *data, ot_u32 size)
{
    ot_u32 sent = 0;
    while (sent < size) {
        ssize_t ret = send(fd, data + sent, size - sent, MSG_NOSIGNAL);
        if (ret < 0) {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(1000);
                continue;
            }
            stereo_log_write("[raw_capture] send failed errno=%d:%s sent=%u/%u\n",
                             errno, strerror(errno), sent, size);
            return OT_FAILURE;
        }
        if (ret == 0) {
            stereo_log_write("[raw_capture] send returned 0 sent=%u/%u\n", sent, size);
            return OT_FAILURE;
        }
        sent += (ot_u32)ret;
    }
    return OT_SUCCESS;
}

static ot_s32 raw_recv_all(ot_s32 fd, ot_u8 *data, ot_u32 size)
{
    ot_u32 got = 0;
    while (got < size) {
        ot_s32 ret = recv(fd, data + got, size - got, 0);
        if (ret <= 0) {
            return OT_FAILURE;
        }
        got += (ot_u32)ret;
    }
    return OT_SUCCESS;
}

static ot_bool raw_is_yuv420sp(ot_eis_img_pixel_format fmt)
{
    return (fmt == OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR ||
            fmt == OT_EIS_IMAGE_FORMAT_YUV_420_SEMIPLANAR) ? OT_TRUE : OT_FALSE;
}

static ot_s32 raw_pack_yuv420sp(const ot_eis_video_frame *frame, ot_u8 *dst, ot_u32 dst_size)
{
    ot_u32 width = frame->attr.width;
    ot_u32 height = frame->attr.height;
    ot_u32 y_size = width * height;
    ot_u32 vu_size = width * height / 2;
    ot_u32 y_map_size = frame->buff.stride[0] * height;
    ot_u32 vu_map_size = frame->buff.stride[1] * height / 2;
    ot_u8 *src_y = (ot_u8 *)frame->buff.virt_addr[0];
    ot_u8 *src_vu = (ot_u8 *)frame->buff.virt_addr[1];
    ot_void *mapped_y = OT_NULL;
    ot_void *mapped_vu = OT_NULL;

    if (dst_size < y_size + vu_size || frame->buff.stride[0] < width || frame->buff.stride[1] < width) {
        stereo_log_write("[raw_capture] invalid pack params %ux%u stride=%u/%u dst=%u\n",
                         width, height, frame->buff.stride[0], frame->buff.stride[1], dst_size);
        return OT_FAILURE;
    }

    if (frame->buff.phys_addr[0] != 0) {
        mapped_y = ot_omi_sys_mmap(frame->buff.phys_addr[0], y_map_size);
        if (!mapped_y) {
            stereo_log_write("[raw_capture] mmap Y failed phys=0x%llx size=%u\n",
                             (unsigned long long)frame->buff.phys_addr[0], y_map_size);
            return OT_FAILURE;
        }
        src_y = (ot_u8 *)mapped_y;
    }

    if (frame->buff.phys_addr[1] != 0) {
        mapped_vu = ot_omi_sys_mmap(frame->buff.phys_addr[1], vu_map_size);
        if (!mapped_vu) {
            stereo_log_write("[raw_capture] mmap UV failed phys=0x%llx size=%u\n",
                             (unsigned long long)frame->buff.phys_addr[1], vu_map_size);
            if (mapped_y) {
                ot_omi_sys_munmap(mapped_y, y_map_size);
            }
            return OT_FAILURE;
        }
        src_vu = (ot_u8 *)mapped_vu;
    }

    if (!src_y || !src_vu) {
        stereo_log_write("[raw_capture] no usable YUV address virt=%p/%p phys=0x%llx/0x%llx\n",
                         frame->buff.virt_addr[0], frame->buff.virt_addr[1],
                         (unsigned long long)frame->buff.phys_addr[0],
                         (unsigned long long)frame->buff.phys_addr[1]);
        if (mapped_vu) {
            ot_omi_sys_munmap(mapped_vu, vu_map_size);
        }
        if (mapped_y) {
            ot_omi_sys_munmap(mapped_y, y_map_size);
        }
        return OT_FAILURE;
    }

    for (ot_u32 row = 0; row < height; row++) {
        memcpy(dst + row * width, src_y + row * frame->buff.stride[0], width);
    }

    for (ot_u32 row = 0; row < height / 2; row++) {
        memcpy(dst + y_size + row * width, src_vu + row * frame->buff.stride[1], width);
    }

    if (mapped_vu) {
        ot_omi_sys_munmap(mapped_vu, vu_map_size);
    }
    if (mapped_y) {
        ot_omi_sys_munmap(mapped_y, y_map_size);
    }

    return OT_SUCCESS;
}

static ot_s32 raw_send_frame(ot_s32 fd, const ot_eis_video_frame *frame, ot_u8 camera_id)
{
    ot_u32 width = frame->attr.width;
    ot_u32 height = frame->attr.height;
    ot_u32 image_size = width * height * 3 / 2;
    ot_u32 payload_size = STEREO_RAW_META_SIZE + image_size;
    ot_u8 *payload = OT_NULL;
    stereo_net_header_t net_header;
    ot_u64 timestamp_us = frame->pts;
    ot_u8 frame_type = (camera_id == 0) ? STEREO_FRAME_TYPE_RAW_LEFT : STEREO_FRAME_TYPE_RAW_RIGHT;

    if (raw_is_yuv420sp(frame->attr.pixel_fmt) != OT_TRUE) {
        stereo_log_write("[raw_capture] unsupported pixel_fmt=%d\n", frame->attr.pixel_fmt);
        return OT_FAILURE;
    }

    payload = (ot_u8 *)malloc(payload_size);
    if (!payload) {
        return OT_FAILURE;
    }
    memset(payload, 0, STEREO_RAW_META_SIZE);
    memcpy(payload, "RAW0", 4);
    raw_put_u16(payload + 4, (ot_u16)width);
    raw_put_u16(payload + 6, (ot_u16)height);
    raw_put_u16(payload + 8, (ot_u16)width);
    raw_put_u16(payload + 10, (ot_u16)width);
    payload[12] = (frame->attr.pixel_fmt == OT_EIS_IMAGE_FORMAT_YVU_420_SEMIPLANAR) ? 0 : 1;
    payload[13] = camera_id;
    raw_put_u16(payload + 14, 270);
    raw_put_u32(payload + 16, image_size);
    raw_put_u32(payload + 20, frame->index);

    if (raw_pack_yuv420sp(frame, payload + STEREO_RAW_META_SIZE, image_size) != OT_SUCCESS) {
        stereo_log_write("[raw_capture] pack frame failed cam=%u %ux%u fmt=%d virt=%p/%p phys=0x%llx/0x%llx\n",
                         camera_id, width, height, frame->attr.pixel_fmt,
                         frame->buff.virt_addr[0], frame->buff.virt_addr[1],
                         (unsigned long long)frame->buff.phys_addr[0],
                         (unsigned long long)frame->buff.phys_addr[1]);
        free(payload);
        return OT_FAILURE;
    }

    if (timestamp_us == 0) {
        struct timeval tv;
        gettimeofday(&tv, OT_NULL);
        timestamp_us = (ot_u64)tv.tv_sec * 1000000 + tv.tv_usec;
    }

    memset(&net_header, 0, sizeof(net_header));
    net_header.magic = htonl(STEREO_NET_MAGIC);
    net_header.frame_type = frame_type;
    net_header.frame_index = htonl(frame->index);
    net_header.timestamp_us = raw_htonll(timestamp_us);
    net_header.payload_size = htonl(payload_size);

    if (raw_send_all(fd, (const ot_u8 *)&net_header, STEREO_NET_HEADER_SIZE) != OT_SUCCESS ||
        raw_send_all(fd, payload, payload_size) != OT_SUCCESS) {
        stereo_log_write("[raw_capture] send frame failed cam=%u payload=%u\n", camera_id, payload_size);
        free(payload);
        return OT_FAILURE;
    }

    stereo_log_write("[raw_capture] sent cam=%u %ux%u bytes=%u fmt=%d\n",
                     camera_id, width, height, image_size, frame->attr.pixel_fmt);
    free(payload);
    return OT_SUCCESS;
}

static ot_s32 raw_capture_and_send_one(ot_s32 fd, ot_u8 camera_id)
{
    ot_s32 ret;
    ot_eis_video_frame frame;
    ot_vi_chn_handle chn_hdl = g_raw_chn[camera_id];

    memset(&frame, 0, sizeof(frame));

    stereo_log_write("[raw_capture] capture cam=%u begin chn=0x%llx\n",
                     camera_id, (unsigned long long)chn_hdl);

    ret = ot_eis_vi_chn_enable_dump(chn_hdl, OT_TRUE, STEREO_RAW_DUMP_DEPTH);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[raw_capture] enable dump cam=%u failed ret=0x%x\n", camera_id, ret);
        return ret;
    }

    stereo_log_write("[raw_capture] get frame cam=%u timeout=%dms\n", camera_id, STEREO_RAW_CAPTURE_TIMEOUT_MS);
    ret = ot_eis_vi_chn_get_frame(chn_hdl, &frame, STEREO_RAW_CAPTURE_TIMEOUT_MS);
    if (ret != OT_SUCCESS) {
        stereo_log_write("[raw_capture] get frame cam=%u failed ret=0x%x\n", camera_id, ret);
        ot_eis_vi_chn_enable_dump(chn_hdl, OT_FALSE, 0);
        return ret;
    }

    stereo_log_write("[raw_capture] got frame cam=%u %ux%u fmt=%d stride=%u/%u virt=%p/%p phys=0x%llx/0x%llx\n",
                     camera_id, frame.attr.width, frame.attr.height, frame.attr.pixel_fmt,
                     frame.buff.stride[0], frame.buff.stride[1],
                     frame.buff.virt_addr[0], frame.buff.virt_addr[1],
                     (unsigned long long)frame.buff.phys_addr[0],
                     (unsigned long long)frame.buff.phys_addr[1]);

    ret = raw_send_frame(fd, &frame, camera_id);
    stereo_log_write("[raw_capture] send cam=%u ret=0x%x\n", camera_id, ret);

    ot_eis_vi_chn_release_frame(chn_hdl, &frame);
    ot_eis_vi_chn_enable_dump(chn_hdl, OT_FALSE, 0);
    return ret;
}

static void raw_handle_client(ot_s32 fd)
{
    ot_u8 cmd[8];
    ot_u32 magic;
    ot_u8 command;
    ot_u8 mask;

    if (raw_recv_all(fd, cmd, sizeof(cmd)) != OT_SUCCESS) {
        return;
    }

    magic = ((ot_u32)cmd[0] << 24) | ((ot_u32)cmd[1] << 16) |
            ((ot_u32)cmd[2] << 8) | (ot_u32)cmd[3];
    command = cmd[4];
    mask = cmd[5];

    if (magic != STEREO_RAW_CAPTURE_CMD_MAGIC || command != STEREO_RAW_CAPTURE_CMD_GRAB) {
        stereo_log_write("[raw_capture] invalid command magic=0x%x cmd=%u\n", magic, command);
        return;
    }

    if (mask == 0) {
        mask = STEREO_RAW_CAPTURE_MASK_LEFT | STEREO_RAW_CAPTURE_MASK_RIGHT;
    }

    stereo_log_write("[raw_capture] trigger mask=0x%x\n", mask);

    if (mask & STEREO_RAW_CAPTURE_MASK_LEFT) {
        ot_s32 ret = raw_capture_and_send_one(fd, 0);
        stereo_log_write("[raw_capture] cam=0 done ret=0x%x\n", ret);
    }
    if (mask & STEREO_RAW_CAPTURE_MASK_RIGHT) {
        ot_s32 ret = raw_capture_and_send_one(fd, 1);
        stereo_log_write("[raw_capture] cam=1 done ret=0x%x\n", ret);
    }
}

static void *raw_capture_proc(void *p)
{
    (void)p;
    prctl(PR_SET_NAME, "raw_capture", 0, 0, 0);

    while (g_raw_run == OT_TRUE) {
        ot_s32 client_fd = accept(g_raw_listen_fd, OT_NULL, OT_NULL);
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
                usleep(100 * 1000);
                continue;
            }
            if (g_raw_run == OT_TRUE) {
                stereo_log_write("[raw_capture] accept failed: %s\n", strerror(errno));
                usleep(100 * 1000);
            }
            continue;
        }

        {
            ot_s32 one = 1;
            ot_s32 client_flags = fcntl(client_fd, F_GETFL, 0);
            if (client_flags >= 0) {
                fcntl(client_fd, F_SETFL, client_flags & (~O_NONBLOCK));
            }
            setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&one, sizeof(one));
        }

        raw_handle_client(client_fd);
        close(client_fd);
    }

    return OT_NULL;
}

ot_s32 stereo_raw_capture_start(ot_vi_chn_handle left_chn_hdl,
                                 ot_vi_chn_handle right_chn_hdl)
{
    ot_s32 flag = 1;
    ot_s32 flags;
    struct sockaddr_in addr;

    if (g_raw_started == OT_TRUE) {
        return OT_SUCCESS;
    }

    g_raw_chn[0] = left_chn_hdl;
    g_raw_chn[1] = right_chn_hdl;

    g_raw_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_raw_listen_fd < 0) {
        stereo_log_write("[raw_capture] socket failed: %s\n", strerror(errno));
        return OT_FAILURE;
    }

    setsockopt(g_raw_listen_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(STEREO_RAW_CAPTURE_PORT);

    if (bind(g_raw_listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        stereo_log_write("[raw_capture] bind port %d failed: %s\n",
                         STEREO_RAW_CAPTURE_PORT, strerror(errno));
        close(g_raw_listen_fd);
        g_raw_listen_fd = -1;
        return OT_FAILURE;
    }

    if (listen(g_raw_listen_fd, 1) < 0) {
        stereo_log_write("[raw_capture] listen failed: %s\n", strerror(errno));
        close(g_raw_listen_fd);
        g_raw_listen_fd = -1;
        return OT_FAILURE;
    }

    flags = fcntl(g_raw_listen_fd, F_GETFL, 0);
    fcntl(g_raw_listen_fd, F_SETFL, flags | O_NONBLOCK);

    g_raw_run = OT_TRUE;
    if (pthread_create(&g_raw_tid, OT_NULL, raw_capture_proc, OT_NULL) != 0) {
        g_raw_run = OT_FALSE;
        close(g_raw_listen_fd);
        g_raw_listen_fd = -1;
        return OT_FAILURE;
    }

    g_raw_started = OT_TRUE;
    stereo_log_write("[raw_capture] listening on 0.0.0.0:%d\n", STEREO_RAW_CAPTURE_PORT);
    return OT_SUCCESS;
}

void stereo_raw_capture_stop(void)
{
    if (g_raw_started != OT_TRUE) {
        return;
    }

    g_raw_run = OT_FALSE;
    if (g_raw_listen_fd >= 0) {
        close(g_raw_listen_fd);
        g_raw_listen_fd = -1;
    }
    pthread_join(g_raw_tid, OT_NULL);
    g_raw_started = OT_FALSE;
    stereo_log_write("[raw_capture] stopped\n");
}
