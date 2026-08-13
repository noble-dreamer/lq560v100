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

Write-Output "=== run.sh ==="
Send-Cmd 'cat /opt/stereo/run.sh'
Write-Output "=== user_init.sh (full) ==="
Send-Cmd 'cat /opt/user_init.sh'

$port.Close()
