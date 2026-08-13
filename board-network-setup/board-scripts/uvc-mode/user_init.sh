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

cd /opt/stereo
chmod a+x *
./run.sh
cd
