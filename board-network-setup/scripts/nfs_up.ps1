# =============================================================================
# nfs_up.ps1 -- One-button bring-up for the full dev link
#   1) Load config.env for parameters
#   2) Start WSL NFS server (rpcbind + nfs-kernel-server)
#   3) Refresh Windows portproxy (NFS ports -> WSL)
#   4) Enable IP forwarding on RNDIS + WSL vSwitch
#   5) Detect WSL subnet, compare with config.env, warn if mismatch
#   6) Sanity check
#
# Requires: Administrator (auto-elevates via UAC).
#
# NOTE: WSL subnet and serial port are per-machine variables.
#       Run detect_env.ps1 on a new machine before this script.
# =============================================================================
param(
    [string]$ConfigFile = ''
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
    Write-Host "[WARN] config.env not found, using defaults" -ForegroundColor Yellow
}

# Read params with fallback defaults
$WslDistro   = if ($cfg['WSL_DISTRO'])    { $cfg['WSL_DISTRO'] }    else { 'Ubuntu-20.04' }
$BoardIp     = if ($cfg['BOARD_IP'])      { $cfg['BOARD_IP'] }      else { '192.168.1.101' }
$PortRpcbind = if ($cfg['PORT_RPCBIND'])  { [int]$cfg['PORT_RPCBIND'] }  else { 111 }
$PortNfs     = if ($cfg['PORT_NFS'])      { [int]$cfg['PORT_NFS'] }      else { 2049 }
$PortMountd  = if ($cfg['PORT_MOUNTD'])   { [int]$cfg['PORT_MOUNTD'] }   else { 20048 }
$Ports = @($PortRpcbind, $PortNfs, $PortMountd)

