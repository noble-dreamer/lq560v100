#!/usr/bin/env python3
"""Decode the dual_model_abab SSH stream on the host (see README for usage)."""
import argparse
import binascii
import json
import os
import struct
import sys
import zlib

MAGIC = b"NPST"
MAGIC_INT = 0x5453504E
HDR = struct.Struct("<I BBBB I Q I I I")
HEADER_SIZE = HDR.size

T_SYNC, T_RESULT, T_TENSOR = 1, 2, 3
KIND_CLASSIFY, KIND_DETECT = 1, 2
FLAG_COMPRESSED = 0x01
TYPE_NAME = {
    T_SYNC: "sync",
    T_RESULT: "result",
    T_TENSOR: "tensor",
    4: "ack",
    5: "control",
    6: "error",
}

def log(*args):
    print(*args, file=sys.stderr)

def crc32(data):
    return binascii.crc32(data) & 0xFFFFFFFF

def iter_frames(stream):
    """Yield (version, flags, ftype, model_id, seq, ts_us, payload)."""
    pending = b""
    while True:
        index = pending.find(MAGIC)
        if index == -1:
            keep = pending[-3:] if len(pending) > 3 else pending
            chunk = stream.read(65536)
            if not chunk:
                if pending:
                    yield ("resync", len(pending))
                return
            pending = keep + chunk
            continue
        if index:
            yield ("resync", index)
            pending = pending[index:]
        if len(pending) < HEADER_SIZE:
            pending += stream.read(HEADER_SIZE - len(pending))
            if len(pending) < HEADER_SIZE:
                return
        raw = pending[:HEADER_SIZE]
        pending = pending[HEADER_SIZE:]
        magic, version, flags, ftype, model_id, seq, ts_us, orig_len, plen, crc = \
            HDR.unpack(raw)
        if magic != MAGIC_INT or version > 1:
            yield ("resync", HEADER_SIZE)
            continue
        while len(pending) < plen:
            chunk = stream.read(plen - len(pending))
            if not chunk:
                return
            pending += chunk
        wire = pending[:plen]
        pending = pending[plen:]
        if crc32(wire) != crc:
            yield ("crc_error", seq)
            continue
        if flags & FLAG_COMPRESSED:
            try:
                payload = zlib.decompress(wire)
            except zlib.error:
                yield ("crc_error", seq)
                continue
            if len(payload) != orig_len:
                yield ("crc_error", seq)
                continue
        else:
            if len(wire) != orig_len:
                yield ("crc_error", seq)
                continue
            payload = wire
        yield (version, flags, ftype, model_id, seq, ts_us, payload)

def parse_sync(payload):
    version, flags = payload[0], payload[1]
    total = struct.unpack_from("<I", payload, 4)[0]
    models = {}
    for model_id, offset in ((0, 8), (1, 28)):
        name = payload[offset:offset + 16].split(b"\x00", 1)[0].decode("ascii", "replace")
        models[model_id] = {"name": name, "kind": payload[offset + 16]}
    return {"version": version, "tensors": bool(flags & 1), "total_frames": total,
            "models": models}

def parse_result(payload):
    kind = payload[0]
    count = struct.unpack_from("<I", payload, 4)[0]
    if kind == KIND_CLASSIFY:
        entries = []
        for index in range(count):
            offset = 8 + index * 8
            idx, score = struct.unpack_from("<If", payload, offset)
            entries.append([idx, round(score, 6)])
        return "classify", entries
    if kind == KIND_DETECT:
        entries = []
        for index in range(count):
            offset = 8 + index * 24
            x1, y1, x2, y2, score, class_id = struct.unpack_from("<5fI", payload, offset)
            entries.append([round(x1, 2), round(y1, 2), round(x2, 2), round(y2, 2),
                            round(score, 4), class_id])
        return "detect", entries
    return "raw", list(payload)

def parse_tensor(payload):
    tensor_index, dtype, dim_size = payload[0], payload[1], payload[2]
    dims = struct.unpack_from("<8I", payload, 4)[:dim_size]
    stride = struct.unpack_from("<I", payload, 36)[0]
    return {"index": tensor_index, "dtype": dtype, "dims": list(dims),
            "stride": stride, "data": payload[40:]}

def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("-o", "--outdir", default="npu_stream_out",
                        help="host output directory (default: npu_stream_out)")
    parser.add_argument("--no-tensors", action="store_true",
                        help="do not write tensor payloads to disk")
    args = parser.parse_args()

    os.makedirs(args.outdir, exist_ok=True)
    results_path = os.path.join(args.outdir, "results.jsonl")
    meta_path = os.path.join(args.outdir, "stream.json")
    meta = {"models": {}}
    expected = {}
    counts = {}

    with open(results_path, "w", encoding="utf-8") as results:
        for frame in iter_frames(sys.stdin.buffer):
            if frame[0] == "resync":
                log("resync: skipped %d bytes of non-frame data" % frame[1])
                continue
            if frame[0] == "crc_error":
                log("crc error: dropped frame with seq %s" % frame[1])
                continue

            _, _, ftype, model_id, seq, ts_us, payload = frame
            counts[ftype] = counts.get(ftype, 0) + 1
            type_name = TYPE_NAME.get(ftype, "?")
            model_name = meta["models"].get(model_id, {}).get("name", str(model_id))

            if ftype == T_SYNC:
                meta.update(parse_sync(payload))
                expected = {key: 0 for key in meta["models"]}
                log("sync: version=%d tensors=%s total_frames=%d models=%s"
                    % (meta["version"], meta["tensors"], meta["total_frames"],
                       {mid: m["name"] for mid, m in meta["models"].items()}))
                with open(meta_path, "w", encoding="utf-8") as meta_file:
                    json.dump(meta, meta_file, indent=2)
                continue

            if ftype == T_RESULT:
                if model_id in expected and seq != expected[model_id]:
                    log("seq gap: %s expected %d got %d"
                        % (model_name, expected[model_id], seq))
                expected[model_id] = seq + 1
                kind_name, entries = parse_result(payload)
                record = {"seq": seq, "model": model_name, "kind": kind_name,
                          "data": entries, "ts_us": ts_us}
                results.write(json.dumps(record) + "\n")
                if kind_name == "classify" and entries:
                    top = ", ".join("%d(%.4f)" % (idx, score) for idx, score in entries)
                    print("[%s] frame %d top-k: %s" % (model_name, seq, top))
                elif kind_name == "detect":
                    print("[%s] frame %d detections: %d"
                          % (model_name, seq, len(entries)))
                continue

            if ftype == T_TENSOR:
                tensor = parse_tensor(payload)
                if not args.no_tensors:
                    name = "%s_frame%04d_out%d.bin" % (model_name, seq, tensor["index"])
                    with open(os.path.join(args.outdir, name), "wb") as tensor_file:
                        tensor_file.write(tensor["data"])
                    log("tensor: wrote %s (%d bytes, dims=%s)"
                        % (name, len(tensor["data"]), tensor["dims"]))
                continue

            log("%s: model=%s seq=%d len=%d" % (type_name, model_name, seq, len(payload)))

    log("stream finished: %s" % counts)

if __name__ == "__main__":
    main()
