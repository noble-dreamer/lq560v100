#!/bin/sh
# 上位机 GUI 启动脚本：板端 stream=3 相机画面 + 检测框实时显示。
# 复用 SSH 通道，不新增端口；关闭窗口/STOP 会让板端干净退出。
# 用法：
#   ./host/run_gui.sh                        # 默认 10fps 连接 192.168.1.101
#   BOARD_FPS=20 ./host/run_gui.sh           # 20fps
#   BOARD_PASSWORD=xxx ./host/run_gui.sh     # 自定义密码
#   ./host/run_gui.sh --playback /tmp/s.bin  # 离线回放，不连板子
# 其余参数原样透传给 host/npu_gui.py。

DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

BOARD_IP=${BOARD_IP:-192.168.1.101}
BOARD_USER=${BOARD_USER:-root}
BOARD_PASSWORD=${BOARD_PASSWORD:-123456}
BOARD_FPS=${BOARD_FPS:-10}
BOARD_DIR=${BOARD_DIR:-/data/npu_demo}

command -v python3 >/dev/null 2>&1 || { echo "缺少 python3"; exit 1; }
python3 - <<'PY' || exit 1
import importlib
for module in ("tkinter", "PIL", "numpy", "paramiko"):
    importlib.import_module(module)
PY

exec python3 "$DIR/npu_gui.py" \
  --host "$BOARD_IP" --user "$BOARD_USER" --password "$BOARD_PASSWORD" \
  --fps "$BOARD_FPS" --board-dir "$BOARD_DIR" "$@"
