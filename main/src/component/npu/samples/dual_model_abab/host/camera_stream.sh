#!/bin/sh
# 主机端接收相机模式（inputB=camera）流式结果。
# 用法：REPEAT=30 ./host/camera_stream.sh
# 环境变量：BOARD_IP / BOARD_USER / REPEAT，默认 192.168.1.101 / root / 30。
# 认证方式与 benchmark_stream.sh 相同：主机需能 ssh root@BOARD_IP。

BOARD_IP=${BOARD_IP:-192.168.1.101}
BOARD_USER=${BOARD_USER:-root}
REPEAT=${REPEAT:-30}
CAMERA_FPS=${CAMERA_FPS:-10}
OUTDIR=./camera_stream_out
HOSTDIR=./camera_stream_logs

MODEL_A=/data/npu_demo/model/mobilenetv2_rgbplanar_b.ortm
INPUT_A=/data/npu_demo/input/ILSVRC2012_val_00024327.rgb
MODEL_B=/data/npu_demo/model/tiny-yolov3_yuv420sp_b.ortm

mkdir -p "$OUTDIR" "$HOSTDIR"

# 板端跑相机模式 stream=1：stdout 只有协议帧，普通日志走 ssh 的 stderr
START=$(date +%s)
echo "config: repeat=$REPEAT camera_fps=$CAMERA_FPS"
ssh -T "$BOARD_USER@$BOARD_IP" "cd /data/npu_demo \
  && ./bin/sample_dual_model_abab \
       $MODEL_A $INPUT_A $MODEL_B camera $REPEAT none 1 $CAMERA_FPS" \
  2>"$HOSTDIR/board_stderr.log" \
  | python3 host/npu_stream_receiver.py -q -o "$OUTDIR" 2>"$HOSTDIR/receiver_stderr.log"
END=$(date +%s)
echo "wall seconds: $((END - START))"

python3 - "$OUTDIR/results.jsonl" <<'PY'
import json
import sys

recs = [json.loads(line) for line in open(sys.argv[1], encoding="utf-8")]
cls = [r for r in recs if r.get("kind") == "classify"]
det = [r for r in recs if r.get("kind") == "detect"]


def dur_stats(rs):
    d = [r["duration_ms"] for r in rs]
    return min(d), sum(d) / len(d), max(d)


print("results total=%d  A(classify)=%d  B(detect)=%d" % (len(recs), len(cls), len(det)))
if cls:
    print("A duration min/avg/max = %.1f/%.1f/%.1f ms" % dur_stats(cls))
if det:
    print("B duration min/avg/max = %.1f/%.1f/%.1f ms" % dur_stats(det))
    boxes = [len(r["data"]) for r in det]
    print("B boxes total=%d, frames_with_boxes=%d/%d"
          % (sum(boxes), sum(1 for b in boxes if b), len(det)))
    print("B src_w/src_h = %s/%s" % (det[0].get("src_w"), det[0].get("src_h")))
    seqs = [r["seq"] for r in det]
    gaps = [b - a for a, b in zip(seqs, seqs[1:])]
    print("B seq gap max = %d" % (max(gaps) if gaps else 0))
    shown = 0
    for r in det:
        if r["data"]:
            print("frame %d detections:" % r["seq"])
            for x1, y1, x2, y2, score, cid in r["data"]:
                print("  class=%d score=%.3f box=(%.1f,%.1f)-(%.1f,%.1f)"
                      % (cid, score, x1, y1, x2, y2))
            shown += 1
            if shown >= 3:
                break
PY

if grep -qE 'resync|crc error|seq gap' "$HOSTDIR/receiver_stderr.log"; then
    echo "STREAM ERRORS FOUND:"
    grep -E 'resync|crc error|seq gap' "$HOSTDIR/receiver_stderr.log"
else
    echo "stream clean: no resync / crc error / seq gap"
fi
