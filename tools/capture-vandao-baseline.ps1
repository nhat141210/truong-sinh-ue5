[CmdletBinding()]
param(
    [string]$OutputDirectory = "Artifacts\VisualTarget\VDT0",
    [switch]$SimulateRuntime
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$invokeMcp = Join-Path $PSScriptRoot "invoke-unreal-mcp.ps1"
$remotePython = Join-Path $PSScriptRoot "invoke-unreal-python-remote.py"
$resolvedOutput = Join-Path $repoRoot $OutputDirectory
New-Item -ItemType Directory -Force -Path $resolvedOutput | Out-Null

function Invoke-CheckedMcpTool {
    param(
        [Parameter(Mandatory = $true)][string]$Toolset,
        [Parameter(Mandatory = $true)][string]$Tool,
        [string]$Arguments = "{}"
    )
    $raw = & $invokeMcp -ToolsetName $Toolset -ToolName $Tool -ArgumentsJson $Arguments | Out-String
    $response = $raw | ConvertFrom-Json
    if (($response.result.PSObject.Properties.Name -contains "isError") -and $response.result.isError) {
        throw (($response.result.content | Select-Object -First 1).text)
    }
    return $response
}

function Invoke-OptionalRemotePython {
    param([Parameter(Mandatory = $true)][string]$Statement)
    & python $remotePython --statement $Statement --timeout 8 | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "Python Remote Execution is unavailable; continuing with MCP capture without editor-only Game View toggling."
    }
}

$sceneToolset = "editor_toolset.toolsets.scene.SceneTools"
$levelPath = "/Game/Maps/VisualTarget/L_VanDaoTong"
$loadArguments = @{ level_path = $levelPath } | ConvertTo-Json -Compress
Invoke-CheckedMcpTool -Toolset $sceneToolset -Tool "load_level" -Arguments $loadArguments | Out-Null
Start-Sleep -Seconds 5
$currentResponse = Invoke-CheckedMcpTool -Toolset $sceneToolset -Tool "get_current_level"
$currentText = ($currentResponse.result.content | Where-Object type -eq "text" | Select-Object -First 1).text
$currentPayload = $currentText | ConvertFrom-Json
if ([string]$currentPayload.returnValue -notlike "*L_VanDaoTong*") {
    throw "Unreal MCP loaded the wrong world: $($currentPayload.returnValue)"
}

Invoke-OptionalRemotePython -Statement "import unreal; unreal.EditorLevelLibrary.editor_set_game_view(True)"

if ($SimulateRuntime) {
    $pieArguments = @{
        options = @{
            bSimulate = $true
            playMode = "PlayMode_Simulate"
            warmupSeconds = 5.0
        }
    } | ConvertTo-Json -Depth 10 -Compress
    Invoke-CheckedMcpTool -Toolset "EditorToolset.EditorAppToolset" -Tool "StartPIE" -Arguments $pieArguments | Out-Null
}

$cameras = @(
    @{ 
        Id = "CAM_VDT_01_ArrivalVista"
        Location = @{ x = -700.0; y = -900.0; z = 190.0 }
        Rotation = @{ pitch = 6.2; yaw = 31.6; roll = 0.0 }
    },
    @{
        Id = "CAM_VDT_02_SectGate"
        Location = @{ x = -200.0; y = -800.0; z = 180.0 }
        Rotation = @{ pitch = 7.6; yaw = 38.7; roll = 0.0 }
    },
    @{
        Id = "CAM_VDT_03_Forecourt"
        Location = @{ x = 3200.0; y = -1050.0; z = 220.0 }
        Rotation = @{ pitch = 2.4; yaw = 19.9; roll = 0.0 }
    },
    @{
        Id = "CAM_VDT_04_WaterGarden"
        Location = @{ x = 1700.0; y = -1100.0; z = 160.0 }
        Rotation = @{ pitch = 1.2; yaw = 18.4; roll = 0.0 }
    },
    @{
        Id = "CAM_VDT_05_MainHall"
        Location = @{ x = 5200.0; y = -900.0; z = 160.0 }
        Rotation = @{ pitch = 7.8; yaw = 34.7; roll = 0.0 }
    }
)

$manifest = [System.Collections.Generic.List[object]]::new()
foreach ($camera in $cameras) {
    $captureArguments = @{
        captureTransform = @{
            location = $camera.Location
            rotation = $camera.Rotation
            scale = @{ x = 1.0; y = 1.0; z = 1.0 }
        }
        annotations = @{
            gridSpacing = 0.0
            gridExtent = 0.0
            gridHeight = 0.0
            maxLabelDistance = 0.0
            classFilter = @{ refPath = "/Script/Engine.Actor" }
            maxLabels = 0
        }
        bShowUI = $false
    } | ConvertTo-Json -Depth 20 -Compress

    $response = Invoke-CheckedMcpTool `
        -Toolset "EditorToolset.EditorAppToolset" `
        -Tool "CaptureViewport" `
        -Arguments $captureArguments

    $capture = (($response.result.content | Where-Object type -eq "text" | Select-Object -First 1).text | ConvertFrom-Json).returnValue
    $imagePath = Join-Path $resolvedOutput ("{0}.png" -f $camera.Id)
    [IO.File]::WriteAllBytes($imagePath, [Convert]::FromBase64String($capture.image.data))

    $manifest.Add([ordered]@{
        id = $camera.Id
        image = $imagePath
        location = $capture.cameraLocation
        rotation = $capture.cameraRotation
        fov = $capture.cameraFOV
    })
}

$manifestPath = Join-Path $resolvedOutput "camera-manifest.json"
$manifest | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath $manifestPath -Encoding utf8
if ($SimulateRuntime) {
    Invoke-CheckedMcpTool -Toolset "EditorToolset.EditorAppToolset" -Tool "StopPIE" | Out-Null
}
Invoke-OptionalRemotePython -Statement "import unreal; unreal.EditorLevelLibrary.editor_set_game_view(False)"
Write-Host "Vấn Đạo Tông baseline capture complete: $manifestPath"
