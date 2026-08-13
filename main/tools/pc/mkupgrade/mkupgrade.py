#!/usr/bin/env python3
"""
Upgrade package creation tool for SD3589C NAND + UBI OTA

Package layout:
    [0:512)                 Header (512 bytes)
    [512:512+64*N)          Component table (64 bytes each)
    [...:total-256)         Data area (NAND components)
    [total-256:total)       Signature slot (256 bytes, 64B raw ECDSA + 0xFF pad)

NAND layout (128MB):
    otfl     0x00000000   1536KB (Recovery U-Boot)
    uflag    0x00180000    512KB (upgrade flag)
    uboot    0x00200000      1MB (Normal U-Boot)
    bl31     0x00300000    512KB (ATF BL31)
    kernel   0x00380000      8MB (uImage-fdt)
    rootfs   0x00B80000      8MB (ramdisk)
    upgrade  0x01380000     32MB (UBI static volume for OTAU pkg)
    userfs   0x03380000     32MB (UBIFS)
    data     0x05380000   ~44.5MB (UBIFS)

Usage example:
    python3 mkupgrade.py -o upgrade.img --sign ota_sign_private.pem \\
        --uboot uboot_spinand.img --bl31 bl31.img \\
        --kernel uImage-fdt.img --rootfs rootfs.ramdisk.img \\
        --fw-version 0x01000200
"""

import sys
import struct
import zlib
import argparse
import os

from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.backends import default_backend

# Must match U-Boot nand_ota_upgrade.c and ota_prepare.c
OTA_PKG_MAGIC = 0x4F544155          # "OTAU"
OTA_PKG_VERSION = 1
OTA_PKG_HEADER_SIZE = 512
OTA_PKG_COMP_SIZE = 64
OTA_PKG_SIG_SIZE = 256
OTA_PKG_SIG_RAW_LEN = 64

OTA_MEDIA_NAND = 0
OTA_FS_RAW = 0

# NAND partition offsets (must match nand.xml)
PART_OFFSET_BL31 = 0x00300000
PART_SIZE_BL31 = 0x00080000   # 512KB
PART_OFFSET_KERNEL = 0x00380000
PART_SIZE_KERNEL = 0x00800000  # 8MB
PART_OFFSET_ROOTFS = 0x00B80000
PART_SIZE_ROOTFS = 0x00800000  # 8MB
PART_OFFSET_UBOOT = 0x00200000
PART_SIZE_UBOOT = 0x00100000   # 1MB


def calc_crc32(data: bytes) -> int:
    return zlib.crc32(data) & 0xFFFFFFFF


def sign_with_ecdsa(data: bytes, private_key_path: str) -> bytes:
    with open(private_key_path, "rb") as f:
        key_data = f.read()

    try:
        private_key = serialization.load_pem_private_key(
            key_data, password=None, backend=default_backend())
    except ValueError:
        private_key = serialization.load_der_private_key(
            key_data, password=None, backend=default_backend())

    signature = private_key.sign(data, ec.ECDSA(hashes.SHA256()))

    from cryptography.hazmat.primitives.asymmetric.utils import decode_dss_signature
    r, s = decode_dss_signature(signature)
    signature = r.to_bytes(32, byteorder='big') + s.to_bytes(32, byteorder='big')
    if len(signature) != OTA_PKG_SIG_RAW_LEN:
        raise ValueError(f"Signature size {len(signature)} != {OTA_PKG_SIG_RAW_LEN}")
    return signature


def verify_ecdsa_signature(data: bytes, signature: bytes, public_key_path: str) -> bool:
    with open(public_key_path, "rb") as f:
        key_data = f.read()
    try:
        public_key = serialization.load_pem_public_key(
            key_data, backend=default_backend())
    except ValueError:
        public_key = serialization.load_der_public_key(
            key_data, backend=default_backend())

    from cryptography.hazmat.primitives.asymmetric.utils import encode_dss_signature
    r = int.from_bytes(signature[:32], byteorder='big')
    s = int.from_bytes(signature[32:], byteorder='big')
    der_signature = encode_dss_signature(r, s)

    try:
        public_key.verify(der_signature, data, ec.ECDSA(hashes.SHA256()))
        return True
    except Exception:
        return False


