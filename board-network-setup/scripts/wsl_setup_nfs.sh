#!/bin/bash
# =============================================================================
# wsl_setup_nfs.sh - Install + configure NFSv3 server on WSL2
#
# Reads NFS_EXPORT_DIR from environment or uses default.
# Run with:
#   wsl -d <DISTRO> --user root -- bash /path/to/wsl_setup_nfs.sh
#
# Idempotent: safe to re-run.
# =============================================================================
set -e

# --- Config (override via env or config.env sourced before calling) ---
EXPORT_DIR="${NFS_EXPORT_DIR:-/home/xiao/dev_ws/V100/SD3589CV100/SD3589C_SDK_V100R001C00SPC001/share_nfs}"
MOUNTD_PORT="${PORT_MOUNTD:-20048}"
SHARE_USER="${WSL_USER:-xiao}"

echo "=== [1/6] ensure share dir: $EXPORT_DIR ==="
mkdir -p "$EXPORT_DIR"
chown "$SHARE_USER:$SHARE_USER" "$EXPORT_DIR" 2>/dev/null || true
chmod 0777 "$EXPORT_DIR"
ls -ld "$EXPORT_DIR"

echo "=== [2/6] install packages ==="
if ! dpkg -s nfs-kernel-server >/dev/null 2>&1 \
    || ! dpkg -s rpcbind          >/dev/null 2>&1 \
    || ! dpkg -s nfs-common       >/dev/null 2>&1; then
    export DEBIAN_FRONTEND=noninteractive
    apt-get install -y --no-install-recommends nfs-kernel-server rpcbind nfs-common 2>&1 | tail -5
else
    echo "packages already installed"
fi

echo "=== [3/6] fix mountd to fixed port $MOUNTD_PORT ==="
sed -i -E "s|^RPCMOUNTDOPTS=.*|RPCMOUNTDOPTS=\"--manage-gids -p $MOUNTD_PORT\"|" \
    /etc/default/nfs-kernel-server
grep RPCMOUNTDOPTS /etc/default/nfs-kernel-server

echo "=== [4/6] /etc/exports ==="
tmp=$(mktemp)
grep -vF "$EXPORT_DIR" /etc/exports 2>/dev/null > "$tmp" || true
echo "$EXPORT_DIR *(rw,sync,no_subtree_check,no_root_squash,insecure)" >> "$tmp"
mv "$tmp" /etc/exports
cat /etc/exports

echo "=== [5/6] (re)start services ==="
service rpcbind            restart 2>&1 | tail -2 || true
sleep 0.3
service nfs-kernel-server  restart 2>&1 | tail -3 || true
sleep 0.3
exportfs -rav 2>&1
echo "--- exportfs -v ---"
exportfs -v

echo "=== [6/6] status ==="
echo "--- rpcinfo (mountd / nfs / portmap) ---"
rpcinfo -p 127.0.0.1 | grep -E 'mountd|nfs |portmapper' | head -20
echo "--- listening (111/2049/$MOUNTD_PORT) ---"
ss -tunlp 2>/dev/null | grep -E ":(111|2049|$MOUNTD_PORT)\b" || true
echo "--- WSL eth0 IPv4 ---"
ip -4 -o addr show eth0 | awk '{print $4}' | cut -d/ -f1
echo "=== DONE ==="
