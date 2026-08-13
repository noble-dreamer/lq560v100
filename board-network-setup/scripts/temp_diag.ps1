$port = "COM7"
$baud = 115200
$outFile = "C:\Users\xiao\.claude\skills\board-network-setup\scripts\serial_output.txt"

$sp = New-Object System.IO.Ports.SerialPort $port, $baud, 'None', 8, 'One'
$sp.NewLine = "`n"
$sp.ReadTimeout = 800
$sp.DtrEnable = $true
$sp.RtsEnable = $true
$sp.Open()
Start-Sleep -Milliseconds 500
$null = $sp.ReadExisting()

# Enter
$sp.Write("`r`n")
Start-Sleep -Milliseconds 300
$null = $sp.ReadExisting()

$allOutput = ""

# Command 1: check sshd process
$sp.Write("ps | grep -i ssh`n")
Start-Sleep -Milliseconds 1500
$out = $sp.ReadExisting()
$allOutput += "=== [1] ps | grep ssh ===`n$out`n"

# Command 2: check port 22
$sp.Write("netstat -tlnp 2>/dev/null`n")
Start-Sleep -Milliseconds 1500
$out = $sp.ReadExisting()
$allOutput += "=== [2] netstat -tlnp ===`n$out`n"

# Command 3: check usb0
$sp.Write("ifconfig usb0 2>/dev/null; ip addr show usb0 2>/dev/null`n")
Start-Sleep -Milliseconds 1000
$out = $sp.ReadExisting()
$allOutput += "=== [3] usb0 ===`n$out`n"

# Command 4: check SSH binaries
$sp.Write("ls -la /data/openssh/bin/sshd /usr/sbin/sshd /usr/local/sbin/sshd 2>&1`n")
Start-Sleep -Milliseconds 1000
$out = $sp.ReadExisting()
$allOutput += "=== [4] sshd binary ===`n$out`n"

# Command 5: check SSH config
$sp.Write("ls -la /data/openssh/etc/ssh/ 2>&1`n")
Start-Sleep -Milliseconds 1000
$out = $sp.ReadExisting()
$allOutput += "=== [5] ssh config dir ===`n$out`n"

# Command 6: check SSH host keys
$sp.Write("ls -la /data/openssh/etc/ssh/ssh_host_* 2>&1; ls -la /etc/ssh/ssh_host_* 2>&1`n")
Start-Sleep -Milliseconds 1000
$out = $sp.ReadExisting()
$allOutput += "=== [6] host keys ===`n$out`n"

# Command 7: ip route
$sp.Write("ip route`n")
Start-Sleep -Milliseconds 1000
$out = $sp.ReadExisting()
$allOutput += "=== [7] ip route ===`n$out`n"

$sp.Close()

# Write to file AND console
$allOutput | Out-File -FilePath $outFile -Encoding UTF8
Write-Output $allOutput
