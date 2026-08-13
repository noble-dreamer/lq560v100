# =============================================================================
# detect_env.ps1 — Auto-detect all per-machine environment variables
#
# Detects:
#   1) WSL IP & subnet (from WSL's eth0 routing table)
#   2) Available serial ports
#   3) RNDIS & WSL vSwitch adapter status
#   4) Compares detected values with config.env, flags mismatches
#   5) Checks board-side routes match current WSL subnet (via SSH or serial)
#
# Usage:
#   powershell -ExecutionPolicy Bypass -File detect_env.ps1
#   powershell -ExecutionPolicy Bypass -File detect_env.ps1 -FixBoard  # via serial
#
# Output: a diagnostic report. Non-zero exit if critical mismatch found.
# =============================================================================
param(
    [string]$ConfigFile = '',
    [switch]$FixBoard     # Attempt to fix board return route via serial
)

$ErrorActionPreference = 'Continue'
$here = Split-Path -Parent $MyInvocation.MyCommand.Path

# --- Load config.env ---
if (-not $ConfigFile) { $ConfigFile = Join-Path $here '..\config.env' }
if (-not (Test-Path $ConfigFile)) { $ConfigFile = Join-Path $here 'config.env' }
$cfg = @{}
if (Test-Path $ConfigFile) {
    Get-Content $ConfigFile | ForEach-Object {
        if ($_ -match '^\s*([A-Z_]+)\s*=\s*(.+)$') {
            $cfg[$Matches[1]] = $Matches[2].Trim('"',"'")
        }
    }
    Write-Host "[config] loaded: $ConfigFile"
} else {
    Write-Host "[WARN] config.env not found at $ConfigFile" -ForegroundColor Yellow
}

$WslDistro  = if ($cfg['WSL_DISTRO'])   { $cfg['WSL_DISTRO'] }   else { 'Ubuntu-20.04' }
$CfgSubnet  = if ($cfg['WSL_SUBNET'])   { $cfg['WSL_SUBNET'] }   else { '' }
$CfgSerial  = if ($cfg['SERIAL_PORT'])  { $cfg['SERIAL_PORT'] }  else { 'COM8' }
$CfgSerialBaud = if ($cfg['SERIAL_BAUD']) { [int]$cfg['SERIAL_BAUD'] } else { 115200 }
$BoardIp    = if ($cfg['BOARD_IP'])     { $cfg['BOARD_IP'] }     else { '192.168.1.101' }
$PcRndisIp  = if ($cfg['PC_RNDIS_IP']) { $cfg['PC_RNDIS_IP'] }  else { '192.168.1.3' }

$issues = @()

Write-Host "========================================"
Write-Host "  Board Network Environment Detection"
Write-Host "========================================"
Write-Host ""

# ============================================================================
# 1. WSL: detect IP and subnet
# ============================================================================
Write-Host "--- [1] WSL ($WslDistro) ---"

try {
    $wslIpRaw = (wsl -d $WslDistro -- hostname -I 2>$null)
    $WslIp = $wslIpRaw.Trim().Split(' ')[0]
} catch {
    $WslIp = ''
}

if ($WslIp) {
    Write-Host "  WSL eth0 IP : $WslIp"
} else {
    Write-Host "  WSL eth0 IP : [WARN] WSL not running or unreachable" -ForegroundColor Yellow
    $issues += "WSL_NOT_RUNNING"
}

# Detect WSL subnet from routing table
# NOTE: wsl output is an array of lines; join to single string before regex
$wslSubnet = ''
try {
    $wslLines = wsl -d $WslDistro -- ip route show 2>$null
    $wslRoute = ($wslLines | Out-String) -replace "`r", ''
    $m = [regex]::Match($wslRoute, '(\d+\.\d+\.\d+\.\d+/\d+)\s+dev\s+eth0\s+proto\s+kernel')
    if ($m.Success) {
        $wslSubnet = $m.Groups[1].Value
    } else {
        # Fallback: match any route on eth0
        $m = [regex]::Match($wslRoute, '(\d+\.\d+\.\d+\.\d+/\d+)\s+dev\s+eth0')
        if ($m.Success) { $wslSubnet = $m.Groups[1].Value }
    }
} catch { }

if ($wslSubnet) {
    Write-Host "  WSL subnet   : $wslSubnet (detected from routing table)"
} else {
    Write-Host "  WSL subnet   : [WARN] could not detect" -ForegroundColor Yellow
    $issues += "SUBNET_UNDETECTABLE"
}

