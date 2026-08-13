#!/bin/sh
# =============================================================================
# user_init.sh - Board boot script for RNDIS + SSH mode
#
# Deploy to: /opt/user_init.sh (back up the current file first)
#
# Direct Windows <-> board SSH uses usb0 only.  No WSL/NFS return route is
# installed here.  IMU_APP is intentionally disabled, so ttyGS0 remains the
# USB ACM getty console used only for recovery/deployment before SSH is up.
# =============================================================================

# --- Standard init ---
user_mount_dir=/opt
export LD_LIBRARY_PATH=$user_mount_dir/lib/:$LD_LIBRARY_PATH
export PATH=$PATH:/opt/bin

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

# USB Gadget: RNDIS + ACM.  Use the board's own safe UVC teardown before
# claiming the shared UDC, even though normal RNDIS boots have no camera gadget.
[ -x /etc/usb_mode_ctl.sh ] && /etc/usb_mode_ctl.sh stop
/opt/bin/usb-ether.sh
( sleep 5; [ -z "$(cat /sys/kernel/config/usb_gadget/ether/UDC 2>/dev/null)" ] && /opt/bin/usb-ether.sh ) &

# Start OpenSSH (FTP uploads arrive without executable permission).
if [ -f /data/openssh/start_sshd.sh ]; then
    chmod +x /data/openssh/start_sshd.sh /data/openssh/bin/*
    /data/openssh/start_sshd.sh &
fi

# Do not call /opt/stereo/run.sh in RNDIS mode: it builds the UVC camera
# gadget and would attempt to bind the UDC already owned by ether.  Do not
# start imu_app or upgrade_triggerd either; getty is started by usb-ether.sh.
