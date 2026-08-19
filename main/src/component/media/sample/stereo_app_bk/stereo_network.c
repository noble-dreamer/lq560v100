/*
  Copyright (c), 2001-2024, Oritek Tech. Co., Ltd.

  TCP server for stereo frame streaming.
  - Board listens on 0.0.0.0:9000.
  - PC connects as client → board starts sending.
  - Non-blocking accept. When no client, frames are silently dropped.
  - If client disconnects, board resumes listening.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include "stereo_network.h"
#include "stereo_types.h"
#include "stereo_media.h"
#include "stereo_yolo.h"

static ot_s32 g_listen_fd  = -1;
static ot_s32 g_client_fd  = -1;

/* -------------------------------------------------------------------------- */
/* Init / Deinit                                                               */
/* -------------------------------------------------------------------------- */

ot_s32 stereo_network_init(void)
{
    ot_s32 flag = 1;
    struct sockaddr_in addr;

    g_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_listen_fd < 0) {
        stereo_log_write("[stereo_net] socket failed: %s\n", strerror(errno));
        return OT_FAILURE;
    }

    setsockopt(g_listen_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(STEREO_NET_PORT);

    if (bind(g_listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        stereo_log_write("[stereo_net] bind failed: %s\n", strerror(errno));
        close(g_listen_fd);
        g_listen_fd = -1;
        return OT_FAILURE;
    }

    if (listen(g_listen_fd, 1) < 0) {
        stereo_log_write("[stereo_net] listen failed: %s\n", strerror(errno));
        close(g_listen_fd);
        g_listen_fd = -1;
        return OT_FAILURE;
    }

    stereo_log_write("[stereo_net] listening on 0.0.0.0:%d\n", STEREO_NET_PORT);
    return OT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* Non-blocking accept: returns OT_SUCCESS if a client just connected           */
/* -------------------------------------------------------------------------- */

ot_s32 stereo_network_accept(void)
{
    ot_s32 flags;

    if (g_client_fd >= 0) {
        return OT_SUCCESS; /* already have a client */
    }

    if (g_listen_fd < 0) {
        return OT_FAILURE;
    }

    /* Set listen fd non-blocking */
    flags = fcntl(g_listen_fd, F_GETFL, 0);
    fcntl(g_listen_fd, F_SETFL, flags | O_NONBLOCK);

    g_client_fd = accept(g_listen_fd, NULL, NULL);
    if (g_client_fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return OT_FAILURE; /* no pending connection — normal */
        }
        return OT_FAILURE;
    }

    /* Restore blocking for listen fd */
    fcntl(g_listen_fd, F_SETFL, flags);

    /* Client socket: TCP_NODELAY */
    {
        ot_s32 one = 1;
        setsockopt(g_client_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&one, sizeof(one));
    }

    stereo_log_write("[stereo_net] client connected\n");
    return OT_SUCCESS;
}

void stereo_network_disconnect(void)
{
    if (g_client_fd >= 0) {
        close(g_client_fd);
        g_client_fd = -1;
    }
    if (g_listen_fd >= 0) {
        close(g_listen_fd);
        g_listen_fd = -1;
    }
}

ot_bool stereo_network_is_connected(void)
{
    return (g_client_fd >= 0) ? OT_TRUE : OT_FALSE;
}

/* -------------------------------------------------------------------------- */
/* Send a single packet                                                         */
/* -------------------------------------------------------------------------- */

static ot_s32 stereo_network_send_packet(ot_s32 fd,
                                          const stereo_jpeg_packet_t *pkt)
{
    ot_s32 ret;
    stereo_net_header_t header;
    ot_u8 *full_pkt;
    ot_u32 total_size;

    memset(&header, 0, sizeof(header));
    header.magic        = htonl(STEREO_NET_MAGIC);
    header.frame_type   = pkt->type;
    header.frame_index  = htonl(pkt->frame_index);
    /* 64-bit big-endian: low32 in upper half, high32 in lower half,
       so that in a little-endian register the bytes appear in network order. */
    header.timestamp_us = (((ot_u64)htonl((ot_u32)(pkt->timestamp_us & 0xFFFFFFFF))) << 32)
                         | ((ot_u64)htonl((ot_u32)(pkt->timestamp_us >> 32)));
    header.payload_size = htonl(pkt->size);

    total_size = STEREO_NET_HEADER_SIZE + pkt->size;
    full_pkt = (ot_u8 *)malloc(total_size);
    if (!full_pkt) return OT_FAILURE;

    memcpy(full_pkt, &header, STEREO_NET_HEADER_SIZE);
    if (pkt->size > 0 && pkt->data) {
        memcpy(full_pkt + STEREO_NET_HEADER_SIZE, pkt->data, pkt->size);
    }

    ot_u32 sent = 0;
    while (sent < total_size) {
        ret = send(fd, full_pkt + sent, total_size - sent, MSG_NOSIGNAL);
        if (ret <= 0) {
            free(full_pkt);
            return OT_FAILURE;
        }
        sent += (ot_u32)ret;
    }

    free(full_pkt);
    return OT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* Send frame triplet (silent drop if no client)                               */
/* -------------------------------------------------------------------------- */

ot_s32 stereo_network_send(const stereo_venc_output_t *output)
{
    if (g_client_fd < 0) {
        return OT_SUCCESS; /* no client → silent drop */
    }

    /* Update metadata */
    for (ot_u32 i = 0; i < output->packet_count; i++) {
        stereo_jpeg_packet_t *pkt = (stereo_jpeg_packet_t *)&output->packets[i];
        pkt->frame_index  = output->frame_index;
        pkt->timestamp_us = output->timestamp_us;
    }

    /* Send all 3 packets */
    for (ot_u32 i = 0; i < output->packet_count; i++) {
        ot_s32 ret = stereo_network_send_packet(g_client_fd, &output->packets[i]);
        if (ret != OT_SUCCESS) {
            stereo_log_write("[stereo_net] send error, client gone\n");
            close(g_client_fd);
            g_client_fd = -1;
            return OT_FAILURE;
        }
    }

    /* Send perf timing after frame triplet */
    double npu_ms = 0, sub_ms = 0;
    stereo_media_get_perf(&npu_ms, &sub_ms);
    stereo_yolo_det_t dets[STEREO_YOLO_MAX_DETS];
    ot_u32 ndet = stereo_yolo_get_last_dets(dets, STEREO_YOLO_MAX_DETS);
    char det_buf[1024];
    int doff = 0;

    doff += snprintf(det_buf + doff, sizeof(det_buf) - doff, "[");
    for (ot_u32 i = 0; i < ndet && doff < (int)sizeof(det_buf) - 16; i++) {
        doff += snprintf(det_buf + doff, sizeof(det_buf) - doff,
                         "%s[%u,%d]", i ? "," : "",
                         dets[i].class_id, dets[i].distance_mm);
    }
    doff += snprintf(det_buf + doff, sizeof(det_buf) - doff, "]");

    char perf_json[1400];
    snprintf(perf_json, sizeof(perf_json),
             "{\"npu_ms\":%.1f,\"sub_ms\":%.1f,\"total_ms\":%.1f,\"dets\":%s}",
             npu_ms, sub_ms, npu_ms + sub_ms, det_buf);
    stereo_jpeg_packet_t perf_pkt;
    memset(&perf_pkt, 0, sizeof(perf_pkt));
    perf_pkt.data = (ot_u8 *)perf_json;
    perf_pkt.size = strlen(perf_json);
    perf_pkt.type = STEREO_FRAME_TYPE_PERF;
    perf_pkt.frame_index = output->frame_index;
    perf_pkt.timestamp_us = output->timestamp_us;
    stereo_network_send_packet(g_client_fd, &perf_pkt);

    return OT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/* Read calibration from /opt/stereo/stereo_calib.json and send to client       */
/* -------------------------------------------------------------------------- */

#define CALIB_FILE_PATH "/opt/stereo/stereo_calib.json"

/* Parse P1 matrix and baseline_mm from the calibration JSON file.
   P1 format in JSON:
     "P1": [[fx, 0, cx, 0], [0, fx, cy, 0], [0, 0, 1, 0]]
   We extract fx (row0 col0), cx (row0 col2), cy (row1 col2).
   baseline_mm is a simple key-value pair. */
static ot_s32 parse_calib_file(double *fx_orig, double *cx_orig,
                                double *cy_orig, double *baseline_mm)
{
    FILE *fp = fopen(CALIB_FILE_PATH, "r");
    if (!fp) {
        stereo_log_write("[stereo_net] calib file not found: %s\n", CALIB_FILE_PATH);
        return OT_FAILURE;
    }

    char buf[8192];
    size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
    buf[n] = '\0';
    fclose(fp);

    /* Find "P1" key */
    char *p1 = strstr(buf, "\"P1\"");
    if (!p1) {
        stereo_log_write("[stereo_net] P1 not found in calib JSON\n");
        return OT_FAILURE;
    }

    /* Find first '[' after P1 (outer array), then second '[' (first inner row).
       JSON may have newlines between brackets, so search individually. */
    char *outer = strchr(p1, '[');
    if (!outer) return OT_FAILURE;
    char *start = strchr(outer + 1, '[');
    if (!start) return OT_FAILURE;

    /* Parse first row: [fx, 0, cx, 0] (sscanf %lf skips whitespace) */
    double v1, v2, v3, v4;
    if (sscanf(start, "[%lf, %lf, %lf, %lf", &v1, &v2, &v3, &v4) < 4) {
        stereo_log_write("[stereo_net] P1 row0 parse failed\n");
        return OT_FAILURE;
    }
    *fx_orig = v1;
    *cx_orig = v3;

    /* Find second row: skip past first ']', find next '[' */
    char *row1_end = strchr(start + 1, ']');
    if (!row1_end) return OT_FAILURE;
    char *row2_start = strchr(row1_end, '[');
    if (!row2_start) return OT_FAILURE;
    if (sscanf(row2_start, "[%lf, %lf, %lf", &v1, &v2, &v3) < 3) {
        stereo_log_write("[stereo_net] P1 row1 parse failed\n");
        return OT_FAILURE;
    }
    *cy_orig = v3;

    /* Find baseline_mm (simple key-value) */
    char *bl = strstr(buf, "\"baseline_mm\"");
    if (!bl) {
        stereo_log_write("[stereo_net] baseline_mm not found in calib JSON\n");
        return OT_FAILURE;
    }
    bl = strchr(bl, ':');
    if (!bl) return OT_FAILURE;
    *baseline_mm = atof(bl + 1);

    stereo_log_write("[stereo_net] calib parsed: fx=%.4f cx=%.4f cy=%.4f baseline=%.3fmm\n",
                     *fx_orig, *cx_orig, *cy_orig, *baseline_mm);
    return OT_SUCCESS;
}

ot_s32 stereo_network_send_calib(void)
{
    if (g_client_fd < 0) {
        return OT_FAILURE;
    }

    /* Read original calibration parameters from file */
    double fx_orig, cx_orig, cy_orig, baseline_mm;
    if (parse_calib_file(&fx_orig, &cx_orig, &cy_orig, &baseline_mm) != OT_SUCCESS) {
        stereo_log_write("[stereo_net] WARNING: calib file parse failed, skipping calib send\n");
        return OT_FAILURE;
    }

    /* Scale from sensor resolution (1280x1080) to transmitted disparity resolution (640x448).
       Processing chain:
         1280x1080 -> CVE(640x540) -> crop(640x448, top=46) -> NPU input(640x448) -> NPU output(320x224)
         -> SubPixel(320x224) -> 2x nearest-neighbor upsample -> transmitted(640x448)
       scale_x = sensor_w / disp_output_w = 1280 / 640 = 2.0
       crop_y_offset at sensor level = CROP_TOP * (sensor_h / cve_h) = 46 * 2 = 92

       Disparity values are at NPU INPUT resolution (640x448), and the transmitted
       disparity is also at 640x448 after upsampling. disp_scale = input_w / disp_output_w = 1.0
       The receiver no longer needs to scale disparity for triangulation. */
    double scale_x = (double)STEREO_SENSOR_WIDTH / STEREO_DISP_OUTPUT_W;
    double crop_y_offset = (double)STEREO_CROP_TOP *
                           ((double)STEREO_SENSOR_HEIGHT / STEREO_CVE_DOWNSAMPLE_H);
    double disp_scale = (double)STEREO_NPU_INPUT_W / STEREO_DISP_OUTPUT_W;

    double fx_disp = fx_orig / scale_x;
    double cx_disp = cx_orig / scale_x;
    double cy_disp = (cy_orig - crop_y_offset) / scale_x;

    /* Build JSON dynamically */
    char json[512];
    snprintf(json, sizeof(json),
        "{"
        "\"fx\":%.4f,"
        "\"cx\":%.4f,"
        "\"cy\":%.4f,"
        "\"baseline_mm\":%.4f,"
        "\"disp_w\":%d,"
        "\"disp_h\":%d,"
        "\"q5_scale\":32,"
        "\"max_disp\":%d,"
        "\"image_w\":%d,"
        "\"image_h\":%d,"
        "\"scale_x\":%.1f,"
        "\"crop_y_offset\":%.0f,"
        "\"disp_scale\":%.1f"
        "}",
        fx_disp, cx_disp, cy_disp,
        baseline_mm,
        STEREO_DISP_OUTPUT_W, STEREO_DISP_OUTPUT_H,
        STEREO_NPU_COST_CHANNELS,
        STEREO_SENSOR_WIDTH, STEREO_SENSOR_HEIGHT,
        scale_x, crop_y_offset, disp_scale);

    stereo_jpeg_packet_t pkt;
    memset(&pkt, 0, sizeof(pkt));
    pkt.data = (ot_u8 *)json;
    pkt.size = strlen(json);
    pkt.type = STEREO_FRAME_TYPE_CALIB;
    pkt.frame_index = 0;
    pkt.timestamp_us = 0;

    ot_s32 ret = stereo_network_send_packet(g_client_fd, &pkt);
    if (ret == OT_SUCCESS) {
        stereo_log_write("[stereo_net] calibration sent (%d bytes): %s\n", pkt.size, json);
    }
    return ret;
}

ot_s32 stereo_network_get_calib(double *fx_orig, double *cx_orig,
                                double *cy_orig, double *baseline_mm)
{
    if (fx_orig == NULL || cx_orig == NULL || cy_orig == NULL ||
        baseline_mm == NULL) {
        return OT_FAILURE;
    }
    return parse_calib_file(fx_orig, cx_orig, cy_orig, baseline_mm);
}

/* -------------------------------------------------------------------------- */
/* Debug log to LOCAL tmpfs (NFS removed to avoid D-state hang)                */
/* -------------------------------------------------------------------------- */

static void stereo_network_write_log_local(const char *line)
{
    /* Write to local tmpfs only — NEVER to NFS from a pipeline thread */
    FILE *fp = fopen(STEREO_DEBUG_LOCAL_PATH "/stereo_app.log", "a");
    if (fp) { fprintf(fp, "%s\n", line); fclose(fp); }
}

void stereo_network_write_log(ot_u32 frame_index, ot_u64 timestamp_us,
                               ot_u32 left_size, ot_u32 right_size, ot_u32 disp_size)
{
    /* Runtime logging disabled */
    (void)frame_index; (void)timestamp_us;
    (void)left_size; (void)right_size; (void)disp_size;
}
