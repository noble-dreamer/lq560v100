// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2026 Oritek Technologies Co., Ltd.
 *
 * Linux OTA preparation application for SD3589C (NAND + UBI)
 *
 * Phase 1 (Linux):
 *   1. Verify upgrade.img header, CRC and version.
 *   2. Write upgrade.img to UBI 'upgrade' static volume.
 *   3. Write uflag with PENDING state for NAND components
 *      (bl31/kernel/rootfs/uboot). Recovery U-Boot will handle those.
 *
 * Usage:
 *   ota_prepare [upgrade_package.img]   (default: /tmp/upgrade/upgrade.img)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/reboot.h>
#include <mtd/mtd-user.h>

#include "tinycrypt/sha256.h"
#include "tinycrypt/ecdsa.h"

/* ---- Local OTA layout constants (embedded; keep in sync with U-Boot/mkupgrade) ---- */

#define OTA_UPGRADE_MAGIC		0x55504752
#define OTA_UPGRADE_FLAG_VERSION	1

#define OTA_STATE_IDLE		0
#define OTA_STATE_PENDING	1

struct ota_upgrade_flag {
	uint32_t magic;
	uint32_t version;
	uint32_t state;
	uint32_t upgrade_source;
	uint32_t upgrade_size;
	uint32_t upgrade_crc;
	uint32_t component_mask;
	uint32_t retry_count;
	uint32_t version_new;
	uint32_t version_old;
	uint32_t completed_mask;
	uint8_t  reserved[16];
	uint32_t crc32;
};

#define OTA_PKG_MAGIC		0x4F544155
#define OTA_PKG_VERSION		1
#define OTA_PKG_HEADER_SIZE	512
#define OTA_PKG_COMP_SIZE	64
#define OTA_PKG_MAX_COMPS	16
#define OTA_PKG_SIG_SIZE	256

#define OTA_MEDIA_NAND		0

struct ota_pkg_header {
	uint32_t magic;
	uint32_t version;
	uint32_t total_size;
	uint32_t component_cnt;
	uint32_t package_crc;
	uint32_t fw_version;
	uint32_t reserved0;
	uint8_t  reserved[484];
};

struct ota_pkg_component {
	uint8_t  name[16];
	uint32_t flash_offset;
	uint32_t flash_size;
	uint32_t data_offset;
	uint32_t data_size;
	uint32_t data_crc;
	uint8_t  media;
	uint8_t  fs_type;
	uint8_t  reserved[26];
};

#define OTA_COMP_UBOOT		(1U << 0)
#define OTA_COMP_BL31		(1U << 1)
#define OTA_COMP_KERNEL		(1U << 2)
#define OTA_COMP_ROOTFS		(1U << 3)

/* NAND partition bounds (must match nand.xml / nand_ota_upgrade.c) */
#define NAND_BLOCK_SIZE		0x20000	/* 128KB erase block */

struct ota_part_bound {
	const char *name;
	uint32_t offset;
	uint32_t size;
};

static const struct ota_part_bound OTA_PART_BOUNDS[] = {
	{ "uboot",  0x00200000, 0x00100000 },
	{ "bl31",   0x00300000, 0x00080000 },
	{ "kernel", 0x00380000, 0x00800000 },
	{ "rootfs", 0x00B80000, 0x00800000 },
};

#define READ_BUF_SIZE		4096
#define UBI_CTRL_DEV		"/dev/ubi_ctrl"
#define UBI_VOL_NAME		"upgrade"
#define UFLAG_PART_NAME		"uflag"
#ifndef OTA_PUBKEY_PATH
#define OTA_PUBKEY_PATH		"/etc/ota_pubkey.bin"
#endif
#ifndef FS_VERSION_PATH
#define FS_VERSION_PATH		"/etc/fs-version"
#endif

/* Version format: 0xMMmmppbb = Major.Minor.Patch.Build */
#define VERSION_MAKE(major, minor, patch, build) \
	(((major) << 24) | ((minor) << 16) | ((patch) << 8) | (build))

/* ---- upgrade.status (design appendix G, shared with usb_receiver) ----
 * 64-byte binary, identical layout to upgrade_status_resp_t in
 * component/upgrade/usb_receiver/usb_receiver.h; written with
 * tmp-file + fsync + rename under flock, same as the receiver does. */
#define OTA_STAGE_VERIFYING	3
#define OTA_STAGE_FLASHING	4
#define OTA_STAGE_SWITCHING	5
#define OTA_STAGE_FAILED	8
#define OTA_SLOT_NONE		0xFF

#define OTA_ERR_NONE		0
#define OTA_ERR_PACKAGE_INVALID	3
#define OTA_ERR_SIGNATURE_FAIL	4
#define OTA_ERR_HASH_FAIL	5
#define OTA_ERR_VERSION_ROLLBACK 6
#define OTA_ERR_FLASH_WRITE_FAIL 7

struct ota_status_file {
	uint32_t stage;
	uint32_t progress;
	uint32_t error_code;
	uint32_t current_slot;
	uint32_t version;
	char info[44];
};

#define OTA_STATUS_DIR		"/data/upgrade"
#define OTA_STATUS_FILE		OTA_STATUS_DIR "/upgrade.status"
#define OTA_STATUS_LOCK_FILE	OTA_STATUS_DIR "/upgrade.status.lock"
#define OTA_STATUS_TMP_FILE	OTA_STATUS_DIR "/upgrade.status.tmp"

