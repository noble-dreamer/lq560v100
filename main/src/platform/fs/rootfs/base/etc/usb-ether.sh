#!/bin/sh
# =============================================================================
# usb-ether.sh - Bring up USB RNDIS + ACM gadget and board usb0 IP
# Lives in rootfs /etc, so an OTA rootfs update also updates this script.
# =============================================================================

VID="${USB_VID:-0x3ef5}"
PID="${USB_PID:-0x15b0}"
MANUFACTURER="${USB_MANUFACTURER:-Oritek}"
PRODUCT="${USB_PRODUCT:-Ethernet}"
SERIALNUMBER="${USB_SERIALNUMBER:-123456}"
BOARD_IP="${BOARD_IP:-192.168.1.101}"
UDC_NAME="$(ls /sys/class/udc/ 2>/dev/null | head -1)"
GADGET_DIR=/sys/kernel/config/usb_gadget/ether

mount -t configfs none /sys/kernel/config/ 2>/dev/null

if [ -z "$UDC_NAME" ]; then
    echo "ERROR: no UDC found"
    exit 1
fi

# If the factory camera gadget still owns the UDC, use its controller for the
# required stop order: uvc_app must close its V4L2 handle before the UDC is
# unbound. Directly unbinding a live UVC gadget can panic this kernel.
if [ -f /sys/kernel/config/usb_gadget/camera/UDC ]; then
    CAMERA_UDC="$(cat /sys/kernel/config/usb_gadget/camera/UDC 2>/dev/null)"
    if [ -n "$CAMERA_UDC" ]; then
        if [ -x /etc/usb_mode_ctl.sh ]; then
            /etc/usb_mode_ctl.sh stop
        else
            echo "" > /sys/kernel/config/usb_gadget/camera/UDC 2>/dev/null
        fi
        sleep 1
    fi
fi

mkdir -p "$GADGET_DIR"
cd "$GADGET_DIR" || exit 1

# Already bound: only ensure the board IP. OpenSSH is the intended remote
# access service.
if [ -n "$(cat UDC 2>/dev/null)" ]; then
    ifconfig usb0 "$BOARD_IP" 2>/dev/null
    exit 0
fi

echo "0x0300" > bcdUSB
echo "0xef" > bDeviceClass
echo "2" > bDeviceSubClass
echo "$VID" > idVendor
echo "$PID" > idProduct
echo "0x3000" > bcdDevice
echo "0x01" > bDeviceProtocol

mkdir -p strings/0x409 configs/c.1/strings/0x409
echo "$MANUFACTURER" > strings/0x409/manufacturer
echo "$PRODUCT" > strings/0x409/product
echo "$SERIALNUMBER" > strings/0x409/serialnumber

echo "0xC0" > configs/c.1/bmAttributes
echo "1" > configs/c.1/MaxPower
echo "RNDIS" > configs/c.1/strings/0x409/configuration

echo "1" > os_desc/use
echo "0xcd" > os_desc/b_vendor_code
echo "MSFT100" > os_desc/qw_sign

mkdir -p functions/rndis.usb0
echo "RNDIS" > functions/rndis.usb0/os_desc/interface.rndis/compatible_id
echo "5162001" > functions/rndis.usb0/os_desc/interface.rndis/sub_compatible_id
[ -e configs/c.1/rndis.usb0 ] || ln -s functions/rndis.usb0 configs/c.1/
[ -e os_desc/c.1 ] || ln -s configs/c.1 os_desc

mkdir -p functions/acm.usb0
[ -e configs/c.1/acm.usb0 ] || ln -s functions/acm.usb0 configs/c.1/

echo "$UDC_NAME" > UDC
sleep 0.5

ifconfig usb0 "$BOARD_IP"
# IMU_APP stays disabled. ttyGS0 is reserved for the COM-port upgrade trigger
# protocol (upgrade_triggerd), NOT a getty login shell, so the legacy PC
# upgrade tool keeps working over the ACM COM port. SSH runs over RNDIS/usb0.
pidof upgrade_triggerd >/dev/null 2>&1 || /etc/upgrade_triggerd &
