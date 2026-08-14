$ErrorActionPreference = 'Stop'
$rndisDir = $PSScriptRoot
$usbEtherPath = Join-Path $rndisDir 'usb-ether.sh'
$userInitPath = Join-Path $rndisDir 'user_init.sh'
$upgradeTriggerPath = Join-Path $rndisDir 'upgrade_triggerd'
$usbEther = [System.IO.File]::ReadAllText($usbEtherPath)
$userInit = [System.IO.File]::ReadAllText($userInitPath)
$upgradeTrigger = [System.IO.File]::ReadAllText($upgradeTriggerPath)
$sb = [System.Text.StringBuilder]::new()

function Add-RemoteScript {
    param(
        [string]$Content,
        [string]$RemotePath
    )

    $normalized = ($Content -replace "`r`n", "`n") -replace "`r", "`n"
    [void]$sb.AppendLine((': > {0}.new' -f $RemotePath))
    foreach ($line in ($normalized -split "`n")) {
        # Send base64 text instead of shell-quoted source lines. This preserves
        # apostrophes, dollar expressions, and backslashes exactly on ash.
        $encoded = [Convert]::ToBase64String([System.Text.Encoding]::UTF8.GetBytes($line + "`n"))
        [void]$sb.AppendLine("printf '%s' '$encoded' | base64 -d >> $RemotePath.new")
    }
    [void]$sb.AppendLine(('sed -i "s/\r$//" {0}.new' -f $RemotePath))
    [void]$sb.AppendLine(('chmod +x {0}.new' -f $RemotePath))
    [void]$sb.AppendLine(('if sh -n {0}.new; then mv {0}.new {0}; else echo "ERROR: syntax check failed for {0}"; fi' -f $RemotePath))
    [void]$sb.AppendLine(('echo "{0}: $(wc -l < {0}) lines"' -f $RemotePath))
}

[void]$sb.AppendLine('echo "=== Pushing RNDIS + SSH + upgrade-trigger boot scripts ==="')
[void]$sb.AppendLine('mkdir -p /opt/bin')
[void]$sb.AppendLine('mkdir -p /data')
[void]$sb.AppendLine('if [ -f /opt/user_init.sh ] && [ ! -f /opt/user_init.sh.pre_rndis_bak ]; then cp /opt/user_init.sh /opt/user_init.sh.pre_rndis_bak; fi')
[void]$sb.AppendLine('if [ -f /opt/bin/usb-ether.sh ] && [ ! -f /opt/bin/usb-ether.sh.pre_rndis_bak ]; then cp /opt/bin/usb-ether.sh /opt/bin/usb-ether.sh.pre_rndis_bak; fi')
Add-RemoteScript -Content $usbEther -RemotePath '/opt/bin/usb-ether.sh'
Add-RemoteScript -Content $userInit -RemotePath '/opt/user_init.sh'
Add-RemoteScript -Content $upgradeTrigger -RemotePath '/data/upgrade_triggerd'
[void]$sb.AppendLine('echo "=== VERIFY ==="')
[void]$sb.AppendLine('grep -n "run.sh\|getty\|imu_app\|upgrade_triggerd" /opt/user_init.sh /opt/bin/usb-ether.sh /data/upgrade_triggerd')
[void]$sb.AppendLine('echo "=== DONE: reboot only after the full OpenSSH bundle is deployed ==="')

$outPath = Join-Path $rndisDir 'push_cmds.txt'
[System.IO.File]::WriteAllText($outPath, $sb.ToString(), [System.Text.UTF8Encoding]::new($false))
$count = ($sb.ToString() -split "`n").Count
Write-Host "Generated $count lines to $outPath"