def generate_ecdsa_keypair(output_dir: str = ".") -> tuple:
    private_key = ec.generate_private_key(ec.SECP256R1(), backend=default_backend())
    private_path = os.path.join(output_dir, "ota_sign_private.pem")
    public_pem_path = os.path.join(output_dir, "ota_sign_public.pem")
    public_bin_path = os.path.join(output_dir, "ota_pubkey.bin")

    with open(private_path, "wb") as f:
        f.write(private_key.private_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PrivateFormat.PKCS8,
            encryption_algorithm=serialization.NoEncryption()))

    public_key = private_key.public_key()
    with open(public_pem_path, "wb") as f:
        f.write(public_key.public_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PublicFormat.SubjectPublicKeyInfo))

    public_numbers = public_key.public_numbers()
    with open(public_bin_path, "wb") as f:
        f.write(public_numbers.x.to_bytes(32, byteorder='big') +
                public_numbers.y.to_bytes(32, byteorder='big'))

    print(f"Generated ECDSA-256 key pair (secp256r1):")
    print(f"  Private key (PEM): {private_path}")
    print(f"  Public key (PEM):  {public_pem_path}")
    print(f"  Public key (BIN):  {public_bin_path}")
    return private_path, public_pem_path


def convert_pem_to_bin_ecdsa(pem_path: str, bin_path: str) -> bool:
    try:
        with open(pem_path, "rb") as f:
            key_data = f.read()
        public_key = serialization.load_pem_public_key(
            key_data, backend=default_backend())
        if not isinstance(public_key, ec.EllipticCurvePublicKey):
            print("Error: Not an ECDSA public key")
            return False
        public_numbers = public_key.public_numbers()
        with open(bin_path, "wb") as f:
            f.write(public_numbers.x.to_bytes(32, byteorder='big') +
                    public_numbers.y.to_bytes(32, byteorder='big'))
        print(f"Converted {pem_path} to {bin_path}")
        return True
    except Exception as e:
        print(f"Error converting key: {e}")
        return False


def pad_to_align(data: bytes, align: int = 512) -> bytes:
    padding = (align - (len(data) % align)) % align
    return data + b"\xFF" * padding


def component_media_fs(name: str) -> tuple:
    """Return default (media, fs_type, flash_offset, flash_size) for known components."""
    if name == "bl31":
        return OTA_MEDIA_NAND, OTA_FS_RAW, PART_OFFSET_BL31, PART_SIZE_BL31
    if name == "kernel":
        return OTA_MEDIA_NAND, OTA_FS_RAW, PART_OFFSET_KERNEL, PART_SIZE_KERNEL
    if name == "rootfs":
        return OTA_MEDIA_NAND, OTA_FS_RAW, PART_OFFSET_ROOTFS, PART_SIZE_ROOTFS
    if name == "uboot":
        return OTA_MEDIA_NAND, OTA_FS_RAW, PART_OFFSET_UBOOT, PART_SIZE_UBOOT
    raise ValueError(f"Unknown component name: {name}")


def build_component(name: str, data: bytes,
                    flash_offset: int = None,
                    flash_size: int = None,
                    media: int = None,
                    fs_type: int = None) -> dict:
    """Build a component descriptor."""
    def_media, def_fs, def_off, def_size = component_media_fs(name)
    if media is None:
        media = def_media
    if fs_type is None:
        fs_type = def_fs
    if flash_offset is None:
        flash_offset = def_off
    if flash_size is None:
        flash_size = def_size

    if len(data) > flash_size:
        raise ValueError(
            f"Component {name}: data size {len(data)} > flash_size {flash_size}")

    return {
        "name": name,
        "flash_offset": flash_offset,
        "flash_size": flash_size,
        "data": data,
        "media": media,
        "fs_type": fs_type,
    }


def encode_component_descriptor(c: dict, data_offset: int) -> bytes:
    name_bytes = c["name"].encode("utf-8").ljust(16, b"\x00")[:16]
    desc = name_bytes
    desc += struct.pack("<IIIII",
                        c["flash_offset"],
                        c["flash_size"],
                        data_offset,
                        len(c["data"]),
                        calc_crc32(c["data"]))
    desc += struct.pack("<BB", c["media"], c["fs_type"])
    desc += b"\x00" * 26
    if len(desc) != OTA_PKG_COMP_SIZE:
        raise ValueError(
            f"Component descriptor size {len(desc)} != {OTA_PKG_COMP_SIZE}")
    return desc


