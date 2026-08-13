# Check and enable IP forwarding
$rndis = Get-NetAdapter | Where-Object { $_.InterfaceDescription -like '*Remote NDIS*' -and $_.Status -eq 'Up' }
$wsl = Get-NetAdapter | Where-Object { $_.Name -like 'vEthernet (WSL*' -and $_.Status -eq 'Up' }

Write-Host "=== Current Status ==="
if ($rndis) {
    $f = Get-NetIPInterface -InterfaceIndex $rndis.ifIndex -AddressFamily IPv4
    Write-Host "RNDIS: $($rndis.Name) ifIndex=$($rndis.ifIndex) Forwarding=$($f.Forwarding)"
} else {
    Write-Host "RNDIS: NOT FOUND or DOWN"
}
if ($wsl) {
    $f = Get-NetIPInterface -InterfaceIndex $wsl.ifIndex -AddressFamily IPv4
    Write-Host "WSL:   $($wsl.Name) ifIndex=$($wsl.ifIndex) Forwarding=$($f.Forwarding)"
} else {
    Write-Host "WSL vSwitch: NOT FOUND or DOWN"
}

# Enable forwarding (requires admin)
$needAdmin = $false
if ($rndis) {
    $f = Get-NetIPInterface -InterfaceIndex $rndis.ifIndex -AddressFamily IPv4
    if ($f.Forwarding -ne 'Enabled') { $needAdmin = $true }
}
if ($wsl) {
    $f = Get-NetIPInterface -InterfaceIndex $wsl.ifIndex -AddressFamily IPv4
    if ($f.Forwarding -ne 'Enabled') { $needAdmin = $true }
}

if ($needAdmin) {
    Write-Host ""
    Write-Host "=== Need to enable forwarding ==="
    if ($rndis) { Set-NetIPInterface -InterfaceIndex $rndis.ifIndex -Forwarding Enabled -ErrorAction SilentlyContinue; Write-Host "RNDIS forwarding set to Enabled" }
    if ($wsl) { Set-NetIPInterface -InterfaceIndex $wsl.ifIndex -Forwarding Enabled -ErrorAction SilentlyContinue; Write-Host "WSL forwarding set to Enabled" }
} else {
    Write-Host "Forwarding already enabled on both adapters"
}
