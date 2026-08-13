Write-Output "=== Finding RNDIS device ==="
$rndis = Get-PnpDevice | Where-Object { $_.FriendlyName -eq 'Remote NDIS Compatible Device' -and $_.Class -eq 'Net' }

foreach ($d in $rndis) {
    Write-Output "Device: $($d.FriendlyName)"
    Write-Output "InstanceId: $($d.InstanceId)"
    Write-Output "Status: $($d.Status)"
    Write-Output ""

    Write-Output "=== Disable ==="
    Disable-PnpDevice -InstanceId $d.InstanceId -Confirm:$false -ErrorAction Stop
    Start-Sleep -Seconds 2

    Write-Output "=== Enable ==="
    Enable-PnpDevice -InstanceId $d.InstanceId -Confirm:$false -ErrorAction Stop
    Start-Sleep -Seconds 5
}

Write-Output "=== After reset: RNDIS adapters ==="
Get-NetAdapter | Where-Object { $_.InterfaceDescription -match 'RNDIS|Remote' } | Format-Table Name, Status, ifIndex

Write-Output "=== All net adapters ==="
Get-NetAdapter | Format-Table Name, Status, ifIndex, InterfaceDescription -AutoSize