def build_upgrade_package(components: list, sign_key: str = None,
                          fw_version: int = 0) -> bytes:
    """
    Build upgrade.img:
        Header + ComponentTable + DataArea + SignatureSlot
    """
    # Sort components: bl31 -> kernel -> rootfs -> uboot
    comp_order = {"bl31": 0, "kernel": 1, "rootfs": 2, "uboot": 3}
    ordered = sorted(components, key=lambda c: comp_order.get(c["name"], 99))

    comp_table_offset = OTA_PKG_HEADER_SIZE
    data_area_offset = comp_table_offset + len(ordered) * OTA_PKG_COMP_SIZE

    comp_table = b""
    data_area = b""
    current_offset = data_area_offset

    for c in ordered:
        desc = encode_component_descriptor(c, current_offset)
        comp_table += desc
        padded = pad_to_align(c["data"], 512)
        data_area += padded
        current_offset += len(padded)

    # Total size includes one signature slot at the end
    total_size = current_offset + OTA_PKG_SIG_SIZE

    # Header
    header = struct.pack("<IIIIIII",
                         OTA_PKG_MAGIC,
                         OTA_PKG_VERSION,
                         total_size,
                         len(ordered),
                         0,              # package_crc placeholder
                         fw_version,
                         0)              # reserved
    header = header.ljust(OTA_PKG_HEADER_SIZE, b"\x00")

    # CRC covers the full header with package_crc zeroed + component_table + data_area
    crc_data = header + comp_table + data_area
    package_crc = calc_crc32(crc_data)
    header = header[:16] + struct.pack("<I", package_crc) + header[20:]

    # Signed payload includes the full header + component table + data area
    signed_data = header + comp_table + data_area

    # Signature
    if sign_key:
        if not os.path.exists(sign_key):
            raise FileNotFoundError(f"Signing key not found: {sign_key}")
        sig_raw = sign_with_ecdsa(signed_data, sign_key)
        print(f"Signed with {sign_key} (ECDSA-256 secp256r1 + SHA256)")
    else:
        sig_raw = b"\xFF" * OTA_PKG_SIG_RAW_LEN

    sig_slot = sig_raw + b"\xFF" * (OTA_PKG_SIG_SIZE - len(sig_raw))

    package = header + comp_table + data_area + sig_slot
    return package


def verify_package(package: bytes, public_key_path: str = None) -> bool:
    if len(package) < OTA_PKG_HEADER_SIZE + OTA_PKG_SIG_SIZE:
        print("Package too small")
        return False

    magic, version, total_size, comp_cnt, stored_crc, fw_version, reserved = \
        struct.unpack("<IIIIIII", package[:28])

    if magic != OTA_PKG_MAGIC:
        print(f"Invalid magic: 0x{magic:08x}")
        return False
    if version != OTA_PKG_VERSION:
        print(f"Version mismatch: {version}")
        return False
    if total_size != len(package):
        print(f"Size mismatch: header={total_size}, actual={len(package)}")
        return False

    signed_len = total_size - OTA_PKG_SIG_SIZE
    # CRC covers the full header with package_crc zeroed + component table + data area
    crc_data = bytearray(package[:signed_len])
    crc_data[16:20] = b"\x00\x00\x00\x00"
    calc_crc = calc_crc32(bytes(crc_data))
    if calc_crc != stored_crc:
        print(f"CRC mismatch: stored=0x{stored_crc:08x}, calc=0x{calc_crc:08x}")
        return False

    print("Package verified OK")
    print(f"  Magic: 0x{magic:08x}")
    print(f"  Version: {version}")
    print(f"  Total size: {total_size} bytes")
    print(f"  Components: {comp_cnt}")
    print(f"  CRC32: 0x{stored_crc:08x}")
    print(f"  fw_version: 0x{fw_version:08x}")

    sig = package[total_size - OTA_PKG_SIG_SIZE:total_size]

    if public_key_path:
        signed_data = package[:signed_len]
        ok = verify_ecdsa_signature(signed_data, sig[:OTA_PKG_SIG_RAW_LEN],
                                    public_key_path)
        print(f"  Signature: {'VALID' if ok else 'INVALID'}")
        if not ok:
            return False

    comp_table_start = OTA_PKG_HEADER_SIZE
    for i in range(comp_cnt):
        off = comp_table_start + i * OTA_PKG_COMP_SIZE
        comp = package[off:off + OTA_PKG_COMP_SIZE]
        name = comp[:16].split(b"\x00")[0].decode("utf-8")
        flash_offset, flash_size, data_offset, data_size, data_crc = \
            struct.unpack("<IIIII", comp[16:36])
        media, fs_type = struct.unpack("<BB", comp[36:38])
        calc_data_crc = calc_crc32(package[data_offset:data_offset + data_size])
        crc_ok = "OK" if calc_data_crc == data_crc else "FAIL"
        print(f"  Component[{i}]: {name}")
        print(f"    Media: {media}, fs_type: {fs_type}")
        print(f"    Flash: 0x{flash_offset:08x} (size 0x{flash_size:08x})")
        print(f"    Data:  offset={data_offset}, size={data_size}")
        print(f"    CRC:   0x{data_crc:08x} ({crc_ok})")

    return True


