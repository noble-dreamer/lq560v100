$port = New-Object System.IO.Ports.SerialPort('COM12', 115200, 'None', 8, 'One')
$port.ReadTimeout = 800
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

# Generate a new SHA-512 password hash on the board
Write-Output "=== Generate new password hash ==="
Send-Cmd "mkpasswd -m sha512 'admin123' 2>/dev/null || python3 -c 'import crypt; print(crypt.crypt(\"admin123\", \"\\$6\\$\" + __import__(\"random\").choice(\"abcdefghijklmnopqrstuvwxyz0123456789\") + \"\\$\"))' 2>/dev/null || echo 'FALLBACK'"

$port.Close()
