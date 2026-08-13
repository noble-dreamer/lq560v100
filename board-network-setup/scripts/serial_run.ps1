param(
    [string]$Port    = '',       # Uses config.env SERIAL_PORT if empty
    [int]$Baud       = 0,
    [string]$CmdFile = '',
    [string]$Cmd     = '',
    [int]$WaitMs     = 1500,
    [int]$ReadMs     = 800
)
# =============================================================================
# serial_run.ps1 -- Send commands to a USB serial console and capture output
#
# Examples:
#   powershell -ExecutionPolicy Bypass -File .\serial_run.ps1 -CmdFile .\cmd.txt
#   powershell -ExecutionPolicy Bypass -File .\serial_run.ps1 -Cmd 'uname -a'
#   powershell -ExecutionPolicy Bypass -File .\serial_run.ps1 -Port COM7 -Cmd 'ip route'
#
# Auto-selection: if -Port or -Baud is not specified, use config.env.  This
# profile uses COM7 / ttyGS0 as a getty console while IMU_APP is disabled.
# =============================================================================
$ErrorActionPreference = 'Stop'

# --- Select serial port if not specified ---
if (-not $Port) {
    $available = @([System.IO.Ports.SerialPort]::GetPortNames() | Sort-Object)
    if ($available.Count -eq 0) { throw "No serial ports available" }

    $configuredPort = ''
    $configFile = Join-Path $PSScriptRoot '..\config.env'
    if (Test-Path $configFile) {
        foreach ($line in (Get-Content -LiteralPath $configFile)) {
            if ($line -match '^\s*SERIAL_PORT\s*=\s*(\S+)') {
                $configuredPort = $Matches[1].Trim('"', "'")
                break
            }
        }
    }

    if ($configuredPort -and $configuredPort -in $available) {
        $Port = $configuredPort
        Write-Host "[serial_run] using config.env SERIAL_PORT=$Port"
    } else {
        $Port = $available[0]
        Write-Host "[serial_run] config port unavailable; using available port: $Port"
    }
}

if ($Baud -le 0) {
    $configuredBaud = 0
    $configFile = Join-Path $PSScriptRoot '..\config.env'
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

$sp = New-Object System.IO.Ports.SerialPort $Port, $Baud, 'None', 8, 'One'
$sp.NewLine = "`n"
$sp.ReadTimeout  = 500
$sp.WriteTimeout = 1000
$sp.DtrEnable    = $true
$sp.RtsEnable    = $true
try {
    $sp.Open()
    Start-Sleep -Milliseconds 150
    try { $null = $sp.ReadExisting() } catch {}
    $sp.Write("`r")
    Start-Sleep -Milliseconds 120
    try { $null = $sp.ReadExisting() } catch {}

    $lines = @()
    if ($CmdFile -ne '' -and (Test-Path $CmdFile)) {
        $lines = Get-Content -LiteralPath $CmdFile
    } elseif ($Cmd -ne '') {
        $lines = $Cmd -split "`r?`n"
    } else {
        throw "need -CmdFile or -Cmd"
    }

    $buf = New-Object System.Text.StringBuilder
    foreach ($l in $lines) {
        if ($null -eq $l) { continue }
        $sp.Write($l + "`n")
        Start-Sleep -Milliseconds 60
    }
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    while ($sw.ElapsedMilliseconds -lt $WaitMs) {
        Start-Sleep -Milliseconds $ReadMs
        try {
            $chunk = $sp.ReadExisting()
            if ($chunk) { [void]$buf.Append($chunk) }
        } catch {}
    }
    Write-Output $buf.ToString()
} finally {
    if ($sp.IsOpen) { $sp.Close() }
    $sp.Dispose()
}
