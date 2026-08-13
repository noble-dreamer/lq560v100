#!/bin/sh
# Hook invoked by usb_receiver when entering / leaving upgrade mode.
# The PC tool's upgrade click must stop the UVC streaming business
# immediately, without any manual step on the device. Adjust the process
# names to the actual camera application of the product.

case "$1" in
    stop)
        killall uvc_app 2>/dev/null
        killall imu_app 2>/dev/null
        ;;
    start)
        /etc/usb_mode_ctl.sh uvc >/dev/null 2>&1 &
        ;;
esac
exit 0
