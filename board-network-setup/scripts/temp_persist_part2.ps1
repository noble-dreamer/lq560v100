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

# ===== Verify usb-ether.sh =====
Write-Output "=== Verify usb-ether.sh ==="
Send-Cmd 'wc -l /opt/bin/usb-ether.sh; grep "sub_compatible" /opt/bin/usb-ether.sh'
Send-Cmd 'sed -n "1,5p" /opt/bin/usb-ether.sh'

# ===== Fix CRLF =====
Write-Output "=== Fix CRLF ==="
Send-Cmd "sed -i 's/\r$//' /opt/bin/usb-ether.sh 2>/dev/null; echo 'crlf fixed'"

# ===== Test run usb-ether.sh to bring up RNDIS =====
Write-Output "=== Run usb-ether.sh ==="
Send-Cmd '/opt/bin/usb-ether.sh 2>&1' 2000

# ===== Verify RNDIS came up =====
Write-Output "=== Verify usb0 ==="
Send-Cmd 'ifconfig usb0; echo "---"; cat /sys/kernel/config/usb_gadget/ether/UDC 2>/dev/null'

# ===== Verify SSH =====
Write-Output "=== Verify SSH ==="
Send-Cmd 'ps | grep sshd'

$port.Close()
