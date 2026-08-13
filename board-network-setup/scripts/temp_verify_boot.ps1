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

Write-Output "=== Boot script ==="
Send-Cmd 'head -30 /opt/user_init.sh'
Write-Output "=== usb-ether.sh exists? ==="
Send-Cmd 'ls -la /opt/bin/usb-ether.sh 2>/dev/null; echo "---"; cat /opt/user_init.sh | head -5'
Write-Output "=== sshd setup ==="
Send-Cmd 'ls -la /data/openssh/start_sshd.sh /data/openssh/bin/sshd 2>/dev/null'
Write-Output "=== lib symlinks ==="
Send-Cmd 'ls -la /data/openssh/lib/libz.so* /data/openssh/lib/libcrypto.so* 2>/dev/null'
Write-Output "=== sshd-session link ==="
Send-Cmd 'ls -la /data/openssh/libexec/sshd-session 2>/dev/null; ls -la /data/openssh/bin/sshd-session 2>/dev/null'

$port.Close()
