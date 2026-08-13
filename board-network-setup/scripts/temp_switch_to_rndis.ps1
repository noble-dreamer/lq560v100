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

# Step 1: Release camera UDC
Write-Output "=== Step 1: Release camera UDC ==="
Send-Cmd 'echo "" > /sys/kernel/config/usb_gadget/camera/UDC'
Send-Cmd 'cat /sys/kernel/config/usb_gadget/camera/UDC'
Start-Sleep -Seconds 1

# Step 2: Create ether gadget
Write-Output "=== Step 2: Create ether gadget ==="
Send-Cmd 'mkdir -p /sys/kernel/config/usb_gadget/ether'
Send-Cmd 'cd /sys/kernel/config/usb_gadget/ether && echo "0x3ef5" > idVendor && echo "0x15b0" > idProduct && echo "0x0300" > bcdUSB && echo "0xef" > bDeviceClass && echo "2" > bDeviceSubClass && echo "0x01" > bDeviceProtocol && echo "0x3000" > bcdDevice'

# Step 3: Configure strings
Write-Output "=== Step 3: Configure strings ==="
Send-Cmd 'mkdir -p /sys/kernel/config/usb_gadget/ether/strings/0x409 && echo "Linux" > /sys/kernel/config/usb_gadget/ether/strings/0x409/manufacturer && echo "RNDIS+ACM Gadget" > /sys/kernel/config/usb_gadget/ether/strings/0x409/product && echo "123456" > /sys/kernel/config/usb_gadget/ether/strings/0x409/serialnumber'

# Step 4: Create config
Write-Output "=== Step 4: Config ==="
Send-Cmd 'mkdir -p /sys/kernel/config/usb_gadget/ether/configs/c.1/strings/0x409 && echo "0xC0" > /sys/kernel/config/usb_gadget/ether/configs/c.1/bmAttributes && echo "1" > /sys/kernel/config/usb_gadget/ether/configs/c.1/MaxPower && echo "RNDIS" > /sys/kernel/config/usb_gadget/ether/configs/c.1/strings/0x409/configuration'

# Step 5: OS descriptors
Write-Output "=== Step 5: OS descriptors ==="
Send-Cmd 'echo "1" > /sys/kernel/config/usb_gadget/ether/os_desc/use && echo "0xcd" > /sys/kernel/config/usb_gadget/ether/os_desc/b_vendor_code && echo "MSFT100" > /sys/kernel/config/usb_gadget/ether/os_desc/qw_sign'

# Step 6: RNDIS function
Write-Output "=== Step 6: RNDIS function ==="
Send-Cmd 'mkdir -p /sys/kernel/config/usb_gadget/ether/functions/rndis.usb0 && echo "RNDIS" > /sys/kernel/config/usb_gadget/ether/functions/rndis.usb0/os_desc/interface.rndis/compatible_id && echo "5162001" > /sys/kernel/config/usb_gadget/ether/functions/rndis.usb0/os_desc/interface.rndis/sub_compatible_id'
Send-Cmd 'ln -s /sys/kernel/config/usb_gadget/ether/functions/rndis.usb0 /sys/kernel/config/usb_gadget/ether/configs/c.1/rndis.usb0 2>/dev/null; ln -s /sys/kernel/config/usb_gadget/ether/configs/c.1 /sys/kernel/config/usb_gadget/ether/os_desc/c.1 2>/dev/null; echo "links done"'

# Step 7: Bind UDC
Write-Output "=== Step 7: Bind UDC ==="
Send-Cmd 'echo "10320000.dwc3" > /sys/kernel/config/usb_gadget/ether/UDC'
Start-Sleep -Seconds 1

# Step 8: Set IP and start sshd
Write-Output "=== Step 8: Set IP ==="
Send-Cmd 'ifconfig usb0 192.168.1.101'
Send-Cmd 'ifconfig usb0'

# Step 9: Start SSH
Write-Output "=== Step 9: Start SSH ==="
Send-Cmd '/data/openssh/start_sshd.sh 2>&1; ps | grep sshd'

# Step 10: Verify
Write-Output "=== Verify ==="
Send-Cmd 'cat /sys/kernel/config/usb_gadget/ether/UDC'
Send-Cmd 'ls /sys/kernel/config/usb_gadget/ether/configs/c.1/'

$port.Close()

Write-Output ""
Write-Output "=== DONE: Gadget switched to RNDIS mode ==="
Write-Output "Board IP: 192.168.1.101"
Write-Output "PID: 0x15b0 (RNDIS)"
