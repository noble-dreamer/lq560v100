#!/usr/bin/env python3
"""
Headless LZ4 verification script.
Connects to stereo_app on the board, receives LZ4-compressed disparity frames,
decompresses them with Python lz4, and checks for XOR 0x80 corruption patterns.

Usage: python3 verify_lz4_fix.py [--host 192.168.1.101] [--port 9000] [--frames 5]
"""

import socket
import struct
import sys
import os
import time
import argparse
import numpy as np

try:
    import lz4.block
except ImportError:
    print("ERROR: pip install lz4")
    sys.exit(1)

# Protocol constants (must match stereo_receiver.py)
MAGIC        = 0x53544D50
HEADER_FMT   = '>IBIQI3s'
HEADER_SIZE  = struct.calcsize(HEADER_FMT)

DISPARITY_W = 320
DISPARITY_H = 208
DISPARITY_RAW_SIZE = DISPARITY_W * DISPARITY_H * 2  # 133120 bytes

OUT_DIR = './lz4_verify_output'


def recv_exact(sock, n):
    """Receive exactly n bytes or return None on disconnect."""
    buf = bytearray()
    while len(buf) < n:
        chunk = sock.recv(min(n - len(buf), 65536))
        if not chunk:
            return None
        buf.extend(chunk)
    return bytes(buf)


def main():
    parser = argparse.ArgumentParser(description='Verify LZ4 fix — headless')
    parser.add_argument('--host', type=str, default='192.168.1.101')
    parser.add_argument('--port', type=int, default=9000)
    parser.add_argument('--frames', type=int, default=5,
                        help='Number of disparity frames to capture')
    args = parser.parse_args()

    os.makedirs(OUT_DIR, exist_ok=True)

    print(f"Connecting to {args.host}:{args.port} ...")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(10)
    sock.connect((args.host, args.port))
    print("Connected!")

    disp_count = 0
    frame_count = 0
    start_time = time.time()

    while disp_count < args.frames:
        # Read header
        header_raw = recv_exact(sock, HEADER_SIZE)
        if header_raw is None:
            print("Disconnected.")
            break

        try:
            magic, ftype, frm_idx, ts_us, payload_size, _ = \
                struct.unpack(HEADER_FMT, header_raw)
        except struct.error:
            print("Header unpack error")
            continue

        if magic != MAGIC:
            print(f"Bad magic: 0x{magic:08X}")
            continue

        if payload_size == 0 or payload_size > 10 * 1024 * 1024:
            print(f"Invalid payload size: {payload_size}")
            continue

        # Read payload
        payload = recv_exact(sock, payload_size)
        if payload is None:
            print("Disconnected during payload read.")
            break

        frame_count += 1

        # We only care about LZ4-compressed disparity (ftype=3)
        if ftype != 3:
            continue

        disp_count += 1

        # Decompress with Python lz4
        try:
            raw = lz4.block.decompress(payload,
                                       uncompressed_size=DISPARITY_RAW_SIZE)
        except Exception as e:
            print(f"[frame {disp_count}] LZ4 decompress FAILED: {e}")
            continue

        img = np.frombuffer(raw, dtype=np.uint16).reshape(
            (DISPARITY_H, DISPARITY_W))

        # Save compressed and decompressed data
        comp_path = os.path.join(OUT_DIR, f'disp_compressed_{disp_count}.bin')
        raw_path  = os.path.join(OUT_DIR, f'disp_decompressed_{disp_count}.npy')
        with open(comp_path, 'wb') as f:
            f.write(payload)
        np.save(raw_path, img)

        # Statistics
        comp_ratio = len(payload) / DISPARITY_RAW_SIZE * 100
        disp_values = img.astype(np.float64)
        # Q5 format: real disparity = value / 32
        real_disp = disp_values / 32.0

        print(f"[frame {disp_count}] idx={frm_idx} ts={ts_us} "
              f"compressed={len(payload)}B ({comp_ratio:.1f}%) "
              f"decompressed={len(raw)}B "
              f"disp range=[{img.min()}-{img.max()}] "
              f"real=[{real_disp.min():.1f}-{real_disp.max():.1f}]")

        # XOR 0x80 corruption check:
        # If the bug exists, some bytes will have bit 7 flipped.
        # In uint16 Q5 disparity, valid range is 0..128*32=4096 (0x1000).
        # Any value with high byte >= 0x80 is suspicious.
        high_byte = (img >> 8).astype(np.uint8)
        suspicious = np.sum(high_byte >= 0x80)
        total = img.size
        print(f"  XOR 0x80 check: {suspicious}/{total} pixels have high byte >= 0x80 "
              f"({'SUSPICIOUS' if suspicious > 0 else 'CLEAN'})")

        # Also check for sudden jumps that indicate corruption
        # (valid disparity should be locally smooth)
        diff_x = np.abs(np.diff(img.astype(np.int32), axis=1))
        diff_y = np.abs(np.diff(img.astype(np.int32), axis=0))
        max_jump_x = diff_x.max()
        max_jump_y = diff_y.max()
        big_jumps = np.sum(diff_x > 100) + np.sum(diff_y > 100)
        print(f"  Smoothness: max_jump_x={max_jump_x} max_jump_y={max_jump_y} "
              f"big_jumps(>100)={big_jumps}")

    sock.close()
    elapsed = time.time() - start_time
    print(f"\nDone: captured {disp_count} disparity frames in {elapsed:.1f}s "
          f"({frame_count} total packets)")
    print(f"Output saved to: {OUT_DIR}/")

    if disp_count > 0:
        print("\n=== VERIFICATION ===")
        # Load all captured frames and check consistency
        frames = []
        for i in range(1, disp_count + 1):
            f = np.load(os.path.join(OUT_DIR, f'disp_decompressed_{i}.npy'))
            frames.append(f)

        # Check inter-frame consistency (consecutive frames should be similar)
        if len(frames) >= 2:
            for i in range(1, len(frames)):
                diff = np.abs(frames[i].astype(np.float64) - frames[i-1].astype(np.float64))
                mean_diff = diff.mean()
                max_diff = diff.max()
                print(f"  Frame {i} vs {i+1}: mean_diff={mean_diff:.1f} max_diff={max_diff}")

        print("\nIf all frames show 'CLEAN' XOR 0x80 check and reasonable")
        print("smoothness, the LZ4 fix is verified correct.")


if __name__ == '__main__':
    main()
