#!/bin/sh
# =============================================================================
# rndis_boot.sh - Default product boot: RNDIS + SSH + COM upgrade trigger
#
# Run from /etc/init.d/S90user after userfs (/opt) and data (/data) are
# mounted. The USB port presents RNDIS+ACM:
#   usb0      -> SSH (192.168.1.101)
#   ttyGS0    -> upgrade_triggerd protocol (UPGRADE:5AA55AA5 / VERSION?)
#
# imu_app/uvc_app are intentionally NOT started. To boot the legacy UVC
# camera mode instead, create /opt/.uvc_mode and reboot.
# =============================================================================

user_mount_dir=/opt
export LD_LIBRARY_PATH=$user_mount_dir/lib/:$LD_LIBRARY_PATH
export PATH=$PATH:/opt/bin

# --- Standard module init (same as the legacy /opt/user_init.sh) ---
if [ -e $user_mount_dir/ompmod/load_lq560v100 ];then
    cd $user_mount_dir/ompmod/
    ./load_lq560v100 -i
fi
if [ -e $user_mount_dir/ompmod/load_lq560v100_quickboot_stage_2 ];then
    cd $user_mount_dir/ompmod/
    ./load_lq560v100_quickboot_stage_2 -i
fi
if [ -e $user_mount_dir/kermod/load_kermod.sh ];then
    cd $user_mount_dir/kermod/
    ./load_kermod.sh
fi

echo device > /proc/10320000.usb30drd/mode
sleep 2

# USB Gadget: RNDIS + ACM. Stop any factory camera gadget first; the safe
# kill-wait-unbind order lives in usb_mode_ctl.sh.
[ -x /etc/usb_mode_ctl.sh ] && /etc/usb_mode_ctl.sh stop
/etc/usb-ether.sh
( sleep 5; [ -z "$(cat /sys/kernel/config/usb_gadget/ether/UDC 2>/dev/null)" ] && /etc/usb-ether.sh ) &

# OpenSSH is shipped in the datafs image at /data/openssh and survives OTA.
if [ -x /data/openssh/start_sshd.sh ]; then
    chmod +x /data/openssh/start_sshd.sh /data/openssh/bin/* 2>/dev/null
    /data/openssh/start_sshd.sh &
fi
