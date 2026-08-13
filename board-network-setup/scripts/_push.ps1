$ErrorActionPreference = 'Stop'
$LocalFile = "usb-ether.sh"
$RemotePath = "/opt/bin/usb-ether.sh"
$Port = "COM13"
$Baud = 115200
$ChunkLen = 200

$bytes = [System.IO.File]::ReadAllBytes($LocalFile)
$txt = [System.Text.Encoding]::UTF8.GetString($bytes) -replace "`r`n", "`n"
$bytes = [System.Text.Encoding]::UTF8.GetBytes($txt)
$b64 = [Convert]::ToBase64String($bytes)
$md5 = (Get-FileHash -Algorithm MD5 -InputStream ([System.IO.MemoryStream]::new($bytes))).Hash.ToLower()
Write-Host "Local size=$($bytes.Length), md5=$md5, b64=$($b64.Length)"

$sp = New-Object System.IO.Ports.SerialPort $Port, $Baud, 'None', 8, 'One'
$sp.ReadTimeout = 2000
$sp.WriteTimeout = 2000
$sp.Handshake = 'None'
$sp.DtrEnable = $true
$sp.RtsEnable = $true

function ReadAll {
    param($p, [int]$ms)
    Start-Sleep -Milliseconds $ms
    $sb = New-Object System.Text.StringBuilder
    $deadline = [DateTime]::UtcNow.AddMilliseconds(600)
    while ([DateTime]::UtcNow -lt $deadline) {
        while ($p.BytesToRead -gt 0) {
            [void]$sb.Append([char]$p.ReadChar())
            $deadline = [DateTime]::UtcNow.AddMilliseconds(250)
        }
        Start-Sleep -Milliseconds 40
    }
    $sb.ToString()
}

try {
    $sp.Open()
    Start-Sleep -Milliseconds 200
    $sp.DiscardInBuffer()
    $sp.DiscardOutBuffer()
    $sp.Write("`r`n")
    [void](ReadAll $sp 200)
    $sp.Write(": > /tmp/u.b64`r`n")
    [void](ReadAll $sp 200)
    $i = 0
    $idx = 0
    while ($i -lt $b64.Length) {
        $len = [Math]::Min($ChunkLen, $b64.Length - $i)
        $sp.Write("echo -n '$($b64.Substring($i, $len))' >> /tmp/u.b64`r`n")
        Start-Sleep -Milliseconds 60
        while ($sp.BytesToRead -gt 0) { [void]$sp.ReadChar() }
        $i += $len
        $idx++
        if ($idx % 30 -eq 0) { Write-Host "  $i/$($b64.Length)" }
    }
    Write-Host "  $($b64.Length)/$($b64.Length)"
    $sp.Write("base64 -d /tmp/u.b64 > $RemotePath.tmp && chmod +x $RemotePath.tmp && md5sum $RemotePath.tmp`r`n")
    $r = ReadAll $sp 1500
    Write-Host $r
    if ($r -match $md5) {
        $sp.Write("mv $RemotePath.tmp $RemotePath && md5sum $RemotePath`r`n")
        Write-Host (ReadAll $sp 1000)
    }
    else { Write-Host "MD5 MISMATCH" -ForegroundColor Red }
    $sp.Write("rm -f /tmp/u.b64`r`n")
    [void](ReadAll $sp 200)
}
finally {
    if ($sp.IsOpen) { $sp.Close() }
    $sp.Dispose()
}