static uint32_t g_status_version;	/* current fw version once known */
static uint32_t g_status_progress;	/* last reported progress */

static void status_update(uint32_t stage, uint32_t progress,
			  uint32_t error_code, const char *info)
{
	struct ota_status_file st;
	int lfd, fd;

	g_status_progress = progress;
	mkdir(OTA_STATUS_DIR, 0755);

	memset(&st, 0, sizeof(st));
	/* preserve nothing except what we explicitly set: the file is owned
	 * by ota_prepare while it runs (receiver only reads it) */
	st.stage = stage;
	st.progress = progress;
	st.error_code = error_code;
	st.current_slot = OTA_SLOT_NONE;
	st.version = g_status_version;
	if (info) {
		strncpy(st.info, info, sizeof(st.info) - 1);
		st.info[sizeof(st.info) - 1] = '\0';
	}

	lfd = open(OTA_STATUS_LOCK_FILE, O_RDWR | O_CREAT, 0644);
	if (lfd >= 0)
		flock(lfd, LOCK_EX);

	fd = open(OTA_STATUS_TMP_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd >= 0) {
		int ok = 0;

		if (write(fd, &st, sizeof(st)) == (ssize_t)sizeof(st) && fsync(fd) == 0)
			ok = 1;
		close(fd);
		/* only rename over the live status file when the tmp file is
		 * complete and durable; a short write must not truncate it */
		if (ok && rename(OTA_STATUS_TMP_FILE, OTA_STATUS_FILE) != 0)
			printf("WARNING: rename status file failed: %s\n", strerror(errno));
	}

	if (lfd >= 0) {
		flock(lfd, LOCK_UN);
		close(lfd);
	}
}

/* report FAILED with the given error code and exit */
#define OTA_FAIL(ec, msg) do { \
		printf("ERROR: %s\n", msg); \
		status_update(OTA_STAGE_FAILED, g_status_progress, (ec), (msg)); \
		unlink(pkg_path); \
		return 1; \
	} while (0)


static uint32_t calc_crc32_continue(uint32_t init_crc, const uint8_t *data, size_t len)
{
	static uint32_t table[256];
	static int init = 0;
	uint32_t crc = init_crc;

	if (!init) {
		for (int i = 0; i < 256; i++) {
			uint32_t c = i;
			for (int j = 0; j < 8; j++) {
				c = (c & 1) ? (0xEDB88320 ^ (c >> 1)) : (c >> 1);
			}
			table[i] = c;
		}
		init = 1;
	}

	for (size_t i = 0; i < len; i++) {
		crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
	}

	return crc;
}

static uint32_t calc_crc32(const uint8_t *data, size_t len)
{
	return calc_crc32_continue(0xFFFFFFFF, data, len) ^ 0xFFFFFFFF;
}

static uint32_t comp_bit_by_name(const char *name)
{
	if (strncmp(name, "bl31", 16) == 0)
		return OTA_COMP_BL31;
	if (strncmp(name, "kernel", 16) == 0)
		return OTA_COMP_KERNEL;
	if (strncmp(name, "rootfs", 16) == 0)
		return OTA_COMP_ROOTFS;
	if (strncmp(name, "uboot", 16) == 0)
		return OTA_COMP_UBOOT;
	return 0;
}

/*
 * Component boundary validation (requirement AR-9): the component must
 * target a known NAND partition, stay inside it, be erase-block aligned,
 * and its data area must stay inside the package (before the signature
 * slot). Mirrors the Recovery-side checks so bad packages are rejected
 * in Linux with a distinguishable error code instead of after reboot.
 */
static int check_component_bounds(const struct ota_pkg_component *c,
				  const struct ota_pkg_component *table,
				  uint32_t cnt, uint32_t total_size,
				  const char *name)
{
	const struct ota_part_bound *pb = NULL;
	uint32_t data_floor = OTA_PKG_HEADER_SIZE + cnt * OTA_PKG_COMP_SIZE;
	uint32_t data_ceil = total_size - OTA_PKG_SIG_SIZE;

	for (uint32_t i = 0; i < sizeof(OTA_PART_BOUNDS) / sizeof(OTA_PART_BOUNDS[0]); i++) {
		if (strncmp(name, OTA_PART_BOUNDS[i].name, 16) == 0) {
			pb = &OTA_PART_BOUNDS[i];
			break;
		}
	}
	if (!pb) {
		printf("ERROR: component '%s': no partition bound\n", name);
		return -1;
	}
	if ((c->flash_offset & (NAND_BLOCK_SIZE - 1)) != 0 ||
	    (c->flash_size & (NAND_BLOCK_SIZE - 1)) != 0) {
		printf("ERROR: component '%s': flash area not %uKB aligned\n",
		       name, NAND_BLOCK_SIZE / 1024);
		return -1;
	}
	/* mirror the Recovery check exactly: the offset must be the
	 * partition start, the size must fit inside the partition */
	if (c->flash_size == 0 || c->flash_offset != pb->offset ||
	    c->flash_size > pb->size) {
		printf("ERROR: component '%s': flash area [0x%x,+0x%x) not at partition [0x%x,+0x%x)\n",
		       name, c->flash_offset, c->flash_size, pb->offset, pb->size);
		return -1;
	}
	if (c->data_size == 0 || c->data_size > c->flash_size) {
		printf("ERROR: component '%s': invalid data_size %u\n", name, c->data_size);
		return -1;
	}
	if (c->data_offset < data_floor || c->data_offset + c->data_size > data_ceil) {
		printf("ERROR: component '%s': data area out of package bounds\n", name);
		return -1;
	}
	/* components must not overlap each other on flash */
	for (uint32_t i = 0; i < cnt; i++) {
		const struct ota_pkg_component *o = &table[i];

		if (o == c)
			continue;
		if (c->flash_offset < o->flash_offset + o->flash_size &&
		    o->flash_offset < c->flash_offset + c->flash_size) {
			printf("ERROR: component '%s' overlaps another component\n", name);
			return -1;
		}
	}
	return 0;
}

