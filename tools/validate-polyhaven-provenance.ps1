[CmdletBinding()]
param()

$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
$assetRoot = Join-Path $repoRoot "SourceArt\Environment\PolyHaven"
$manifestPath = Join-Path $assetRoot "manifest.json"

if (-not (Test-Path -LiteralPath $manifestPath -PathType Leaf)) {
    throw "Missing Poly Haven manifest: $manifestPath"
}

$manifest = Get-Content -Raw -LiteralPath $manifestPath | ConvertFrom-Json -AsHashtable
if ($manifest.license -ne "CC0 1.0 Universal" -or -not $manifest.license_url) {
    throw "Poly Haven manifest is missing the approved CC0 license declaration"
}

$files = @(Get-ChildItem -LiteralPath $assetRoot -Recurse -File | Where-Object {
    $_.FullName -ne $manifestPath
})
$missing = @()
$mismatched = @()
foreach ($file in $files) {
    $relativePath = $file.FullName.Substring($assetRoot.Length + 1).Replace("\", "/")
    if (-not $manifest.sha256.ContainsKey($relativePath)) {
        $missing += $relativePath
        continue
    }
    $actual = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash
    if ($actual -ne $manifest.sha256[$relativePath]) {
        $mismatched += $relativePath
    }
}

$orphaned = @($manifest.sha256.Keys | Where-Object {
    -not (Test-Path -LiteralPath (Join-Path $assetRoot $_) -PathType Leaf)
})
if ($missing.Count -or $mismatched.Count -or $orphaned.Count) {
    throw "Poly Haven provenance validation failed: missing=$($missing.Count), mismatched=$($mismatched.Count), orphaned=$($orphaned.Count)"
}

Write-Host "Poly Haven provenance validation PASS: files=$($files.Count), hashes=$($manifest.sha256.Count), license=$($manifest.license)"
