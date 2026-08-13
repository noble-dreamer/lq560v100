$bin = 'C:\Users\xiao\.claude\skills\board-network-setup\openssh\bin\sshd'
$f = [System.IO.File]::ReadAllBytes($bin)
Write-Host "size: $($f.Length) bytes"
Write-Host "magic: $([BitConverter]::ToString($f[0..3]))"
Write-Host "class byte: $($f[4]) (1=32bit, 2=64bit)"
$archByte = $f[18]
$archNames = @{0xB7='AArch64'; 0x28='ARM32'; 0x3E='x86_64'; 0x03='x86'}
$name = if ($archNames.ContainsKey($archByte)) { $archNames[$archByte] } else { "0x$($archByte.ToString('X2'))" }
Write-Host "arch: $name"
