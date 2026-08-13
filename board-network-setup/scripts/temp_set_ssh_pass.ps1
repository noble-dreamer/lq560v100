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

# Check if busybox has mkpasswd
Write-Output "=== Check mkpasswd ==="
Send-Cmd 'which mkpasswd 2>/dev/null; busybox --help 2>/dev/null | grep -i passwd | head -3'

# Try openssl passwd on the board
Write-Output "=== Try generate hash ==="
Send-Cmd 'openssl passwd -6 -salt xxxxxxxx "admin123" 2>/dev/null || echo "openssl not available"'

$port.Close()
