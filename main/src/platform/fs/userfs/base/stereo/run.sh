#!/bin/sh
export UVC_DEVICE_CNT=1
export BCDUVC=0x0110
export VID="0x3ef5"
export PID="0x0ef0"
export MANUFACTURED="Oritek"
export PRODUCT="Stereo-UVC"
export SERIALNUMBER="325468953"

export YUYV="depth calib stereo"

export TransferMode="bulk"
export PerfMode="v4l2"

export CamControl1=0x0A
export CamControl2=0xAA
export CamControl3=0x00
export ProcControl1=0x7F
export ProcControl2=0x17
export ProcControl3=0x00
export EcdControl1=0x00
export EcdControl2=0x00
export EcdControl3=0x00
export EcdRtControl1=0x00
export EcdRtControl2=0x00
export EcdRtControl3=0x00
export ExtControl1=0x20
export ExtControl2=0x00

sh ./usb-uvc.sh || exit 1

# MANAGED mode: usb_mode_ctl owns UDC bind + app start order (bind must happen
# before uvc_app, which needs /dev/video0). Only start apps here when run
# standalone (non-managed), where usb-uvc.sh already bound the UDC itself.
if [ -z "$USB_MODE_CTL_MANAGED" ]; then
    sleep 0.5
    ./uvc_app &
    sleep 0.5
    setsid getty -L 1500000 ttyGS0 vt100 &
    # Do not start imu_app: it continuously sends IMU samples over the USB
    # ACM port, which is reserved for debugging in this build.
    # ./imu_app &
fi
