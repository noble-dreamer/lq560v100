#!/usr/bin/env python3
"""
Stereo Matching Receiver (TCP client)
Connects to the SD3589CV100 board (192.168.1.101:9000) and displays
left image, right image, and disparity map in a structured window.

Protocol (big-endian):
  +0   magic (4B): 0x53544D50 ("STMP")
  +4   frame_type (1B): 0=left, 1=right, 2=disparity(raw), 3=disparity(LZ4)
  +5   frame_index (4B): uint32 monotonic counter
  +9   timestamp_us (8B): uint64 microseconds
  +17  payload_size (4B): uint32 data length
  +21  reserved (3B): 0x00
  +24  payload (variable):
       - type 0/1: JPEG encoded image
       - type 2: raw uint16 sub-pixel disparity (640x448, Q5 fixed-point)
       - type 3: LZ4-compressed uint16 disparity (decompresses to 573440 bytes)

Disparity format:
  uint16 Q5 fixed-point: integer part = value >> 5, fractional = (value & 0x1F) / 32
  Range: 0 ~ 128*32 = 4096 (128 disparity candidates)

Usage:
  python3 stereo_receiver.py [--host 192.168.1.101] [--port 9000] [--save]

Requires:
  pip install lz4   (for LZ4 disparity decompression)
"""

import socket
import struct
import sys
import os
import json
import argparse
import time
import datetime
import threading
import numpy as np
import cv2
from collections import deque

# LZ4 decompression for compressed disparity (pip install lz4)
try:
    import lz4.block
    HAS_LZ4 = True
except ImportError:
    HAS_LZ4 = False
    print("[receiver] WARNING: lz4 module not found (pip install lz4). "
          "LZ4-compressed disparity will fail.")

import ctypes

# Fast PNG encoder (libspng: no filter, zlib level=1) for uint16 disparity
# Built from rust/disp_png/ (Rust FFI to libspng, or C fallback via gcc)
_SO_PATHS = [
    os.path.join(os.path.dirname(os.path.abspath(__file__)),
                 '..', 'rust', 'disp_png', 'libdisp_png.so'),
    os.path.join(os.path.dirname(os.path.abspath(__file__)),
                 '..', 'rust', 'disp_png', 'target', 'release', 'libdisp_png.so'),
]
_disp_png_lib = None
for _p in _SO_PATHS:
    if os.path.isfile(_p):
        try:
            _disp_png_lib = ctypes.CDLL(_p)
            _disp_png_lib.save_disparity_png.restype = ctypes.c_int
            _disp_png_lib.save_disparity_png.argtypes = [
                ctypes.c_void_p,    # data pointer
                ctypes.c_size_t,    # len
                ctypes.c_uint32,    # width
                ctypes.c_uint32,    # height
                ctypes.c_char_p,    # path (null-terminated)
            ]
            print(f"[receiver] Fast PNG encoder loaded: {_p}")
            break
        except OSError as e:
            print(f"[receiver] Failed to load fast PNG lib {_p}: {e}")

_HAS_FAST_PNG = _disp_png_lib is not None

# ── Protocol constants ───────────────────────────────────────────────────────

MAGIC        = 0x53544D50
HEADER_FMT   = '>IBIQI3s'
HEADER_SIZE  = struct.calcsize(HEADER_FMT)

FRAME_TYPE_NAMES  = {0: 'left', 1: 'right', 2: 'disparity', 3: 'disparity',
                     4: 'raw_left', 5: 'raw_right', 6: 'calib', 7: 'perf',
                     8: 'model_int_disp', 9: 'model_cost_vol',
                     10: 'model_input_left', 11: 'model_input_right'}
FRAME_TYPE_LABELS = {0: 'Left', 1: 'Right', 2: 'Disparity', 3: 'Disparity (LZ4)',
                     4: 'Raw Left', 5: 'Raw Right', 6: 'Calibration', 7: 'Perf',
                     8: 'Model Int Disp', 9: 'Model Cost Vol',
                     10: 'Model Input L', 11: 'Model Input R'}

RAW_CAPTURE_PORT = 9001
MODEL_OUTPUT_CTRL_PORT = 9002
RAW_CMD_MAGIC = 0x53545243  # "STRC"
RAW_CMD_GRAB = 0x01
RAW_META_FMT = '>4sHHHHBBHII'
RAW_META_SIZE = struct.calcsize(RAW_META_FMT)
RAW_PIXEL_FORMATS = {0: 'nv21', 1: 'nv12'}

# NPU native output dimensions (before board-side 2x upsampling)
NPU_OUTPUT_W = 320
NPU_OUTPUT_H = 224

# Transmitted disparity dimensions (after board-side 2x nearest-neighbor upsampling)
DISPARITY_W = 640           # board-side upsampled disparity width
DISPARITY_H = 448           # board-side upsampled disparity height
DISPARITY_PIXELS = DISPARITY_W * DISPARITY_H       # 286720 pixels
DISPARITY_RAW_SIZE = DISPARITY_PIXELS * 2          # 573440 bytes (uint16)
DISPARITY_MAX_DISP = 128    # max disparity candidates (model: h448_w640_128_sub)
DISPARITY_Q5_SCALE = 32     # Q5 fractional scale
# Disparity pseudocolor: JET with min/max normalization (disp 0..max_disp → 0..255)

# Model output dimensions (raw NPU outputs before SubPixel/upsampling)
MODEL_INT_DISP_W = NPU_OUTPUT_W       # 320
MODEL_INT_DISP_H = NPU_OUTPUT_H       # 224
MODEL_INT_DISP_SIZE = MODEL_INT_DISP_W * MODEL_INT_DISP_H  # 76800 bytes (uint8)
MODEL_COST_VOL_CHANNELS = 128
MODEL_COST_VOL_SIZE = MODEL_COST_VOL_CHANNELS * MODEL_INT_DISP_W * MODEL_INT_DISP_H  # 9175040 bytes

# Model input dimensions (CVE output: 640x448 RGB888 planar uint8)
MODEL_INPUT_W = 640
MODEL_INPUT_H = 448
MODEL_INPUT_SIZE = MODEL_INPUT_W * MODEL_INPUT_H * 3  # 860160 bytes (RGB888 planar)

# Display layout: right camera is no longer shown; the left (detection)
# image and the native-resolution disparity map share a single column.
LEFT_DISP_W = 1024               # left display width (1280x1080 -> 1024x864, 32:27)
LEFT_DISP_H = 864                # left display height
DISPARITY_DISP_W = DISPARITY_W   # 640 native
DISPARITY_DISP_H = DISPARITY_H   # 448 native
TOP_ROW_W = LEFT_DISP_W          # single image column

# Disparity panel placement in composite image (for mouse click mapping)
# Layout: left image on top, disparity centered below it
DISP_PANEL_OFFSET_X = (TOP_ROW_W - DISPARITY_DISP_W) // 2
DISP_PANEL_OFFSET_Y = 28 + LEFT_DISP_H + 28

# Triangulation window size for averaging
MEASURE_WINDOW = 9  # 9x9 window

# ── Display layout constants ──────────────────────────────────────────────────

PANEL_COLOR      = (45, 45, 45)     # dark grey background
TEXT_COLOR        = (200, 200, 200)  # light grey text
TEXT_COLOR_GREEN  = (0, 255, 0)
TEXT_COLOR_YELLOW = (0, 255, 255)
TEXT_COLOR_RED    = (0, 0, 255)
INFO_PANEL_W      = 320              # right-side info panel width

