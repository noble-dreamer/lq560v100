$port = New-Object System.IO.Ports.SerialPort('COM12', 115200, 'None', 8, 'One')
$port.ReadTimeout = 1000
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

Write-Output "=== Releasing UDC ==="
Send-Cmd 'echo "" > /sys/kernel/config/usb_gadget/ether/UDC'
Write-Output "=== Sleeping 2s for Windows to see disconnect ==="
Start-Sleep -Seconds 2
Write-Output "=== Re-binding UDC ==="
Send-Cmd 'echo "10320000.dwc3" > /sys/kernel/config/usb_gadget/ether/UDC' 1500
Write-Output "=== Verify ==="
Send-Cmd 'cat /sys/kernel/config/usb_gadget/ether/UDC'
Send-Cmd 'ifconfig usb0'

$port.Close()
