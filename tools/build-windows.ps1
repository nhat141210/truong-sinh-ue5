[CmdletBinding()]
param(
    [string]$UnrealRoot = "C:\Program Files\Epic Games\UE_5.8",
    [string]$ProjectFile = (Join-Path (Split-Path -Parent $PSScriptRoot) "TruongSinhUE5.uproject"),
    [switch]$IncludeShipping
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$projectFullPath = [System.IO.Path]::GetFullPath($ProjectFile)
$buildBat = Join-Path $UnrealRoot "Engine\Build\BatchFiles\Build.bat"

if (-not (Test-Path -LiteralPath $buildBat -PathType Leaf)) {
    throw "Missing Build.bat at $buildBat. Pass the exact UE 5.8 installation with -UnrealRoot."
}
if (-not (Test-Path -LiteralPath $projectFullPath -PathType Leaf)) {
    throw "Missing project file: $projectFullPath"
}
$openEditor = @(Get-Process -Name "UnrealEditor", "UnrealEditor-Cmd" -ErrorAction SilentlyContinue)
if ($openEditor.Count -gt 0) {
    $ids = ($openEditor | ForEach-Object { $_.Id }) -join ", "
    throw "UnrealEditor/UnrealEditor-Cmd is still running (PID $ids). Close it before native build; the build gate is intentionally exclusive."
}

function Invoke-UnrealBuild {
    param(
        [Parameter(Mandatory = $true)][string]$Target,
        [Parameter(Mandatory = $true)][string]$Configuration
    )

    Write-Host "Building $Target Win64 $Configuration..."
    & $buildBat $Target Win64 $Configuration "-Project=$projectFullPath" -WaitMutex -NoHotReloadFromIDE
    if ($LASTEXITCODE -ne 0) {
        throw "$Target Win64 $Configuration failed with exit code $LASTEXITCODE."
    }
}

Invoke-UnrealBuild -Target "TruongSinhUE5Editor" -Configuration "Development"
Invoke-UnrealBuild -Target "TruongSinhUE5" -Configuration "Development"

if ($IncludeShipping) {
    Invoke-UnrealBuild -Target "TruongSinhUE5" -Configuration "Shipping"
}

Write-Host "Requested Unreal builds completed successfully." -ForegroundColor Green
