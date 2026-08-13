$port = New-Object System.IO.Ports.SerialPort('COM12', 115200, 'None', 8, 'One')
$port.ReadTimeout = 800
$port.DtrEnable = $true
$port.RtsEnable = $true
$port.Open()
Start-Sleep -Milliseconds 300
$null = $port.ReadExisting()

function Send-Cmd($cmd, $waitMs = 800) {
    $port.Write("$cmd`n")
    Start-Sleep -Milliseconds $waitMs
    Write-Output $port.ReadExisting()
}

# Check current gadget config
Write-Output "=== Current gadget functions ==="
Send-Cmd 'ls /sys/kernel/config/usb_gadget/ether/configs/c.1/'

Write-Output "=== Remove ACM function ==="
Send-Cmd 'rm /sys/kernel/config/usb_gadget/ether/configs/c.1/acm.usb0 2>/dev/null; echo "done"'

Write-Output "=== Remove os_desc for ACM if exists ==="
Send-Cmd 'ls /sys/kernel/config/usb_gadget/ether/functions/ 2>/dev/null'

Write-Output "=== Rebind UDC ==="
Send-Cmd 'echo "" > /sys/kernel/config/usb_gadget/ether/UDC' 500
Start-Sleep -Seconds 2
Send-Cmd 'echo "10320000.dwc3" > /sys/kernel/config/usb_gadget/ether/UDC' 1500

Write-Output "=== Verify ==="
Send-Cmd 'cat /sys/kernel/config/usb_gadget/ether/UDC'
Send-Cmd 'ls /sys/kernel/config/usb_gadget/ether/configs/c.1/'

$port.Close()
