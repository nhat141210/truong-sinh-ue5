[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$project = Join-Path $repoRoot "TruongSinhUE5.uproject"
$editorCmd = "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
$targetMap = "/Game/Maps/VisualTarget/L_VanDaoTong"

if (-not (Test-Path -LiteralPath $editorCmd)) {
    throw "UnrealEditor-Cmd.exe not found: $editorCmd"
}
if (Get-Process UnrealEditor, UnrealEditor-Cmd -ErrorAction SilentlyContinue) {
    throw "Close Unreal Editor before rebuilding Vấn Đạo Tông."
}

function Invoke-UnrealPython {
    param([Parameter(Mandatory = $true)][string]$ScriptPath)
    $resolvedScript = (Resolve-Path -LiteralPath (Join-Path $repoRoot $ScriptPath)).Path
    $scriptArgument = "-script=$resolvedScript"
    & $editorCmd $project -run=pythonscript $scriptArgument -unattended -nop4 -nosplash
    if ($LASTEXITCODE -ne 0) {
        throw "Unreal Python failed ($LASTEXITCODE): $ScriptPath"
    }
}

Push-Location $repoRoot
try {
    Invoke-UnrealPython "tools\preflight-vandao-import.py"
    Invoke-UnrealPython "tools\preflight-vandao-architecture.py"
    Invoke-UnrealPython "tools\import-vandao-original-kit.py"
    Invoke-UnrealPython "tools\import-vandao-pavilion.py"
    Invoke-UnrealPython "tools\import-vandao-island.py"
    Invoke-UnrealPython "tools\import-vandao-master-estate.py"

    $env:TRUONGSINH_VISUAL_MAP_PACKAGE = $targetMap
    try {
        Invoke-UnrealPython "tools\create-m2b-corridor.py"
    }
    finally {
        Remove-Item Env:\TRUONGSINH_VISUAL_MAP_PACKAGE -ErrorAction SilentlyContinue
    }

    Invoke-UnrealPython "tools\create-vandao-tong-visual.py"
    Invoke-UnrealPython "tools\validate-vandao-tong.py"
    Invoke-UnrealPython "tools\audit-vandao-map-actors.py"
}
finally {
    Pop-Location
}

Write-Host "Vấn Đạo Tông deterministic rebuild PASS: $targetMap"
