param(
    [string]$BoardIp = '',
    [string]$BundleDir = ''
)
# =============================================================================
# deploy_openssh_ftp.ps1 -- Upload the bundled OpenSSH tree over temporary FTP.
# The board-side bootstrap detects the final marker and starts sshd itself.
# =============================================================================
$ErrorActionPreference = 'Stop'

if (-not $BoardIp) {
    $configFile = Join-Path $PSScriptRoot '..\config.env'
    if (Test-Path $configFile) {
        foreach ($line in (Get-Content -LiteralPath $configFile)) {
            if ($line -match '^\s*BOARD_IP\s*=\s*(\S+)') {
                $BoardIp = $Matches[1].Trim('"', "'")
                break
            }
        }
    }
}
if (-not $BoardIp) { $BoardIp = '192.168.1.101' }

if (-not $BundleDir) { $BundleDir = Join-Path $PSScriptRoot '..\openssh' }
$BundleDir = (Resolve-Path -LiteralPath $BundleDir).Path.TrimEnd([char]92, [char]47)

$curl = Join-Path $env:SystemRoot 'System32\curl.exe'
if (-not (Test-Path $curl)) { $curl = 'curl.exe' }

function Upload-File([string]$LocalPath, [string]$RemotePath) {
    $url = "ftp://$BoardIp$RemotePath"
    & $curl '--fail' '--silent' '--show-error' '--disable-epsv' '--ftp-create-dirs' `
        '--user' 'anonymous:anonymous' '--upload-file' $LocalPath $url
    if ($LASTEXITCODE -ne 0) { throw "FTP upload failed: $RemotePath" }
}

$files = Get-ChildItem -LiteralPath $BundleDir -Recurse -File | Sort-Object FullName
if ($files.Count -eq 0) { throw "No files found in $BundleDir" }

Write-Host "Uploading $($files.Count) OpenSSH files to $BoardIp ..."
foreach ($file in $files) {
    # GetChildItem can normalize WSL UNC leading separators differently from
    # Resolve-Path. The final `openssh/` component is stable in either form.
    $match = [regex]::Match($file.FullName, '(?i)[\\/]openssh[\\/](.+)$')
    if (-not $match.Success) { throw "Cannot derive bundle-relative path: $($file.FullName)" }
    $relative = $match.Groups[1].Value -replace '\\', '/'
    $remote = '/data/openssh/' + $relative
    Write-Host "  $relative"
    Upload-File $file.FullName $remote
}

# This zero-byte file is the transaction commit marker watched by the detached
# board bootstrap. Do not create it before every bundle file succeeds.
$marker = Join-Path $env:TEMP 'openssh-upload-ready.marker'
try {
    [System.IO.File]::WriteAllBytes($marker, [byte[]]@())
    Upload-File $marker '/data/openssh/.ssh-upload-ready'
} finally {
    Remove-Item -LiteralPath $marker -Force -ErrorAction SilentlyContinue
}

Write-Host 'Upload complete; board bootstrap is starting sshd.'
