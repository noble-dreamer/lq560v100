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

Write-Output "=== sshd_config ==="
Send-Cmd 'cat /data/openssh/etc/ssh/sshd_config | head -20'
Write-Output "=== root password hash ==="
Send-Cmd 'grep "^root:" /data/etc/shadow 2>/dev/null || grep "^root:" /etc/shadow 2>/dev/null'
Write-Output "=== PermitRootLogin ==="
Send-Cmd 'grep -i "PermitRootLogin\|PasswordAuth\|PubkeyAuth" /data/openssh/etc/ssh/sshd_config'
Write-Output "=== Login test from local ==="
# Try SSH to localhost to verify it works
Send-Cmd 'LD_LIBRARY_PATH=/data/openssh/lib timeout 3 /data/openssh/bin/ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null root@127.0.0.1 "echo LOGIN-OK" 2>&1 || echo "LOCAL-SSH-TEST-DONE"' 3000

$port.Close()
