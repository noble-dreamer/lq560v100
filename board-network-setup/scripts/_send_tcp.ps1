$ErrorActionPreference = 'Stop'
$filePath = "\\wsl$\Ubuntu-20.04\tmp\openssh.tar.gz"
$hostAddr = "192.168.1.101"
$port = 8888

$bytes = [System.IO.File]::ReadAllBytes($filePath)
Write-Host "Sending $($bytes.Length) bytes to ${hostAddr}:${port}..."

$client = New-Object System.Net.Sockets.TcpClient
try {
    $client.Connect($hostAddr, $port)
    $stream = $client.GetStream()
    $stream.Write($bytes, 0, $bytes.Length)
    $stream.Flush()
    Write-Host "Sent successfully!"
} finally {
    $client.Close()
}
