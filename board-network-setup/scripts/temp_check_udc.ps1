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

# Check which gadget owns the UDC
Write-Output '=== camera UDC ==='
Send-Cmd 'cat /sys/kernel/config/usb_gadget/camera/UDC'
Write-Output '=== ether UDC ==='
Send-Cmd 'cat /sys/kernel/config/usb_gadget/ether/UDC'
Write-Output '=== Available UDC ==='
Send-Cmd 'ls /sys/class/udc/'
Write-Output '=== dmesg rndis ==='
Send-Cmd 'dmesg | grep -i -E "dwc3|gadget|rndis|usb0" | tail -10'

$port.Close()
