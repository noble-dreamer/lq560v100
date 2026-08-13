# Quick check: IP forwarding on RNDIS and WSL adapters
$adapters = Get-NetAdapter
foreach ($a in $adapters) {
    if ($a.InterfaceDescription -like '*RNDIS*' -or $a.Name -like '*WSL*') {
        $fwd = Get-NetIPInterface -InterfaceIndex $a.ifIndex -AddressFamily IPv4
        Write-Output "Name=$($a.Name) Idx=$($a.ifIndex) Status=$($a.Status) Fwd=$($fwd.Forwarding)"
    }
}
