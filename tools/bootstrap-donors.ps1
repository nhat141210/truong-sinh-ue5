[CmdletBinding()]
param(
    [string]$Root = (Split-Path -Parent $PSScriptRoot),
    [switch]$IncludeMcpTools
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Invoke-Git {
    param([Parameter(ValueFromRemainingArguments = $true)][string[]]$Arguments)
    & git @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "git $($Arguments -join ' ') failed with exit code $LASTEXITCODE."
    }
}

function Test-PinnedCheckout {
    param(
        [string]$Path,
        [string]$Commit
    )

    if (-not (Test-Path -LiteralPath (Join-Path $Path ".git"))) {
        throw "$Path already exists but is not a Git checkout. Move it manually; this script will not overwrite it."
    }

    $actual = (& git -C $Path rev-parse HEAD).Trim()
    if ($actual -ne $Commit) {
        throw "$Path already exists at $actual, expected pinned commit $Commit. Resolve it manually; this script will not reset it."
    }
}

function Initialize-Donor {
    param([pscustomobject]$Donor)

    $target = Join-Path $Root $Donor.clonePath
    if (Test-Path -LiteralPath $target) {
        Test-PinnedCheckout -Path $target -Commit $Donor.commit
        Write-Host "[PASS] $($Donor.id) already pinned at $($Donor.commit)"
        return
    }

    $parent = Split-Path -Parent $target
    New-Item -ItemType Directory -Force -Path $parent | Out-Null

    if ($Donor.id -eq "cultivation-world-simulator") {
        Write-Host "Cloning sparse donor $($Donor.id)..."
        Invoke-Git clone --filter=blob:none --no-checkout $Donor.url $target
        Invoke-Git -C $target sparse-checkout init --cone
        Invoke-Git -C $target sparse-checkout set --cone @($Donor.sparsePaths)
        Invoke-Git -C $target checkout --detach $Donor.commit
    }
    else {
        Write-Host "Cloning donor $($Donor.id)..."
        Invoke-Git clone --filter=blob:none --no-checkout $Donor.url $target
        Invoke-Git -C $target checkout --detach $Donor.commit
    }

    Test-PinnedCheckout -Path $target -Commit $Donor.commit
    Write-Host "[PASS] $($Donor.id) pinned at $($Donor.commit)"
}

$lockPath = Join-Path $Root "donor-lock.json"
if (-not (Test-Path -LiteralPath $lockPath)) {
    throw "Missing donor-lock.json at $lockPath."
}

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    throw "Git is required. Run verify-windows-environment.ps1 first."
}

$lock = Get-Content -LiteralPath $lockPath -Raw | ConvertFrom-Json
foreach ($donor in $lock.donors) {
    $isOptional = ($null -ne $donor.PSObject.Properties["optional"]) -and [bool]$donor.optional

    if ($isOptional -and $donor.id -eq "monolith" -and -not $IncludeMcpTools) {
        Write-Host "[SKIP] monolith is optional MCP tooling; pass -IncludeMcpTools to clone it."
        continue
    }

    Initialize-Donor -Donor $donor
}

Write-Host "Donor bootstrap complete. Donors are local references; do not copy a donor into the game without a compatibility review." -ForegroundColor Green
