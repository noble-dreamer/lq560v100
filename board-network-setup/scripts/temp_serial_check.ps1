$port = New-Object System.IO.Ports.SerialPort('COM12', 115200, 'None', 8, 'One')
$port.ReadTimeout = 500
$port.DtrEnable = $true
$port.RtsEnable = $true
$port.Open()
Start-Sleep -Milliseconds 300
$null = $port.ReadExisting()

# Send a newline to wake up the shell
$port.Write("`r")
Start-Sleep -Milliseconds 200
$port.ReadExisting() | Out-Null

# uname
$port.Write("uname -a`n")
Start-Sleep -Milliseconds 800
Write-Output '=== uname ==='
Write-Output $port.ReadExisting()

# ifconfig
$port.Write("ifconfig -a`n")
Start-Sleep -Milliseconds 500
Write-Output '=== ifconfig ==='
Write-Output $port.ReadExisting()

# ps check for sshd
$port.Write("ps | grep sshd`n")
Start-Sleep -Milliseconds 500
Write-Output '=== sshd ==='
Write-Output $port.ReadExisting()

# df check
$port.Write("df -h / /data /tmp`n")
Start-Sleep -Milliseconds 500
Write-Output '=== df ==='
Write-Output $port.ReadExisting()

# Check USB gadget
$port.Write("ls /sys/kernel/config/usb_gadget/`n")
Start-Sleep -Milliseconds 500
Write-Output '=== usb gadget ==='
Write-Output $port.ReadExisting()

$port.Close()
