[CmdletBinding()]
param(
    [ValidateSet("2k", "4k")]
    [string]$Resolution = "4k",
    [string]$Destination = (Join-Path (Split-Path -Parent $PSScriptRoot) "SourceArt\Environment\PolyHaven\hdri")
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$assetId = "ninomaru_teien"
$files = Invoke-RestMethod -Uri "https://api.polyhaven.com/files/$assetId"
$entry = $files.hdri.$Resolution.hdr
if (-not $entry.url -or -not $entry.md5) {
    throw "Poly Haven API did not return the expected $Resolution HDR entry for $assetId"
}

New-Item -ItemType Directory -Force -Path $Destination | Out-Null
$output = Join-Path $Destination "${assetId}_${Resolution}.hdr"
$partial = "$output.download"
try {
    Invoke-WebRequest -Uri $entry.url -OutFile $partial -TimeoutSec 300
    $actualMd5 = (Get-FileHash -LiteralPath $partial -Algorithm MD5).Hash.ToLowerInvariant()
    if ($actualMd5 -ne ([string]$entry.md5).ToLowerInvariant()) {
        throw "MD5 mismatch for $assetId ($Resolution): expected $($entry.md5), got $actualMd5"
    }
    Move-Item -LiteralPath $partial -Destination $output -Force
}
finally {
    if (Test-Path -LiteralPath $partial) {
        Remove-Item -LiteralPath $partial -Force
    }
}

$result = Get-Item -LiteralPath $output | Select-Object FullName, Length,
    @{ Name = "MD5"; Expression = { (Get-FileHash $_.FullName -Algorithm MD5).Hash } },
    @{ Name = "SHA256"; Expression = { (Get-FileHash $_.FullName -Algorithm SHA256).Hash } }
$result
