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

# ===== Step 1: Check current state =====
Write-Output "=== Step 1: Check current state ==="
Send-Cmd "ifconfig usb0 2>/dev/null; echo '---'; ls /sys/kernel/config/usb_gadget/ether/UDC 2>/dev/null && cat /sys/kernel/config/usb_gadget/ether/UDC 2>/dev/null; echo '---'; ps | grep sshd"

# ===== Step 2: Ensure RNDIS is up (in case it dropped) =====
Write-Output "=== Step 2: Ensure RNDIS gadget ==="
# Check if ether gadget exists, if not create it
Send-Cmd "ls /sys/kernel/config/usb_gadget/ether/ 2>/dev/null && echo 'ether exists' || echo 'ether missing'"

# ===== Step 3: Backup current user_init.sh =====
Write-Output "=== Step 3: Backup user_init.sh ==="
Send-Cmd "cp /opt/user_init.sh /opt/user_init.sh.camera_bak 2>/dev/null; echo 'backup done'; ls -la /opt/user_init.sh*"

# ===== Step 4: Create /opt/bin/ if needed =====
Write-Output "=== Step 4: Ensure /opt/bin/ ==="
Send-Cmd "mkdir -p /opt/bin; ls -la /opt/bin/"

# ===== Step 5: Write usb-ether.sh =====
Write-Output "=== Step 5: Write usb-ether.sh ==="
# Use a simplified version that doesn't depend on env vars
$usbEtherScript = @'
#!/bin/sh
# USB RNDIS gadget bring-up
VID="0x3ef5"
PID="0x15b0"
MANUFACTURER="Oritek"
PRODUCT="RNDIS"
SERIALNUMBER="123456"
BOARD_IP="192.168.1.101"
UDC_NAME="$(ls /sys/class/udc/ 2>/dev/null | head -1)"
GADGET_DIR="/sys/kernel/config/usb_gadget/ether"

mount -t configfs none /sys/kernel/config/ 2>/dev/null

if [ -z "$UDC_NAME" ]; then
    echo "ERROR: no UDC found"
    exit 1
fi

# Release camera gadget if present
if [ -f /sys/kernel/config/usb_gadget/camera/UDC ]; then
    CAMERA_UDC="$(cat /sys/kernel/config/usb_gadget/camera/UDC 2>/dev/null)"
    if [ -n "$CAMERA_UDC" ]; then
        echo "" > /sys/kernel/config/usb_gadget/camera/UDC 2>/dev/null
        sleep 1
    fi
fi

# Already bound
if [ -d "$GADGET_DIR" ] && [ -n "$(cat $GADGET_DIR/UDC 2>/dev/null)" ]; then
    ifconfig usb0 "$BOARD_IP" 2>/dev/null
    exit 0
fi

mkdir -p "$GADGET_DIR"
cd "$GADGET_DIR" || exit 1

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

echo "$UDC_NAME" > UDC
sleep 0.5

ifconfig usb0 "$BOARD_IP"
'@

# Write via heredoc, chunk by chunk
Write-Output "Writing usb-ether.sh..."
$port.Write("cat > /opt/bin/usb-ether.sh <<'ENDOFSCRIPT'`n")
Start-Sleep -Milliseconds 200
foreach ($line in ($usbEtherScript -split "`n")) {
    $port.Write("$line`n")
    Start-Sleep -Milliseconds 30
}
$port.Write("ENDOFSCRIPT`n")
Start-Sleep -Milliseconds 500
Write-Output $port.ReadExisting()

# Set executable
Send-Cmd "chmod +x /opt/bin/usb-ether.sh; ls -la /opt/bin/usb-ether.sh"

$port.Close()
