/*
  Copyright (c), 2001-2022, Oritek Tech. Co., Ltd.
 */

#ifndef __STEREO_TYPES_H__
#define __STEREO_TYPES_H__

#include "ot_common.h"
#include "ot_eis_common.h"
#include "ot_eis_vproc.h"
#include "ot_eis_venc.h"
#include "ot_avp_cve_comm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* Image dimensions                                                           */
/* -------------------------------------------------------------------------- */
#define STEREO_SENSOR_WIDTH         1280
#define STEREO_SENSOR_HEIGHT        1080

#define STEREO_CVE_DOWNSAMPLE_W     640
#define STEREO_CVE_DOWNSAMPLE_H     540

#define STEREO_CVE_CROP_W           640
#define STEREO_CVE_CROP_H           448

#define STEREO_NPU_INPUT_W          640
#define STEREO_NPU_INPUT_H          448

#define STEREO_NPU_OUTPUT_W         320
#define STEREO_NPU_OUTPUT_H         224

/* Disparity output dimensions after board-side 2x nearest-neighbor upsampling.
   NPU natively outputs 320x224 (float32 disparity); model includes subpixel.
   The result is then upsampled to 640x448 for network transmission and display. */
#define STEREO_DISP_OUTPUT_W        640
#define STEREO_DISP_OUTPUT_H        448

/* Disparity candidates / max disparity range.
   For dual-output models: cost volume channel count.
   For single-output models: max disparity value for display normalization.
   Model stereo_s_ori_h448_w640_128_sub has 128 disparity candidates. */
#define STEREO_NPU_COST_CHANNELS    128

/* Center crop: 640x540 -> 640x448, 46px from top and bottom */
#define STEREO_CROP_LEFT            0
#define STEREO_CROP_TOP             46

/* -------------------------------------------------------------------------- */
/* Pipeline queue depth                                                       */
/* -------------------------------------------------------------------------- */
#define STEREO_QUEUE_CAPACITY       2   /* MUST be power of 2 for lock-free SPSC ring */
#define STEREO_VPROC_QUEUE_DEPTH    2

/* -------------------------------------------------------------------------- */
/* Network constants                                                          */
/* -------------------------------------------------------------------------- */
#define STEREO_NET_MAGIC            0x53544D50  /* "STMP" */
#define STEREO_NET_PORT             9000
#define STEREO_RAW_CAPTURE_PORT     9001
#define STEREO_NET_IP               "192.168.1.101"
#define STEREO_NET_HEADER_SIZE      24

/* -------------------------------------------------------------------------- */
/* Frame types for network protocol                                           */
/* -------------------------------------------------------------------------- */
typedef enum {
    STEREO_FRAME_TYPE_LEFT       = 0x00,
    STEREO_FRAME_TYPE_RIGHT      = 0x01,
    STEREO_FRAME_TYPE_DISPARITY  = 0x02,  /* raw uint8 disparity (288x192)   */
    STEREO_FRAME_TYPE_DISPARITY_LZ4 = 0x03, /* LZ4-compressed disparity block */
    STEREO_FRAME_TYPE_RAW_LEFT   = 0x04,  /* triggered raw full-res left NV21 */
    STEREO_FRAME_TYPE_RAW_RIGHT  = 0x05,  /* triggered raw full-res right NV21 */
    STEREO_FRAME_TYPE_CALIB      = 0x06,  /* calibration JSON (sent once on connect) */
    STEREO_FRAME_TYPE_PERF       = 0x07,  /* perf timing JSON (NPU/SubPixel ms) */
} stereo_frame_type_e;

/* -------------------------------------------------------------------------- */
/* Network packet header (big-endian, packed)                                 */
/* -------------------------------------------------------------------------- */
typedef struct __attribute__((packed)) {
    ot_u32  magic;           /* 0x53544D50 */
    ot_u8   frame_type;      /* stereo_frame_type_e */
    ot_u32  frame_index;
    ot_u64  timestamp_us;
    ot_u32  payload_size;
    ot_u8   reserved[3];
} stereo_net_header_t;

