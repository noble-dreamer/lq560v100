param(
    [string]$Command = "ip route",
    [int]$WaitMs = 2000,
    [string]$Port = '',  # Defaults to config.env SERIAL_PORT (COM7 here)
    [int]$Baud = 0       # Defaults to config.env SERIAL_BAUD
)

# Quick serial command — thin wrapper around serial_run.ps1 for diagnostics
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$runner = Join-Path $here 'serial_run.ps1'
if (Test-Path $runner) {
    & $runner -Cmd $Command -WaitMs $WaitMs -Port $Port -Baud $Baud
} else {
    # Inline fallback (when serial_run.ps1 not available)
    if (-not $Port) {
        $configFile = Join-Path $here '..\config.env'
        if (Test-Path $configFile) {
            foreach ($line in (Get-Content -LiteralPath $configFile)) {
                if ($line -match '^\s*SERIAL_PORT\s*=\s*(\S+)') {
                    $Port = $Matches[1].Trim('"', "'")
                    break
                }
            }
        }
        if (-not $Port) {
            Write-Error 'Specify a serial console with -Port or set SERIAL_PORT in config.env.'
            exit 1
        }
    }
    if ($Baud -le 0) {
        $configFile = Join-Path $here '..\config.env'
        if (Test-Path $configFile) {
            foreach ($line in (Get-Content -LiteralPath $configFile)) {
                if ($line -match '^\s*SERIAL_BAUD\s*=\s*(\d+)') {
                    $Baud = [int]$Matches[1]
                    break
                }
            }
        }
    }
    if ($Baud -le 0) { $Baud = 115200 }
    $sp = New-Object System.IO.Ports.SerialPort $Port, $Baud, 'None', 8, 'One'
    $sp.NewLine = "`n"
    $sp.ReadTimeout = 500
    $sp.DtrEnable = $true; $sp.RtsEnable = $true
    $sp.Open()
    Start-Sleep -Milliseconds 300
    $null = $sp.ReadExisting()
    $sp.Write("`r")
    Start-Sleep -Milliseconds 300
    $null = $sp.ReadExisting()
    $sp.Write("$Command`n")
    Start-Sleep -Milliseconds $WaitMs
    $out = $sp.ReadExisting()
    $sp.Close()
    Write-Output $out
}
