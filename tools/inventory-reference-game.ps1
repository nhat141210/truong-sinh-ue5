[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [Alias("ReferenceRoot")]
    [string]$ReferenceGamePath,

    [Parameter(Mandatory = $true)]
    [ValidateNotNullOrEmpty()]
    [Alias("OutputRoot")]
    [string]$VaultPath,

    [switch]$FullHash
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Test-PathInside {
    param(
        [Parameter(Mandatory = $true)][string]$Candidate,
        [Parameter(Mandatory = $true)][string]$Container
    )

    $candidateFull = [System.IO.Path]::GetFullPath($Candidate).TrimEnd([System.IO.Path]::DirectorySeparatorChar, [System.IO.Path]::AltDirectorySeparatorChar)
    $containerFull = [System.IO.Path]::GetFullPath($Container).TrimEnd([System.IO.Path]::DirectorySeparatorChar, [System.IO.Path]::AltDirectorySeparatorChar)

    return $candidateFull.Equals($containerFull, [System.StringComparison]::OrdinalIgnoreCase) -or
        $candidateFull.StartsWith($containerFull + [System.IO.Path]::DirectorySeparatorChar, [System.StringComparison]::OrdinalIgnoreCase)
}

$resolvedReference = (Resolve-Path -LiteralPath $ReferenceGamePath).Path
if (-not (Test-Path -LiteralPath $resolvedReference -PathType Container)) {
    throw "ReferenceGamePath must be an existing game installation directory."
}

$resolvedVault = [System.IO.Path]::GetFullPath($VaultPath)
if (Test-PathInside -Candidate $resolvedVault -Container $resolvedReference) {
    throw "VaultPath must not be inside ReferenceGamePath; this script never writes into the reference installation."
}

$resolvedOutput = Join-Path $resolvedVault "hashes"
if (Test-PathInside -Candidate $resolvedOutput -Container $resolvedReference) {
    throw "VaultPath must not be inside ReferenceGamePath; this script never writes into the reference installation."
}

$repoRoot = [System.IO.Path]::GetFullPath((Split-Path -Parent $PSScriptRoot))
if (Test-PathInside -Candidate $resolvedVault -Container $repoRoot) {
    throw "VaultPath must be outside the repository. Use a separate local audit directory such as D:\GameDev\ReferenceVault\ImmortalWayOfLife."
}

New-Item -ItemType Directory -Force -Path $resolvedOutput | Out-Null

$files = Get-ChildItem -LiteralPath $resolvedReference -File -Recurse | Sort-Object FullName
$records = foreach ($file in $files) {
    $relativePath = [System.IO.Path]::GetRelativePath($resolvedReference, $file.FullName).Replace('\', '/')
    $record = [ordered]@{
        relativePath = $relativePath
        extension = $file.Extension.ToLowerInvariant()
        sizeBytes = $file.Length
        lastWriteUtc = $file.LastWriteTimeUtc.ToString("o")
        sha256 = $null
    }

    if ($FullHash) {
        $record.sha256 = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash
    }

    [pscustomobject]$record
}

$timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
$manifest = [ordered]@{
    schemaVersion = 1
    createdUtc = (Get-Date).ToUniversalTime().ToString("o")
    referenceRoot = $resolvedReference
    fullHash = [bool]$FullHash
    fileCount = @($records).Count
    note = "Metadata/hash audit only. This script never copies game files and the output must remain outside Git."
    files = @($records)
}

$jsonPath = Join-Path $resolvedOutput "reference-inventory-$timestamp.json"
$csvPath = Join-Path $resolvedOutput "reference-inventory-$timestamp.csv"
$manifest | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $jsonPath -Encoding utf8
$records | Export-Csv -LiteralPath $csvPath -NoTypeInformation -Encoding utf8

Write-Host "Wrote audit metadata: $jsonPath"
Write-Host "Wrote audit CSV: $csvPath"
Write-Host "No files were copied from the reference installation." -ForegroundColor Green
