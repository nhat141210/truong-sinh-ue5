[CmdletBinding()]
param(
    [string]$UnrealRoot = "C:\Program Files\Epic Games\UE_5.8",
    [int]$MinimumFreeGb = 150
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$failures = [System.Collections.Generic.List[string]]::new()
$warnings = [System.Collections.Generic.List[string]]::new()

function Write-Check {
    param(
        [string]$Name,
        [bool]$Passed,
        [string]$Detail
    )

    $status = if ($Passed) { "PASS" } else { "FAIL" }
    Write-Host ("[{0}] {1}: {2}" -f $status, $Name, $Detail)
}

function Add-Requirement {
    param(
        [string]$Name,
        [bool]$Passed,
        [string]$Detail
    )

    Write-Check -Name $Name -Passed $Passed -Detail $Detail
    if (-not $Passed) {
        $failures.Add($Name)
    }
}

Write-Host "Truong Sinh UE5 — Windows environment verification"
Write-Host "Repository: $repoRoot"

$powerShell7 = $PSVersionTable.PSVersion.Major -ge 7
Add-Requirement -Name "PowerShell 7" -Passed $powerShell7 -Detail $(if ($powerShell7) { $PSVersionTable.PSVersion.ToString() } else { "Run this script with PowerShell 7 (pwsh), not Windows PowerShell." })

$git = Get-Command git -ErrorAction SilentlyContinue
Add-Requirement -Name "Git" -Passed ($null -ne $git) -Detail $(if ($git) { (& git --version) } else { "Install Git for Windows." })

$gitLfs = $false
if ($git) {
    & git lfs version *> $null
    $gitLfs = $LASTEXITCODE -eq 0
}
Add-Requirement -Name "Git LFS" -Passed $gitLfs -Detail $(if ($gitLfs) { (& git lfs version) } else { "Install Git LFS before adding Unreal binary assets." })

$ueEditor = Join-Path $UnrealRoot "Engine\Binaries\Win64\UnrealEditor.exe"
Add-Requirement -Name "Unreal Engine 5.8" -Passed (Test-Path -LiteralPath $ueEditor) -Detail $(if (Test-Path -LiteralPath $ueEditor) { $ueEditor } else { "Expected UnrealEditor.exe below $UnrealRoot. Pass -UnrealRoot if installed elsewhere." })

$buildVersionPath = Join-Path $UnrealRoot "Engine\Build\Build.version"
$engineBuildDetail = "Build.version missing"
$engineBuildValid = $false
if (Test-Path -LiteralPath $buildVersionPath -PathType Leaf) {
    try {
        $engineBuild = Get-Content -LiteralPath $buildVersionPath -Raw | ConvertFrom-Json
        $engineBuildDetail = "{0}.{1}.{2} changelist {3}" -f $engineBuild.MajorVersion, $engineBuild.MinorVersion, $engineBuild.PatchVersion, $engineBuild.Changelist
        $engineBuildValid = ($engineBuild.MajorVersion -eq 5) -and ($engineBuild.MinorVersion -eq 8)
    }
    catch {
        $engineBuildDetail = "Cannot parse $buildVersionPath"
    }
}
Add-Requirement -Name "Exact UE build" -Passed $engineBuildValid -Detail $engineBuildDetail

$requiredEditorPlugins = @("ModelContextProtocol", "AllToolsets", "PythonScriptPlugin")
foreach ($pluginName in $requiredEditorPlugins) {
    $pluginMatch = Get-ChildItem -LiteralPath (Join-Path $UnrealRoot "Engine\Plugins") -Filter "$pluginName.uplugin" -File -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
    Add-Requirement -Name "UE plugin $pluginName" -Passed ($null -ne $pluginMatch) -Detail $(if ($pluginMatch) { $pluginMatch.FullName } else { "Plugin is not present in this exact UE build." })
}

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
$vsInstall = $null
if (Test-Path -LiteralPath $vswhere) {
    $vsInstall = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
}
Add-Requirement -Name "Visual Studio C++" -Passed (-not [string]::IsNullOrWhiteSpace($vsInstall)) -Detail $(if ($vsInstall) { $vsInstall } else { "Install a Visual Studio version supported by this exact UE 5.8 build, with Game development with C++." })

$windowsSdkRoot = $null
try {
    $windowsSdkRoot = (Get-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\Windows Kits\Installed Roots" -ErrorAction Stop).KitsRoot10
}
catch {
    $windowsSdkRoot = $null
}
$windowsSdkInstalled = -not [string]::IsNullOrWhiteSpace($windowsSdkRoot) -and (Test-Path -LiteralPath (Join-Path $windowsSdkRoot "Include"))
Add-Requirement -Name "Windows SDK" -Passed $windowsSdkInstalled -Detail $(if ($windowsSdkInstalled) { $windowsSdkRoot } else { "Install a Windows 10/11 SDK with Visual Studio's Game development with C++ workload." })

$drive = (Get-Item -LiteralPath $repoRoot).PSDrive
$freeGb = [math]::Round($drive.Free / 1GB, 1)
Add-Requirement -Name "Free disk" -Passed ($freeGb -ge $MinimumFreeGb) -Detail ("{0} GB free on {1}; minimum {2} GB" -f $freeGb, $drive.Name, $MinimumFreeGb)

$referenceVault = Join-Path $repoRoot "ReferenceVault"
if (Test-Path -LiteralPath $referenceVault) {
    $warnings.Add("ReferenceVault exists. Keep it local and out of Git.")
}

$external = Join-Path $repoRoot "_external"
if (Test-Path -LiteralPath $external) {
    $warnings.Add("_external exists. bootstrap-donors.ps1 will verify pinned commits rather than overwrite it.")
}

foreach ($warning in $warnings) {
    Write-Host "[WARN] $warning" -ForegroundColor Yellow
}

if ($failures.Count -gt 0) {
    Write-Host ("Environment check failed: {0}" -f ($failures -join ", ")) -ForegroundColor Red
    exit 1
}

Write-Host "Environment check passed. Record the exact UE build, then run .\tools\build-windows.ps1." -ForegroundColor Green