# COCO 80 class names. The board ships the per-frame detection list
# (class_id, distance_mm) inside the type-7 perf JSON, so the host only maps
# the id back to a name — no image reverse-decoding is needed.
COCO_NAMES = [
    "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train",
    "truck", "boat", "traffic light", "fire hydrant", "stop sign",
    "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
    "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella",
    "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard",
    "sports ball", "kite", "baseball bat", "baseball glove", "skateboard",
    "surfboard", "tennis racket", "bottle", "wine glass", "cup", "fork",
    "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange",
    "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair",
    "couch", "potted plant", "bed", "dining table", "toilet", "tv",
    "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave",
    "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase",
    "scissors", "teddy bear", "hair drier", "toothbrush",
]

def _class_rgb(cid: int):
    """Host replica of the board's golden-angle class color (RGB)."""
    hue = (cid * 137) % 360
    h = hue / 60.0
    i = int(h) % 6
    f = h - int(h)
    q = 1.0 - f
    if i == 0:
        r, g, b = 1.0, f, 0.0
    elif i == 1:
        r, g, b = q, 1.0, 0.0
    elif i == 2:
        r, g, b = 0.0, 1.0, f
    elif i == 3:
        r, g, b = 0.0, q, 1.0
    elif i == 4:
        r, g, b = f, 0.0, 1.0
    else:
        r, g, b = 1.0, 0.0, q
    return int(r * 255), int(g * 255), int(b * 255)

# ── Helpers ───────────────────────────────────────────────────────────────────

def recv_exact(sock: socket.socket, n: int) -> bytes:
    """Receive exactly n bytes, or None on timeout / peer close."""
    data = b''
    while len(data) < n:
        try:
            chunk = sock.recv(n - len(data))
        except socket.timeout:
            return None
        if not chunk:
            return None
        data += chunk
    return data


def make_label_bar(text, width, height=30, color=PANEL_COLOR, text_color=TEXT_COLOR_GREEN):
    """Create a horizontal label bar with centered text."""
    bar = np.full((height, width, 3), color, dtype=np.uint8)
    (tw, th), _ = cv2.getTextSize(text, cv2.FONT_HERSHEY_SIMPLEX, 0.7, 2)
    x = (width - tw) // 2
    y = (height + th) // 2
    cv2.putText(bar, text, (x, y), cv2.FONT_HERSHEY_SIMPLEX, 0.7, text_color, 2, cv2.LINE_AA)
    return bar


def draw_info_panel(info, w=INFO_PANEL_W, h=720, detections=None):
    """Draw the right-side info panel with frame metadata."""
    panel = np.full((h, w, 3), PANEL_COLOR, dtype=np.uint8)
    y = 35
    dy = 28

    # Title
    cv2.putText(panel, "Stereo Matching", (12, y),
                cv2.FONT_HERSHEY_SIMPLEX, 0.8, TEXT_COLOR_GREEN, 2, cv2.LINE_AA)
    y += 40
    cv2.line(panel, (8, y), (w - 8, y), (80, 80, 80), 1)
    y += 20

    rows = [
        ("Frame:",   f"{info.get('frame_idx', 0)}"),
        ("FPS:",     f"{info.get('fps', 0):.1f}"),
        ("", ""),
        ("Left:",    f"{info.get('left_w', 0)}x{info.get('left_h', 0)}  {info.get('left_kb', 0)} KB"),
        ("Right:",   f"{info.get('right_w', 0)}x{info.get('right_h', 0)}  {info.get('right_kb', 0)} KB"),
        ("Disp:",    f"{info.get('disp_w', 0)}x{info.get('disp_h', 0)}  {info.get('disp_kb', 0)} KB"),
        ("", ""),
        ("Total:",   f"{info.get('total_kb', 0)} KB/frame"),
        ("Bandwidth:", f"{info.get('mbps', 0):.1f} Mbps"),
        ("", ""),
        ("Elapsed:", f"{info.get('elapsed', 0):.0f}s"),
        ("Frames:",  f"{info.get('total_frames', 0)}"),
        ("", ""),
        ("Host:",    info.get('host', '')),
        ("Port:",    str(info.get('port', 0))),
        ("Calib:",   "Yes" if info.get('has_calib') else "No"),
        ("", ""),
        ("NPU:",     f"{info.get('npu_ms', 0):.1f} ms"),
        ("SubPixel:",f"{info.get('sub_ms', 0):.1f} ms"),
        ("Total:",   f"{info.get('total_ms', 0):.1f} ms"),
        ("", ""),
        ("Capture:", f"ON [{info.get('capture_count', 0)}]" if info.get('capturing') else "OFF"),
        ("Dropped:",  f"{info.get('drop_count', 0)}"),
    ]

    for label, value in rows:
        if label:
            cv2.putText(panel, label, (12, y),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.55, TEXT_COLOR_YELLOW, 1, cv2.LINE_AA)
            cv2.putText(panel, value, (110, y),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.55, TEXT_COLOR, 1, cv2.LINE_AA)
        y += dy

    # ── Detections section: class label + distance for every box on the left ──
    cv2.line(panel, (8, y + 4), (w - 8, y + 4), (80, 80, 80), 1)
    y += 18
    if detections:
        n = len(detections)
        n_cls = len({d[0] for d in detections})
        cv2.putText(panel, f"Targets: {n}  Classes: {n_cls}", (12, y),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.55, TEXT_COLOR_GREEN, 1, cv2.LINE_AA)
        y += dy
        for cid, name, dist in detections:
            if y > h - 60:
                break
            text = f"{name}: {dist:.2f}m" if dist is not None else f"{name}: -- m"
            b, g, r = _class_rgb(int(cid))
            cv2.putText(panel, text, (12, y),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (b, g, r), 1, cv2.LINE_AA)
            y += 20
    else:
        cv2.putText(panel, "Targets: none", (12, y),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.55, TEXT_COLOR, 1, cv2.LINE_AA)

    # Connection status at bottom
    y = h - 45
    cv2.line(panel, (8, y), (w - 8, y), (80, 80, 80), 1)
    y += 25
    status = "CONNECTED" if info.get('connected') else "DISCONNECTED"
    status_color = TEXT_COLOR_GREEN if info.get('connected') else TEXT_COLOR_RED
    cv2.putText(panel, status, (12, y),
                cv2.FONT_HERSHEY_SIMPLEX, 0.65, status_color, 2, cv2.LINE_AA)

    return panel


# ── Main class ────────────────────────────────────────────────────────────────

