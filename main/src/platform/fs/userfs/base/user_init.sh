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
sleep 0.5

# Default USB mode is UVC(+ACM); usb_mode_ctl is the sole gadget/UDC owner.
# It builds the gadget via /opt/stereo/run.sh, binds the UDC, then starts
# uvc_app/imu_app and the COM-port upgrade trigger. If the stereo payload is
# broken it falls back to upgrade-only mode so the device stays recoverable.
if [ -d $user_mount_dir/stereo ];then
    cd $user_mount_dir/stereo/ || exit 1
    chmod a+x *
    /etc/usb_mode_ctl.sh uvc
    cd
fi