# Compare with config.env
if ($wslSubnet -and $CfgSubnet) {
    if ($wslSubnet -ne $CfgSubnet) {
        Write-Host ("  *** MISMATCH: config.env WSL_SUBNET={0} but actual={1} ***" -f $CfgSubnet, $wslSubnet) -ForegroundColor Red
        Write-Host "  Fix: edit config.env, set WSL_SUBNET=$wslSubnet" -ForegroundColor Yellow
        Write-Host "  Fix: on board, update user_init.sh WSL_SUBNET=$wslSubnet" -ForegroundColor Yellow
        $issues += "SUBNET_MISMATCH"
    } else {
        Write-Host "  Subnet check : OK (matches config.env)" -ForegroundColor Green
    }
} elseif (-not $CfgSubnet) {
    Write-Host "  config.env   : WSL_SUBNET not set (add: WSL_SUBNET=$wslSubnet)" -ForegroundColor Yellow
}

# ============================================================================
# 2. Serial ports
# ============================================================================
Write-Host ""
Write-Host "--- [2] Serial Ports ---"

$availablePorts = @([System.IO.Ports.SerialPort]::GetPortNames() | Sort-Object)
if ($availablePorts.Count -eq 0) {
    Write-Host "  [WARN] No serial ports found" -ForegroundColor Yellow
    $issues += "NO_SERIAL"
} else {
    Write-Host "  Available: $($availablePorts -join ', ')"
}

# Check configured port
if ($CfgSerial) {
    if ($CfgSerial -in $availablePorts) {
        Write-Host "  config.env SERIAL_PORT=$CfgSerial : OK" -ForegroundColor Green
    } else {
        Write-Host "  config.env SERIAL_PORT=$CfgSerial : NOT FOUND (available: $($availablePorts -join ', '))" -ForegroundColor Red
        Write-Host "  Fix: edit config.env, set SERIAL_PORT to one of: $($availablePorts -join ', ')" -ForegroundColor Yellow
        $issues += "SERIAL_MISMATCH"
    }
}

# Quick board scan. COM7 is a valid USB ACM getty console when IMU_APP is
# disabled, so use the configured baud rate for it rather than excluding it.
$knownBoardPorts = @($CfgSerial, 'COM6', 'COM8', 'COM13', 'COM14') |
    Where-Object { $_ } |
    Select-Object -Unique
$boardResponded = @()
foreach ($p in $availablePorts) {
    if ($p -notin $knownBoardPorts) { continue }  # skip system ports
    try {
        $probeBaud = if ($p -eq $CfgSerial) { $CfgSerialBaud } else { 115200 }
        $sp = New-Object System.IO.Ports.SerialPort $p, $probeBaud, 'None', 8, 'One'
        $sp.ReadTimeout = 150
        $sp.DtrEnable = $true; $sp.RtsEnable = $true
        $sp.Open()
        Start-Sleep -Milliseconds 100
        $null = $sp.ReadExisting()
        $sp.Write("`r")
        Start-Sleep -Milliseconds 150
        $buf = $sp.ReadExisting()
        $sp.Close()
        if ($buf -match 'Welcome to Linux|login:|# |~ #') {
            Write-Host "  $p : [BOARD RESPONDS] use this for serial commands" -ForegroundColor Green
            $boardResponded += $p
        }
    } catch {
        # port busy or inaccessible, skip probe
    }
}

if ($boardResponded.Count -eq 0) {
    Write-Host "  [INFO] No board responded on known ports ($($knownBoardPorts -join ', '))." -ForegroundColor Gray
    Write-Host "  If board is connected, try manually with serial_run.ps1 -Port <PORT>" -ForegroundColor Gray
}

# ============================================================================
# 3. Network adapters
# ============================================================================
Write-Host ""
Write-Host "--- [3] Network Adapters ---"

$rndis = Get-NetAdapter | Where-Object { $_.InterfaceDescription -like '*Remote NDIS*' -and $_.Status -eq 'Up' }
$wslVs = Get-NetAdapter | Where-Object { $_.Name -like 'vEthernet (WSL*' -and $_.Status -eq 'Up' }

