#!/bin/sh
# =============================================================================
# nfs-mount.sh - Mount the NFS share on the board
# Path on device: /opt/bin/nfs-mount.sh (suggested)
#
# Usage:
#   nfs-mount.sh              # mount (default)
#   nfs-mount.sh u | umount   # unmount
#   nfs-mount.sh status       # show mount status
#
# All parameters can be overridden via environment variables:
#   SERVER, EXPORT, MOUNTPOINT, OPTS
# =============================================================================

# --- Config (override via env) ---
SERVER="${SERVER:-192.168.1.3}"
EXPORT="${EXPORT:-/home/xiao/dev_ws/V100/SD3589CV100/SD3589C_SDK_V100R001C00SPC001/share_nfs}"
MOUNTPOINT="${MOUNTPOINT:-/tmp/nfs}"
OPTS="${OPTS:-nolock,vers=3,proto=tcp,soft,timeo=30,rsize=8192,wsize=8192}"

case "$1" in
    u|umount|-u)
        umount "$MOUNTPOINT" 2>/dev/null && echo "umount $MOUNTPOINT OK" \
            || echo "umount $MOUNTPOINT failed (not mounted?)"
        exit 0
        ;;
    status|s)
        if mount | grep -q " on $MOUNTPOINT type nfs"; then
            echo "MOUNTED: $(mount | grep " on $MOUNTPOINT ")"
        else
            echo "NOT mounted"
        fi
        exit 0
        ;;
esac

# Already mounted?
if mount | grep -q " on $MOUNTPOINT type nfs"; then
    echo "already mounted: $(mount | grep $MOUNTPOINT)"
    exit 0
fi

mkdir -p "$MOUNTPOINT"

echo "mounting $SERVER:$EXPORT -> $MOUNTPOINT ..."
mount -t nfs -o "$OPTS" "$SERVER:$EXPORT" "$MOUNTPOINT" || {
    echo "ERROR: mount failed"
    echo "Hints:"
    echo "  - Is PC running nfs_up.ps1? (WSL NFS + portproxy)"
    echo "  - ping $SERVER from board?"
    echo "  - Board has route to WSL? (ip route)"
    exit 1
}

echo "OK:"
mount | grep "$MOUNTPOINT"
ls -la "$MOUNTPOINT"
