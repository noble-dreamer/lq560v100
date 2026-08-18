#!/usr/bin/env python3
"""WSL2 host GUI for the dual_model_abab camera stream (stream level 3).

Reuses the SSH stdio channel of npu_stream_receiver.py: no new port or service.
Camera image frames (type 7) are decoded NV12 -> RGB and rendered live. Closing
the window sends CONTROL STOP on stdin and waits for the board to exit cleanly.
"""
import argparse
import struct
import threading
import time

import numpy as np
import paramiko
import tkinter as tk
from PIL import Image, ImageTk
from tkinter import messagebox

import npu_stream_receiver as nsr

MAGIC_INT = 0x5453504E
HDR = struct.Struct("<I BBBB I Q I I I")
T_CONTROL, CTRL_STOP = 5, 1
DISPLAY_W, DISPLAY_H = 640, 480
DEFAULT_MODEL_A = "model/mobilenetv2_rgbplanar_b.ortm"
DEFAULT_INPUT_A = "input/ILSVRC2012_val_00024327.rgb"
DEFAULT_MODEL_B = "model/tiny-yolov3_yuv420sp_b.ortm"
RECOVER_HINT = ("If the board process crashed, run "
                "/opt/ompmod/load_lq560v100 -a on the board to reset media.")


def nv12_to_rgb(width, height, nv12):
    """BT.601 full-range NV12 -> RGB uint8 ndarray (HxWx3)."""
    y_size = width * height
    y = np.frombuffer(nv12, np.uint8, count=y_size).reshape(height, width)
    uv = np.frombuffer(nv12, np.uint8, count=y_size // 2, offset=y_size)
    uv = uv.reshape(height // 2, width)
    u = uv[:, 0::2].repeat(2, axis=0).repeat(2, axis=1).astype(np.int32) - 128
    v = uv[:, 1::2].repeat(2, axis=0).repeat(2, axis=1).astype(np.int32) - 128
    y16 = y.astype(np.int32) - 16
    r = ((298 * y16 + 409 * v + 128) >> 8).clip(0, 255).astype(np.uint8)
    g = ((298 * y16 - 100 * u - 208 * v + 128) >> 8).clip(0, 255).astype(np.uint8)
    b = ((298 * y16 + 516 * u + 128) >> 8).clip(0, 255).astype(np.uint8)
    return np.stack([r, g, b], axis=2)


def control_frame(command):
    payload = bytes([command])
    header = HDR.pack(MAGIC_INT, 1, 0, T_CONTROL, 0, 0, 0,
                      len(payload), len(payload), nsr.crc32(payload))
    return header + payload


class GuiApp:
    def __init__(self, root, args):
        self.root = root
        self.args = args
        self.lock = threading.Lock()
        self.latest = None    # (seq, rgb ndarray)
        self.stopping = False
        self.stop_sent_at = None
        self.eof = False
        self.error = None
        self.client = None
        self.channel_stdin = None
        self.photo = None
        self.photo_seq = None

        self.canvas = tk.Canvas(root, width=DISPLAY_W, height=DISPLAY_H, bg="black")
        self.canvas.grid(row=0, column=0, padx=8, pady=8)
        self.info = tk.StringVar(value="connecting...")
        tk.Label(root, textvariable=self.info, justify="left", anchor="nw").grid(row=0, column=1, sticky="n")
        self.stop_btn = tk.Button(root, text="STOP", command=self.request_stop)
        self.stop_btn.grid(row=1, column=0, columnspan=2, pady=4)
        root.protocol("WM_DELETE_WINDOW", self.request_stop)

    def open_stream(self):
        if self.args.playback:
            return open(self.args.playback, "rb")
        client = paramiko.SSHClient()
        client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        try:
            client.connect(self.args.host, username=self.args.user,
                           password=self.args.password, timeout=8,
                           allow_agent=False, look_for_keys=False)
            client.get_transport().set_keepalive(5)
        except Exception as exc:
            self.error = "SSH connect failed: %s" % exc
            return None
        self.client = client
        cmd = ("cd %s && ./bin/sample_dual_model_abab %s %s %s camera %d none 3 %d"
               % (self.args.board_dir, self.args.model_a, self.args.input_a, self.args.model_b, self.args.repeat, self.args.fps))
        try:
            stdin, stdout, _stderr = client.exec_command(cmd, get_pty=False)
        except Exception as exc:
            self.error = "SSH exec failed: %s" % exc
            return None
        self.channel_stdin = stdin
        return stdout

    def start(self):
        stream = self.open_stream()
        if stream is None:
            self.finish()
            return
        threading.Thread(target=self.reader, args=(stream,), daemon=True).start()
        self.root.after(50, self.tick)

    def reader(self, stream):
        try:
            for frame in nsr.iter_frames(stream):
                if frame[0] in ("resync", "crc_error"):
                    continue
                _version, _flags, ftype, _model, seq, _ts, payload = frame
                if ftype == nsr.T_IMAGE:
                    image = nsr.parse_image(payload)
                    rgb = nv12_to_rgb(image["w"], image["h"], image["nv12"])
                    with self.lock:
                        self.latest = (seq, rgb)
        except Exception as exc:
            self.error = str(exc)
        finally:
            self.eof = True
            try:
                stream.close()
            except Exception:
                pass

    def tick(self):
        if self.eof or self.error:
            self.finish()
            return
        if self.stopping and self.stop_sent_at and time.monotonic() - self.stop_sent_at > 12:
            self.error = "board did not exit after STOP"
            self.finish()
            return
        with self.lock:
            latest = self.latest
        if latest is not None and latest[0] != self.photo_seq:
            self.render(latest)
        self.info.set("seq: %s" % (latest[0] if latest else "-"))
        self.root.after(max(20, 1000 // max(1, self.args.fps)), self.tick)

    def render(self, latest):
        seq, rgb = latest
        image = Image.fromarray(rgb).resize((DISPLAY_W, DISPLAY_H), Image.BILINEAR)
        self.photo = ImageTk.PhotoImage(image)
        self.canvas.delete("all")
        self.canvas.create_image(0, 0, anchor="nw", image=self.photo)
        self.photo_seq = seq

    def request_stop(self):
        if self.stopping:
            return
        self.stopping = True
        self.stop_sent_at = time.monotonic()
        try:
            if self.channel_stdin is not None:
                self.channel_stdin.write(control_frame(CTRL_STOP))
                self.channel_stdin.flush()
        except Exception as exc:
            self.error = "STOP send failed: %s" % exc
        self.stop_btn.config(state="disabled", text="stopping...")

    def finish(self):
        if self.error:
            messagebox.showerror("Stream error", "%s\n%s" % (self.error, RECOVER_HINT))
        elif self.eof and not self.stopping:
            messagebox.showwarning("Stream ended", RECOVER_HINT)
        try:
            if self.client is not None:
                self.client.close()
        except Exception:
            pass
        self.root.destroy()


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--host", default="192.168.1.101")
    parser.add_argument("--user", default="root")
    parser.add_argument("--password", default="123456")
    parser.add_argument("--fps", type=int, default=10, choices=range(1, 31))
    parser.add_argument("--board-dir", default="/data/npu_demo")
    parser.add_argument("--repeat", type=int, default=1000000)
    parser.add_argument("--model-a", default=DEFAULT_MODEL_A)
    parser.add_argument("--input-a", default=DEFAULT_INPUT_A)
    parser.add_argument("--model-b", default=DEFAULT_MODEL_B)
    parser.add_argument("--playback",
                        help="replay a recorded stream file instead of SSH")
    args = parser.parse_args()

    root = tk.Tk()
    root.title("LQ560V100 dual-model demo")
    GuiApp(root, args).start()
    root.mainloop()


if __name__ == "__main__":
    main()