if ($rndis) {
    $fwd = (Get-NetIPInterface -InterfaceIndex $rndis.ifIndex -AddressFamily IPv4).Forwarding
    Write-Host "  RNDIS        : $($rndis.Name) idx=$($rndis.ifIndex) Forwarding=$fwd"
    if ($fwd -ne 'Enabled') {
        Write-Host "  [WARN] RNDIS forwarding not enabled. Run: Set-NetIPInterface -InterfaceIndex $($rndis.ifIndex) -Forwarding Enabled" -ForegroundColor Yellow
        $issues += "RNDIS_FWD_OFF"
    }
    if ($PcRndisIp) {
        $ifIp = Get-NetIPAddress -InterfaceIndex $rndis.ifIndex -AddressFamily IPv4 | Select-Object -First 1
        if ($ifIp) { Write-Host "  RNDIS IP     : $($ifIp.IPAddress)" }
    }
} else {
    Write-Host "  RNDIS        : NOT connected / DOWN" -ForegroundColor Yellow
    Write-Host "  [ACTION] Connect board USB, ensure RNDIS gadget started" -ForegroundColor Yellow
    $issues += "RNDIS_DOWN"
}

if ($wslVs) {
    $fwd = (Get-NetIPInterface -InterfaceIndex $wslVs.ifIndex -AddressFamily IPv4).Forwarding
    Write-Host "  WSL vSwitch  : $($wslVs.Name) idx=$($wslVs.ifIndex) Forwarding=$fwd"
    if ($fwd -ne 'Enabled') {
        Write-Host "  [WARN] WSL forwarding not enabled. Run: Set-NetIPInterface -InterfaceIndex $($wslVs.ifIndex) -Forwarding Enabled" -ForegroundColor Yellow
        $issues += "WSL_FWD_OFF"
    }
} else {
    Write-Host "  WSL vSwitch  : NOT found / DOWN" -ForegroundColor Yellow
    $issues += "WSL_VS_DOWN"
}

# ============================================================================
# 4. Connectivity checks
# ============================================================================
Write-Host ""
Write-Host "--- [4] Connectivity ---"

# Windows → Board
$winPing = Test-Connection -ComputerName $BoardIp -Count 1 -Quiet -ErrorAction SilentlyContinue
if ($winPing) {
    Write-Host "  Windows → Board ($BoardIp) ping : OK" -ForegroundColor Green
} else {
    Write-Host "  Windows → Board ($BoardIp) ping : FAIL" -ForegroundColor Red
    Write-Host "  [ACTION] Check board power, USB cable, RNDIS gadget" -ForegroundColor Yellow
    $issues += "WIN_PING_FAIL"
}

# Windows → Board SSH
if ($winPing) {
    $sshTest = Test-NetConnection -ComputerName $BoardIp -Port 22 -WarningAction SilentlyContinue -ErrorAction SilentlyContinue
    if ($sshTest.TcpTestSucceeded) {
        Write-Host "  SSH port 22  : OPEN" -ForegroundColor Green
    } else {
        Write-Host "  SSH port 22  : CLOSED (check sshd on board)" -ForegroundColor Yellow
        $issues += "SSH_CLOSED"
    }
}

# WSL → Board
if ($WslIp) {
    $wslPing = wsl -d $WslDistro -- ping -c 1 -W 2 $BoardIp 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  WSL → Board ($BoardIp) ping  : OK" -ForegroundColor Green
    } else {
        Write-Host "  WSL → Board ($BoardIp) ping  : FAIL" -ForegroundColor Red
        Write-Host "  Likely causes:" -ForegroundColor Yellow
        Write-Host "    a) IP forwarding disabled → run nfs_up.ps1 step [3]" -ForegroundColor Yellow
        Write-Host "    b) Board missing return route → run: ip route add $wslSubnet via $PcRndisIp" -ForegroundColor Yellow
        Write-Host "    c) Board return route stale (check: ip route | grep $PcRndisIp)" -ForegroundColor Yellow
        $issues += "WSL_PING_FAIL"
    }
}

# ============================================================================
# 5. Board-side check (via serial if available)
# ============================================================================
Write-Host ""
Write-Host "--- [5] Board Route Check ---"

$boardRoute = ''
$boardSerialPort = ''

# Try to reach board via serial to check its view
if ($CfgSerial -and $CfgSerial -in $availablePorts) {
    $boardSerialPort = $CfgSerial
} else {
    # Only try a hardware-console port that actually responded in the scan.
    foreach ($p in $boardResponded) {
        $boardSerialPort = $p
        break
    }
}

