[CmdletBinding()]
param()

$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
$blenderCommand = Get-Command blender -ErrorAction SilentlyContinue
if ($blenderCommand) {
    $blender = $blenderCommand.Source
} else {
    $blender = Join-Path ${env:ProgramFiles} "Blender Foundation\Blender 5.2\blender.exe"
}
if (-not (Test-Path -LiteralPath $blender -PathType Leaf)) {
    throw "Blender 5.2 was not found. Install Blender or add blender.exe to PATH."
}

Push-Location $repoRoot
try {
    & $blender --background --python (Join-Path $repoRoot "tools\audit-vandao-provenance.py")
    if ($LASTEXITCODE -ne 0) {
        throw "Blender provenance audit failed with exit code $LASTEXITCODE"
    }
} finally {
    Pop-Location
}
