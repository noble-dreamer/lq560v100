/*
  Copyright (c), 2021-2023, Oritek Tech. Co., Ltd.
 */

/* usb_receiver: USB upgrade thin-agent.
 *
 * Runs as a user-space daemon on the device. It talks to the PC upgrade
 * tool through the Vendor Bulk interface of the UVC + Vendor composite
 * Gadget, implemented with FunctionFS (see /etc/init.d/S89usbffs).
 *
 * Duties (design 6.3.2, 12.2):
 *  - handle CMD_GET_VERSION / ENTER_UPGRADE / SEND_PACKAGE / SEND_DATA /
 *    QUERY_STATUS / EXIT_UPGRADE on the control endpoint;
 *  - receive 64KB fragments on the Bulk OUT endpoint and store them to
 *    tmpfs /tmp/upgrade/upgrade.img (seq/offset/CRC32 checked, out-of-order
 *    and retransmitted fragments handled);
 *  - maintain /data/upgrade/upgrade.status (64B, appendix G) with the
 *    progress weights of design 6.2.6;
 *  - once the package is complete and its total CRC32 matches, mark
 *    stage=WAITING and launch ota_prepare, then keep answering
 *    CMD_QUERY_STATUS while ota_prepare runs.
 *
 * The device-side upgrade state machine (design 7.3.2):
 *   IDLE -> RECEIVING -> WAITING -> VERIFYING -> FLASHING -> SWITCHING
 *   (VERIFYING/FLASHING/SWITCHING are driven by ota_prepare via the
 *   shared status file; FAILED can be reached from any stage).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <poll.h>
#include <signal.h>
#include <endian.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <linux/usb/functionfs.h>
#include <linux/usb/ch9.h>

#include "ot_eis_type.h"
#include "usb_receiver.h"

/* glibc htoleXX are functions and cannot be used in static initializers;
 * the target CPU is little-endian, so on-wire descriptor fields are
 * stored as-is */
#define FFS_LE32(x) (x)
#define FFS_LE16(x) (x)

#define recv_log(format, arg...)  printf(format, ##arg)
#define recv_err(format, arg...)  printf("Error: [%s-%d]" format, __FUNCTION__, __LINE__, ##arg)

#ifndef OT_SUCCESS
#define OT_SUCCESS  0
#define OT_FAILURE  (-1)
#endif

#define EP0_READ_BUF        256
#define FRAG_IO_CHUNK       (16 * 1024)
#define BULK_TIMEOUT_MS     5000    /* design 6.2.5: bulk transfer timeout 5s */
#define POLL_IDLE_MS        1000
#define OTA_PREPARE_PATH    "/usr/bin/ota_prepare"
#define USB_RECEIVER_READY_FILE   "/tmp/usb_receiver.ready"
#define USB_UPGRADE_ENTERED_FILE  "/tmp/usb_upgrade_entered"

/* ------------------------------------------------------------------ */
/* FunctionFS descriptors: 1 vendor interface, bulk OUT + bulk IN      */
/* ------------------------------------------------------------------ */

#define FFS_INTF_CLASS      0xFF    /* vendor specific */
#define FFS_INTF_SUBCLASS   0x00
#define FFS_INTF_PROTOCOL   0x00
#define EP_OUT_ADDR         0x01
#define EP_IN_ADDR          0x82
#define EP_FS_PKT_SIZE      64      /* full-speed bulk max packet */
#define EP_HS_PKT_SIZE      512
#define EP_SS_PKT_SIZE      1024
#define EP_SS_MAX_BURST     5

