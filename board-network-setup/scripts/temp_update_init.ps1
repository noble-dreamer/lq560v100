$port = New-Object System.IO.Ports.SerialPort('COM12', 115200, 'None', 8, 'One')
$port.ReadTimeout = 1000
$port.DtrEnable = $true
$port.RtsEnable = $true
$port.Open()
Start-Sleep -Milliseconds 300
$null = $port.ReadExisting()

function Send-Cmd($cmd, $waitMs = 1000) {
    $port.Write("$cmd`n")
    Start-Sleep -Milliseconds $waitMs
    Write-Output $port.ReadExisting()
}

# ===== Write new user_init.sh =====
Write-Output "=== Writing new user_init.sh ==="

# Send the heredoc line by line
$newInit = @'
#!/bin/sh
# =============================================================================
# user_init.sh - Boot script with RNDIS networking + SSH
# Original backed up to: /opt/user_init.sh.camera_bak
# =============================================================================
user_mount_dir=/opt
export LD_LIBRARY_PATH=$user_mount_dir/lib/:$LD_LIBRARY_PATH
export PATH=$PATH:/opt/bin

# --- Kernel modules ---
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

# --- USB RNDIS networking ---
if [ -x /opt/bin/usb-ether.sh ]; then
    /opt/bin/usb-ether.sh
fi

# --- OpenSSH ---
if [ -x /data/openssh/start_sshd.sh ]; then
    /data/openssh/start_sshd.sh &
fi

# --- App startup (no camera/im) ---
cd /opt/stereo
chmod a+x uvc_app
./run.sh
cd
'@

$port.Write("cat > /opt/user_init.sh <<'ENDOFINIT'`n")
Start-Sleep -Milliseconds 300
foreach ($line in ($newInit -split "`n")) {
    $port.Write("$line`n")
    Start-Sleep -Milliseconds 50
}
$port.Write("ENDOFINIT`n")
Start-Sleep -Milliseconds 1000
$out = $port.ReadExisting()
Write-Output $out

# ===== Fix CRLF and chmod =====
Write-Output "=== Fix and verify ==="
Send-Cmd "sed -i 's/\r$//' /opt/user_init.sh; chmod +x /opt/user_init.sh; echo 'done'"

# ===== Verify content =====
Write-Output "=== Verify user_init.sh ==="
Send-Cmd "wc -l /opt/user_init.sh"
Send-Cmd "head -5 /opt/user_init.sh"
Send-Cmd "grep -n 'usb-ether\|start_sshd\|run.sh' /opt/user_init.sh"

Write-Output "=== Full new user_init.sh ==="
Send-Cmd "cat /opt/user_init.sh" 3000

$port.Close()
