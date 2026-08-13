#!/bin/sh
######################################################
# set_resolution()  fi ll resoluton
# $1 formats
# $2 base_path
# $3 0 or 1: means need fi ll dwMaxVideoFrameBufferSize
# $4 format name string
#######################################################
function set_resolution()
{
    for i in $1
    do
        echo "$i"
        case $i in
        "stereo")
            mkdir $2/stereo/
            echo -e "333333" > $2/stereo/dwFrameInterval
            echo "333333" > $2/stereo/dwDefaultFrameInterval
            echo "442368000" > $2/stereo/dwMaxBitRate
            if [ $3 -eq 1 ]; then
                echo "1843200" > $2/stereo/dwMaxVideoFrameBufferSize
            fi
            echo "442368000" > $2/stereo/dwMinBitRate
            echo "480"  > $2/stereo/wHeight
            echo "1920" > $2/stereo/wWidth
            ;;
        "calib")
            mkdir $2/calib/
            echo -e "333333" > $2/calib/dwFrameInterval
            echo "333333" > $2/calib/dwDefaultFrameInterval
            echo "294912000" > $2/calib/dwMaxBitRate
            if [ $3 -eq 1 ]; then
                echo "1228800" > $2/calib/dwMaxVideoFrameBufferSize
            fi
            echo "294912000" > $2/calib/dwMinBitRate
            echo "480"  > $2/calib/wHeight
            echo "1280" > $2/calib/wWidth
            ;;
        "depth")
            mkdir $2/depth/
            echo -e "333333" > $2/depth/dwFrameInterval
            echo "333333" > $2/depth/dwDefaultFrameInterval
            echo "147456000" > $2/depth/dwMaxBitRate
            if [ $3 -eq 1 ]; then
                echo "614400" > $2/depth/dwMaxVideoFrameBufferSize
            fi
            echo "147456000" > $2/depth/dwMinBitRate
            echo "480" > $2/depth/wHeight
            echo "640" > $2/depth/wWidth
            ;;
        *)
            echo "$4 $i is invalid!"
            ;;
        esac
    done
}

######################################################
# set_format()  fill format
# no argument
#######################################################
function set_format()
{
    #YUV
    if [ -n "$YUYV" ]; then
        echo "Add YUYV..."
        mkdir streaming/uncompressed/yuyv/
        echo -en "\x59\x55\x59\x32\x00\x00\x10\x00\x80\x00\x00\xaa\x00\x38\x9b\x71" > streaming/uncompressed/yuyv/guidFormat
        echo 16 > streaming/uncompressed/yuyv/bBitsPerPixel
        set_resolution "$YUYV" streaming/uncompressed/yuyv/ 1 "YUYV"
        ln -s streaming/uncompressed/yuyv/ streaming/header/h/
        echo -e "Added YUYV\n"
    fi
}

mount -t configfs none /sys/kernel/config/ 2>/dev/null
# Normal-mode gadget = UVC + CDC-ACM (no vendor/FFS). Managed solely by
# usb_mode_ctl, which binds the UDC; this helper only builds the functions.
mkdir -p /sys/kernel/config/usb_gadget/camera
cd /sys/kernel/config/usb_gadget/camera
# Composite device class (IAD): Windows loads usbccgp and splits UVC/ACM into
# usbvideo / usbser. Required for a multi-function composite to enumerate right.
echo "0xEF" > bDeviceClass
echo "0x02" > bDeviceSubClass
echo "0x01" > bDeviceProtocol
echo $VID > idVendor
echo $PID > idProduct
# bcdDevice: bump this when UVC/ACM descriptors change, so Windows discards its
# cached device descriptor and re-parses (avoids stale-descriptor enumeration).
echo 0x0101 > bcdDevice
mkdir -p strings/0x409
echo ${MANUFACTURER:-$MANUFACTURED} > strings/0x409/manufacturer
echo $PRODUCT > strings/0x409/product
echo $SERIALNUMBER > strings/0x409/serialnumber

make_function()
{
    mkdir functions/uvc.usb${1}
    cd functions/uvc.usb${1}
    mkdir control/header/h/
    echo $BCDUVC > control/header/h/bcdUVC
    echo "48000000" > control/header/h/dwClockFrequency
    ln -s control/header/h/ control/class/fs/
    [ -d control/class/hs ] && ln -s control/header/h/ control/class/hs/
    ln -s control/header/h/ control/class/ss/

cat <<EOF> control/terminal/camera/default/bmControls
$CamControl1
$CamControl2
$CamControl3
EOF
cat <<EOF> control/processing/default/bmControls
$ProcControl1
$ProcControl2
$ProcControl3
EOF
cat <<EOF> control/encoding/default/bmControls
$EcdControl1
$EcdControl2
$EcdControl3
EOF
cat <<EOF> control/encoding/default/bmControlsRuntime
$EcdRtControl1
$EcdRtControl2
$EcdRtControl3
EOF
cat <<EOF> control/extension/default/bmControls
$ExtControl1
$ExtControl2
EOF

    mkdir streaming/header/h/
    set_format
    ln -s streaming/header/h/ streaming/class/fs/
    ln -s streaming/header/h/ streaming/class/hs/
    ln -s streaming/header/h/ streaming/class/ss/
    if [ "$TransferMode" == "bulk" ];then
        echo -e "setting mode to bulk"
        echo "bulk" > streaming_transfer
        echo 32768 > streaming_maxpacket
        echo 15 > streaming_maxburst
    else
        echo -e "setting mode to isoc"
        echo "isoc" > streaming_transfer
        echo 1024 > streaming_maxpacket
    fi
    if [ "$PerfMode" == "v4l2" ]; then
        echo "v4l2" > performance_mode
    fi
    #-Create and setup configuration
    cd ../../
}

for i in `seq 0 $(expr ${UVC_DEVICE_CNT} - 1)` ; do
    make_function $i
done

mkdir -p configs/c.1/
echo "0x01" > configs/c.1/MaxPower
echo "0xc0" > configs/c.1/bmAttributes
mkdir -p configs/c.1/strings/0x409/
echo "Config  1" > configs/c.1/strings/0x409/configuration
for i in `seq 0 $(expr ${UVC_DEVICE_CNT} - 1)` ; do
    ln -s functions/uvc.usb${i}/ configs/c.1/
done

# CDC-ACM virtual serial port: the always-on COM port the PC uses to send the
# "enter upgrade" command (upgrade_triggerd reads /dev/ttyGS0). Linked after UVC
# so UVC keeps the leading interface numbers. No vendor/FFS here -> no WCID issue.
mkdir functions/acm.usb0
ln -s functions/acm.usb0 configs/c.1/

# usb_mode_ctl is the only UDC owner. This helper creates UVC + ACM only.
if [ -z "$USB_MODE_CTL_MANAGED" ]; then
    UDC=$(ls /sys/class/udc | head -1)
    [ -n "$UDC" ] && echo "$UDC" > /sys/kernel/config/usb_gadget/camera/UDC
fi
