#!/bin/sh
# Sole owner of the USB gadget and its UDC. UVC(+ACM) and upgrade are mutually
# exclusive modes; every gadget/UDC change goes through here.
G=/sys/kernel/config/usb_gadget/camera
FFS=/dev/usb-ffs/upgrade
READY=/tmp/usb_receiver.ready
ENTERED=/tmp/usb_upgrade_entered
GETTY_PID=/tmp/usb_getty.pid
LOCK=/tmp/usb_mode_ctl.lock
SESSION=/tmp/usb_upgrade_session
UDC="$(ls /sys/class/udc 2>/dev/null | head -1)"

teardown_gadget()
{
    # Ordered removal: configfs refuses to delete a function while a config
    # still references it, so a single "rm -rf $G" by readdir order can leave
    # hollow function dirs and then fail to remove the gadget. Drop config
    # links first, then config dirs, then functions, then the gadget shell.
    rm -f "$G"/os_desc/c.1 "$G"/configs/c.1/*.usb* "$G"/configs/c.1/ffs.* 2>/dev/null
    rmdir "$G"/configs/c.1/strings/0x409 "$G"/configs/c.1/strings "$G"/configs/c.1 "$G"/configs 2>/dev/null
    rm -rf "$G"/functions/* 2>/dev/null
    rmdir "$G"/os_desc "$G"/strings/0x409 "$G"/strings "$G" 2>/dev/null
}

stop_mode()
{
    # Let any in-flight USB control transfer (XU SET_CUR status stage) finish
    # while uvc_app is still alive to send it.
    sleep 1
    # CRITICAL ORDER: kill the media apps FIRST and wait for them to release
    # their V4L2 fds WHILE THE GADGET IS STILL BOUND. Unbinding the UDC before
    # uvc_app closes /dev/video0 races uvc_v4l2_release()->uvc_function_disconnect
    # against gadget teardown and triggers a NULL-deref panic in
    # usb_gadget_deactivate(). Never unbind-then-kill; always kill-wait-unbind.
    killall uvc_app imu_app usb_receiver 2>/dev/null
    [ -f "$GETTY_PID" ] && kill "$(cat "$GETTY_PID")" 2>/dev/null
    i=0
    while pidof uvc_app imu_app >/dev/null 2>&1 && [ "$i" -lt 30 ]; do
        sleep 0.1
        i=$((i + 1))
    done
    # If anything survived SIGTERM, force-kill before unbind so no V4L2 fd is
    # still held when we tear the gadget down (the panic case above).
    pidof uvc_app imu_app >/dev/null 2>&1 && killall -9 uvc_app imu_app 2>/dev/null
    sleep 1
    # Safe to drop the pullup now (no V4L2 fd held).
    [ -f "$G/UDC" ] && echo '' > "$G/UDC" 2>/dev/null
    umount "$FFS" 2>/dev/null
    rm -f "$READY" "$ENTERED" "$GETTY_PID" "$SESSION"
    teardown_gadget
}

start_uvc()
{
    stop_mode
    [ -n "$UDC" ] || exit 1
    if (cd /opt/stereo && USB_MODE_CTL_MANAGED=1 sh ./run.sh); then
        # bind FIRST so the UVC function registers /dev/video0 before uvc_app
        # opens it (otherwise uvc_app exits with "unable to get real path").
        echo "$UDC" > "$G/UDC" || exit 1
        # Children must NOT inherit the mode lock (fd 9): flock is tied to the
        # open file description, so a long-running child holding fd 9 keeps
        # the lock forever and every later trigger blocks indefinitely.
        (exec 9>&-; cd /opt/stereo && ./uvc_app &)
        # Keep the USB ACM port available for debug output. imu_app continuously
        # publishes IMU samples through this port, so do not start it here.
        # (exec 9>&-; cd /opt/stereo && ./imu_app &)
        # The ACM virtual serial port is a login shell. Do not start
        # upgrade_triggerd here: it would compete with getty for ttyGS0 input.
        (exec 9>&-; setsid getty -L 1500000 ttyGS0 vt100 & echo $! > "$GETTY_PID")
    else
        # UVC bring-up failed (e.g. stereo payload missing on this build): fall
        # back to upgrade-only (norevert) so the device stays recoverable via
        # WinUSB/OTA instead of booting up with no USB at all.
        start_upgrade norevert
    fi
}

start_upgrade()
{
    # $1 = "norevert" -> skip the 30s idle->UVC watchdog (used by the start_uvc
    # fallback, which must stay in upgrade mode rather than cycle every 30s and
    # interrupt an in-progress upgrade).
    stop_mode
    [ -n "$UDC" ] || exit 1
    mkdir -p "$G/strings/0x409" "$G/configs/c.1/strings/0x409" "$FFS" || exit 1
    echo 0x3ef5 > "$G/idVendor"
    echo 0x0ef1 > "$G/idProduct"
    echo 0x0200 > "$G/bcdUSB"
    echo Oritek > "$G/strings/0x409/manufacturer"
    echo Stereo-Upgrade > "$G/strings/0x409/product"
    echo 325468953 > "$G/strings/0x409/serialnumber"
    echo 500 > "$G/configs/c.1/MaxPower"
    echo upgrade > "$G/configs/c.1/strings/0x409/configuration"
    mkdir "$G/functions/ffs.upgrade" || exit 1
    echo 1 > "$G/os_desc/use"
    echo 0xC1 > "$G/os_desc/b_vendor_code"
    echo MSFT100 > "$G/os_desc/qw_sign"
    mount -t functionfs upgrade "$FFS" || exit 1
    /usr/bin/usb_receiver 9>&- &
    i=0
    while [ ! -f "$READY" ] && [ "$i" -lt 50 ]; do
        sleep 0.1
        i=$((i + 1))
    done
    [ -f "$READY" ] || exit 1
    ln -s "$G/functions/ffs.upgrade" "$G/configs/c.1/ffs.upgrade" || exit 1
    ln -s "$G/configs/c.1" "$G/os_desc/c.1" 2>/dev/null
    echo "$UDC" > "$G/UDC" || exit 1
    # A mode switch alone must not strand the product in WinUSB. The receiver
    # creates ENTERED only after CMD_ENTER_UPGRADE is accepted; if the PC sent
    # the trigger but never followed through, revert to UVC after 30s.
    if [ "$1" != "norevert" ]; then
        # Session mark: only THIS invocation's watchdog may revert. A later
        # switch writes a new mark, so a stale watchdog (from a reverted/failed
        # session) won't kill a fresh in-progress one.
        echo $$ > "$SESSION"
        sess=$$
        # the watchdog must not hold the mode lock for its 30s lifetime,
        # and its own "usb_mode_ctl uvc" call would deadlock against it
        (exec 9>&-; sleep 60; [ "$(cat "$SESSION" 2>/dev/null)" = "$sess" ] && [ ! -f "$ENTERED" ] && /etc/usb_mode_ctl.sh uvc) &
    fi
}

mount -t configfs none /sys/kernel/config 2>/dev/null
[ -d /sys/kernel/config/usb_gadget ] || exit 1
for m in /proc/*usb30drd*/mode; do [ -e "$m" ] && echo device > "$m"; done
# Drop concurrent triggers (XU + COM port, or a double-click): a second
# usb_mode_ctl while one is running exits immediately instead of queueing and
# re-enumerating the upgrade device a second time.
exec 9>"$LOCK"
flock -n 9 || exit 0
case "$1" in
    uvc) start_uvc ;;
    upgrade) start_upgrade ;;
    stop) stop_mode ;;
    status) echo "UDC=$(cat "$G/UDC" 2>/dev/null)"; ps | grep -E 'uvc_app|imu_app|usb_receiver|getty' | grep -v grep ;;
    *) echo "usage: $0 {uvc|upgrade|stop|status}" >&2; exit 2 ;;
esac
