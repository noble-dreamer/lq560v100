#!/usr/bin/env python3
"""
Generate uflag binary for SD3589C NAND OTA upgrade.

Usage: python3 upgrade_flag_tool.py <output_file>

Generates a 512KB uflag partition with 4 redundant upgrade_flag in IDLE state.
NAND scheme: 4-page redundancy (128KB stride).
"""
import sys
import struct
import zlib

UPGRADE_FLAG_MAGIC = 0x55504752      # "UPGR"
UPGRADE_FLAG_VERSION = 1
UPGRADE_STATE_IDLE = 0

UFLAG_SIZE = 512 * 1024              # 512KB total
REDUNDANT_NUM = 4
REDUNDANT_STRIDE = 128 * 1024        # 128KB stride


def calc_crc32(data):
    return zlib.crc32(data) & 0xFFFFFFFF


def make_upgrade_flag():
    """Build upgrade_flag structure (64 bytes) for IDLE state"""
    flag_body = struct.pack('<IIIIIIIIII',
        UPGRADE_FLAG_MAGIC,      # magic
        UPGRADE_FLAG_VERSION,    # version
        UPGRADE_STATE_IDLE,      # state = IDLE
        0,                       # upgrade_source
        0,                       # upgrade_size
        0,                       # upgrade_crc
        0,                       # component_mask
        3,                       # retry_count
        0,                       # version_new
        0,                       # version_old
    )
    flag_body += struct.pack('<I', 0)   # completed_mask
    flag_body += b'\x00' * 16           # reserved[16]
    crc = calc_crc32(flag_body)
    flag_body += struct.pack('<I', crc) # crc32
    return flag_body


def main():
    if len(sys.argv) != 2:
        print("Usage: python3 upgrade_flag_tool.py <output_file>")
        print("")
        print("Generates NAND uflag binary with 4 redundant upgrade_flag in IDLE state.")
        print("4-page redundancy: 512KB total, 128KB stride.")
        sys.exit(1)

    output_file = sys.argv[1]

    # Generate upgrade_flag
    flag = make_upgrade_flag()
    assert len(flag) == 64, f"upgrade_flag size mismatch: {len(flag)}"

    # Generate uflag partition image (512KB, erased state 0xFF)
    uflag_image = bytearray([0xFF] * UFLAG_SIZE)

    # Write 4 redundant copies at 128KB stride
    for i in range(REDUNDANT_NUM):
        offset = i * REDUNDANT_STRIDE
        uflag_image[offset:offset+len(flag)] = flag

    with open(output_file, 'wb') as f:
        f.write(uflag_image)

    print(f"Generated {output_file} ({len(uflag_image)} bytes)")
    print(f"  upgrade_flag: magic=0x{UPGRADE_FLAG_MAGIC:08x}, state=IDLE(0)")
    print(f"  4-page redundancy: {REDUNDANT_NUM} copies @ 128KB stride")


if __name__ == '__main__':
    main()