class StereoReceiver:
    def __init__(self, host: str = '192.168.1.101', port: int = 9000,
                 raw_port: int = RAW_CAPTURE_PORT, save_frames: bool = False,
                 display_scale: float = 1.0):
        self.host = host
        self.port = port
        self.raw_port = raw_port
        self.save_frames = save_frames
        self.display_scale = display_scale
        self.sock = None
        self.frame_count = 0
        self.start_time = None
        self.connected = False
        self.fps_history = deque(maxlen=30)
        # per-frame data sizes for info panel
        self.left_size = 0
        self.right_size = 0
        self.disp_size = 0
        self.bandwidth_history = deque(maxlen=15)

        # Calibration parameters (received from board via type=6 frame)
        self.calib = None  # dict with fx, cx, cy, baseline_mm, etc.

        # Measurement state (mouse click on disparity)
        self.measure_point = None   # (x_model, y_model) in 640x448 disparity coords
        self.measure_result = None  # dict with X, Y, Z, disp_val, valid_pixels
        self.last_disparity = None  # last received disparity array (640x448 uint16)

        # Performance timing (received from board via type=7 frame)
        self.perf = None  # dict with npu_ms, sub_ms, total_ms

        # Model output (received via type=8/9 frames)
        self.model_int_disp = None   # uint8 [240, 320] model integer disparity
        self.model_cost_vol = None   # uint8 [128, 224, 320] model cost volume
        self.model_input_left = None   # uint8 [480, 640, 3] CVE left RGB888 planar
        self.model_input_right = None  # uint8 [480, 640, 3] CVE right RGB888 planar
        self.model_output_enabled = False  # True when board is sending model output

        # One-shot deep learning data capture (triggered by 'm' key)
        self._pending_model_capture = False  # flag set by 'm' key, consumed by receiver thread

        # Continuous capture state
        self.capturing = False       # True when continuous capture is active
        self.capture_dir = None     # output directory path
        self.capture_count = 0      # number of frames saved in current session

        # Three-thread pipeline: receiver → decode → display
        self._running = False        # main loop flag
        self._latest_raw = None      # latest raw frames (JPEG bytes) from receiver
        self._raw_lock = threading.Lock()
        self._latest_decoded = None  # latest decoded frames (numpy arrays)
        self._decoded_lock = threading.Lock()
        self._drop_count = 0         # total frames dropped (raw + decoded)

        # ── Performance: pre-computed structures ──
        # Disparity LUT: Q5 value (0..3072) → BGR color. A single indexed
        # lookup replaces 5 per-frame array ops (astype/div/clip/applyColorMap/mask).
        self._disp_lut = self._build_disp_lut()

        # Cache label bars (static text — avoid recreating 3 arrays every frame)
        self._label_l = make_label_bar("Left Camera", LEFT_DISP_W, 28)
        self._label_d = make_label_bar(
            f"Disparity ({DISPARITY_DISP_W}x{DISPARITY_DISP_H} Q5, JET norm=0..{DISPARITY_MAX_DISP})",
            TOP_ROW_W, 28)

        # Pre-allocate composite buffer. Eliminates ~15 MB/frame of temporary
        # vstack/hstack allocations. Layout: top 1600x672 images + disparity
        # row + right info panel.
        self._comp_h = 28 + LEFT_DISP_H + 28 + DISPARITY_DISP_H  # 1368
        self._comp_w = TOP_ROW_W + INFO_PANEL_W                  # 1344
        self._composite = np.empty((self._comp_h, self._comp_w, 3), dtype=np.uint8)

        # Current disparity reference (copy deferred to mouse-click handler)
        self._current_disparity = None

    def _build_disp_lut(self):
        """Pre-compute Q5→BGR lookup table for JET pseudocolor.
        Normalizes disparity to [0, max_disp] then maps to 0-255 for JET colormap.
        Replaces 5 per-frame array ops with a single indexed lookup."""
        max_q5 = DISPARITY_MAX_DISP * DISPARITY_Q5_SCALE + 1  # 4097
        q5 = np.arange(max_q5, dtype=np.float32) / DISPARITY_Q5_SCALE
        # Normalize: (disp - min_disp) / (max_disp - min_disp) * 255
        # min_disp=0, max_disp=DISPARITY_MAX_DISP (matches test_onnx_sub.py approach)
        normalized = np.clip(q5 / DISPARITY_MAX_DISP, 0, 1)
        idx = (normalized * 255).astype(np.uint8)
        jet = cv2.applyColorMap(
            np.arange(256, dtype=np.uint8).reshape(256, 1),
            cv2.COLORMAP_JET).reshape(256, 3)
        lut = jet[idx]            # (4097, 3) uint8
        lut[q5 < 0.1] = [0, 0, 0]  # invalid = black
        # Extend to full uint16 range (safety: avoids IndexError on corrupt data)
        full_lut = np.zeros((65536, 3), dtype=np.uint8)
        full_lut[:max_q5] = lut
        return full_lut

    def connect(self):
        """Connect to the board as TCP client."""
        while True:
            try:
                self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.sock.settimeout(5.0)
                # Disable Nagle's algorithm to reduce send latency
                self.sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                # Limit TCP receive buffer to ~1 frame to prevent frame accumulation.
                # Without this, the kernel buffers up to 6MB (~12 frames at 488KB/frame),
                # causing ~400ms latency. 200KB forces TCP backpressure so the board
                # drops frames instead of the PC accumulating stale ones.
                self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 200 * 1024)
                print(f"[receiver] Connecting to {self.host}:{self.port}...")
                self.sock.connect((self.host, self.port))
                self.connected = True
                print(f"[receiver] Connected to {self.host}:{self.port}")
                return True
            except (ConnectionRefusedError, socket.timeout, OSError) as e:
                print(f"[receiver] Connection failed: {e}, retrying in 2s...")
                self.connected = False
                if self.sock:
                    self.sock.close()
                time.sleep(2)

    def read_frame(self) -> dict:
        """Read one complete frame set (left + right + disparity).
        Returns dict with 'left','right','disparity' images and metadata, or None.
        Disparity is received as raw uint16 Q5 (640x448), L/R as JPEG.
        Calibration frames (type=6) are parsed inline and not counted
        as part of the 3-frame triplet."""
        result = {}
        target_frame_idx = None
        sizes = {}

        while len(result) < 3:
            header_raw = recv_exact(self.sock, HEADER_SIZE)
            if header_raw is None:
                self.connected = False
                return None

            try:
                magic, ftype, frm_idx, ts_us, payload_size, _ = \
                    struct.unpack(HEADER_FMT, header_raw)
            except struct.error:
                print(f"[receiver] header unpack error")
                continue

            if magic != MAGIC:
                print(f"[receiver] bad magic: 0x{magic:08X}")
                continue

            if payload_size == 0 or payload_size > 10 * 1024 * 1024:
                print(f"[receiver] invalid payload size: {payload_size}")
                continue

            payload = recv_exact(self.sock, payload_size)
            if payload is None:
                self.connected = False
                return None

            type_name = FRAME_TYPE_NAMES.get(ftype, 'unknown')

            # ── Calibration frame: parse and store, do NOT count as image ──
            if ftype == 6:
                try:
                    calib_json = json.loads(payload.decode('utf-8'))
                    self.calib = calib_json
                    print(f"[receiver] Calibration received: fx={calib_json.get('fx')}, "
                          f"baseline={calib_json.get('baseline_mm')}mm, "
                          f"disp={calib_json.get('disp_w')}x{calib_json.get('disp_h')}")
                except Exception as e:
                    print(f"[receiver] calib parse error: {e}")
                continue

            # ── Perf frame: parse and store, do NOT count as image ──
            if ftype == 7:
                try:
                    self.perf = json.loads(payload.decode('utf-8'))
                except Exception:
                    pass
                continue

            # ── Model integer disparity (type=8): uint8 [240, 320] ──
            if ftype == 8:
                if payload_size == MODEL_INT_DISP_SIZE:
                    self.model_int_disp = np.frombuffer(payload, dtype=np.uint8).reshape(
                        (MODEL_INT_DISP_H, MODEL_INT_DISP_W))
                else:
                    print(f"[receiver] model int_disp size mismatch: got {payload_size}, "
                          f"expected {MODEL_INT_DISP_SIZE}")
                continue

            # ── Model cost volume (type=9): uint8 [128, 224, 320] ──
            if ftype == 9:
                if payload_size == MODEL_COST_VOL_SIZE:
                    self.model_cost_vol = np.frombuffer(payload, dtype=np.uint8).reshape(
                        (MODEL_COST_VOL_CHANNELS, DISPARITY_H, DISPARITY_W))
                else:
                    print(f"[receiver] model cost_vol size mismatch: got {payload_size}, "
                          f"expected {MODEL_COST_VOL_SIZE}")
                continue

            # ── Model input left (type=0x0A): uint8 [480, 640, 3] RGB888 planar ──
            if ftype == 10:
                if payload_size == MODEL_INPUT_SIZE:
                    self.model_input_left = np.frombuffer(payload, dtype=np.uint8).reshape(
                        (MODEL_INPUT_H, MODEL_INPUT_W, 3))
                else:
                    print(f"[receiver] model input_left size mismatch: got {payload_size}, "
                          f"expected {MODEL_INPUT_SIZE}")
                continue

            # ── Model input right (type=0x0B): uint8 [480, 640, 3] RGB888 planar ──
            if ftype == 11:
                if payload_size == MODEL_INPUT_SIZE:
                    self.model_input_right = np.frombuffer(payload, dtype=np.uint8).reshape(
                        (MODEL_INPUT_H, MODEL_INPUT_W, 3))
                else:
                    print(f"[receiver] model input_right size mismatch: got {payload_size}, "
                          f"expected {MODEL_INPUT_SIZE}")
                continue

            if ftype == 2:
                # Disparity: raw uint16 Q5 data (640x448x2 = 573440 bytes)
                if payload_size == DISPARITY_RAW_SIZE:
                    img = np.frombuffer(payload, dtype=np.uint16).reshape(
                        (DISPARITY_H, DISPARITY_W))
                else:
                    print(f"[receiver] disparity size mismatch: got {payload_size}, "
                          f"expected {DISPARITY_RAW_SIZE}")
                    continue
            elif ftype == 3:
                # Disparity: LZ4-compressed uint16 block (decompresses to 573440 bytes)
                if not HAS_LZ4:
                    print("[receiver] LZ4 disparity received but lz4 module not installed! "
                          "Run: pip install lz4")
                    continue
                try:
                    raw = lz4.block.decompress(payload,
                                              uncompressed_size=DISPARITY_RAW_SIZE)
                    img = np.frombuffer(raw, dtype=np.uint16).reshape(
                        (DISPARITY_H, DISPARITY_W))
                except Exception as e:
                    print(f"[receiver] LZ4 decompress failed: {e}, "
                          f"compressed={payload_size}B")
                    continue
            elif ftype in (4, 5):
                print(f"[receiver] raw capture packet unexpectedly arrived on stream: type={ftype}")
                continue
            else:
                # L/R: JPEG encoded — store raw bytes, decode in display thread.
                # Moving imdecode out of the receiver thread keeps recv loop fast
                # (~5ms/frame vs ~30ms), preventing TCP buffer accumulation.
                img = payload  # raw JPEG bytes

            # Frame index validation: if a new frame index arrives before we
            # have all 3 components, discard the partial result and restart
            # with the new frame. This prevents mixing L/R from one frame
            # with disparity from another (causes flickering).
            if target_frame_idx is None:
                target_frame_idx = frm_idx
            elif frm_idx != target_frame_idx:
                result.clear()
                sizes.clear()
                target_frame_idx = frm_idx

            # Skip duplicate types for the same frame (safety check)
            if type_name in result:
                continue

            result[type_name] = img
            sizes[type_name] = payload_size

        self.left_size  = sizes.get('left', 0)
        self.right_size = sizes.get('right', 0)
        self.disp_size  = sizes.get('disparity', 0)

        return result

    def build_display(self, frames: dict) -> np.ndarray:
        """Build a single composite frame."""
        left_raw  = frames.get('left')
        right_raw = frames.get('right')
        disparity = frames.get('disparity')

        if left_raw is None or right_raw is None or disparity is None:
            return None

        # ── Decode JPEG at full resolution (1280x1080) for saving; display resized ──
        if isinstance(left_raw, (bytes, bytearray)):
            left = cv2.imdecode(np.frombuffer(left_raw, np.uint8), cv2.IMREAD_COLOR)
        else:
            left = left_raw
        if isinstance(right_raw, (bytes, bytearray)):
            right = cv2.imdecode(np.frombuffer(right_raw, np.uint8), cv2.IMREAD_COLOR)
        else:
            right = right_raw

        if left is None or right is None:
            return None

        # ── Resize to display size (left only; right is no longer shown) ──
        left_disp  = cv2.resize(left, (LEFT_DISP_W, LEFT_DISP_H))

        # ── Disparity pseudocolor via pre-computed LUT (single indexed lookup) ──
        if disparity.dtype == np.uint16:
            disp_color = self._disp_lut[disparity]  # (480, 640, 3) uint8
        elif len(disparity.shape) == 2:
            disp_color = cv2.applyColorMap(disparity.astype(np.uint8), cv2.COLORMAP_JET)
        else:
            disp_color = disparity
        if (disp_color.shape[0], disp_color.shape[1]) != (DISPARITY_DISP_H, DISPARITY_DISP_W):
            disp_color = cv2.resize(disp_color, (DISPARITY_DISP_W, DISPARITY_DISP_H))

        # ── Build composite in pre-allocated buffer (no vstack/hstack) ──
        buf = self._composite
        LABEL_H  = 28
        TOP_W    = TOP_ROW_W          # 1600
        OFFSET_X = (TOP_W - DISPARITY_DISP_W) // 2

        y = 0
        # Top labels (cached)
        buf[y:y+LABEL_H, 0:TOP_W] = self._label_l
        y += LABEL_H
        # Left camera image (full width of the image column)
        buf[y:y+LEFT_DISP_H, 0:TOP_W] = left_disp
        y += LEFT_DISP_H
        # Disparity label (cached)
        buf[y:y+LABEL_H, 0:TOP_W] = self._label_d
        y += LABEL_H
        # Disparity panel (centered in the bottom row)
        disp_y = y
        buf[y:y+DISPARITY_DISP_H, 0:TOP_W] = PANEL_COLOR
        buf[y:y+DISPARITY_DISP_H, OFFSET_X:OFFSET_X+DISPARITY_DISP_W] = disp_color

        # Store current disparity reference (no copy — deferred to mouse-click)
        self._current_disparity = disparity

        # ── Measurement overlay (draw directly on composite buffer) ──
        if self.measure_point is not None and self.measure_result is not None:
            mx, my = self.measure_point
            r = self.measure_result
            dx = OFFSET_X + int(mx * DISPARITY_DISP_W / DISPARITY_W)
            dy = disp_y + int(my * DISPARITY_DISP_H / DISPARITY_H)
            win_half = int((MEASURE_WINDOW // 2) * DISPARITY_DISP_W / DISPARITY_W)
            cv2.rectangle(buf,
                          (dx - win_half, dy - win_half),
                          (dx + win_half, dy + win_half),
                          (0, 255, 255), 1)
            cv2.drawMarker(buf, (dx, dy), (0, 255, 0),
                           cv2.MARKER_CROSS, 16, 2)
            if r['valid_pixels'] > 0 and r.get('disp_real', 0) > 0.1:
                text1 = f"({mx},{my})  disp={r['disp_real']:.2f}px  valid={r['valid_pixels']}/{MEASURE_WINDOW*MEASURE_WINDOW}"
                text2 = f"X={r['X']:.1f}mm  Y={r['Y']:.1f}mm  Z={r['Z']:.1f}mm"
            else:
                text1 = f"({mx},{my})  no valid disparity"
                text2 = ""
            y_text = dy + 22
            for line in [text1, text2]:
                if not line:
                    continue
                (tw, th), _ = cv2.getTextSize(line, cv2.FONT_HERSHEY_SIMPLEX, 0.5, 1)
                tx = max(OFFSET_X, min(dx - tw // 2, OFFSET_X + DISPARITY_DISP_W - tw))
                ty = max(15, min(y_text, disp_y + DISPARITY_DISP_H - 5))
                cv2.rectangle(buf, (tx - 2, ty - th - 2), (tx + tw + 2, ty + 2),
                              (0, 0, 0), -1)
                cv2.putText(buf, line, (tx, ty),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1, cv2.LINE_AA)
                y_text += 18

        # ── Info panel ──
        now = time.time()
        self.fps_history.append(now)
        if len(self.fps_history) >= 2:
            fps = len(self.fps_history) / (self.fps_history[-1] - self.fps_history[0] + 1e-6)
        else:
            fps = 0.0

        elapsed = now - self.start_time if self.start_time else 0
        total_kb = (self.left_size + self.right_size + self.disp_size) / 1024.0

        # bandwidth estimate (Mbps)
        self.bandwidth_history.append(total_kb * 8 / 1000.0)
        mbps = sum(self.bandwidth_history) / len(self.bandwidth_history) * fps if len(self.bandwidth_history) > 0 else 0

        info = {
            'frame_idx':    self.frame_count + 1,
            'fps':          fps,
            'left_w':       left.shape[1],
            'left_h':       left.shape[0],
            'left_kb':      self.left_size / 1024,
            'right_w':      right.shape[1],
            'right_h':      right.shape[0],
            'right_kb':     self.right_size / 1024,
            'disp_w':       disparity.shape[1],
            'disp_h':       disparity.shape[0],
            'disp_kb':      self.disp_size / 1024,
            'total_kb':     total_kb,
            'mbps':         mbps,
            'elapsed':      elapsed,
            'total_frames': self.frame_count,
            'host':         self.host,
            'port':         self.port,
            'connected':    self.connected,
            'has_calib':    self.calib is not None,
            'npu_ms':       self.perf.get('npu_ms', 0) if self.perf else 0,
            'sub_ms':       self.perf.get('sub_ms', 0) if self.perf else 0,
            'total_ms':     self.perf.get('total_ms', 0) if self.perf else 0,
            'capturing':    self.capturing,
            'capture_count':self.capture_count,
            'drop_count':   self._drop_count,
        }
        detections = []
        if self.perf and self.perf.get('dets'):
            for item in self.perf['dets']:
                try:
                    cid = int(item[0])
                    dist_mm = int(item[1])
                except (TypeError, ValueError, IndexError):
                    continue
                name = COCO_NAMES[cid] if 0 <= cid < len(COCO_NAMES) else "obj"
                detections.append((cid, name, dist_mm / 1000.0 if dist_mm > 0 else None))
        info_panel = draw_info_panel(info, INFO_PANEL_W, self._comp_h, detections)
        buf[:, TOP_W:TOP_W + INFO_PANEL_W] = info_panel

        return buf

    def _decode_raw_capture_payload(self, payload: bytes, ftype: int, frm_idx: int, ts_us: int):
        if len(payload) < RAW_META_SIZE:
            raise ValueError(f"raw payload too small: {len(payload)}")

        meta = payload[:RAW_META_SIZE]
        raw = payload[RAW_META_SIZE:]
        magic, width, height, stride_y, stride_uv, pixfmt, camera_id, rotation_hint, data_size, raw_frame_index = \
            struct.unpack(RAW_META_FMT, meta)

        if magic != b'RAW0':
            raise ValueError(f"bad raw meta magic: {magic!r}")
        if data_size != len(raw):
            raise ValueError(f"raw data size mismatch: meta={data_size}, payload={len(raw)}")
        if pixfmt not in RAW_PIXEL_FORMATS:
            raise ValueError(f"unsupported raw pixel format id: {pixfmt}")

        yuv = np.frombuffer(raw, dtype=np.uint8).reshape((height * 3 // 2, width))
        code = cv2.COLOR_YUV2BGR_NV21 if pixfmt == 0 else cv2.COLOR_YUV2BGR_NV12
        bgr = cv2.cvtColor(yuv, code)

        if rotation_hint == 270:
            bgr = cv2.rotate(bgr, cv2.ROTATE_90_COUNTERCLOCKWISE)
        elif rotation_hint == 90:
            bgr = cv2.rotate(bgr, cv2.ROTATE_90_CLOCKWISE)
        elif rotation_hint == 180:
            bgr = cv2.rotate(bgr, cv2.ROTATE_180)

        side = 'left' if ftype == 4 else 'right'
        stamp = datetime.datetime.fromtimestamp(ts_us / 1_000_000).strftime('%Y%m%d_%H%M%S_%f')
        return {
            'side': side,
            'raw': raw,
            'bgr': bgr,
            'stamp': stamp,
            'frame_index': frm_idx,
            'raw_frame_index': raw_frame_index,
            'width': width,
            'height': height,
            'pixfmt': pixfmt,
            'pixfmt_name': RAW_PIXEL_FORMATS[pixfmt],
        }

    def _save_raw_capture_frame(self, frame: dict):
        os.makedirs('captured/raw', exist_ok=True)
        side = frame['side']
        raw_name = (f"captured/raw/raw_{frame['stamp']}_frame_{frame['frame_index']:06d}_"
                    f"{side}_{frame['width']}x{frame['height']}_{frame['pixfmt_name']}.nv21")
        with open(raw_name, 'wb') as fp:
            fp.write(frame['raw'])

        bgr = frame['bgr']
        png_name = (f"captured/raw/raw_{frame['stamp']}_frame_{frame['frame_index']:06d}_"
                    f"{side}_{bgr.shape[1]}x{bgr.shape[0]}.png")
        cv2.imwrite(png_name, bgr, [cv2.IMWRITE_PNG_COMPRESSION, 3])
        print(f"[receiver] saved raw {side}: {raw_name}")
        print(f"[receiver] saved lossless png {side}: {png_name}")

    def _build_raw_preview(self, frames: list, fps: float = 0.0):
        if not frames:
            return None

        ordered = sorted(frames, key=lambda frame: 0 if frame['side'] == 'left' else 1)
        panels = []
        for frame in ordered:
            img = frame['bgr']
            target_w = 640
            target_h = max(1, int(img.shape[0] * target_w / img.shape[1]))
            disp = cv2.resize(img, (target_w, target_h))
            label = make_label_bar(
                f"Raw {frame['side'].capitalize()} {img.shape[1]}x{img.shape[0]}",
                target_w, 28)
            panels.append(np.vstack((label, disp)))

        preview = np.hstack(panels)
        info = np.full((54, preview.shape[1], 3), PANEL_COLOR, dtype=np.uint8)
        cv2.putText(info, f"FPS: {fps:.1f}   Press 's' to save current raw pair, 'q' to quit",
                    (12, 34), cv2.FONT_HERSHEY_SIMPLEX, 0.7, TEXT_COLOR_GREEN, 2, cv2.LINE_AA)
        return np.vstack((preview, info))

    def trigger_raw_capture(self, mask: int = 0x03, save: bool = True):
        raw_frames = []
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as raw_sock:
            raw_sock.settimeout(10.0)
            raw_sock.connect((self.host, self.raw_port))
            raw_sock.sendall(struct.pack('>IBB2s', RAW_CMD_MAGIC, RAW_CMD_GRAB, mask, b'\x00\x00'))

            expected = (1 if mask & 0x01 else 0) + (1 if mask & 0x02 else 0)
            if expected == 0:
                expected = 2

            for _ in range(expected):
                header_raw = recv_exact(raw_sock, HEADER_SIZE)
                if header_raw is None:
                    raise RuntimeError("raw capture connection closed while reading header")

                magic, ftype, frm_idx, ts_us, payload_size, _ = struct.unpack(HEADER_FMT, header_raw)
                if magic != MAGIC:
                    raise RuntimeError(f"bad raw packet magic: 0x{magic:08X}")
                if ftype not in (4, 5):
                    raise RuntimeError(f"unexpected raw packet type: {ftype}")
                if payload_size <= RAW_META_SIZE or payload_size > 8 * 1024 * 1024:
                    raise RuntimeError(f"invalid raw payload size: {payload_size}")

                payload = recv_exact(raw_sock, payload_size)
                if payload is None:
                    raise RuntimeError("raw capture connection closed while reading payload")

                frame = self._decode_raw_capture_payload(payload, ftype, frm_idx, ts_us)
                raw_frames.append(frame)

        if save:
            for frame in raw_frames:
                self._save_raw_capture_frame(frame)
        return raw_frames

    def run_raw_preview(self, mask: int = 0x03):
        print(f"[receiver] Raw preview from {self.host}:{self.raw_port}")
        print("[receiver] Press 's' to save current raw pair, 'q' to quit.")
        last_frames = []
        fps_times = deque(maxlen=20)

        while True:
            try:
                frames = self.trigger_raw_capture(mask=mask, save=False)
            except Exception as e:
                print(f"[receiver] raw preview capture failed: {e}")
                time.sleep(0.2)
                key = cv2.waitKey(1) & 0xFF
                if key == ord('q'):
                    break
                continue

            last_frames = frames
            now = time.time()
            fps_times.append(now)
            fps = 0.0
            if len(fps_times) >= 2:
                fps = (len(fps_times) - 1) / (fps_times[-1] - fps_times[0] + 1e-6)

            preview = self._build_raw_preview(frames, fps=fps)
            if preview is not None:
                cv2.imshow('Raw Capture Live Preview', preview)

            key = cv2.waitKey(1) & 0xFF
            if key == ord('q'):
                print("[receiver] raw preview quit requested")
                break
            if key == ord('s'):
                for frame in last_frames:
                    self._save_raw_capture_frame(frame)

        cv2.destroyAllWindows()

    def compute_3d_point(self, x_model: int, y_model: int) -> dict:
        """Compute 3D coordinates (X, Y, Z in mm) from a point in the 640x448
        disparity image using 9x9 window mean filtering.

        Requires self.calib and self.last_disparity to be set.
        Returns dict with X, Y, Z, disp_raw, disp_real, valid_pixels, or None.
        """
        if self.calib is None or self.last_disparity is None:
            return None

        disp = self.last_disparity  # uint16 Q5, shape (448, 640)
        h, w = disp.shape

        # Extract 9x9 window centered at (x_model, y_model)
        half = MEASURE_WINDOW // 2
        x0 = max(0, x_model - half)
        x1 = min(w, x_model + half + 1)
        y0 = max(0, y_model - half)
        y1 = min(h, y_model + half + 1)

        window = disp[y0:y1, x0:x1]

        # Average only non-zero (valid) disparity values
        valid_mask = (window > 0)
        valid_count = valid_mask.sum()
        if valid_count == 0:
            return {'X': 0, 'Y': 0, 'Z': 0, 'disp_raw': 0, 'disp_real': 0,
                    'valid_pixels': 0, 'window': (x0, y0, x1, y1)}

        disp_raw = window[valid_mask].astype(np.float64).mean()

        fx = self.calib.get('fx', 0)
        cx = self.calib.get('cx', 0)
        cy = self.calib.get('cy', 0)
        baseline = self.calib.get('baseline_mm', 0)
        q5 = self.calib.get('q5_scale', 32)
        # Disparity values are at NPU INPUT resolution (640x448).
        # After board-side 2x upsampling, transmitted disparity is also 640x448.
        # fx/cx/cy are scaled to 640x448 (scale_x = 1280/640 = 2.0).
        # disp_scale = input_w / disp_output_w = 640/640 = 1.0 (no scaling needed).
        disp_scale = self.calib.get('disp_scale', 1.0)

        disp_real = disp_raw / q5  # real disparity in pixels at NPU input resolution
        # Scale disparity to match fx/cx/cy resolution (disp_scale=1.0, no-op)
        disp_eff = disp_real / disp_scale
        if disp_eff < 0.1:
            return {'X': 0, 'Y': 0, 'Z': 0, 'disp_raw': disp_raw, 'disp_real': disp_real,
                    'valid_pixels': int(valid_count), 'window': (x0, y0, x1, y1)}

        # Triangulation: Z = fx * baseline / disp, X = (x-cx)*baseline/disp, etc.
        Z = fx * baseline / disp_eff
        X = (x_model - cx) * baseline / disp_eff
        Y = (y_model - cy) * baseline / disp_eff

        return {
            'X': X, 'Y': Y, 'Z': Z,
            'disp_raw': disp_raw,
            'disp_real': disp_real,
            'valid_pixels': int(valid_count),
            'window': (x0, y0, x1, y1),
        }

    def on_mouse(self, event, x, y, flags, param):
        """Mouse callback: left-click on disparity panel to measure 3D point."""
        if event != cv2.EVENT_LBUTTONDOWN:
            return

        # Scale mouse coordinates from display to composite resolution
        if self.display_scale < 1.0:
            x = int(x / self.display_scale)
            y = int(y / self.display_scale)

        # Check if click is within the disparity panel region in composite image
        ox = DISP_PANEL_OFFSET_X
        oy = DISP_PANEL_OFFSET_Y
        dw = DISPARITY_DISP_W  # 640
        dh = DISPARITY_DISP_H  # 480

        if x < ox or x >= ox + dw or y < oy or y >= oy + dh:
            return  # click outside disparity area

        # Map from composite display coords to 640x448 disparity coords
        x_model = int((x - ox) * DISPARITY_W / dw)
        y_model = int((y - oy) * DISPARITY_H / dh)

        # Clamp to valid range
        x_model = max(0, min(DISPARITY_W - 1, x_model))
        y_model = max(0, min(DISPARITY_H - 1, y_model))

        self.measure_point = (x_model, y_model)
        # Copy current disparity for measurement (deferred from build_display)
        if self._current_disparity is not None:
            self.last_disparity = self._current_disparity.copy()
        self.measure_result = self.compute_3d_point(x_model, y_model)

        if self.measure_result:
            r = self.measure_result
            if r['valid_pixels'] > 0 and r['disp_real'] > 0.1:
                print(f"[measure] ({x_model},{y_model}) -> "
                      f"X={r['X']:.1f}mm Y={r['Y']:.1f}mm Z={r['Z']:.1f}mm "
                      f"(disp={r['disp_real']:.2f}px, valid={r['valid_pixels']}/{MEASURE_WINDOW*MEASURE_WINDOW})")
            else:
                print(f"[measure] ({x_model},{y_model}) -> no valid disparity in window")

    def display_frames(self, frames: dict):
        """Build and show the composite display."""
        composite = self.build_display(frames)
        if composite is None:
            return
        if self.display_scale < 1.0:
            h, w = composite.shape[:2]
            small = cv2.resize(composite,
                               (int(w * self.display_scale), int(h * self.display_scale)),
                               interpolation=cv2.INTER_AREA)
            cv2.imshow('Stereo Deep Learning Matching', small)
        else:
            cv2.imshow('Stereo Deep Learning Matching', composite)

    def _save_disp_png(self, disp_array, path: str):
        """Save uint16 disparity as PNG.
        Uses libspng fast encoder (no filter, zlib level=1) when available;
        falls back to cv2.imwrite on error or missing library."""
        if _HAS_FAST_PNG:
            arr = np.ascontiguousarray(disp_array)
            h, w = arr.shape
            ret = _disp_png_lib.save_disparity_png(
                arr.ctypes.data, arr.nbytes,
                w, h, path.encode('utf-8'))
            if ret == 0:
                return
            print(f"[receiver] fast PNG failed (code={ret}), fallback to cv2")
        cv2.imwrite(path, disp_array)

    def save_frame_set(self, frames: dict, idx: int):
        """Save frame triplet to disk.
        Prefers raw JPEG bytes ('*_raw' keys, 1280x1080) to avoid re-encode.
        Falls back to decoded numpy arrays if raw bytes unavailable.
        Disparity is saved as PNG (uint16, lossless)."""
        os.makedirs('captured', exist_ok=True)
        for name, data in frames.items():
            if name.endswith('_raw'):
                # Raw JPEG bytes — write directly (full 1280x1080, no re-encode)
                save_name = name[:-4]  # strip '_raw'
                if isinstance(data, (bytes, bytearray)):
                    with open(f'captured/frame_{idx:06d}_{save_name}.jpg', 'wb') as f:
                        f.write(data)
            elif isinstance(data, (bytes, bytearray)):
                with open(f'captured/frame_{idx:06d}_{name}.jpg', 'wb') as f:
                    f.write(data)
            elif isinstance(data, np.ndarray) and data.dtype == np.uint16:
                # Disparity uint16 — save as PNG (lossless, fast spng encoder)
                self._save_disp_png(data, f'captured/frame_{idx:06d}_{name}.png')
            elif isinstance(data, np.ndarray):
                # Decoded array — skip if raw bytes already saved under '_raw'
                if name + '_raw' in frames:
                    continue
                cv2.imwrite(f'captured/frame_{idx:06d}_{name}.jpg', data,
                            [cv2.IMWRITE_JPEG_QUALITY, 95])

    def set_model_output_enabled(self, enabled: bool):
        """Send enable/disable command to board model output control port (9002)."""
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as ctrl_sock:
                ctrl_sock.settimeout(3.0)
                ctrl_sock.connect((self.host, MODEL_OUTPUT_CTRL_PORT))
                ctrl_sock.sendall(bytes([0x01 if enabled else 0x00]))
            self.model_output_enabled = enabled
            print(f"[receiver] model output {'ENABLED' if enabled else 'DISABLED'}")
        except Exception as e:
            print(f"[receiver] model output control failed: {e}")
            self.model_output_enabled = False

    def toggle_capture(self, frames: dict):
        """Toggle continuous capture mode on/off (left/right/disparity only)."""
        if not self.capturing:
            # Start capturing
            stamp = datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
            self.capture_dir = f'captured/continuous_{stamp}'
            os.makedirs(self.capture_dir, exist_ok=True)
            self.capturing = True
            self.capture_count = 0
            print(f"[capture] START continuous capture -> {self.capture_dir}/")
            # Save first frame immediately
            self.save_continuous_frame(frames)
        else:
            # Stop capturing
            self.capturing = False
            print(f"[capture] STOP. Saved {self.capture_count} frames to {self.capture_dir}/")
            self.capture_dir = None

    def trigger_model_capture(self):
        """Trigger one-shot capture of deep learning input + output data.
        Enables model output on board, waits for next frame, saves to disk, disables."""
        if self._pending_model_capture:
            print("[model_capture] already pending, wait...")
            return
        # Enable model output on board
        self.set_model_output_enabled(True)
        self._pending_model_capture = True
        print("[model_capture] triggered, waiting for next frame...")

    def _save_model_capture(self):
        """Save one set of deep learning input + output data to disk.
        Called from receiver thread after model data has been received."""
        stamp = datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
        out_dir = f'captured/model_capture_{stamp}'
        os.makedirs(out_dir, exist_ok=True)

        saved_files = []
        # Model input: CVE output (640x448 RGB888 planar)
        if self.model_input_left is not None:
            self.model_input_left.tofile(f'{out_dir}/model_input_left_rgb888planar_640x448.bin')
            saved_files.append('model_input_left_rgb888planar_640x448.bin')
        if self.model_input_right is not None:
            self.model_input_right.tofile(f'{out_dir}/model_input_right_rgb888planar_640x448.bin')
            saved_files.append('model_input_right_rgb888planar_640x448.bin')
        # Model output: integer disparity (224x320 uint8)
        if self.model_int_disp is not None:
            cv2.imwrite(f'{out_dir}/model_int_disp.png', self.model_int_disp)
            saved_files.append('model_int_disp.png')
        # Model output: cost volume (96x224x320 uint8)
        if self.model_cost_vol is not None:
            self.model_cost_vol.tofile(f'{out_dir}/model_cost_vol_96x224x320.bin')
            saved_files.append('model_cost_vol_96x224x320.bin')

        print(f"[model_capture] saved {len(saved_files)} files to {out_dir}/")
        for f in saved_files:
            print(f"  {f}")

    def save_continuous_frame(self, frames: dict):
        """Save one frame triplet (left, right, disparity) for continuous capture.
        Prefers raw JPEG bytes ('left_raw'/'right_raw', 1280x1080) for lossless save.
        Falls back to decoded arrays if raw bytes unavailable.
        disparity: PNG (uint16, lossless, preserves Q5 fixed-point)."""
        if not self.capturing or self.capture_dir is None:
            return
        idx = self.capture_count
        # Prefer raw JPEG bytes; fall back to decoded arrays
        left_raw = frames.get('left_raw')
        left = left_raw if isinstance(left_raw, (bytes, bytearray)) else frames.get('left')
        right_raw = frames.get('right_raw')
        right = right_raw if isinstance(right_raw, (bytes, bytearray)) else frames.get('right')
        disp = frames.get('disparity')
        if left is not None:
            if isinstance(left, (bytes, bytearray)):
                with open(f'{self.capture_dir}/{idx:06d}_left.jpg', 'wb') as f:
                    f.write(left)
            else:
                cv2.imwrite(f'{self.capture_dir}/{idx:06d}_left.jpg', left,
                            [cv2.IMWRITE_JPEG_QUALITY, 95])
        if right is not None:
            if isinstance(right, (bytes, bytearray)):
                with open(f'{self.capture_dir}/{idx:06d}_right.jpg', 'wb') as f:
                    f.write(right)
            else:
                cv2.imwrite(f'{self.capture_dir}/{idx:06d}_right.jpg', right,
                            [cv2.IMWRITE_JPEG_QUALITY, 95])
        if disp is not None:
            self._save_disp_png(disp, f'{self.capture_dir}/{idx:06d}_disp.png')
        self.capture_count += 1

    def _receiver_loop(self):
        """Receiver thread: continuously read frames, keep only the latest.
        If main thread is slow (display), old frames are silently dropped."""
        while self._running:
            try:
                frames = self.read_frame()
            except Exception as e:
                print(f"[receiver] read error: {e}")
                break

            if frames is None:
                print("[receiver] connection lost, reconnecting...")
                self.connected = False
                if self.sock:
                    self.sock.close()
                self.connect()
                continue

            # Continuous capture: save every received frame in receiver thread
            if self.capturing:
                self.save_continuous_frame(frames)

            # One-shot model capture: check if all 4 model data items arrived
            if self._pending_model_capture:
                if (self.model_input_left is not None and
                    self.model_input_right is not None and
                    self.model_int_disp is not None and
                    self.model_cost_vol is not None):
                    self._save_model_capture()
                    self._pending_model_capture = False
                    self.set_model_output_enabled(False)

            # Store as latest raw frame for decode thread
            with self._raw_lock:
                if self._latest_raw is not None:
                    self._drop_count += 1
                self._latest_raw = frames

        print("[receiver] receiver thread exiting")

    def _decode_loop(self):
        """Decode thread: decode JPEG in parallel with display.
        Takes latest raw frames from receiver, decodes JPEG, stores decoded
        numpy arrays for display thread. Since cv2.imdecode releases the GIL,
        this truly runs in parallel with numpy operations in the display thread."""
        while self._running:
            # Take latest raw frames from receiver
            raw = None
            with self._raw_lock:
                if self._latest_raw is not None:
                    raw = self._latest_raw
                    self._latest_raw = None

            if raw is None:
                time.sleep(0.001)
                continue

            left_raw = raw.get('left')
            right_raw = raw.get('right')

            if isinstance(left_raw, (bytes, bytearray)):
                left = cv2.imdecode(np.frombuffer(left_raw, np.uint8), cv2.IMREAD_COLOR)
            else:
                left = left_raw
            if isinstance(right_raw, (bytes, bytearray)):
                right = cv2.imdecode(np.frombuffer(right_raw, np.uint8), cv2.IMREAD_COLOR)
            else:
                right = right_raw

            if left is None or right is None:
                continue

            # Include raw JPEG bytes for lossless full-resolution saving.
            # Decoded arrays (1280x1080) are used for display; raw bytes are
            # preferred by save_frame_set / save_continuous_frame to avoid re-encode.
            decoded = {'left': left, 'right': right, 'disparity': raw.get('disparity'),
                       'left_raw': raw.get('left'), 'right_raw': raw.get('right')}
            with self._decoded_lock:
                if self._latest_decoded is not None:
                    self._drop_count += 1
                self._latest_decoded = decoded

        print("[receiver] decode thread exiting")

    def run(self):
        """Main loop — receiver thread reads frames, main thread displays.
        Old frames are automatically dropped if display can't keep up."""
        self.connect()
        self.start_time = time.time()
        print("[receiver] Running. Press 'q' to quit, 's' to save single frame,")
        print("[receiver]   'c' to toggle continuous capture,")
        print("[receiver]   'm' to capture one frame of deep learning input+output,")
        print("[receiver]   'r' to capture raw 1280x1080.")
        print("[receiver] Click on disparity map to measure 3D coordinates.")

        cv2.namedWindow('Stereo Deep Learning Matching', cv2.WINDOW_NORMAL)
        # WINDOW_NORMAL opens a tiny default window; resize to composite dimensions
        cv2.resizeWindow('Stereo Deep Learning Matching', self._comp_w, self._comp_h)
        cv2.setMouseCallback('Stereo Deep Learning Matching', self.on_mouse)

        # Start receiver + decode threads
        self._running = True
        self._latest_raw = None
        self._latest_decoded = None
        self._drop_count = 0
        recv_thread = threading.Thread(target=self._receiver_loop, daemon=True)
        recv_thread.start()
        decode_thread = threading.Thread(target=self._decode_loop, daemon=True)
        decode_thread.start()

        while self._running:
            # Consume latest decoded frame from decode thread
            frames = None
            with self._decoded_lock:
                if self._latest_decoded is not None:
                    frames = self._latest_decoded
                    self._latest_decoded = None

            if frames is not None:
                self.frame_count += 1
                self.display_frames(frames)

                if self.save_frames and self.frame_count % 30 == 0:
                    self.save_frame_set(frames, self.frame_count)

            # Non-blocking key wait
            key = cv2.waitKey(1 if frames is not None else 10) & 0xFF
            if key == ord('q'):
                print("[receiver] quit requested")
                self._running = False
                break
            elif key == ord('s') and frames is not None:
                self.save_frame_set(frames, self.frame_count)
                print(f"[receiver] saved frame {self.frame_count}")
            elif key == ord('r'):
                try:
                    self.trigger_raw_capture()
                except Exception as e:
                    print(f"[receiver] raw capture failed: {e}")
            elif key == ord('c') and frames is not None:
                self.toggle_capture(frames)
            elif key == ord('m'):
                self.trigger_model_capture()

        self._running = False
        recv_thread.join(timeout=3)
        decode_thread.join(timeout=3)
        self.cleanup()

    def cleanup(self):
        elapsed = time.time() - self.start_time if self.start_time else 0
        print(f"[receiver] Frames: {self.frame_count}, "
              f"Dropped: {self._drop_count}, "
              f"Elapsed: {elapsed:.1f}s, "
              f"Avg FPS: {self.frame_count / max(elapsed, 0.001):.1f}")

        if self.sock:
            self.sock.close()
        cv2.destroyAllWindows()
        print("[receiver] shutdown complete")


# ── Entry point ───────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description='Stereo Matching Receiver — displays L/R/disparity from board')
    parser.add_argument('--host', type=str, default='192.168.1.101',
                        help='Board IP (default: 192.168.1.101)')
    parser.add_argument('--port', type=int, default=9000,
                        help='TCP port (default: 9000)')
    parser.add_argument('--raw-port', type=int, default=RAW_CAPTURE_PORT,
                        help=f'Raw capture control port (default: {RAW_CAPTURE_PORT})')
    parser.add_argument('--capture-raw', action='store_true',
                        help='Trigger raw full-resolution capture; with --show-raw enters live preview')
    parser.add_argument('--show-raw', action='store_true',
                        help="Live-display raw left/right images; press 's' to save current pair")
    parser.add_argument('--save', action='store_true',
                        help='Periodically save frames to disk')
    parser.add_argument('--scale', type=float, default=1.0,
                        help='Display scale: 0.5=half-res (800x476), 1.0=full-res 1600x952 (default)')
    args = parser.parse_args()

    receiver = StereoReceiver(host=args.host, port=args.port,
                              raw_port=args.raw_port, save_frames=args.save,
                              display_scale=args.scale)
    if args.capture_raw:
        if args.show_raw:
            receiver.run_raw_preview()
        else:
            print(f"[receiver] Triggering raw capture at {args.host}:{args.raw_port}...")
            receiver.trigger_raw_capture(save=True)
        return
    try:
        receiver.run()
    except KeyboardInterrupt:
        print("\n[receiver] interrupted")
        receiver.cleanup()


if __name__ == '__main__':
    main()
