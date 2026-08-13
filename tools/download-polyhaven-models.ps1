[CmdletBinding()]
param(
    [string]$Destination = (Join-Path (Split-Path -Parent $PSScriptRoot) "SourceArt\Environment\PolyHaven\model")
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$assets = @(
    @{ Id = "pine_sapling_small"; Resolution = "1k" },
    @{ Id = "shrub_02"; Resolution = "1k" },
    @{ Id = "fern_02"; Resolution = "1k" },
    @{ Id = "rock_face_01"; Resolution = "2k" },
    @{ Id = "rock_moss_set_01"; Resolution = "2k" }
)

foreach ($asset in $assets) {
    $assetDirectory = Join-Path $Destination $asset.Id
    New-Item -ItemType Directory -Force -Path $assetDirectory | Out-Null
    $files = Invoke-RestMethod "https://api.polyhaven.com/files/$($asset.Id)"
    $entry = $files.gltf.($asset.Resolution).gltf
    $gltfPath = Join-Path $assetDirectory "$($asset.Id)_$($asset.Resolution).gltf"
    Invoke-WebRequest -Uri $entry.url -OutFile $gltfPath -TimeoutSec 120
    foreach ($include in $entry.include.PSObject.Properties) {
        $relative = $include.Name.Replace('/', [System.IO.Path]::DirectorySeparatorChar)
        $output = Join-Path $assetDirectory $relative
        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $output) | Out-Null
        Invoke-WebRequest -Uri $include.Value.url -OutFile $output -TimeoutSec 180
    }
}

Get-ChildItem -LiteralPath $Destination -Recurse -File |
    Select-Object FullName, Length, @{ Name = "SHA256"; Expression = { (Get-FileHash $_.FullName -Algorithm SHA256).Hash } }
