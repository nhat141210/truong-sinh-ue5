param(
    [string]$EngineRoot = "C:\Program Files\Epic Games\UE_5.8"
)

$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $PSScriptRoot
$sourceRoot = Join-Path $EngineRoot "Templates\TemplateResources\High\LevelPrototyping\Content"
$destinationRoot = Join-Path $projectRoot "Content\LevelPrototyping"

$expected = [ordered]@{
    "Meshes\SM_ChamferCube.uasset"              = "b20a3ae9668ec599db519af3f84b67952d0e31ac4427b80ee812232edde91f53"
    "Meshes\SM_Cylinder.uasset"                 = "76aaa3c2b812ff790fb57ae7736b748e4e1556afbb13466fd787d4bacd2c7dfe"
    "Meshes\SM_Plane.uasset"                    = "39ab83dc57a9a27a80476711610f72e81c0685a05d4aa95413b3e4e7f6c4a3f2"
    "Meshes\SM_QuarterCylinder.uasset"          = "4087b3f85753d9ccbced7d00d42208dad4d21185f24a97b0c86db31be867b733"
    "Meshes\SM_QuarterCylinderOuter.uasset"     = "ab58a5224da57452f367c9d55ea4976466138fc2cb8ec9e2fa92748c86a07f11"
    "Materials\M_FlatCol.uasset"                = "41dbbda4b463a224c29ee01765e1f52a8ae63f2f94f1440ee267fe7b1a481585"
    "Materials\MI_DefaultColorway.uasset"       = "786a4dfcaaa6f4daf459c77bda247c2c138140680a8246d521974d0cc4fc8a8e"
    "Materials\M_PrototypeGrid.uasset"          = "bf8625ad5917cee705867038d8a590b25465e6b5d5818a738a584e515d7cbdfc"
    "Materials\MF_ProcGrid.uasset"              = "e5ee13abdb241d9811f6c1bcc0a607d229fa7adc5f46bfd5a9d5fedbfa279683"
    "Materials\MI_PrototypeGrid_Gray.uasset"    = "084681bf680dad66b8c8ca803d927053cf46756f2f2e62d18fcc439e07de7263"
    "Textures\T_GridChecker_A.uasset"           = "bb8334767064b107ec4f4407ef1365e8b9e2dfeff5984e66d4a6232254ddd02a"
}

if (-not (Test-Path -LiteralPath $sourceRoot -PathType Container)) {
    throw "UE 5.8 High LevelPrototyping mesh directory was not found: $sourceRoot"
}

New-Item -ItemType Directory -Force -Path $destinationRoot | Out-Null

foreach ($entry in $expected.GetEnumerator()) {
    $source = Join-Path $sourceRoot $entry.Key
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Required UE template mesh was not found: $source"
    }

    $sourceHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $source).Hash.ToLowerInvariant()
    if ($sourceHash -ne $entry.Value) {
        throw "Unexpected SHA256 for $($entry.Key). Expected $($entry.Value), got $sourceHash"
    }

    $destination = Join-Path $destinationRoot $entry.Key
    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $destination) | Out-Null
    Copy-Item -LiteralPath $source -Destination $destination -Force
    $destinationHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $destination).Hash.ToLowerInvariant()
    if ($destinationHash -ne $entry.Value) {
        throw "Copied asset verification failed for $destination"
    }
}

Write-Host "UE 5.8 architecture kit install PASS: $($expected.Count) verified assets"
Write-Host "Destination package root: /Game/LevelPrototyping"
