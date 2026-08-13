# Fix IP forwarding on both adapters needed for WSL↔Board communication
$ErrorActionPreference = "Stop"

# Find adapters
$rndis = Get-NetAdapter | Where-Object { $_.InterfaceDescription -like '*RNDIS*' -and $_.Status -eq 'Up' }
$wslVs = Get-NetAdapter | Where-Object { $_.Name -like '*WSL*' -and $_.Status -eq 'Up' }

Write-Output "=== Adapters found ==="
if ($rndis) {
    $fwd = Get-NetIPInterface -InterfaceIndex $rndis.ifIndex -AddressFamily IPv4
    Write-Output "RNDIS: Name=$($rndis.Name) Idx=$($rndis.ifIndex) Fwd=$($fwd.Forwarding)"
} else {
    Write-Output "RNDIS: NOT FOUND"
}

if ($wslVs) {
    $fwd = Get-NetIPInterface -InterfaceIndex $wslVs.ifIndex -AddressFamily IPv4
    Write-Output "WSL:   Name=$($wslVs.Name) Idx=$($wslVs.ifIndex) Fwd=$($fwd.Forwarding)"
} else {
    Write-Output "WSL: NOT FOUND"
}

Write-Output ""
Write-Output "=== Enabling forwarding ==="

if ($rndis) {
    Set-NetIPInterface -InterfaceIndex $rndis.ifIndex -Forwarding Enabled
    $fwd = (Get-NetIPInterface -InterfaceIndex $rndis.ifIndex -AddressFamily IPv4).Forwarding
    Write-Output "RNDIS forwarding now: $fwd"
}

if ($wslVs) {
    Set-NetIPInterface -InterfaceIndex $wslVs.ifIndex -Forwarding Enabled
    $fwd = (Get-NetIPInterface -InterfaceIndex $wslVs.ifIndex -AddressFamily IPv4).Forwarding
    Write-Output "WSL forwarding now: $fwd"
}

Write-Output "Done."
