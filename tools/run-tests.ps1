[CmdletBinding()]
param(
    [string]$UnrealRoot = "C:\Program Files\Epic Games\UE_5.8",
    [string]$ProjectFile = (Join-Path (Split-Path -Parent $PSScriptRoot) "TruongSinhUE5.uproject"),
    [string]$TestFilter = "TruongSinh",
    [string]$ReportDirectory = (Join-Path (Split-Path -Parent $PSScriptRoot) "Artifacts\Automation"),
    [switch]$NoCompile
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$editorCmd = Join-Path $UnrealRoot "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
$projectFullPath = [System.IO.Path]::GetFullPath($ProjectFile)
$reportFullPath = [System.IO.Path]::GetFullPath($ReportDirectory)

if (-not (Test-Path -LiteralPath $editorCmd)) {
    throw "Missing UnrealEditor-Cmd.exe at $editorCmd. Pass -UnrealRoot for your UE 5.8 installation."
}
if (-not (Test-Path -LiteralPath $projectFullPath -PathType Leaf)) {
    throw "Missing project file: $projectFullPath"
}
$openEditor = @(Get-Process -Name "UnrealEditor", "UnrealEditor-Cmd" -ErrorAction SilentlyContinue)
if ($openEditor.Count -gt 0) {
    $ids = ($openEditor | ForEach-Object { $_.Id }) -join ", "
    throw "UnrealEditor/UnrealEditor-Cmd is still running (PID $ids). Close it before commandlet automation; the test gate is intentionally exclusive."
}

New-Item -ItemType Directory -Force -Path $reportFullPath | Out-Null
$compileArg = if ($NoCompile) { "-nocompile" } else { "" }
$execCommands = "Automation RunTests $TestFilter"

Write-Host "Running automation filter '$TestFilter'..."
& $editorCmd $projectFullPath -unattended -nop4 -nosplash -NullRHI `
    "-ExecCmds=$execCommands" `
    "-TestExit=Automation Test Queue Empty" `
    "-ReportExportPath=$reportFullPath" $compileArg

if ($LASTEXITCODE -ne 0) {
    throw "Automation returned exit code $LASTEXITCODE. See $reportFullPath and Saved\Logs."
}

$reportFiles = @(Get-ChildItem -LiteralPath $reportFullPath -Filter "*.json" -File -Recurse -ErrorAction SilentlyContinue)
if ($reportFiles.Count -eq 0) {
    throw "Automation exited successfully but exported no JSON report. Treat this as a failed test gate."
}

$reportText = ($reportFiles | ForEach-Object { Get-Content -LiteralPath $_.FullName -Raw }) -join "`n"
if ($reportText -notmatch [regex]::Escape($TestFilter)) {
    throw "Automation report contains no test matching '$TestFilter'. A zero-test run is not a pass."
}
if ($reportText -match '"state"\s*:\s*"(Fail|Failed|NotRun|Skipped)"') {
    throw "Automation report contains failed or unexecuted tests. Inspect $reportFullPath."
}

Write-Host "Automation completed. Report: $reportFullPath" -ForegroundColor Green
