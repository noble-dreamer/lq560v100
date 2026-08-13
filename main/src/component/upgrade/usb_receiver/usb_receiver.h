/*
  Copyright (c), 2021-2023, Oritek Tech. Co., Ltd.
 */

/* usb_receiver: USB upgrade thin-agent (FunctionFS Vendor interface).
 *
 * Protocol definition: SD3589C USB upgrade architecture design doc (V1.3),
 * section 6.2.
 * All multi-byte protocol fields are little-endian. CRC32 is CRC-32/IEEE 802.3
 * (zlib crc32() compatible).
 *
 * Alignment notes with the PC side (must stay in sync):
 *  - usb_cmd_t.crc32 is an incremental CRC over two segments:
 *    [magic..payload_len] (16 bytes, offset 0) + [param..reserved] (44 bytes,
 *    offset 20), i.e. the whole 64-byte command except the crc32 field itself.
 *  - usb_cmd_t.seq is incremented by the PC on OUT commands only; the device
 *    never enforces seq continuity and only echoes seq in responses.
 *  - STATUS_RESP.info is a UTF-8 string with terminating '\0'.
 */

#ifndef USB_RECEIVER_H
#define USB_RECEIVER_H

#include "ot_eis_type.h"

/* ------------------------------------------------------------------ */
/* USB control transfer SETUP mapping (design 6.2.1)                   */
/* ------------------------------------------------------------------ */
#define USB_REQ_TYPE_OUT        0x40    /* Vendor, Device, Host->Device */
#define USB_REQ_TYPE_IN         0xC0    /* Vendor, Device, Device->Host */
#define USB_CTRL_DATA_LEN       64      /* wLength of every command/status */

#define USB_CMD_MAGIC           0x4F525555  /* "ORUU" */
#define USB_PROTO_VERSION       1

/* command codes (design 6.2.2) */
#define USB_CMD_GET_VERSION     0x01
#define USB_CMD_ENTER_UPGRADE   0x02
#define USB_CMD_SEND_PACKAGE    0x03
#define USB_CMD_SEND_DATA       0x04
#define USB_CMD_QUERY_STATUS    0x05
#define USB_CMD_EXIT_UPGRADE    0x06
#define USB_CMD_STATUS_RESP     0x10

/* CMD_SEND_PACKAGE params */
#define USB_PKG_PARAM_SIZE_IDX  0       /* param[0] = total_size */
#define USB_PKG_PARAM_CRC_IDX   1       /* param[1] = total_crc32 */

typedef struct {
    ot_u32 magic;          /* USB_CMD_MAGIC */
    ot_u16 version;        /* USB_PROTO_VERSION */
    ot_u16 cmd;            /* command code */
    ot_u32 seq;            /* sequence number, echoed back in responses */
    ot_u32 payload_len;    /* following bulk data length (CMD_SEND_DATA only) */
    ot_u32 crc32;          /* incremental CRC32 of [0:16) + [20:64) */
    ot_u32 param[8];       /* command parameters */
    ot_u32 reserved[3];    /* must be 0, ignored by receiver */
} usb_cmd_t;               /* total 64 bytes */

#define USB_CMD_CRC_SEG1_OFF    0       /* magic .. payload_len */
#define USB_CMD_CRC_SEG1_LEN    16
#define USB_CMD_CRC_SEG2_OFF    20      /* param .. reserved (after crc32 field) */
#define USB_CMD_CRC_SEG2_LEN    44

/* ------------------------------------------------------------------ */
/* Bulk data fragment format (design 6.2.3)                            */
/* ------------------------------------------------------------------ */
#define USB_FRAG_MAX_DATA       (64 * 1024) /* 64KB per fragment */

typedef struct {
    ot_u32 seq;            /* fragment index, from 0, independent of usb_cmd_t.seq */
    ot_u32 offset;         /* file offset */
    ot_u32 data_len;       /* payload length of this fragment (<= 64KB) */
    /* ot_u8 data[data_len]; */
    /* ot_u32 crc32;       CRC32 over seq + offset + data_len + data */
} usb_frag_hdr_t;          /* on-wire header: 12 bytes */

#define USB_FRAG_HDR_LEN        12
#define USB_FRAG_CRC_LEN        4
#define USB_FRAG_MAX_LEN        (USB_FRAG_HDR_LEN + USB_FRAG_MAX_DATA + USB_FRAG_CRC_LEN)

/* ------------------------------------------------------------------ */
/* STATUS_RESP / upgrade.status file (design 6.2.4, appendix G)        */
/* ------------------------------------------------------------------ */
#define USB_STAGE_IDLE          0
#define USB_STAGE_RECEIVING     1
#define USB_STAGE_WAITING       2
#define USB_STAGE_VERIFYING     3
#define USB_STAGE_FLASHING      4
#define USB_STAGE_SWITCHING     5
#define USB_STAGE_SUCCESS       7
#define USB_STAGE_FAILED        8

#define USB_SLOT_NONE           0xFF    /* single-partition mode: no slot */

