[CmdletBinding()]
param(
    [string]$VaultRoot = "C:\ProgramData\Epic\EpicGamesLauncher\VaultCache",
    [string]$OutputPath = (Join-Path (Split-Path -Parent $PSScriptRoot) "Saved\EpicVaultInventory.json")
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $VaultRoot -PathType Container)) {
    throw "Epic VaultCache does not exist: $VaultRoot"
}

$projectRoot = Split-Path -Parent $PSScriptRoot
$outputFullPath = [System.IO.Path]::GetFullPath($OutputPath)
$outputDirectory = Split-Path -Parent $outputFullPath
New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null

$packs = @()
Get-ChildItem -LiteralPath $VaultRoot -Directory -Force | ForEach-Object {
    $packRoot = $_.FullName
    $projectFiles = @(Get-ChildItem -LiteralPath $packRoot -Recurse -Filter *.uproject -File -ErrorAction SilentlyContinue)
    $manifestFiles = @(Get-ChildItem -LiteralPath $packRoot -Recurse -Include *.manifest,*.item -File -ErrorAction SilentlyContinue)
    $contentRoots = @(Get-ChildItem -LiteralPath $packRoot -Recurse -Directory -ErrorAction SilentlyContinue | Where-Object Name -eq "Content")
    $packs += [pscustomobject]@{
        name = $_.Name
        root = $packRoot
        projectFiles = @($projectFiles | ForEach-Object { $_.FullName })
        contentRoots = @($contentRoots | ForEach-Object { $_.FullName })
        manifestFiles = @($manifestFiles | ForEach-Object { $_.FullName })
        bytes = [long](Get-ChildItem -LiteralPath $packRoot -Recurse -File -ErrorAction SilentlyContinue | Measure-Object Length -Sum).Sum
    }
}

$inventory = [ordered]@{
    generatedUtc = [DateTime]::UtcNow.ToString("o")
    vaultRoot = [System.IO.Path]::GetFullPath($VaultRoot)
    projectRoot = [System.IO.Path]::GetFullPath($projectRoot)
    packs = $packs
}

$inventory | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $outputFullPath -Encoding utf8
Write-Host "Epic Vault inventory written: $outputFullPath"
Write-Host "Detected packs: $($packs.Count)"
foreach ($pack in $packs) {
    Write-Host "- $($pack.name): $([Math]::Round($pack.bytes / 1GB, 2)) GiB, projects=$($pack.projectFiles.Count), contentRoots=$($pack.contentRoots.Count)"
}