/* -------------------------------------------------------------------------- */
/* JPEG encoded frame payload                                                 */
/* -------------------------------------------------------------------------- */
typedef struct {
    ot_u8           *data;
    ot_u32           size;
    ot_u64           timestamp_us;
    ot_u32           frame_index;
    stereo_frame_type_e type;
} stereo_jpeg_packet_t;

/* -------------------------------------------------------------------------- */
/* Frame pair from VPROC acquisition (stage 1 → 2)                            */
/* -------------------------------------------------------------------------- */
typedef struct {
    /* VPROC full-resolution frames (1280x1080 YVU420SP) */
    ot_eis_img_frame    left_full;
    ot_eis_img_frame    right_full;
    ot_u32              frame_index;
    ot_u64              timestamp_us;
    /* VPROC channel handles for release */
    ot_eis_handle       left_chn_hdl;
    ot_eis_handle       right_chn_hdl;
} stereo_frame_pair_t;

/* -------------------------------------------------------------------------- */
/* Preprocessed frames for NPU (stage 2 → 3)                                  */
/* -------------------------------------------------------------------------- */
typedef struct {
    /* Full resolution frames for JPEG encoding */
    ot_eis_img_frame    left_full;
    ot_eis_img_frame    right_full;
    /* CVE preprocessed images (640x448 RGB888 planar, SMR-backed) */
    ot_avp_cve_img      left_crop;
    ot_avp_cve_img      right_crop;
    ot_u32              frame_index;
    ot_u64              timestamp_us;
    /* Channel handles for release of full frames */
    ot_eis_handle       left_chn_hdl;
    ot_eis_handle       right_chn_hdl;
} stereo_npu_input_t;

/* -------------------------------------------------------------------------- */
/* NPU output with full frames (stage 3 → 4)                                  */
/* -------------------------------------------------------------------------- */
typedef struct {
    /* Full resolution frames for JPEG encoding */
    ot_eis_img_frame    left_full;
    ot_eis_img_frame    right_full;
    /* Sub-pixel disparity output (320x208 uint16 Q5, dynamically allocated) */
    ot_u16             *disparity;
    ot_u32              disp_bytes;      /* total bytes = W*H*sizeof(uint16) */
    ot_u32              frame_index;
    ot_u64              timestamp_us;
    /* Channel handles for release */
    ot_eis_handle       left_chn_hdl;
    ot_eis_handle       right_chn_hdl;
} stereo_infer_output_t;

/* -------------------------------------------------------------------------- */
/* VENC encode output (stage 4 → 5)                                           */
/* -------------------------------------------------------------------------- */
#define STEREO_VENC_CH_LEFT      0
#define STEREO_VENC_CH_RIGHT     1
#define STEREO_VENC_CH_DISPARITY 2
#define STEREO_VENC_CH_NUM       3

typedef struct {
    stereo_jpeg_packet_t    packets[STEREO_VENC_CH_NUM];
    ot_u32                  packet_count;
    ot_u32                  frame_index;
    ot_u64                  timestamp_us;
} stereo_venc_output_t;

/* -------------------------------------------------------------------------- */
/* Debug log paths                                                             */
/* -------------------------------------------------------------------------- */
#define STEREO_DEBUG_LOCAL_PATH  "/data/stereo_debug"
#define STEREO_NFS_LOG_PATH      "/tmp/nfs"

/* Unified run log file under /data (real-time writes, never NFS) */
#define STEREO_RUN_LOG_FILE      STEREO_DEBUG_LOCAL_PATH "/stereo_run.log"

/* -------------------------------------------------------------------------- */
/* Unified thread-safe debug logger.                                           */
/*   stereo_log_write() prints to stdout AND appends a timestamped line to      */
/*   STEREO_RUN_LOG_FILE on local eMMC. Safe to call from any thread.           */
/* -------------------------------------------------------------------------- */
void stereo_log_init(void);
void stereo_log_deinit(void);
void stereo_log_write(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#ifdef __cplusplus
}
#endif

#endif /* __STEREO_TYPES_H__ */