static int find_mtd_by_name(const char *name, char *path, size_t path_len)
{
	FILE *fp = fopen("/proc/mtd", "r");
	if (!fp)
		return -1;

	char line[256];
	while (fgets(line, sizeof(line), fp)) {
		int dev_num;
		char mtd_name[32];

		if (sscanf(line, "mtd%d: %*s %*s \"%31[^\"]\"", &dev_num, mtd_name) == 2) {
			if (strcmp(mtd_name, name) == 0) {
				snprintf(path, path_len, "/dev/mtd%d", dev_num);
				fclose(fp);
				return 0;
			}
		}
	}

	fclose(fp);
	return -1;
}

static int read_sys_file(const char *path, char *buf, size_t len)
{
	FILE *fp = fopen(path, "r");

	if (!fp)
		return -1;
	if (!fgets(buf, len, fp)) {
		fclose(fp);
		return -1;
	}
	fclose(fp);
	buf[strcspn(buf, "\n")] = '\0';
	return 0;
}

/*
 * Confirm /dev/ubi2 is really bound to the MTD of the upgrade partition.
 * Startup scripts assign userfs=ubi0, data=ubi1, upgrade=ubi2; if ubi2
 * were claimed by another partition, writing /dev/ubi2_0 would overwrite
 * someone else's volume, so refuse to proceed.
 */
static int ubi_verify_binding(int expected_mtd_num)
{
	char buf[32];
	int mtd_num;

	if (read_sys_file("/sys/class/ubi/ubi2/mtd_num", buf, sizeof(buf)) != 0) {
		printf("ERROR: cannot read /sys/class/ubi/ubi2/mtd_num\n");
		return -1;
	}
	mtd_num = atoi(buf);
	if (mtd_num != expected_mtd_num) {
		printf("ERROR: /dev/ubi2 bound to mtd%d (expected mtd%d=upgrade), refuse\n",
		       mtd_num, expected_mtd_num);
		return -1;
	}
	return 0;
}

static int load_public_key(tc_ecdsa_key_t *key)
{
	FILE *fp = fopen(OTA_PUBKEY_PATH, "rb");
	if (!fp) {
		printf("WARNING: Public key file %s not found, skipping ECDSA verify\n",
		       OTA_PUBKEY_PATH);
		return -1;
	}

	if (fread(key->x, 1, 32, fp) != 32 || fread(key->y, 1, 32, fp) != 32) {
		printf("ERROR: Failed to read public key\n");
		fclose(fp);
		return -1;
	}

	fclose(fp);
	printf("INFO: Loaded ECDSA public key from %s\n", OTA_PUBKEY_PATH);
	return 0;
}