if ($boardSerialPort) {
    Write-Host "  Trying serial $boardSerialPort to check board routes..."
    try {
        $routeBaud = if ($boardSerialPort -eq $CfgSerial) { $CfgSerialBaud } else { 115200 }
        $sp = New-Object System.IO.Ports.SerialPort $boardSerialPort, $routeBaud, 'None', 8, 'One'
        $sp.NewLine = "`n"
        $sp.ReadTimeout = 500
        $sp.DtrEnable = $true; $sp.RtsEnable = $true
        $sp.Open()
        Start-Sleep -Milliseconds 300
        $null = $sp.ReadExisting()
        $sp.Write("`r")
        Start-Sleep -Milliseconds 300
        $null = $sp.ReadExisting()
        $sp.Write("ip route`n")
        Start-Sleep -Milliseconds 2000
        $boardRoute = $sp.ReadExisting()
        $sp.Close()
    } catch {
        Write-Host "  Serial access failed: $_" -ForegroundColor Yellow
    }

    if ($boardRoute) {
        Write-Host "  Board routes:"
        foreach ($l in ($boardRoute -split "`n" | Where-Object { $_ -match '\d+\.\d+\.\d+\.\d+' })) {
            Write-Host "    $l"
        }

        # Check if board has a route back to WSL subnet
        if ($wslSubnet) {
            $subnetPrefix = $wslSubnet -replace '/\d+$',''
            if ($boardRoute -match [regex]::Escape($subnetPrefix)) {
                $boardVia = if ($boardRoute -match "$subnetPrefix[^\n]*via\s+(\S+)") { $Matches[1] } else { '' }
                if ($boardVia -eq $PcRndisIp) {
                    Write-Host "  Board return route for WSL subnet : OK (via $PcRndisIp)" -ForegroundColor Green
                } else {
                    Write-Host "  Board return route for WSL subnet : [WARN] wrong gateway=$boardVia, expected=$PcRndisIp" -ForegroundColor Yellow
                    $issues += "BOARD_ROUTE_WRONG_GW"
                }
            } else {
                Write-Host "  Board return route for WSL subnet : MISSING" -ForegroundColor Red
                Write-Host "  Fix: echo 'ip route add $wslSubnet via $PcRndisIp' | serial" -ForegroundColor Yellow
                $issues += "BOARD_ROUTE_MISSING"
            }
        }
    }
} else {
    Write-Host "  No serial port available — cannot check board routes" -ForegroundColor Yellow
    Write-Host "  Manually on board: ip route | grep $PcRndisIp" -ForegroundColor Yellow
}

# ============================================================================
# Summary
# ============================================================================
Write-Host ""
Write-Host "========================================"
if ($issues.Count -eq 0) {
    Write-Host "  ALL CHECKS PASSED — environment is ready" -ForegroundColor Green
    Write-Host "========================================"
    exit 0
} else {
    Write-Host "  ISSUES FOUND ($($issues.Count)):" -ForegroundColor Red
    foreach ($i in $issues) { Write-Host "    - $i" -ForegroundColor Red }
    Write-Host "========================================"

    Write-Host ""
    Write-Host "=== Quick Fix Commands ==="
    if ('SUBNET_MISMATCH' -in $issues -and $wslSubnet) {
        Write-Host "  # Update config.env:"
        Write-Host "  sed -i 's|WSL_SUBNET=.*|WSL_SUBNET=$wslSubnet|' config.env"
        Write-Host ""
        Write-Host "  # Fix board return route NOW (via serial):"
        Write-Host "  #  ip route del <old>; ip route add $wslSubnet via $PcRndisIp"
        Write-Host ""
        Write-Host "  # Update board user_init.sh (persist):"
        Write-Host "  sed -i 's|WSL_SUBNET=\"\${WSL_SUBNET:-.*}\"|WSL_SUBNET=\"\${WSL_SUBNET:-$wslSubnet}\"|' /opt/user_init.sh"
    }
    if ('RNDIS_FWD_OFF' -in $issues -or 'WSL_FWD_OFF' -in $issues) {
        Write-Host "  # Run nfs_up.ps1 to fix forwarding"
    }
    if ('BOARD_ROUTE_MISSING' -in $issues -and $wslSubnet) {
        Write-Host "  # On board (serial):"
        Write-Host "  ip route add $wslSubnet via $PcRndisIp dev usb0"
    }
    if ('SERIAL_MISMATCH' -in $issues) {
        Write-Host "  # Update config.env SERIAL_PORT to one of: $($availablePorts -join ', ')"
    }

    exit 1
}