static const struct {
    struct usb_functionfs_descs_head_v2 header;
    __le32 fs_count;
    __le32 hs_count;
    __le32 ss_count;
    __le32 os_count;
    struct {
        struct usb_interface_descriptor intf;
        struct usb_endpoint_descriptor_no_audio ep_out;
        struct usb_endpoint_descriptor_no_audio ep_in;
    } __attribute__((packed)) fs_descs, hs_descs;
    struct {
        struct usb_interface_descriptor intf;
        struct usb_endpoint_descriptor_no_audio ep_out;
        struct usb_ss_ep_comp_descriptor ep_out_comp;
        struct usb_endpoint_descriptor_no_audio ep_in;
        struct usb_ss_ep_comp_descriptor ep_in_comp;
    } __attribute__((packed)) ss_descs;
    struct usb_os_desc_header os_desc_header;
    struct usb_ext_compat_desc os_desc_compat;
} __attribute__((packed)) g_ffs_descs = {
    .header = {
        .magic = FFS_LE32(FUNCTIONFS_DESCRIPTORS_MAGIC_V2),
        .length = FFS_LE32(sizeof(g_ffs_descs)),
        /* FUNCTIONFS_ALL_CTRL_RECIP: the protocol uses vendor commands
         * addressed to the device recipient (0x40/0xC0); without this
         * flag the kernel does not pass such requests to user space
         * (f_fs.c ffs_func_req_match requires it for recipients other
         * than interface/endpoint). */
        .flags = FFS_LE32(FUNCTIONFS_HAS_FS_DESC | FUNCTIONFS_HAS_HS_DESC | FUNCTIONFS_HAS_SS_DESC |
                          FUNCTIONFS_HAS_MS_OS_DESC | FUNCTIONFS_ALL_CTRL_RECIP),
    },
    .fs_count = FFS_LE32(3),
    .hs_count = FFS_LE32(3),
    .ss_count = FFS_LE32(5),
    .os_count = FFS_LE32(1),
    .fs_descs = {
        .intf = {
            .bLength = sizeof(struct usb_interface_descriptor),
            .bDescriptorType = USB_DT_INTERFACE,
            .bInterfaceNumber = 0,
            .bNumEndpoints = 2,
            .bInterfaceClass = FFS_INTF_CLASS,
            .bInterfaceSubClass = FFS_INTF_SUBCLASS,
            .bInterfaceProtocol = FFS_INTF_PROTOCOL,
            .iInterface = 1,
        },
        .ep_out = {
            .bLength = sizeof(struct usb_endpoint_descriptor_no_audio),
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = EP_OUT_ADDR,
            .bmAttributes = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize = FFS_LE16(EP_FS_PKT_SIZE),
        },
        .ep_in = {
            .bLength = sizeof(struct usb_endpoint_descriptor_no_audio),
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = EP_IN_ADDR,
            .bmAttributes = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize = FFS_LE16(EP_FS_PKT_SIZE),
        },
    },
    .hs_descs = {
        .intf = {
            .bLength = sizeof(struct usb_interface_descriptor),
            .bDescriptorType = USB_DT_INTERFACE,
            .bInterfaceNumber = 0,
            .bNumEndpoints = 2,
            .bInterfaceClass = FFS_INTF_CLASS,
            .bInterfaceSubClass = FFS_INTF_SUBCLASS,
            .bInterfaceProtocol = FFS_INTF_PROTOCOL,
            .iInterface = 1,
        },
        .ep_out = {
            .bLength = sizeof(struct usb_endpoint_descriptor_no_audio),
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = EP_OUT_ADDR,
            .bmAttributes = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize = FFS_LE16(EP_HS_PKT_SIZE),
        },
        .ep_in = {
            .bLength = sizeof(struct usb_endpoint_descriptor_no_audio),
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = EP_IN_ADDR,
            .bmAttributes = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize = FFS_LE16(EP_HS_PKT_SIZE),
        },
    },
    .ss_descs = {
        .intf = {
            .bLength = sizeof(struct usb_interface_descriptor),
            .bDescriptorType = USB_DT_INTERFACE,
            .bInterfaceNumber = 0,
            .bNumEndpoints = 2,
            .bInterfaceClass = FFS_INTF_CLASS,
            .bInterfaceSubClass = FFS_INTF_SUBCLASS,
            .bInterfaceProtocol = FFS_INTF_PROTOCOL,
            .iInterface = 1,
        },
        .ep_out = {
            .bLength = sizeof(struct usb_endpoint_descriptor_no_audio),
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = EP_OUT_ADDR,
            .bmAttributes = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize = FFS_LE16(EP_SS_PKT_SIZE),
        },
        .ep_out_comp = {
            .bLength = sizeof(struct usb_ss_ep_comp_descriptor),
            .bDescriptorType = USB_DT_SS_ENDPOINT_COMP,
            .bMaxBurst = EP_SS_MAX_BURST,
        },
        .ep_in = {
            .bLength = sizeof(struct usb_endpoint_descriptor_no_audio),
            .bDescriptorType = USB_DT_ENDPOINT,
            .bEndpointAddress = EP_IN_ADDR,
            .bmAttributes = USB_ENDPOINT_XFER_BULK,
            .wMaxPacketSize = FFS_LE16(EP_SS_PKT_SIZE),
        },
        .ep_in_comp = {
            .bLength = sizeof(struct usb_ss_ep_comp_descriptor),
            .bDescriptorType = USB_DT_SS_ENDPOINT_COMP,
            .bMaxBurst = EP_SS_MAX_BURST,
        },
    },
    /* Microsoft WCID extended compat ID (design 6.3.1: the PC uses WinUSB,
     * so the Vendor interface must expose a WCID descriptor). The data is
     * handed to the kernel FunctionFS driver; the composite layer answers
     * the 0xEE OS string descriptor (configfs os_desc qw_sign) and the
     * extended compat ID request (vendor code 0xC1, wIndex 0x0004) itself.
     * interface/bFirstInterfaceNumber are the interface index local to this
     * FunctionFS function (single interface -> 0); the kernel maps it to
     * the configuration interface number at bind time. */
    .os_desc_header = {
        .interface = 0,
        .dwLength = FFS_LE32(sizeof(g_ffs_descs.os_desc_header) +
                             sizeof(g_ffs_descs.os_desc_compat)),
        .bcdVersion = FFS_LE16(1),
        .wIndex = FFS_LE16(4),
        .bCount = 1,
        .Reserved = 0,
    },
    .os_desc_compat = {
        .bFirstInterfaceNumber = 0,
        .Reserved1 = 1,
        .CompatibleID = { 'W', 'I', 'N', 'U', 'S', 'B', 0, 0 },
    },
};

static const struct {
    struct usb_functionfs_strings_head header;
    struct {
        __le16 code;
        const char str[8];
    } __attribute__((packed)) lang0;
} __attribute__((packed)) g_ffs_strings = {
    .header = {
        .magic = FFS_LE32(FUNCTIONFS_STRINGS_MAGIC),
        .length = FFS_LE32(sizeof(g_ffs_strings)),
        .str_count = FFS_LE32(1),
        .lang_count = FFS_LE32(1),
    },
    .lang0 = {
        .code = FFS_LE16(0x0409),
        .str = "upgrade",
    },
};

/* ------------------------------------------------------------------ */
/* receive context                                                     */
/* ------------------------------------------------------------------ */

typedef struct {
    ot_u32 total_size;      /* from CMD_SEND_PACKAGE param[0] */
    ot_u32 total_crc32;     /* from CMD_SEND_PACKAGE param[1] */
    ot_u32 received;        /* bytes accepted so far */
    ot_u32 expect_seq;      /* next expected fragment seq */
    ot_u32 crc_acc;         /* incremental CRC32 of the whole file */
    ot_s32 pkg_fd;          /* /tmp/upgrade/upgrade.img */
    ot_bool pkg_started;    /* CMD_SEND_PACKAGE accepted */
    time_t start_time;      /* CMD_ENTER_UPGRADE time */
} pkg_rx_t;

