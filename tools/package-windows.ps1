[CmdletBinding()]
param(
    [string]$UnrealRoot = "C:\Program Files\Epic Games\UE_5.8",
    [string]$ProjectFile = (Join-Path (Split-Path -Parent $PSScriptRoot) "TruongSinhUE5.uproject"),
    [Parameter(Mandatory = $true)]
    [string]$ArchiveDirectory,
    [ValidateSet("Development", "Shipping")]
    [string]$Configuration = "Development",
    [switch]$Clean
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = [System.IO.Path]::GetFullPath((Split-Path -Parent $PSScriptRoot))
$projectFullPath = [System.IO.Path]::GetFullPath($ProjectFile)
$archiveFullPath = [System.IO.Path]::GetFullPath($ArchiveDirectory)
$uat = Join-Path $UnrealRoot "Engine\Build\BatchFiles\RunUAT.bat"
$packageMarkerName = ".truong-sinh-package-output"

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

function Test-RootDirectory {
    param([Parameter(Mandatory = $true)][string]$Path)

    $fullPath = [System.IO.Path]::GetFullPath($Path)
    $root = [System.IO.Path]::GetPathRoot($fullPath)
    return $fullPath.TrimEnd([System.IO.Path]::DirectorySeparatorChar, [System.IO.Path]::AltDirectorySeparatorChar).Equals(
        $root.TrimEnd([System.IO.Path]::DirectorySeparatorChar, [System.IO.Path]::AltDirectorySeparatorChar),
        [System.StringComparison]::OrdinalIgnoreCase)
}

if (-not (Test-Path -LiteralPath $uat)) {
    throw "Missing RunUAT.bat at $uat. Pass -UnrealRoot for your UE 5.8 installation."
}
if (-not (Test-Path -LiteralPath $projectFullPath -PathType Leaf)) {
    throw "Missing project file: $projectFullPath"
}
if (Test-PathInside -Candidate $archiveFullPath -Container $repoRoot) {
    throw "ArchiveDirectory must be outside the repository to avoid committing package output."
}
if (Test-RootDirectory -Path $archiveFullPath) {
    throw "ArchiveDirectory cannot be a drive or share root. Choose a dedicated child directory."
}
if ((Test-Path -LiteralPath $archiveFullPath) -and -not $Clean) {
    throw "ArchiveDirectory already exists: $archiveFullPath. Use a new build directory or pass -Clean explicitly."
}

if ($Clean -and (Test-Path -LiteralPath $archiveFullPath)) {
    $archiveItem = Get-Item -LiteralPath $archiveFullPath -Force
    if (($archiveItem.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
        throw "Refusing to clean a reparse point: $archiveFullPath"
    }

    $markerPath = Join-Path $archiveFullPath $packageMarkerName
    if (-not (Test-Path -LiteralPath $markerPath -PathType Leaf)) {
        throw "Refusing to clean $archiveFullPath because it is not a directory previously created by this package script."
    }

    $markerProject = (Get-Content -LiteralPath $markerPath -Raw).Trim()
    if ($markerProject -ne $projectFullPath) {
        throw "Refusing to clean $archiveFullPath because its package marker does not match this project."
    }

    Remove-Item -LiteralPath $archiveFullPath -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $archiveFullPath | Out-Null
Set-Content -LiteralPath (Join-Path $archiveFullPath $packageMarkerName) -Value $projectFullPath -Encoding utf8 -NoNewline

$arguments = @(
    "BuildCookRun",
    "-project=$projectFullPath",
    "-noP4",
    "-platform=Win64",
    "-clientconfig=$Configuration",
    "-build",
    "-cook",
    "-stage",
    "-package",
    "-pak",
    "-iostore",
    "-prereqs",
    "-archive",
    "-archivedirectory=$archiveFullPath",
    "-utf8output"
)

Write-Host "Packaging $Configuration Windows build to $archiveFullPath..."
& $uat @arguments
if ($LASTEXITCODE -ne 0) {
    throw "BuildCookRun returned exit code $LASTEXITCODE. Inspect the UAT log before retrying."
}

Write-Host "Windows package completed: $archiveFullPath" -ForegroundColor Green