# --- admin elevate ---
$current = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $current.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Host "[!] elevating ..." -ForegroundColor Yellow
    $argv = "-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`""
    if ($ConfigFile) { $argv += " -ConfigFile `"$ConfigFile`"" }
    Start-Process powershell.exe -ArgumentList $argv -Verb RunAs -Wait
    exit
}

# === [1/4] (re)start NFS server inside WSL ===
Write-Host "=== [1/4] (re)start NFS server in WSL ($WslDistro) ==="
$wslScript = Join-Path $here 'wsl_setup_nfs.sh'
$wslMntPath = $wslScript -replace '\\','/' -replace '^([A-Za-z]):','/mnt/$1'
$wslMntPath = $wslMntPath -replace '/mnt/([A-Z])', { '/mnt/' + $_.Groups[1].Value.ToLower() }
wsl -d $WslDistro --user root -- bash $wslMntPath

# === [2/4] sync portproxy ===
Write-Host "`n=== [2/4] sync Windows portproxy ==="
$WslIp = (wsl -d $WslDistro -- hostname -I).Trim().Split(' ')[0]
if (-not $WslIp) { Write-Host "ERROR: cannot detect WSL IP"; exit 1 }
Write-Host "  WSL IP = $WslIp"

foreach ($p in $Ports) {
    netsh interface portproxy delete v4tov4 listenport=$p listenaddress=0.0.0.0 2>$null | Out-Null
    netsh interface portproxy add v4tov4 listenport=$p listenaddress=0.0.0.0 connectport=$p connectaddress=$WslIp | Out-Null
    Write-Host "  portproxy 0.0.0.0:$p -> ${WslIp}:$p"
}

# Firewall
Remove-NetFirewallRule -DisplayName 'WSL-NFS-Server'     -ErrorAction SilentlyContinue
Remove-NetFirewallRule -DisplayName 'WSL-NFS-Server-UDP' -ErrorAction SilentlyContinue
New-NetFirewallRule -DisplayName 'WSL-NFS-Server'     -Direction Inbound -Action Allow -Protocol TCP -LocalPort $Ports | Out-Null
New-NetFirewallRule -DisplayName 'WSL-NFS-Server-UDP' -Direction Inbound -Action Allow -Protocol UDP -LocalPort $Ports | Out-Null

# === [3/5] detect WSL subnet & validate against config ===
Write-Host "`n=== [3/5] detect WSL subnet ==="
$actualSubnet = ''
try {
    $wslLines = wsl -d $WslDistro -- ip route show 2>$null
    $wslRoute = ($wslLines | Out-String) -replace "`r", ''
    $m = [regex]::Match($wslRoute, '(\d+\.\d+\.\d+\.\d+/\d+)\s+dev\s+eth0\s+proto\s+kernel')
    if ($m.Success) {
        $actualSubnet = $m.Groups[1].Value
    } else {
        $m = [regex]::Match($wslRoute, '(\d+\.\d+\.\d+\.\d+/\d+)\s+dev\s+eth0')
        if ($m.Success) { $actualSubnet = $m.Groups[1].Value }
    }
} catch { }

$configSubnet = if ($cfg['WSL_SUBNET']) { $cfg['WSL_SUBNET'] } else { '' }
$pcRndisIp = if ($cfg['PC_RNDIS_IP']) { $cfg['PC_RNDIS_IP'] } else { '192.168.1.3' }

if ($actualSubnet) {
    Write-Host "  Actual WSL subnet  : $actualSubnet"
    Write-Host "  Config WSL_SUBNET  : $configSubnet"
    if ($configSubnet -and $actualSubnet -ne $configSubnet) {
        Write-Host "  *** WARNING: WSL subnet MISMATCH! ***" -ForegroundColor Yellow
        Write-Host "  config.env has WSL_SUBNET=$configSubnet" -ForegroundColor Yellow
        Write-Host "  Actual subnet is  $actualSubnet" -ForegroundColor Yellow
        Write-Host "  → Board return route may be WRONG. Fix commands:" -ForegroundColor Yellow
        Write-Host "    1) Edit config.env: WSL_SUBNET=$actualSubnet" -ForegroundColor Yellow
        Write-Host "    2) On board: ip route del $configSubnet via $pcRndisIp 2>/dev/null" -ForegroundColor Yellow
        Write-Host "    3) On board: ip route add $actualSubnet via $pcRndisIp" -ForegroundColor Yellow
    } elseif (-not $configSubnet) {
        Write-Host "  config.env has no WSL_SUBNET — set it: WSL_SUBNET=$actualSubnet" -ForegroundColor Yellow
    } else {
        Write-Host "  Subnet matches config : OK" -ForegroundColor Green
    }
} else {
    Write-Host "  [WARN] Could not detect WSL subnet" -ForegroundColor Yellow
    $actualSubnet = $configSubnet  # fallback to config
}

# === [4/5] enable IP forwarding ===
Write-Host "`n=== [4/5] enable IP forwarding ==="
$rndis = Get-NetAdapter | Where-Object { $_.InterfaceDescription -like 'Remote NDIS*' -and $_.Status -eq 'Up' }
$wslVs = Get-NetAdapter | Where-Object { $_.Name -like 'vEthernet (WSL*' -and $_.Status -eq 'Up' }

if ($rndis) {
    Set-NetIPInterface -InterfaceIndex $rndis.ifIndex -Forwarding Enabled -ErrorAction SilentlyContinue
    Write-Host ("  RNDIS ({0}, idx={1}) -> Forwarding=Enabled" -f $rndis.Name, $rndis.ifIndex)
} else {
    Write-Host "  [WARN] RNDIS adapter not found (board not connected?)" -ForegroundColor Yellow
}
if ($wslVs) {
    Set-NetIPInterface -InterfaceIndex $wslVs.ifIndex -Forwarding Enabled -ErrorAction SilentlyContinue
    Write-Host ("  WSL vSwitch ({0}, idx={1}) -> Forwarding=Enabled" -f $wslVs.Name, $wslVs.ifIndex)
} else {
    Write-Host "  [WARN] WSL vSwitch adapter not found" -ForegroundColor Yellow
}

# === [4.5/5] fix WSL default route (RNDIS must not be default gateway) ===
Write-Host "`n=== [4.5/5] fix WSL default route ==="
wsl -d $WslDistro --user root -- bash -c "
    # Remove any default route via RNDIS gateway (breaks internet)
    if ip route | grep -q '^default via .* dev eth5'; then
        ip route del default via \$(ip route | grep '^default via .* dev eth5' | awk '{print \$3}') 2>/dev/null
        echo '  Removed RNDIS default route'
    else
        echo '  No RNDIS default route to fix'
    fi
" 2>&1 | Select-Object -Last 3

# === [5/5] sanity check ===
Write-Host "`n=== [5/5] sanity check ==="
Write-Host "--- portproxy ---"
netsh interface portproxy show v4tov4

Write-Host "`n--- ping board ($BoardIp) ---"
Test-Connection -ComputerName $BoardIp -Count 2 -ErrorAction SilentlyContinue |
    Select-Object Address, ResponseTime | Format-Table -AutoSize

Write-Host "--- WSL -> board ping ---"
$wslPing = wsl -d $WslDistro -- bash -lc "ping -c 1 -W 2 $BoardIp 2>&1 | tail -2"
Write-Output $wslPing

$wslSubnet = if ($actualSubnet) { $actualSubnet } elseif ($cfg['WSL_SUBNET']) { $cfg['WSL_SUBNET'] } else { '172.29.192.0/20' }
Write-Host @"

========================================
  DONE.  Board side (if not persisted):
  # 1) Ensure return route to WSL subnet (run via serial or SSH):
  ip route del <old_subnet> via $pcRndisIp 2>/dev/null
  ip route add $wslSubnet via $pcRndisIp
  # 2) Mount NFS:
  /opt/bin/nfs-mount.sh
========================================
"@