static int read_signature(const char *pkg_path, uint32_t total_size, uint8_t *sig)
{
	FILE *fp = fopen(pkg_path, "rb");
	if (!fp)
		return -1;

	if (fseek(fp, total_size - OTA_PKG_SIG_SIZE, SEEK_SET) != 0) {
		fclose(fp);
		return -1;
	}

	if (fread(sig, 1, OTA_PKG_SIG_SIZE, fp) != OTA_PKG_SIG_SIZE) {
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}

static int is_signature_uninitialized(const uint8_t *sig)
{
	int all_ff = 1;
	int all_00 = 1;

	for (int i = 0; i < OTA_PKG_SIG_SIZE; i++) {
		if (sig[i] != 0xFF)
			all_ff = 0;
		if (sig[i] != 0x00)
			all_00 = 0;
	}
	return all_ff || all_00;
}

static int verify_ecdsa_signature(const char *pkg_path, uint32_t total_size)
{
	FILE *fp;
	uint8_t sig[OTA_PKG_SIG_SIZE];
	tc_ecdsa_key_t pubkey;
	tc_sha256_state_t sha_ctx;
	uint8_t hash[TC_SHA256_DIGEST_SIZE];
	uint8_t buf[READ_BUF_SIZE];
	size_t signed_len = total_size - OTA_PKG_SIG_SIZE;
	size_t remain;
	int ret = -1;

	if (read_signature(pkg_path, total_size, sig) != 0) {
		printf("ERROR: Failed to read signature\n");
		return -1;
	}

	if (is_signature_uninitialized(sig)) {
#ifdef CONFIG_OTA_DEBUG
		printf("WARNING: Signature uninitialized (unsigned pkg), skipping ECDSA verify [DEBUG]\n");
		return 0;
#else
		/* Production firmware must be signed: reject unsigned packages
		 * (signature area all 0xFF/0x00), no bypass. */
		printf("ERROR: unsigned package (sig all 0xFF/0x00), reject\n");
		return -1;
#endif
	}

	if (load_public_key(&pubkey) != 0) {
#ifdef CONFIG_OTA_DEBUG
		printf("WARNING: No public key available, skipping ECDSA verify [DEBUG]\n");
		return 0;
#else
		/* Production firmware must be verifiable: a missing public key means the
		 * signature cannot be checked, so reject rather than pass. */
		printf("ERROR: no public key, cannot verify, reject\n");
		return -1;
#endif
	}

	fp = fopen(pkg_path, "rb");
	if (!fp)
		return -1;

	tc_sha256_init(&sha_ctx);

	remain = signed_len;
	while (remain > 0) {
		size_t chunk = (remain < READ_BUF_SIZE) ? remain : READ_BUF_SIZE;
		size_t n = fread(buf, 1, chunk, fp);
		if (n != chunk) {
			printf("ERROR: Failed to read signature region\n");
			goto out;
		}
		tc_sha256_update(&sha_ctx, buf, n);
		remain -= n;
	}

	tc_sha256_final(hash, &sha_ctx);

	if (tc_ecdsa_verify(hash, sizeof(hash), sig, &pubkey) != 1) {
		printf("ERROR: ECDSA signature verification FAILED\n");
		goto out;
	}

	printf("INFO: ECDSA signature verification OK\n");
	ret = 0;

out:
	fclose(fp);
	return ret;
}

static int read_system_version(uint32_t *out_ver)
{
	FILE *fp;
	char buf[64];

	/* Anti-rollback version source (same as 3589E): the first field of
	 * /etc/fs-version, parsed as hex. /proc/cmdline image_ver is not used
	 * (it comes from the boot image header version_ext, which is always 0
	 * with the current configuration, so anti-rollback would be a no-op).
	 * fs-version looks like "01000100\trootbox\tnohelp info"; strtoul
	 * stops at the first non-hex character (the tab), giving 0x01000100.
	 * NOTE: every firmware release MUST bump this field, otherwise a new
	 * package whose version <= current version is rejected.
	 * fail-closed: a missing file or a non-hex first field rejects the
	 * upgrade (treating ver_old as 0 would silently disable anti-rollback). */
	fp = fopen(FS_VERSION_PATH, "r");
	if (!fp) {
		printf("ERROR: cannot open %s: %s\n", FS_VERSION_PATH, strerror(errno));
		return -1;
	}
	if (!fgets(buf, sizeof(buf), fp)) {
		printf("ERROR: %s is empty\n", FS_VERSION_PATH);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	{
		char *endptr;
		unsigned long v = strtoul(buf, &endptr, 16);

		if (endptr == buf) {
			printf("ERROR: %s: first field is not hex, refuse upgrade\n",
			       FS_VERSION_PATH);
			return -1;
		}
		*out_ver = (uint32_t)v;
	}
	return 0;
}

static int read_package_header(const char *pkg_path, struct ota_pkg_header *hdr)
{
	FILE *fp = fopen(pkg_path, "rb");
	if (!fp) {
		printf("ERROR: Cannot open %s: %s\n", pkg_path, strerror(errno));
		return -1;
	}

	if (fseek(fp, 0, SEEK_END) != 0) {
		fclose(fp);
		return -1;
	}
	long file_size = ftell(fp);
	if (file_size < (long)(OTA_PKG_HEADER_SIZE + OTA_PKG_SIG_SIZE)) {
		printf("ERROR: Package too small: %ld\n", file_size);
		fclose(fp);
		return -1;
	}

	if (fseek(fp, 0, SEEK_SET) != 0 ||
	    fread(hdr, 1, sizeof(*hdr), fp) != sizeof(*hdr)) {
		printf("ERROR: Failed to read package header\n");
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}

static int read_component_table(const char *pkg_path, uint32_t comp_cnt,
				struct ota_pkg_component *table)
{
	FILE *fp = fopen(pkg_path, "rb");
	if (!fp)
		return -1;

	if (fseek(fp, OTA_PKG_HEADER_SIZE, SEEK_SET) != 0) {
		fclose(fp);
		return -1;
	}

	size_t table_size = comp_cnt * sizeof(struct ota_pkg_component);
	if (fread(table, 1, table_size, fp) != table_size) {
		printf("ERROR: Failed to read component table\n");
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}

static int calc_package_crc(const char *pkg_path, uint32_t total_size, uint32_t *out_crc)
{
	uint8_t buf[READ_BUF_SIZE];
	uint8_t header[OTA_PKG_HEADER_SIZE];
	uint32_t crc = 0xFFFFFFFF;
	FILE *fp;
	size_t data_start = OTA_PKG_HEADER_SIZE;
	size_t data_end = total_size - OTA_PKG_SIG_SIZE; /* exclude signature slot */

	fp = fopen(pkg_path, "rb");
	if (!fp)
		return -1;

	/* CRC covers the full header with package_crc zeroed out */
	if (fseek(fp, 0, SEEK_SET) != 0)
		goto fail;
	if (fread(header, 1, OTA_PKG_HEADER_SIZE, fp) != OTA_PKG_HEADER_SIZE)
		goto fail;
	memset(header + 16, 0, 4); /* package_crc is at offset 16 */
	crc = calc_crc32_continue(crc, header, OTA_PKG_HEADER_SIZE);

	/* component table + data area */
	if (fseek(fp, data_start, SEEK_SET) != 0)
		goto fail;

	size_t remain = data_end - data_start;
	while (remain > 0) {
		size_t chunk = (remain < READ_BUF_SIZE) ? remain : READ_BUF_SIZE;
		size_t n = fread(buf, 1, chunk, fp);
		if (n != chunk)
			goto fail;
		crc = calc_crc32_continue(crc, buf, n);
		remain -= n;
	}

	*out_crc = crc ^ 0xFFFFFFFF;
	fclose(fp);
	return 0;

fail:
	fclose(fp);
	return -1;
}

static int ubi_attach(int mtd_num)
{
	char cmd[128];
	int ret;

	/* Check if already attached */
	if (access("/dev/ubi2", F_OK) == 0) {
		printf("INFO: UBI device 2 already attached\n");
		return ubi_verify_binding(mtd_num);
	}

	snprintf(cmd, sizeof(cmd), "/opt/bin/ubiattach -m %d -d 2 %s", mtd_num, UBI_CTRL_DEV);
	printf("INFO: %s\n", cmd);
	ret = system(cmd);
	if (ret != 0) {
		/* Try to check if attach succeeded despite error */
		if (access("/dev/ubi2", F_OK) == 0) {
			printf("INFO: UBI attach succeeded (ignoring error %d)\n", ret);
			return ubi_verify_binding(mtd_num);
		}
		return ret;
	}
	return ubi_verify_binding(mtd_num);
}

static int ubi_detach(void)
{
	return system("/opt/bin/ubidetach -d 2");
}

static int ubi_create_volume(uint32_t vol_size)
{
	char cmd[128];
	snprintf(cmd, sizeof(cmd), "/opt/bin/ubimkvol /dev/ubi2 -N %s -s %u -t static",
		 UBI_VOL_NAME, vol_size);
	printf("INFO: %s\n", cmd);
	return system(cmd);
}

static int ubi_write_volume(const char *pkg_path, uint32_t pkg_size)
{
	char size_str[16];
	pid_t pid;
	int status;

	/* Pass arguments via fork/exec instead of a system() command line so
	 * that spaces or shell metacharacters in pkg_path cannot truncate the
	 * command or inject shell code (this process runs as root). */
	snprintf(size_str, sizeof(size_str), "%u", pkg_size);
	printf("INFO: /opt/bin/ubiupdatevol /dev/ubi2_0 -s %s %s\n", size_str, pkg_path);

	pid = fork();
	if (pid < 0) {
		printf("ERROR: fork failed: %s\n", strerror(errno));
		return -1;
	}
	if (pid == 0) {
		execl("/opt/bin/ubiupdatevol", "ubiupdatevol", "/dev/ubi2_0",
		      "-s", size_str, pkg_path, (char *)NULL);
		_exit(127);
	}
	if (waitpid(pid, &status, 0) < 0) {
		printf("ERROR: waitpid failed: %s\n", strerror(errno));
		return -1;
	}
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
		printf("ERROR: ubiupdatevol exited abnormally (status=0x%x)\n", status);
		return -1;
	}
	return 0;
}

static int write_uflag(uint32_t pkg_size, uint32_t pkg_crc,
		       uint32_t comp_mask, uint32_t ver_new, uint32_t ver_old)
{
	char mtd_path[32];
	struct ota_upgrade_flag flag;
	struct ota_upgrade_flag readback;
	mtd_info_t mtd_info;
	int fd;
	int ret = -1;

	if (find_mtd_by_name(UFLAG_PART_NAME, mtd_path, sizeof(mtd_path)) != 0) {
		printf("ERROR: Cannot find %s partition\n", UFLAG_PART_NAME);
		return -1;
	}

	fd = open(mtd_path, O_RDWR);
	if (fd < 0) {
		printf("ERROR: Cannot open %s: %s\n", mtd_path, strerror(errno));
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &mtd_info) < 0) {
		printf("ERROR: MEMGETINFO failed: %s\n", strerror(errno));
		goto out_close;
	}

	printf("INFO: %s partition: %s, size=%u, erasesize=%u\n",
	       UFLAG_PART_NAME, mtd_path, mtd_info.size, mtd_info.erasesize);

	memset(&flag, 0, sizeof(flag));
	flag.magic = OTA_UPGRADE_MAGIC;
	flag.version = OTA_UPGRADE_FLAG_VERSION;
	flag.state = OTA_STATE_PENDING;
	flag.upgrade_source = 0; /* UBI upgrade volume */
	flag.upgrade_size = pkg_size;
	flag.upgrade_crc = pkg_crc;
	flag.component_mask = comp_mask;
	flag.retry_count = 3;
	flag.version_new = ver_new;
	flag.version_old = ver_old;
	flag.completed_mask = 0;
	flag.crc32 = calc_crc32((uint8_t *)&flag, sizeof(flag) - sizeof(flag.crc32));

	/* Write 4 redundant copies at 128KB stride, tolerating bad blocks
	 * per copy: a bad block only disables its own copy (the Recovery
	 * reader skips bad copies the same way, fixed stride, nothing is
	 * relocated). Each copy gets its own erase because one MEMERASE
	 * over the whole partition would abort on the first bad block and
	 * make the device unupgradeable. Erase/write/readback failures
	 * mark the block bad so the kernel and Recovery skip it from now
	 * on. Success requires at least one good copy. */
	{
		int i;
		int ok = 0;
		size_t page_size = 2048; /* SPI NAND page size */
		uint8_t page_buf[page_size];
		memset(page_buf, 0xFF, page_size);
		memcpy(page_buf, &flag, sizeof(flag));

		for (i = 0; i < 4; i++) {
			loff_t offset = i * 0x20000; /* 128KB stride */
			erase_info_t erase1;

			if (ioctl(fd, MEMGETBADBLOCK, &offset) > 0) {
				printf("WARNING: %s copy %d: bad block, skip\n",
				       UFLAG_PART_NAME, i);
				continue;
			}

			erase1.start = offset;
			erase1.length = mtd_info.erasesize;
			if (ioctl(fd, MEMERASE, &erase1) < 0) {
				printf("WARNING: %s copy %d: erase failed, mark bad, skip\n",
				       UFLAG_PART_NAME, i);
				ioctl(fd, MEMSETBADBLOCK, &offset);
				continue;
			}

			if (lseek(fd, offset, SEEK_SET) < 0 ||
			    write(fd, page_buf, page_size) != (ssize_t)page_size) {
				printf("WARNING: %s copy %d: write failed, mark bad, skip\n",
				       UFLAG_PART_NAME, i);
				ioctl(fd, MEMSETBADBLOCK, &offset);
				continue;
			}

			if (lseek(fd, offset, SEEK_SET) < 0 ||
			    read(fd, &readback, sizeof(readback)) != (ssize_t)sizeof(readback) ||
			    memcmp(&flag, &readback, sizeof(flag)) != 0) {
				printf("WARNING: %s copy %d: readback failed, mark bad, skip\n",
				       UFLAG_PART_NAME, i);
				ioctl(fd, MEMSETBADBLOCK, &offset);
				continue;
			}
			ok++;
		}

		if (ok == 0) {
			printf("ERROR: %s: all copies failed\n", UFLAG_PART_NAME);
			goto out_close;
		}
		if (ok < 4)
			printf("WARNING: %s: only %d/4 copies written\n",
			       UFLAG_PART_NAME, ok);
		else
			printf("  %s 4 copies written and verified OK\n", UFLAG_PART_NAME);
	}
	ret = 0;

out_close:
	close(fd);
	return ret;
}

int main(int argc, char *argv[])
{
	const char *pkg_path;
	struct ota_pkg_header hdr;
	struct ota_pkg_component table[OTA_PKG_MAX_COMPS];
	uint32_t pkg_crc = 0;
	uint32_t comp_mask = 0;
	uint32_t ver_new = 0;
	uint32_t ver_old = 0;
	char upgrade_mtd_path[32];
	int upgrade_mtd_num = -1;
	int ret = 1;

	printf("============================================\n");
	printf(" OTA Prepare (SD3589C NAND + UBI)\n");
	printf("============================================\n");

	/* defense in depth: when launched by usb_receiver the child resets
	 * SIGCHLD before exec, but make sure system()/waitpid semantics are
	 * normal no matter how we were started */
	signal(SIGCHLD, SIG_DFL);

	if (argc < 2) {
		pkg_path = "/tmp/upgrade/upgrade.img";
		printf("No argument, using default: %s\n", pkg_path);
	} else {
		pkg_path = argv[1];
	}

	/* from here ota_prepare owns the status file (receiver only reads it).
	 * fill the version first so even failures during header parsing
	 * report the real current version instead of 0 */
	if (read_system_version(&ver_old) == 0)
		g_status_version = ver_old;
	status_update(OTA_STAGE_VERIFYING, 30, OTA_ERR_NONE, "verifying package");

	/* Read and validate package header */
	if (read_package_header(pkg_path, &hdr) != 0)
		OTA_FAIL(OTA_ERR_PACKAGE_INVALID, "read package header failed");

	if (hdr.magic != OTA_PKG_MAGIC) {
		printf("ERROR: Invalid magic: 0x%08x (expected 0x%08x)\n",
		       hdr.magic, OTA_PKG_MAGIC);
		OTA_FAIL(OTA_ERR_PACKAGE_INVALID, "invalid package magic");
	}
	printf("  Magic: 0x%08x OK\n", hdr.magic);

	if (hdr.version != OTA_PKG_VERSION) {
		printf("ERROR: Unsupported package version: %u\n", hdr.version);
		OTA_FAIL(OTA_ERR_PACKAGE_INVALID, "unsupported package version");
	}
	printf("  Version: %u OK\n", hdr.version);

	if (hdr.component_cnt == 0 || hdr.component_cnt > OTA_PKG_MAX_COMPS) {
		printf("ERROR: Invalid component count: %u\n", hdr.component_cnt);
		OTA_FAIL(OTA_ERR_PACKAGE_INVALID, "invalid component count");
	}

	/* total_size must equal the real file size and cover at least
	 * header + component table + signature slot */
	{
		struct stat st;

		if (stat(pkg_path, &st) != 0) {
			printf("ERROR: stat %s failed: %s\n", pkg_path, strerror(errno));
			OTA_FAIL(OTA_ERR_PACKAGE_INVALID, "stat package failed");
		}
		if ((uint64_t)st.st_size != hdr.total_size) {
			printf("ERROR: total_size mismatch (header=%u, file=%lld)\n",
			       hdr.total_size, (long long)st.st_size);
			OTA_FAIL(OTA_ERR_PACKAGE_INVALID, "package total_size mismatch");
		}
		if (hdr.total_size < OTA_PKG_HEADER_SIZE +
		    hdr.component_cnt * OTA_PKG_COMP_SIZE + OTA_PKG_SIG_SIZE) {
			printf("ERROR: total_size %u too small for %u components\n",
			       hdr.total_size, hdr.component_cnt);
			OTA_FAIL(OTA_ERR_PACKAGE_INVALID, "package truncated");
		}
	}

	/* Version anti-rollback */
	ver_new = hdr.fw_version;
	if (read_system_version(&ver_old) != 0) {
		printf("ERROR: cannot determine current system version, refuse upgrade\n");
		OTA_FAIL(OTA_ERR_VERSION_ROLLBACK, "cannot read current version");
	}
	g_status_version = ver_old;
	if (ver_new < ver_old) {
		printf("ERROR: Version rollback detected (new=0x%08x, old=0x%08x)\n",
		       ver_new, ver_old);
		OTA_FAIL(OTA_ERR_VERSION_ROLLBACK, "version rollback rejected");
	}
	printf("  Version check passed: new=0x%08x >= old=0x%08x\n",
	       ver_new, ver_old);

	if (read_component_table(pkg_path, hdr.component_cnt, table) != 0)
		OTA_FAIL(OTA_ERR_PACKAGE_INVALID, "read component table failed");

	for (uint32_t i = 0; i < hdr.component_cnt; i++) {
		struct ota_pkg_component *c = &table[i];
		uint32_t bit = comp_bit_by_name((const char *)c->name);
		char name_buf[17];

		memcpy(name_buf, c->name, 16);
		name_buf[16] = '\0';

		if (bit == 0) {
			printf("ERROR: Unknown component '%s'\n", name_buf);
			OTA_FAIL(OTA_ERR_PACKAGE_INVALID, "unknown component");
		}
		comp_mask |= bit;

		if (c->media != OTA_MEDIA_NAND) {
			printf("ERROR: Unsupported media type %u for '%s'\n",
			       c->media, name_buf);
			OTA_FAIL(OTA_ERR_PACKAGE_INVALID, "unsupported component media");
		}

		if (check_component_bounds(c, table, hdr.component_cnt,
					   hdr.total_size, name_buf) != 0) {
			OTA_FAIL(OTA_ERR_PACKAGE_INVALID, "component boundary invalid");
		}
	}
	printf("  Components: %u, mask=0x%x\n", hdr.component_cnt, comp_mask);

	/* Package CRC */
	printf("INFO: Calculating package CRC...\n");
	if (calc_package_crc(pkg_path, hdr.total_size, &pkg_crc) != 0)
		OTA_FAIL(OTA_ERR_HASH_FAIL, "package crc calc failed");
	if (pkg_crc != hdr.package_crc) {
		printf("ERROR: CRC mismatch (header=0x%08x, calc=0x%08x)\n",
		       hdr.package_crc, pkg_crc);
		OTA_FAIL(OTA_ERR_HASH_FAIL, "package crc mismatch");
	}
	printf("  Package CRC: 0x%08x OK\n", pkg_crc);

	/* ECDSA signature verification */
	printf("INFO: Verifying ECDSA signature...\n");
	if (verify_ecdsa_signature(pkg_path, hdr.total_size) != 0) {
		printf("ERROR: ECDSA signature verification failed\n");
		OTA_FAIL(OTA_ERR_SIGNATURE_FAIL, "signature verify failed");
	}

	/* verification done, entering UBI write phase */
	status_update(OTA_STAGE_FLASHING, 50, OTA_ERR_NONE, "writing UBI volume");

	/* Find upgrade MTD partition */
	if (find_mtd_by_name("upgrade", upgrade_mtd_path, sizeof(upgrade_mtd_path)) != 0) {
		printf("ERROR: Cannot find upgrade partition\n");
		OTA_FAIL(OTA_ERR_FLASH_WRITE_FAIL, "upgrade partition not found");
	}
	if (sscanf(upgrade_mtd_path, "/dev/mtd%d", &upgrade_mtd_num) != 1) {
		printf("ERROR: Cannot parse MTD number from %s\n", upgrade_mtd_path);
		OTA_FAIL(OTA_ERR_FLASH_WRITE_FAIL, "parse upgrade mtd failed");
	}
	printf("INFO: Found upgrade partition: %s (mtd%d)\n",
	       upgrade_mtd_path, upgrade_mtd_num);

	/*
	 * Correct order: try ubiattach FIRST; only if attach fails does the
	 * partition lack a valid UBI layout and need ubiformat.
	 * ("not attached" != "no valid UBI" — a detached-but-valid partition
	 * must not be reformatted: wear + destroys contents)
	 */
	if (access("/dev/ubi2", F_OK) != 0) {
		char cmd[128];
		snprintf(cmd, sizeof(cmd), "/opt/bin/ubiattach -m %d -d 2 %s",
			 upgrade_mtd_num, UBI_CTRL_DEV);
		printf("INFO: trying ubiattach first: %s\n", cmd);
		system(cmd); /* result checked via /dev/ubi2 below */
	}

	if (access("/dev/ubi2", F_OK) != 0) {
		char cmd[128];
		printf("INFO: ubiattach failed, formatting upgrade partition...\n");
		snprintf(cmd, sizeof(cmd), "/opt/bin/ubiformat %s -y", upgrade_mtd_path);
		if (system(cmd) != 0) {
			printf("ERROR: ubiformat failed\n");
			OTA_FAIL(OTA_ERR_FLASH_WRITE_FAIL, "ubiformat failed");
		}
	}

	/* Attach UBI */
	if (ubi_attach(upgrade_mtd_num) != 0) {
		printf("ERROR: ubiattach failed\n");
		OTA_FAIL(OTA_ERR_FLASH_WRITE_FAIL, "ubiattach failed");
	}

	/* Check if volume exists and has correct size */
	if (access("/dev/ubi2_0", F_OK) == 0) {
		char vol_name[64];

		/* Verify the volume name before writing, never overwrite a
		 * foreign volume */
		if (read_sys_file("/sys/class/ubi/ubi2_0/name", vol_name,
				  sizeof(vol_name)) != 0 ||
		    strcmp(vol_name, UBI_VOL_NAME) != 0) {
			printf("ERROR: /dev/ubi2_0 is not volume '%s', refuse\n",
			       UBI_VOL_NAME);
			ubi_detach();
			OTA_FAIL(OTA_ERR_FLASH_WRITE_FAIL, "ubi volume name mismatch");
		}
		/* Volume exists, check size */
		FILE *fp = fopen("/sys/class/ubi/ubi2_0/data_bytes", "r");
		if (fp) {
			uint32_t vol_size;
			if (fscanf(fp, "%u", &vol_size) == 1) {
				if (vol_size < hdr.total_size) {
					printf("INFO: Existing volume too small (%u < %u), recreating...\n",
					       vol_size, hdr.total_size);
					fclose(fp);
					if (system("/opt/bin/ubirmvol /dev/ubi2 -N upgrade") != 0) {
						printf("ERROR: ubirmvol failed\n");
						ubi_detach();
						OTA_FAIL(OTA_ERR_FLASH_WRITE_FAIL, "ubirmvol failed");
					}
					if (ubi_create_volume(hdr.total_size) != 0) {
						printf("ERROR: ubimkvol failed\n");
						ubi_detach();
						OTA_FAIL(OTA_ERR_FLASH_WRITE_FAIL, "ubimkvol failed");
					}
				} else {
					fclose(fp);
				}
			} else {
				fclose(fp);
			}
		}
	} else {
		/* Volume does not exist, create it */
		printf("INFO: Creating static volume '%s' (%u bytes)...\n",
		       UBI_VOL_NAME, hdr.total_size);
		if (ubi_create_volume(hdr.total_size) != 0) {
			printf("ERROR: ubimkvol failed\n");
			ubi_detach();
			OTA_FAIL(OTA_ERR_FLASH_WRITE_FAIL, "ubimkvol failed");
		}
	}

	/* Write package to UBI volume */
	printf("INFO: Writing upgrade package to UBI volume...\n");
	if (ubi_write_volume(pkg_path, hdr.total_size) != 0) {
		printf("ERROR: ubiupdatevol failed\n");
		ubi_detach();
		OTA_FAIL(OTA_ERR_FLASH_WRITE_FAIL, "ubi volume write failed");
	}

	/* Detach UBI */
	ubi_detach();

	status_update(OTA_STAGE_FLASHING, 90, OTA_ERR_NONE, "package written, writing uflag");

	/* Write uflag */
	printf("INFO: Writing uflag...\n");
	if (write_uflag(hdr.total_size, pkg_crc, comp_mask, ver_new, ver_old) != 0) {
		printf("ERROR: Failed to write uflag\n");
		OTA_FAIL(OTA_ERR_FLASH_WRITE_FAIL, "write uflag failed");
	}

	printf("\n========================================\n");
	printf(" OTA preparation COMPLETE\n");
	printf("========================================\n");
	printf("  Upgrade package written to UBI 'upgrade' volume\n");
	printf("  uflag set to PENDING, component mask=0x%x\n", comp_mask);
	printf("  Next boot will enter Recovery mode for NAND upgrade.\n");
	printf("  Rebooting in 3 seconds...\n");

	status_update(OTA_STAGE_SWITCHING, 95, OTA_ERR_NONE, "rebooting to recovery");

	/* Auto reboot after 3 seconds */
	sync();
	sleep(3);
	if (reboot(RB_AUTOBOOT) != 0) {
		/* uflag is already PENDING: any later manual reboot still enters
		 * recovery, so do not mark the upgrade FAILED */
		printf("ERROR: reboot failed: %s\n", strerror(errno));
		status_update(OTA_STAGE_SWITCHING, 95, OTA_ERR_NONE,
			      "auto reboot failed, reboot manually");
		return 1;
	}

	ret = 0;
	return ret;
}
