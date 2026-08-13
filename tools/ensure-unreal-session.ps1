[CmdletBinding()]
param(
    [int]$TimeoutSeconds = 180
)

$ErrorActionPreference = 'Stop'
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ProjectFile = Join-Path $ProjectRoot 'TruongSinhUE5.uproject'
$EditorExe = 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe'
$EditorConfigDirectory = Join-Path $ProjectRoot 'Saved\Config\WindowsEditor'
$PythonSettingsFile = Join-Path $EditorConfigDirectory 'EditorPerProjectUserSettings.ini'

if (-not (Test-Path -LiteralPath $EditorExe)) {
    throw "Unreal Editor not found: $EditorExe"
}
if (-not (Test-Path -LiteralPath $ProjectFile)) {
    throw "Project not found: $ProjectFile"
}

# Python Remote Execution is intentionally machine-local.  UE rewrites this
# user setting when an editor session closes, so ensure it exists before every
# persistent start without committing it to the project.
New-Item -ItemType Directory -Force -Path $EditorConfigDirectory | Out-Null
if (-not (Test-Path -LiteralPath $PythonSettingsFile)) {
    New-Item -ItemType File -Force -Path $PythonSettingsFile | Out-Null
}
$settings = Get-Content -LiteralPath $PythonSettingsFile -Raw
if ($settings -notmatch '(?m)^\[\/Script\/PythonScriptPlugin\.PythonScriptPluginSettings\]') {
    Add-Content -LiteralPath $PythonSettingsFile -Value "`r`n[/Script/PythonScriptPlugin.PythonScriptPluginSettings]`r`nbRemoteExecution=True`r`nRemoteExecutionMulticastGroupEndpoint=239.0.0.1:6766`r`nRemoteExecutionMulticastBindAddress=127.0.0.1`r`nRemoteExecutionSendBufferSizeBytes=2097152`r`nRemoteExecutionReceiveBufferSizeBytes=2097152`r`nRemoteExecutionMulticastTtl=0`r`n"
} elseif ($settings -notmatch '(?m)^bRemoteExecution=True\s*$') {
    Add-Content -LiteralPath $PythonSettingsFile -Value "bRemoteExecution=True"
}

$ProjectProcesses = Get-CimInstance Win32_Process -Filter "Name='UnrealEditor.exe'" |
    Where-Object { $_.CommandLine -and $_.CommandLine.Contains($ProjectFile) }

if (-not $ProjectProcesses) {
    $Process = Start-Process -FilePath $EditorExe -ArgumentList @(
        $ProjectFile,
        '-log'
    ) -WindowStyle Minimized -PassThru
    Write-Host "Started persistent TruongSinhUE5 Editor (PID $($Process.Id))."
} else {
    Write-Host "Reusing persistent TruongSinhUE5 Editor (PID $($ProjectProcesses.ProcessId -join ', '))."
}

$Deadline = (Get-Date).AddSeconds($TimeoutSeconds)
do {
    $McpReady = Test-NetConnection -ComputerName 127.0.0.1 -Port 8000 `
        -InformationLevel Quiet -WarningAction SilentlyContinue
    if ($McpReady) {
        Write-Host 'Persistent Unreal session ready: MCP 127.0.0.1:8000.'
        exit 0
    }
    Start-Sleep -Milliseconds 500
} while ((Get-Date) -lt $Deadline)

throw "Unreal Editor started but MCP port 8000 was not ready within $TimeoutSeconds seconds."