def add_component_arg(parser, name: str, help_text: str):
    parser.add_argument(f"--{name}", help=help_text)
    parser.add_argument(f"--{name}-offset", type=lambda x: int(x, 16), default=None)
    parser.add_argument(f"--{name}-size", type=lambda x: int(x, 16), default=None)
    parser.add_argument(f"--{name}-media", type=lambda x: int(x, 16), default=None)
    parser.add_argument(f"--{name}-fs", type=lambda x: int(x, 16), default=None)


def main():
    parser = argparse.ArgumentParser(
        description="Create SD3589C NAND + UBI OTA upgrade package",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python3 mkupgrade.py -o upgrade.img --sign ota_sign_private.pem \\
      --uboot uboot_spinand.img --bl31 bl31.img \\
      --kernel uImage-fdt.img --rootfs rootfs.ramdisk.img \\
      --fw-version 0x01000200

  python3 mkupgrade.py -v upgrade.img --verify-sign ota_sign_public.pem

  python3 mkupgrade.py --gen-key
        """,
    )

    parser.add_argument("-o", "--output", default="upgrade.img",
                        help="Output file path (default: upgrade.img)")
    parser.add_argument("-v", "--verify", help="Verify an existing package")
    parser.add_argument("--sign", help="ECDSA private key PEM for signing")
    parser.add_argument("--verify-sign", help="ECDSA public key PEM for verification")
    parser.add_argument("--gen-key", action="store_true",
                        help="Generate test ECDSA-256 key pair")
    parser.add_argument("--pem-to-bin", nargs=2, metavar=('PEM_FILE', 'BIN_FILE'),
                        help="Convert PEM public key to binary format")
    parser.add_argument("--fw-version", type=lambda x: int(x, 16),
                        help="Firmware version for anti-rollback (hex, e.g. 0x01000200; required when building a package)")

    add_component_arg(parser, "uboot", "U-Boot image file (uboot_spinand.img)")
    add_component_arg(parser, "bl31", "BL31 image file (bl31.img)")
    add_component_arg(parser, "kernel", "Kernel uImage file (uImage-fdt.img)")
    add_component_arg(parser, "rootfs", "Rootfs ramdisk file (rootfs.ramdisk.img)")

    args = parser.parse_args()

    if args.gen_key:
        generate_ecdsa_keypair()
        sys.exit(0)

    if args.pem_to_bin:
        ok = convert_pem_to_bin_ecdsa(args.pem_to_bin[0], args.pem_to_bin[1])
        sys.exit(0 if ok else 1)

    if args.verify:
        with open(args.verify, "rb") as f:
            package = f.read()
        ok = verify_package(package, args.verify_sign)
        sys.exit(0 if ok else 1)

    components = []
    comp_names = ["bl31", "kernel", "rootfs", "uboot"]
    for name in comp_names:
        path = getattr(args, name)
        if not path:
            continue
        if not os.path.exists(path):
            parser.error(f"{name} file not found: {path}")
        with open(path, "rb") as f:
            data = f.read()
        comp = build_component(
            name, data,
            flash_offset=getattr(args, f"{name}_offset"),
            flash_size=getattr(args, f"{name}_size"),
            media=getattr(args, f"{name}_media"),
            fs_type=getattr(args, f"{name}_fs"),
        )
        components.append(comp)
        print(f"{name}: {len(data)} bytes")

    if not components:
        parser.error("At least one component is required")

    if args.fw_version is None:
        parser.error("--fw-version is required when building a package")

    print(f"\nBuilding upgrade package with {len(components)} components...")
    print(f"  fw_version: 0x{args.fw_version:08x}")
    package = build_upgrade_package(components, args.sign, args.fw_version)

    with open(args.output, "wb") as f:
        f.write(package)

    print(f"\nGenerated {args.output} ({len(package)} bytes)")

    print("\nVerifying package...")
    if not verify_package(package, args.verify_sign):
        print("ERROR: package self-verification failed", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