typedef struct {
    ot_u32 stage;
    ot_u32 progress;       /* 0-100, weighted per design 6.2.6 */
    ot_u32 error_code;     /* 0 = success, see error codes below */
    ot_u32 current_slot;   /* fixed USB_SLOT_NONE */
    ot_u32 version;        /* current firmware version from /etc/fs-version */
    ot_s8  info[44];       /* UTF-8 string with terminating '\0' */
} upgrade_status_resp_t;   /* total 64 bytes */

#define USB_STATUS_FILE         "/data/upgrade/upgrade.status"
#define USB_STATUS_LOCK_FILE    "/data/upgrade/upgrade.status.lock"
#define USB_STATUS_TMP_FILE     "/data/upgrade/upgrade.status.tmp"

/* ------------------------------------------------------------------ */
/* error codes (aligned with PC side, doc/USB_Upgrade_UI_Interaction)  */
/* ------------------------------------------------------------------ */
#define USB_ERR_NONE                0
#define USB_ERR_DEVICE_NOT_FOUND    1
#define USB_ERR_PROTOCOL_MISMATCH   2
#define USB_ERR_PACKAGE_INVALID     3
#define USB_ERR_SIGNATURE_FAIL      4
#define USB_ERR_HASH_FAIL           5
#define USB_ERR_VERSION_ROLLBACK    6
#define USB_ERR_FLASH_WRITE_FAIL    7
#define USB_ERR_TIMEOUT             8
#define USB_ERR_USER_CANCEL         9
#define USB_ERR_CRC_FAIL            10
#define USB_ERR_NO_MEMORY           11

/* ------------------------------------------------------------------ */
/* receive limits / paths (design 4.2, 6.3.2)                          */
/* ------------------------------------------------------------------ */
#define USB_PKG_MAX_SIZE        (28UL * 1024 * 1024)  /* max OTAU package */
#define USB_TMPFS_MIN_FREE_PCT  10      /* require total_size + 10% free */
#define USB_UPGRADE_DIR         "/tmp/upgrade"
#define USB_UPGRADE_PKG_FILE    "/tmp/upgrade/upgrade.img"
#define USB_UPGRADE_DATA_DIR    "/data/upgrade"

#define USB_TOTAL_TIMEOUT_SEC   600     /* ENTER_UPGRADE -> reboot() */

/* progress weights (design 6.2.6) */
#define USB_PROGRESS_RECV_MAX       30  /* stage 1: 0-30 */
#define USB_PROGRESS_WAITING        30  /* stage 2: fixed 30 */
#define USB_PROGRESS_VERIFY_MIN     30  /* stage 3: 30-50 */
#define USB_PROGRESS_VERIFY_MAX     50
#define USB_PROGRESS_FLASH_MIN      50  /* stage 4: 50-90 */
#define USB_PROGRESS_FLASH_MAX      90
#define USB_PROGRESS_REBOOT         95  /* stage 5: <100 before reboot() */
#define USB_PROGRESS_DONE           100 /* stage 7 */

/* FunctionFS mount point and endpoint files (created by gadget script) */
#define USB_FFS_DIR             "/dev/usb-ffs/upgrade"
#define USB_FFS_EP0             USB_FFS_DIR "/ep0"
#define USB_FFS_EP_OUT          USB_FFS_DIR "/ep1"  /* bulk OUT (host->device) */
#define USB_FFS_EP_IN           USB_FFS_DIR "/ep2"  /* bulk IN  (device->host) */

/* ------------------------------------------------------------------ */
/* upgrade_flag in the uflag partition (design appendix A): only what   */
/* get_completed_upgrade_version() needs. Keep in sync with              */
/* component/upgrade/ota_prepare/ota_prepare.c and upgrade_flag_tool.py. */
/* ------------------------------------------------------------------ */
#define UPGRADE_FLAG_MAGIC      0x55504752  /* "UPGR" */
#define UPGRADE_FLAG_VERSION    1
#define UPGRADE_STATE_IDLE      0

typedef struct {
    ot_u32 magic;
    ot_u32 version;
    ot_u32 state;
    ot_u32 upgrade_source;   /* 0 = UBI upgrade volume */
    ot_u32 upgrade_size;
    ot_u32 upgrade_crc;
    ot_u32 component_mask;
    ot_u32 retry_count;
    ot_u32 version_new;
    ot_u32 version_old;
    ot_u32 completed_mask;
    ot_u8  reserved[16];
    ot_u32 crc32;            /* CRC32 of all fields above */
} upgrade_flag_t;            /* total 64 bytes */

#define UFLAG_COPY_NUM          4
#define UFLAG_COPY_SPACING      0x20000     /* 128KB */
#define OTA_UFLAG_BYNAME        "/dev/by-name/uflag"

/* Microsoft WCID: vendor code shared with the gadget configfs script
 * (S89usbffs os_desc b_vendor_code). The extended compat ID descriptor
 * itself is embedded in the FunctionFS descriptor blob (see g_ffs_descs in
 * usb_receiver.c) and answered by the kernel composite layer, so that
 * Windows loads WinUSB for the Vendor interface automatically. */
#define USB_WCID_VENDOR_CODE    0xC1

/* media business integration hook (see media_pause() in usb_receiver.c) */
#define USB_MEDIA_CTL_SCRIPT    "/etc/upgrade_media_ctl.sh"

#endif /* USB_RECEIVER_H */