static pkg_rx_t g_rx = { .pkg_fd = -1 };
static ot_s32 g_ep0 = -1;
static ot_s32 g_ep_out = -1;
static ot_s32 g_ep_in = -1;
static ot_bool g_configured = OT_FALSE;
static volatile ot_bool g_running = OT_TRUE;
static ot_u8 g_frag_buf[USB_FRAG_MAX_LEN];

/* CRC-32/IEEE 802.3, bit-identical to zlib crc32(): pass 0 and OT_NULL to
 * start, then chain the previous return value for incremental data. Kept
 * local so the receiver does not depend on the opensource zlib build. */
static ot_u32 crc32_ieee(ot_u32 crc, const ot_u8 *buf, ot_u32 len)
{
    static ot_u32 table[256];
    static ot_bool table_ready = OT_FALSE;
    ot_u32 i, j, c;

    if (table_ready == OT_FALSE) {
        for (i = 0; i < 256; i++) {
            c = i;
            for (j = 0; j < 8; j++) {
                c = (c & 1) ? (0xEDB88320U ^ (c >> 1)) : (c >> 1);
            }
            table[i] = c;
        }
        table_ready = OT_TRUE;
    }

    if (buf == OT_NULL) {
        return 0;
    }

    crc = crc ^ 0xFFFFFFFFU;
    for (i = 0; i < len; i++) {
        crc = table[(crc ^ buf[i]) & 0xFFU] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFFU;
}
static time_t g_waiting_since;      /* when ota_prepare was launched (stage WAITING) */
static ot_bool g_media_paused;      /* camera apps killed on ENTER_UPGRADE, not yet resumed */
static ot_u32 g_last_progress = 0xFFFFFFFFU;  /* status-write throttle, reset by rx_reset() */

/* ota_prepare should move the stage past WAITING well within this budget;
 * otherwise it failed to start (e.g. exec error) and we report FAILED */
#define OTA_PREPARE_START_TIMEOUT_SEC   120

static ot_u32 get_completed_upgrade_version(ot_void)
{
    ot_s32 fd = open(OTA_UFLAG_BYNAME, O_RDONLY);
    if (fd < 0) {
        return 0;
    }

    upgrade_flag_t flag;
    ot_u32 image_ver = 0;
    for (ot_u32 i = 0; i < UFLAG_COPY_NUM; i++) {
        off_t off = (off_t)i * UFLAG_COPY_SPACING;
        if (pread(fd, &flag, sizeof(flag), off) != (ssize_t)sizeof(flag)) {
            continue;
        }
        ot_u32 crc = crc32_ieee(0, (const ot_u8 *)&flag, offsetof(upgrade_flag_t, crc32));
        if (flag.magic == UPGRADE_FLAG_MAGIC &&
            flag.version == UPGRADE_FLAG_VERSION &&
            flag.state == UPGRADE_STATE_IDLE &&
            flag.version_new != 0 && crc == flag.crc32) {
            image_ver = flag.version_new;
            break;
        }
    }
    close(fd);
    return image_ver;
}

/* ------------------------------------------------------------------ */
/* status file (design appendix G): tmp file + fsync + rename + flock  */
/* ------------------------------------------------------------------ */

static ot_u32 get_image_version(ot_void)
{
    /* Version source: /etc/fs-version first field (hex) -- the same
     * reference ota_prepare uses for anti-rollback and mkupgrade uses
     * for --fw-version. (/proc/cmdline image_ver is always 0 with the
     * current boot image configuration.) After a successful upgrade the
     * new rootfs carries the new fs-version, so the PC's post-reboot
     * version comparison sees the new version. */
    ot_s8 buf[64];
    FILE *file = fopen("/etc/fs-version", "r");

    if (file != OT_NULL) {
        if (fgets(buf, sizeof(buf), file) != OT_NULL) {
            ot_s8 *endptr = OT_NULL;
            ot_u32 ver = (ot_u32)strtoul(buf, (ot_s8 **)&endptr, 16);

            if (endptr != buf) {
                fclose(file);
                return ver;
            }
        }
        fclose(file);
    }

    /* fallback: version recorded in uflag by a just-finished upgrade */
    return get_completed_upgrade_version();
}

static void status_default(upgrade_status_resp_t *st)
{
    memset(st, 0, sizeof(*st));
    st->stage = USB_STAGE_IDLE;
    st->current_slot = USB_SLOT_NONE;
    st->version = get_image_version();
}

static ot_s32 status_read(upgrade_status_resp_t *st)
{
    status_default(st);

    ot_s32 fd = open(USB_STATUS_FILE, O_RDONLY);
    if (fd < 0) {
        return OT_FAILURE;  /* no file -> IDLE */
    }
    ssize_t len = read(fd, st, sizeof(*st));
    close(fd);
    if (len != (ssize_t)sizeof(*st)) {
        status_default(st);
        return OT_FAILURE;
    }
    st->version = get_image_version();
    return OT_SUCCESS;
}

static ot_s32 status_write(const upgrade_status_resp_t *st)
{
    ot_s32 lfd = open(USB_STATUS_LOCK_FILE, O_RDWR | O_CREAT, 0644);
    if (lfd >= 0) {
        if (flock(lfd, LOCK_EX) != 0) {
            recv_err("flock %s failed: %s\n", USB_STATUS_LOCK_FILE, strerror(errno));
        }
    }

    ot_s32 ret = OT_FAILURE;
    ot_s32 fd = open(USB_STATUS_TMP_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        if (write(fd, st, sizeof(*st)) == (ssize_t)sizeof(*st) && fsync(fd) == 0) {
            ret = OT_SUCCESS;
        }
        close(fd);
        if (ret == OT_SUCCESS) {
            if (rename(USB_STATUS_TMP_FILE, USB_STATUS_FILE) != 0) {
                recv_err("rename status file failed: %s\n", strerror(errno));
                ret = OT_FAILURE;
            }
        }
    } else {
        recv_err("open %s failed: %s\n", USB_STATUS_TMP_FILE, strerror(errno));
    }

    if (lfd >= 0) {
        flock(lfd, LOCK_UN);
        close(lfd);
    }
    if (ret != OT_SUCCESS) {
        recv_err("status write failed\n");
    }
    return ret;
}

static void status_update(ot_u32 stage, ot_u32 progress, ot_u32 error_code, const ot_s8 *info)
{
    upgrade_status_resp_t st;

    status_read(&st);
    st.stage = stage;
    st.progress = progress;
    st.error_code = error_code;
    st.current_slot = USB_SLOT_NONE;
    if (info != OT_NULL) {
        strncpy(st.info, info, sizeof(st.info) - 1);
        st.info[sizeof(st.info) - 1] = '\0';
    } else {
        st.info[0] = '\0';
    }
    status_write(&st);
}

/* ------------------------------------------------------------------ */
/* media business pause/resume (design 6.3.1: stop UVC streaming)      */
/* ------------------------------------------------------------------ */

/* The media/UVA business is outside this component. Pausing is done via
 * an optional board hook script so that the integration point is explicit;
 * if the script is absent the pause is a no-op (TODO: wire to the media
 * SDK once the video service API is frozen).
 *
 * system() needs normal SIGCHLD semantics: this process keeps SIGCHLD at
 * SIG_IGN (to auto-reap the ota_prepare child), which makes glibc system()
 * always fail with ECHILD, so restore SIG_DFL for the duration of the call. */
static void media_hook(const ot_s8 *action)
{
    if (access(USB_MEDIA_CTL_SCRIPT, X_OK) != 0) {
        recv_log("usb_receiver: media hook %s absent, skip '%s'\n",
                 USB_MEDIA_CTL_SCRIPT, action);
        return;
    }

    ot_s8 cmd[128];
    snprintf(cmd, sizeof(cmd), "%s %s", USB_MEDIA_CTL_SCRIPT, action);

    void (*old)(int) = signal(SIGCHLD, SIG_DFL);
    ot_s32 ret = system(cmd);
    signal(SIGCHLD, old);
    if (ret != 0) {
        recv_err("media hook '%s' failed (ret=%d)\n", action, ret);
    }
}

static void media_pause(ot_void)
{
    g_media_paused = OT_TRUE;
    media_hook("stop");
}

static void media_resume(ot_void)
{
    g_media_paused = OT_FALSE;
    media_hook("start");
}

/* ------------------------------------------------------------------ */
/* command helpers                                                     */
/* ------------------------------------------------------------------ */

static ot_u32 cmd_crc32(const usb_cmd_t *cmd)
{
    /* PC side convention: incremental CRC over [magic..payload_len] (16B)
     * then [param..reserved] (44B), i.e. everything except crc32 itself. */
    const ot_u8 *raw = (const ot_u8 *)cmd;
    ot_u32 crc = crc32_ieee(0, OT_NULL, 0);

    crc = crc32_ieee(crc, raw + USB_CMD_CRC_SEG1_OFF, USB_CMD_CRC_SEG1_LEN);
    crc = crc32_ieee(crc, raw + USB_CMD_CRC_SEG2_OFF, USB_CMD_CRC_SEG2_LEN);
    return crc;
}

static ot_s32 cmd_check(const usb_cmd_t *cmd)
{
    if (cmd->magic != USB_CMD_MAGIC) {
        recv_err("bad cmd magic 0x%x\n", cmd->magic);
        return OT_FAILURE;
    }
    if (cmd->version != USB_PROTO_VERSION) {
        recv_err("protocol version %u mismatch\n", cmd->version);
        status_update(USB_STAGE_FAILED, 0, USB_ERR_PROTOCOL_MISMATCH, "protocol version mismatch");
        return OT_FAILURE;
    }
    if (cmd->crc32 != cmd_crc32(cmd)) {
        recv_err("cmd crc32 mismatch: 0x%x != 0x%x\n", cmd->crc32, cmd_crc32(cmd));
        return OT_FAILURE;
    }
    return OT_SUCCESS;
}

/* reply a STATUS_RESP (64B) to an IN control request */
static void status_respond(ot_void)
{
    upgrade_status_resp_t st;

    status_read(&st);
    if (write(g_ep0, &st, sizeof(st)) != (ssize_t)sizeof(st)) {
        recv_err("write status resp failed: %s\n", strerror(errno));
    }
}

/* ------------------------------------------------------------------ */
/* receive state machine                                               */
/* ------------------------------------------------------------------ */

static void rx_reset(ot_void)
{
    if (g_rx.pkg_fd >= 0) {
        close(g_rx.pkg_fd);
        g_rx.pkg_fd = -1;
    }
    unlink(USB_UPGRADE_PKG_FILE);
    g_rx.total_size = 0;
    g_rx.total_crc32 = 0;
    g_rx.received = 0;
    g_rx.expect_seq = 0;
    g_rx.crc_acc = crc32_ieee(0, OT_NULL, 0);
    g_rx.pkg_started = OT_FALSE;
    g_last_progress = 0xFFFFFFFFU;  /* new session: re-arm the status throttle */
}

static void rx_abort_to_idle(ot_u32 error_code, const ot_s8 *info)
{
    rx_reset();
    media_resume();
    status_update(USB_STAGE_IDLE, 0, error_code, info);
}

static void handle_enter_upgrade(ot_void)
{
    upgrade_status_resp_t st;

    status_read(&st);
    if (st.stage != USB_STAGE_IDLE && st.stage != USB_STAGE_FAILED) {
        recv_err("enter upgrade in stage %u rejected\n", st.stage);
        status_respond();   /* report current stage unchanged */
        return;
    }

    rx_reset();
    mkdir(USB_UPGRADE_DIR, 0755);
    mkdir(USB_UPGRADE_DATA_DIR, 0755);
    g_rx.start_time = time(OT_NULL);
    (void)close(open(USB_UPGRADE_ENTERED_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0600));
    media_pause();

    status_update(USB_STAGE_RECEIVING, 0, USB_ERR_NONE, "enter upgrade mode");
    recv_log("usb_receiver: enter upgrade mode\n");
    status_respond();
}

static void handle_send_package(const usb_cmd_t *cmd)
{
    upgrade_status_resp_t st;
    struct statvfs vfs;

    status_read(&st);
    if (st.stage != USB_STAGE_RECEIVING) {
        recv_err("send package in stage %u rejected\n", st.stage);
        return;
    }

    ot_u32 total_size = cmd->param[USB_PKG_PARAM_SIZE_IDX];
    ot_u32 total_crc = cmd->param[USB_PKG_PARAM_CRC_IDX];
    if (total_size == 0 || total_size > USB_PKG_MAX_SIZE) {
        recv_err("invalid package size %u\n", total_size);
        status_update(USB_STAGE_RECEIVING, 0, USB_ERR_PACKAGE_INVALID, "invalid package size");
        return;
    }

    if (statvfs(USB_UPGRADE_DIR, &vfs) != 0) {
        recv_err("statvfs %s failed: %s\n", USB_UPGRADE_DIR, strerror(errno));
        status_update(USB_STAGE_RECEIVING, 0, USB_ERR_NO_MEMORY, "tmpfs unavailable");
        return;
    }
    ot_u64 free_bytes = (ot_u64)vfs.f_bavail * vfs.f_frsize;
    ot_u64 need = (ot_u64)total_size + (ot_u64)total_size / USB_TMPFS_MIN_FREE_PCT;
    if (free_bytes < need) {
        recv_err("tmpfs free %llu < need %llu\n", free_bytes, need);
        status_update(USB_STAGE_RECEIVING, 0, USB_ERR_NO_MEMORY, "tmpfs space not enough");
        return;
    }

    rx_reset();
    g_rx.pkg_fd = open(USB_UPGRADE_PKG_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (g_rx.pkg_fd < 0) {
        recv_err("open %s failed: %s\n", USB_UPGRADE_PKG_FILE, strerror(errno));
        status_update(USB_STAGE_RECEIVING, 0, USB_ERR_NO_MEMORY, "create package file failed");
        return;
    }
    g_rx.total_size = total_size;
    g_rx.total_crc32 = total_crc;
    g_rx.pkg_started = OT_TRUE;
    g_rx.start_time = time(OT_NULL);    /* re-arm the 600s budget for the transfer phase */
    recv_log("usb_receiver: package meta size=%u crc=0x%x\n", total_size, total_crc);
}

/* read exactly len bytes from the bulk OUT endpoint, 5s timeout */
static ot_s32 bulk_read(ot_u8 *buf, ot_u32 len)
{
    ot_u32 done = 0;
    struct pollfd pfd = { .fd = g_ep_out, .events = POLLIN };

    while (done < len) {
        ot_s32 pret = poll(&pfd, 1, BULK_TIMEOUT_MS);
        if (pret <= 0) {
            recv_err("bulk read poll %s\n", pret == 0 ? "timeout" : strerror(errno));
            return OT_FAILURE;
        }
        ssize_t rlen = read(g_ep_out, buf + done, len - done);
        if (rlen <= 0) {
            recv_err("bulk read failed: %s\n", strerror(errno));
            return OT_FAILURE;
        }
        done += (ot_u32)rlen;
    }
    return OT_SUCCESS;
}

static void rx_finish_check(ot_void)
{
    close(g_rx.pkg_fd);
    g_rx.pkg_fd = -1;

    if (g_rx.crc_acc != g_rx.total_crc32) {
        recv_err("total crc32 mismatch: 0x%x != 0x%x\n", g_rx.crc_acc, g_rx.total_crc32);
        /* Drop the package and report FAILED: the PC may start a fresh
         * upgrade immediately (ENTER_UPGRADE is accepted from FAILED).
         * Staying in RECEIVING would trap the device for 600s because
         * the PC only retransmits the last fragment, never the whole
         * package (B5 fix). */
        rx_reset();
        media_resume();
        status_update(USB_STAGE_FAILED, 0, USB_ERR_CRC_FAIL, "package crc32 mismatch");
        return;
    }

    recv_log("usb_receiver: package received ok, launch ota_prepare\n");
    status_update(USB_STAGE_WAITING, USB_PROGRESS_WAITING, USB_ERR_NONE, "verify and flash");

    if (access(OTA_PREPARE_PATH, X_OK) != 0) {
        recv_err("%s not executable: %s\n", OTA_PREPARE_PATH, strerror(errno));
        status_update(USB_STAGE_FAILED, USB_PROGRESS_WAITING, USB_ERR_FLASH_WRITE_FAIL,
                      "ota_prepare missing");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        /* undo the parent's SIGCHLD=SIG_IGN (kept across exec): with SIGCHLD
         * ignored, glibc system()/waitpid always fail with ECHILD and every
         * UBI helper in ota_prepare would falsely report failure */
        signal(SIGCHLD, SIG_DFL);
        execl(OTA_PREPARE_PATH, "ota_prepare", USB_UPGRADE_PKG_FILE, (ot_s8 *)OT_NULL);
        _exit(127); /* child: exec failed */
    }
    if (pid < 0) {
        recv_err("fork ota_prepare failed: %s\n", strerror(errno));
        status_update(USB_STAGE_FAILED, USB_PROGRESS_WAITING, USB_ERR_FLASH_WRITE_FAIL,
                      "start ota_prepare failed");
        return;
    }
    g_waiting_since = time(OT_NULL);
}

static void handle_send_data(const usb_cmd_t *cmd)
{
    upgrade_status_resp_t st;
    usb_frag_hdr_t hdr;
    ot_u32 frag_crc;

    status_read(&st);
    if (st.stage != USB_STAGE_RECEIVING || g_rx.pkg_started != OT_TRUE) {
        recv_err("send data in stage %u (started %d) rejected\n", st.stage, g_rx.pkg_started);
        return;
    }

    ot_u32 expect_len = USB_FRAG_HDR_LEN + cmd->payload_len + USB_FRAG_CRC_LEN;
    if (cmd->payload_len == 0 || cmd->payload_len > USB_FRAG_MAX_DATA ||
        expect_len > sizeof(g_frag_buf)) {
        recv_err("invalid payload_len %u\n", cmd->payload_len);
        status_update(USB_STAGE_RECEIVING, st.progress, USB_ERR_PACKAGE_INVALID, "invalid fragment");
        return;
    }

    if (bulk_read(g_frag_buf, expect_len) != OT_SUCCESS) {
        status_update(USB_STAGE_RECEIVING, st.progress, USB_ERR_TIMEOUT, "bulk read timeout");
        return;
    }

    memcpy(&hdr, g_frag_buf, sizeof(hdr));

    /* validate data_len before using it in any address arithmetic:
     * a corrupted value would otherwise send the crc memcpy far out of
     * bounds and crash the daemon */
    if (hdr.data_len != cmd->payload_len || hdr.data_len > USB_FRAG_MAX_DATA) {
        recv_err("frag data_len %u != payload_len %u\n", hdr.data_len, cmd->payload_len);
        status_update(USB_STAGE_RECEIVING, st.progress, USB_ERR_CRC_FAIL, "fragment length mismatch");
        return;
    }

    memcpy(&frag_crc, g_frag_buf + USB_FRAG_HDR_LEN + hdr.data_len, sizeof(frag_crc));

    ot_u32 calc = crc32_ieee(0, OT_NULL, 0);
    calc = crc32_ieee(calc, g_frag_buf, USB_FRAG_HDR_LEN + hdr.data_len);
    if (calc != frag_crc) {
        recv_err("frag %u crc32 mismatch: 0x%x != 0x%x\n", hdr.seq, calc, frag_crc);
        status_update(USB_STAGE_RECEIVING, st.progress, USB_ERR_CRC_FAIL, "fragment crc32 mismatch");
        return;
    }

    /* retransmission of the last accepted fragment: accept silently without
     * rewrite; CMD_SEND_DATA is an OUT control transfer, so no ep0 response
     * may be written here (writing ep0 would stall the transfer). The PC
     * reads the outcome via CMD_QUERY_STATUS. */
    if (hdr.seq + 1 == g_rx.expect_seq && hdr.offset + hdr.data_len == g_rx.received) {
        recv_log("usb_receiver: dup frag %u, skip\n", hdr.seq);
        /* clear a stale error from the failed attempt so the PC stops
         * retransmitting (the status write is throttled below and would
         * otherwise keep the old error_code visible) */
        if (st.error_code != USB_ERR_NONE) {
            status_update(USB_STAGE_RECEIVING, st.progress, USB_ERR_NONE, OT_NULL);
        }
        return;
    }

    if (hdr.seq != g_rx.expect_seq || hdr.offset != g_rx.received) {
        recv_err("frag seq %u/%u offset %u/%u out of order\n",
                 hdr.seq, g_rx.expect_seq, hdr.offset, g_rx.received);
        status_update(USB_STAGE_RECEIVING, st.progress, USB_ERR_CRC_FAIL, "fragment out of order");
        return;
    }

    if (pwrite(g_rx.pkg_fd, g_frag_buf + USB_FRAG_HDR_LEN, hdr.data_len, hdr.offset) !=
        (ssize_t)hdr.data_len) {
        recv_err("write package failed: %s\n", strerror(errno));
        status_update(USB_STAGE_FAILED, st.progress, USB_ERR_FLASH_WRITE_FAIL, "write tmpfs failed");
        return;
    }

    g_rx.received += hdr.data_len;
    g_rx.expect_seq++;
    g_rx.crc_acc = crc32_ieee(g_rx.crc_acc, g_frag_buf + USB_FRAG_HDR_LEN, hdr.data_len);

    ot_u32 progress = (ot_u32)(((ot_u64)g_rx.received * USB_PROGRESS_RECV_MAX) / g_rx.total_size);
    /* Throttle status file writes: a UBIFS fsync+rename per 64KB
     * fragment is needlessly expensive, only persist on percent change.
     * Exception: a stale error_code must always be cleared so the PC
     * sees the retransmitted fragment was accepted. */
    if (progress != g_last_progress || st.error_code != USB_ERR_NONE) {
        g_last_progress = progress;
        status_update(USB_STAGE_RECEIVING, progress, USB_ERR_NONE, OT_NULL);
    }

    if (g_rx.received == g_rx.total_size) {
        rx_finish_check();
    }
}

static void handle_exit_upgrade(ot_void)
{
    upgrade_status_resp_t st;

    status_read(&st);
    /* only valid during the receiving stage (design 6.2.2) */
    if (st.stage == USB_STAGE_RECEIVING) {
        recv_log("usb_receiver: exit upgrade (user cancel)\n");
        rx_abort_to_idle(USB_ERR_NONE, "user cancel");
    } else {
        recv_err("exit upgrade in stage %u ignored\n", st.stage);
    }
    status_respond();
}

/* ------------------------------------------------------------------ */
/* control request dispatch                                            */
/* ------------------------------------------------------------------ */

static void handle_setup(const struct usb_ctrlrequest *req)
{
    ot_u16 req_type = req->bRequestType;
    usb_cmd_t cmd;

    /* The Microsoft WCID vendor request (bRequest = 0xC1) is answered by
     * the kernel composite layer from the MS OS descriptors embedded in
     * g_ffs_descs; it never reaches user space. */

    if (req_type == USB_REQ_TYPE_IN) {
        /* IN commands carry no data stage from the host; the 64B
         * response is the STATUS_RESP status structure. */
        switch (req->bRequest) {
            case USB_CMD_GET_VERSION:
            case USB_CMD_QUERY_STATUS:
                status_respond();
                break;
            case USB_CMD_ENTER_UPGRADE:
                handle_enter_upgrade();
                break;
            case USB_CMD_EXIT_UPGRADE:
                handle_exit_upgrade();
                break;
            default:
                recv_err("unknown IN cmd 0x%x\n", req->bRequest);
                break;
        }
        return;
    }

    if (req_type != USB_REQ_TYPE_OUT || req->wLength != USB_CTRL_DATA_LEN) {
        recv_err("unexpected setup type 0x%x len %u\n", req_type, req->wLength);
        return;
    }

    if (read(g_ep0, &cmd, sizeof(cmd)) != (ssize_t)sizeof(cmd)) {
        recv_err("read cmd data stage failed: %s\n", strerror(errno));
        return;
    }
    if (cmd_check(&cmd) != OT_SUCCESS) {
        return;
    }

    switch (cmd.cmd) {
        case USB_CMD_SEND_PACKAGE:
            handle_send_package(&cmd);
            break;
        case USB_CMD_SEND_DATA:
            handle_send_data(&cmd);
            break;
        default:
            recv_err("unknown OUT cmd 0x%x\n", cmd.cmd);
            break;
    }
}

/* ------------------------------------------------------------------ */
/* FunctionFS endpoint handling                                        */
/* ------------------------------------------------------------------ */

static ot_s32 ffs_open(ot_void)
{
    /* O_CLOEXEC on all endpoint fds: the ota_prepare child must not hold
     * them (a lingering fd blocks umount in the gadget script stop/restart path) */
    g_ep0 = open(USB_FFS_EP0, O_RDWR | O_CLOEXEC);
    if (g_ep0 < 0) {
        recv_err("open %s failed: %s\n", USB_FFS_EP0, strerror(errno));
        return OT_FAILURE;
    }
    if (write(g_ep0, &g_ffs_descs, sizeof(g_ffs_descs)) != (ssize_t)sizeof(g_ffs_descs)) {
        recv_err("write descriptors failed: %s\n", strerror(errno));
        close(g_ep0);
        g_ep0 = -1;
        return OT_FAILURE;
    }
    if (write(g_ep0, &g_ffs_strings, sizeof(g_ffs_strings)) != (ssize_t)sizeof(g_ffs_strings)) {
        recv_err("write strings failed: %s\n", strerror(errno));
        close(g_ep0);
        g_ep0 = -1;
        return OT_FAILURE;
    }
    /* The mode controller waits for this only after both ep0 writes finish. */
    (void)close(open(USB_RECEIVER_READY_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0600));
    return OT_SUCCESS;
}

static ot_s32 ffs_open_eps(ot_void)
{
    if (g_ep_out < 0) {
        g_ep_out = open(USB_FFS_EP_OUT, O_RDWR | O_CLOEXEC);
        if (g_ep_out < 0) {
            recv_err("open %s failed: %s\n", USB_FFS_EP_OUT, strerror(errno));
            return OT_FAILURE;
        }
    }
    if (g_ep_in < 0) {
        g_ep_in = open(USB_FFS_EP_IN, O_RDWR | O_CLOEXEC);
        if (g_ep_in < 0) {
            recv_err("open %s failed: %s\n", USB_FFS_EP_IN, strerror(errno));
            close(g_ep_out);
            g_ep_out = -1;
            return OT_FAILURE;
        }
    }
    return OT_SUCCESS;
}

static void ffs_close_eps(ot_void)
{
    if (g_ep_out >= 0) {
        close(g_ep_out);
        g_ep_out = -1;
    }
    if (g_ep_in >= 0) {
        close(g_ep_in);
        g_ep_in = -1;
    }
}

static void handle_ep0_events(ot_void)
{
    ot_u8 buf[EP0_READ_BUF];
    ssize_t len = read(g_ep0, buf, sizeof(buf));

    if (len < (ssize_t)sizeof(struct usb_functionfs_event)) {
        recv_err("ep0 read failed: %s\n", len < 0 ? strerror(errno) : "short read");
        return;
    }

    const struct usb_functionfs_event *ev = (const struct usb_functionfs_event *)buf;
    ot_u32 count = (ot_u32)len / sizeof(*ev);
    for (ot_u32 i = 0; i < count; i++) {
        switch (ev[i].type) {
            case FUNCTIONFS_BIND:
                recv_log("usb_receiver: functionfs bind\n");
                break;
            case FUNCTIONFS_ENABLE:
                recv_log("usb_receiver: functionfs enable\n");
                if (ffs_open_eps() == OT_SUCCESS) {
                    g_configured = OT_TRUE;
                }
                break;
            case FUNCTIONFS_DISABLE:
                recv_log("usb_receiver: functionfs disable\n");
                g_configured = OT_FALSE;
                ffs_close_eps();
                break;
            case FUNCTIONFS_SETUP:
                handle_setup(&ev[i].u.setup);
                break;
            case FUNCTIONFS_SUSPEND:
            case FUNCTIONFS_RESUME:
                break;
            default:
                break;
        }
    }
}

/* total upgrade timeout guard (design 6.2.5: 600s from ENTER_UPGRADE) */
static void check_total_timeout(ot_void)
{
    upgrade_status_resp_t st;

    status_read(&st);

    /* Terminal FAILED reached without a reboot: bad package or a
     * verify/flash failure reported by ota_prepare, an ota_prepare launch
     * failure, or the WAITING start timeout below. The camera apps killed
     * on ENTER_UPGRADE would otherwise stay dead until a manual reboot,
     * which breaks "UVC keeps working when not upgrading". Resume once;
     * the media hook itself is pidof-guarded and idempotent. */
    if (st.stage == USB_STAGE_FAILED && g_media_paused) {
        recv_log("usb_receiver: FAILED with media paused, resuming camera\n");
        media_resume();
    }

    /* WAITING watchdog: ota_prepare was launched but never moved the stage
     * forward (exec failure etc.) -> report FAILED instead of hanging */
    if (st.stage == USB_STAGE_WAITING) {
        if (g_waiting_since == 0) {
            g_waiting_since = time(OT_NULL);    /* receiver restarted mid-flow */
        } else if (time(OT_NULL) - g_waiting_since > OTA_PREPARE_START_TIMEOUT_SEC) {
            recv_err("ota_prepare start timeout (%ds), report FAILED\n",
                     OTA_PREPARE_START_TIMEOUT_SEC);
            status_update(USB_STAGE_FAILED, USB_PROGRESS_WAITING, USB_ERR_FLASH_WRITE_FAIL,
                          "ota_prepare not responding");
            g_waiting_since = 0;
        }
        return;
    }
    g_waiting_since = 0;

    /* the 600s guard covers the whole RECEIVING phase from ENTER_UPGRADE
     * (g_rx.start_time is recorded there), including the window before
     * CMD_SEND_PACKAGE arrives; ota_prepare owns the status from WAITING on */
    if (st.stage != USB_STAGE_RECEIVING) {
        return;
    }
    if (g_rx.start_time != 0 && time(OT_NULL) - g_rx.start_time > USB_TOTAL_TIMEOUT_SEC) {
        recv_err("total upgrade timeout (%ds), cleanup\n", USB_TOTAL_TIMEOUT_SEC);
        rx_abort_to_idle(USB_ERR_TIMEOUT, "upgrade timeout");
    }
}

static void sig_handler(ot_s32 sig)
{
    (ot_void)sig;
    g_running = OT_FALSE;
}

/* The /data partition (UBIFS) is mounted by S90user, which runs AFTER
 * S89usbffs starts this daemon, so the initial status-file setup has to
 * wait until /data is available. Called from the main loop; runs once. */
static void status_boot_init(ot_void)
{
    static ot_bool done = OT_FALSE;
    upgrade_status_resp_t st;

    if (done) {
        return;
    }
    if (mkdir(USB_UPGRADE_DATA_DIR, 0755) != 0 && errno != EEXIST) {
        return;     /* /data not mounted yet, retry next loop iteration */
    }
    done = OT_TRUE;

    /* Any non-IDLE stage at startup is stale: the receiver just booted,
     * so no transfer can be in progress. After a recovery reboot (stage
     * was SWITCHING) the PC confirms the result via version comparison,
     * so resetting to IDLE here is safe and keeps ENTER_UPGRADE usable. */
    if (status_read(&st) != OT_SUCCESS) {
        status_write(&st);
    } else if (st.stage != USB_STAGE_IDLE) {
        recv_log("usb_receiver: stale stage %u at boot, reset to IDLE\n", st.stage);
        status_update(USB_STAGE_IDLE, 0, st.error_code, st.info);
    }
}

ot_s32 main(ot_void)
{
    struct pollfd pfd;

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGCHLD, SIG_IGN);   /* auto-reap the ota_prepare child */

    mkdir(USB_UPGRADE_DIR, 0755);
    unlink(USB_RECEIVER_READY_FILE);
    unlink(USB_UPGRADE_ENTERED_FILE);

    if (ffs_open() != OT_SUCCESS) {
        return OT_FAILURE;
    }

    recv_log("usb_receiver: start, wait for gadget enable\n");
    while (g_running) {
        pfd.fd = g_ep0;
        pfd.events = POLLIN;
        pfd.revents = 0;
        ot_s32 pret = poll(&pfd, 1, POLL_IDLE_MS);
        if (pret < 0) {
            if (errno == EINTR) {
                continue;
            }
            recv_err("poll failed: %s\n", strerror(errno));
            break;
        }
        if (pret > 0 && (pfd.revents & POLLIN)) {
            handle_ep0_events();
        }
        status_boot_init();
        check_total_timeout();
    }

    rx_reset();
    ffs_close_eps();
    if (g_ep0 >= 0) {
        close(g_ep0);
    }
    unlink(USB_RECEIVER_READY_FILE);
    recv_log("usb_receiver: exit\n");
    return OT_SUCCESS;
}
