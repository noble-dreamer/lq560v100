$path = "C:\Users\xiao\.claude\skills\board-network-setup\scripts\user_init_new.sh"
$txt = (Get-Content $path -Raw) -replace "`r`n", "`n"
[System.IO.File]::WriteAllText($path, $txt, [System.Text.UTF8Encoding]::new($false))
Write-Host "LF fixed, size: $((Get-Item $path).Length) bytes"
