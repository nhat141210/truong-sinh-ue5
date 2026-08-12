param(
    [string]$EngineRoot = 'C:\Program Files\Epic Games\UE_5.8'
)

$ErrorActionPreference = 'Stop'

$sourceRoot = Join-Path $EngineRoot 'Templates\TemplateResources\High\Characters\Content'
$projectContent = Join-Path $PSScriptRoot '..\Content\Characters'

if (-not (Test-Path -LiteralPath $sourceRoot)) {
    throw "UE 5.8 Characters template resource not found: $sourceRoot"
}

# Keep the import deliberately small: one visible mannequin, its material chain,
# skeleton/preview rigs, and only the locomotion clips used by the native shell.
$relativeFiles = @(
    'Mannequins\Meshes\SKM_Manny_Simple.uasset',
    'Mannequins\Meshes\SK_Mannequin.uasset',
    'Mannequins\Rigs\CR_Mannequin_Body.uasset',
    'Mannequins\Rigs\PA_Mannequin.uasset',
    'Mannequins\Materials\M_Mannequin.uasset',
    'Mannequins\Materials\Manny\MI_Manny_01_New.uasset',
    'Mannequins\Materials\Manny\MI_Manny_02_New.uasset',
    'Mannequins\Textures\Manny\T_Manny_01_BN.uasset',
    'Mannequins\Textures\Manny\T_Manny_01_D.uasset',
    'Mannequins\Textures\Manny\T_Manny_01_MRA.uasset',
    'Mannequins\Textures\Manny\T_Manny_02_BN.uasset',
    'Mannequins\Textures\Manny\T_Manny_02_D.uasset',
    'Mannequins\Textures\Manny\T_Manny_02_MRA.uasset',
    'Mannequins\Textures\Manny\T_Manny_02_N.uasset',
    'Mannequins\Textures\Shared\T_UE_Logo_M.uasset',
    'Mannequins\Anims\Unarmed\MM_Idle.uasset',
    'Mannequins\Anims\Unarmed\Jog\MF_Unarmed_Jog_Fwd.uasset',
    'Mannequins\Anims\Unarmed\Jump\MM_Fall_Loop.uasset'
)

foreach ($relativeFile in $relativeFiles) {
    $source = Join-Path $sourceRoot $relativeFile
    $destination = Join-Path $projectContent $relativeFile
    $destinationDirectory = Split-Path -Parent $destination

    if (-not (Test-Path -LiteralPath $source)) {
        throw "Required UE template asset not found: $source"
    }

    New-Item -ItemType Directory -Path $destinationDirectory -Force | Out-Null

    if (Test-Path -LiteralPath $destination) {
        $sourceHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $source).Hash
        $destinationHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $destination).Hash
        if ($sourceHash -ne $destinationHash) {
            throw "Refusing to overwrite a different project asset: $destination"
        }
        continue
    }

    Copy-Item -LiteralPath $source -Destination $destination
}

Write-Host "Imported $($relativeFiles.Count) UE 5.8.1 mannequin foundation assets into Content/Characters."
