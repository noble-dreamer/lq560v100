# Quick debug V2
$raw = wsl -d Ubuntu-20.04 -- ip route show 2>&1
Write-Host "RAW length: $($raw.Length)"
Write-Host "RAW bytes (first 200):"
$bytes = [System.Text.Encoding]::UTF8.GetBytes($raw)
for ($i = 0; $i -lt [Math]::Min(200, $bytes.Length); $i++) {
    Write-Host -NoNewline "$([char]$bytes[$i])($($bytes[$i])) "
}
Write-Host ""

# Try explicit regex match
$m = [regex]::Match($raw, '(\d+\.\d+\.\d+\.\d+/\d+)\s+dev\s+eth0\s+proto\s+kernel')
if ($m.Success) {
    Write-Host "FOUND: $($m.Groups[1].Value)"
} else {
    Write-Host "NOT FOUND with proto kernel"
    $m2 = [regex]::Match($raw, '(\d+\.\d+\.\d+\.\d+/\d+)\s+dev\s+eth0')
    if ($m2.Success) {
        Write-Host "FOUND (fallback): $($m2.Groups[1].Value)"
    } else {
        Write-Host "NOT FOUND at all"
    }
}
