param(
    [Parameter(Mandatory=$true)][string]$LocalFile,
    [Parameter(Mandatory=$true)][string]$RemotePath,
    [string]$Port    = '',
    [int]$Baud       = 0,
    [int]$ChunkLen   = 200
)
# =============================================================================
# serial_push.ps1 -- Transfer a local file to the device through the serial
#                    console using base64 chunks. Binary-safe; MD5 verified
#                    before mv.
#
# Example:
#   powershell -ExecutionPolicy Bypass -File .\serial_push.ps1 `
#       -LocalFile  ..\..\opt\bin\usb-ether.sh `
#       -RemotePath /opt/bin/usb-ether.sh
# =============================================================================
$ErrorActionPreference = 'Stop'

# Resolve the current getty/debug console from config.env unless explicitly
# supplied by the caller.  In this profile COM7 is valid because IMU_APP is
# disabled.
if (-not $Port) {
    $configFile = Join-Path $PSScriptRoot '..\config.env'
    if (Test-Path $configFile) {
        foreach ($line in (Get-Content -LiteralPath $configFile)) {
            if ($line -match '^\s*SERIAL_PORT\s*=\s*(\S+)') {
                $Port = $Matches[1].Trim('"', "'")
                break
            }
        }
    }
    if (-not $Port) { throw 'Specify a serial console with -Port or set SERIAL_PORT in config.env.' }
}

if ($Baud -le 0) {
    $configFile = Join-Path $PSScriptRoot '..\config.env'
    $configuredBaud = 0
    if (Test-Path $configFile) {
        foreach ($line in (Get-Content -LiteralPath $configFile)) {
            if ($line -match '^\s*SERIAL_BAUD\s*=\s*(\d+)') {
                $configuredBaud = [int]$Matches[1]
                break
            }
        }
    }
    $Baud = if ($configuredBaud -gt 0) { $configuredBaud } else { 115200 }
}

$bytes = [System.IO.File]::ReadAllBytes($LocalFile)
$b64   = [Convert]::ToBase64String($bytes)
$md5   = (Get-FileHash -Algorithm MD5 -InputStream ([System.IO.MemoryStream]::new($bytes))).Hash.ToLower()
Write-Host "Local size=$($bytes.Length), md5=$md5, b64=$($b64.Length)"

$sp = New-Object System.IO.Ports.SerialPort $Port, $Baud, 'None', 8, 'One'
$sp.ReadTimeout=2000; $sp.WriteTimeout=2000
$sp.Handshake='None'; $sp.DtrEnable=$true; $sp.RtsEnable=$true

function Read-All($p, [int]$ms) {
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
    $sp.Open(); Start-Sleep -Milliseconds 200
    $sp.DiscardInBuffer(); $sp.DiscardOutBuffer()
    $sp.Write("`r`n"); [void](Read-All $sp 200)
    $sp.Write(": > /tmp/u.b64`r`n"); [void](Read-All $sp 200)
    $i=0; $idx=0
    while ($i -lt $b64.Length) {
        $len = [Math]::Min($ChunkLen, $b64.Length-$i)
        $sp.Write("echo -n '$($b64.Substring($i,$len))' >> /tmp/u.b64`r`n")
        Start-Sleep -Milliseconds 60
        while ($sp.BytesToRead -gt 0) { [void]$sp.ReadChar() }
        $i += $len; $idx++
        if ($idx % 30 -eq 0) { Write-Host "  $i/$($b64.Length)" }
    }
    Write-Host "  $($b64.Length)/$($b64.Length)"
    $sp.Write("base64 -d /tmp/u.b64 > $RemotePath.tmp && chmod +x $RemotePath.tmp && md5sum $RemotePath.tmp`r`n")
    $r = Read-All $sp 1500; Write-Host $r
    if ($r -match $md5) {
        $sp.Write("mv $RemotePath.tmp $RemotePath && md5sum $RemotePath`r`n")
        Write-Host (Read-All $sp 1000)
    } else { Write-Host "MD5 MISMATCH" -ForegroundColor Red }
    $sp.Write("rm -f /tmp/u.b64`r`n"); [void](Read-All $sp 200)
} finally { if ($sp.IsOpen) { $sp.Close() }; $sp.Dispose() }
