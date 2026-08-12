#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$root_dir"

required_docs=(
  00_START_HERE 01_PRODUCT_BRIEF 02_ENDLESS_SANDBOX_CONTRACT 03_DESIGN_REFERENCE_POLICY
  04_SYSTEM_RESEARCH_GUIDE 05_EXPERIENCE_CAPABILITY_MATRIX 06_GAME_DESIGN_DOCUMENT
  07_SYSTEM_RULEBOOK 08_AUTO_RESOLUTION_SPEC 09_CONTENT_CATALOG 10_UI_UX_SPEC
  11_TECHNICAL_ARCHITECTURE 12_DATA_SCHEMAS 13_SAVE_AND_DETERMINISM
  14_WORLD_AND_LEVEL_DESIGN 15_ART_DIRECTION 16_CHARACTER_ANIMATION_PIPELINE
  17_MATERIAL_LIGHTING_VFX 18_3D_ACTION_PRESENTATION 19_PERFORMANCE_BUDGET
  20_ASSET_AND_REFERENCE_INDEX 21_DONOR_REUSE_MAP 22_PRODUCTION_ROADMAP
  23_TEST_PLAN 24_WINDOWS_WORKFLOW 25_AI_AGENT_PLAYBOOK 26_DECISION_LOG
  27_IMPLEMENTATION_STATUS 28_UE58_FOUNDATION_CHECKLIST
)

for doc in "${required_docs[@]}"; do
  test -s "docs/${doc}.md" || { echo "Missing or empty docs/${doc}.md" >&2; exit 1; }
done

for template in SYSTEM_RESEARCH_TEMPLATE AUTO_RESOLUTION_EXPERIMENT_TEMPLATE CONTENT_ENTRY_TEMPLATE AUTHORED_EVENT_TEMPLATE VISUAL_REVIEW_TEMPLATE; do
  test -s "docs/templates/${template}.md" || { echo "Missing or empty template ${template}" >&2; exit 1; }
done

jq empty donor-lock.json
jq empty TruongSinhUE5.uproject

test -s Config/DefaultInput.ini || { echo "Missing Config/DefaultInput.ini" >&2; exit 1; }
test -s tools/build-windows.ps1 || { echo "Missing tools/build-windows.ps1" >&2; exit 1; }

required_foundation_sources=(
  Source/TruongSinhCore/Public/Core/TruongSinhTypes.h
  Source/TruongSinhCore/Public/Core/TruongSinhDeterministicRng.h
  Source/TruongSinhCore/Private/Core/TruongSinhDeterministicRng.cpp
  Source/TruongSinhSimulation/Public/Simulation/TruongSinhGameSimulation.h
  Source/TruongSinhSimulation/Private/Simulation/TruongSinhGameSimulation.cpp
  Source/TruongSinhSimulation/Public/Simulation/TruongSinhLifeState.h
  Source/TruongSinhSimulation/Private/Simulation/TruongSinhLifeState.cpp
  Source/TruongSinhResolution/Public/Resolution/TruongSinhActivityResolution.h
  Source/TruongSinhResolution/Private/Resolution/TruongSinhActivityResolution.cpp
  Source/TruongSinhSave/Public/Save/TruongSinhSaveGameV2.h
  Source/TruongSinhSave/Private/Save/TruongSinhSaveGameV2.cpp
  Source/TruongSinhTests/Private/TruongSinhBootstrapSpec.cpp
)
for source_file in "${required_foundation_sources[@]}"; do
  test -s "$source_file" || { echo "Missing M1 foundation source: $source_file" >&2; exit 1; }
done

if rg -q '^\[/Script/EngineSettings.GameMapsSettings\]|^\[/Script/Engine.InputSettings\]' Config/DefaultGame.ini; then
  echo "GameMapsSettings belongs in DefaultEngine.ini and InputSettings in DefaultInput.ini." >&2
  exit 1
fi

jq -e '
  ([.Modules[].Name] | length) == ([.Modules[].Name] | unique | length) and
  ([.Plugins[].Name] | length) == ([.Plugins[].Name] | unique | length) and
  ([.Plugins[] | select(.Name == "ModelContextProtocol" and (.TargetAllowList | index("Editor")))] | length) == 1 and
  ([.Plugins[] | select(.Name == "AllToolsets" and (.TargetAllowList | index("Editor")))] | length) == 1 and
  ([.Plugins[] | select(.Name == "PythonScriptPlugin" and (.TargetAllowList | index("Editor")))] | length) == 1
' TruongSinhUE5.uproject >/dev/null || {
  echo "Module/plugin descriptors are duplicated or required Editor-only UE5.8 tools are missing." >&2
  exit 1
}

if rg -n '(FGameplayActionRequest|\bFGameCommand\b|\bFCombatSnapshot\b|\bIInteractionProvider\b)' docs Source; then
  echo "Legacy contract name found; use the canonical FTruongSinh*/ITruongSinh* names." >&2
  exit 1
fi

canonical_modules=(
  Source/TruongSinhCore Source/TruongSinhSimulation Source/TruongSinhResolution
  Source/TruongSinhNarrative Source/TruongSinhSave
)
if rg -n '(FMath::Rand|FMath::FRand|\bstd::rand\b|\brand\s*\(|DeltaSeconds|GetWorld\(\)->GetTimeSeconds)' "${canonical_modules[@]}"; then
  echo "Forbidden nondeterministic/frame-time dependency found in canonical modules." >&2
  exit 1
fi

for test_name in \
  TruongSinh.Core.DeterministicRng.GoldenVectorV1 \
  TruongSinh.Simulation.CanonicalCommand \
  TruongSinh.Simulation.DeterministicReplay \
  TruongSinh.Save.RoundTripV2 \
  TruongSinh.Resolution.DeterministicAutoActivity \
  TruongSinh.Life.LifespanMonotonicity \
  TruongSinh.Soul.PossessionIdentityAndFallback; do
  rg -q --fixed-strings "$test_name" Source/TruongSinhTests/Private/TruongSinhBootstrapSpec.cpp || {
    echo "Missing bootstrap automation test source: $test_name" >&2
    exit 1
  }
done

if rg -n '(TruongSinhCombat|TruongSinhQuest|02_PARITY_CONTRACT|05_PARITY_MATRIX|08_COMBAT_SPEC|18_3D_COMBAT_PRESENTATION)' \
  AGENTS.md README.md TruongSinhUE5.uproject Source docs; then
  echo "Superseded combat/quest/parity contract name found." >&2
  exit 1
fi

if rg -n '(FMath::Rand|FMath::FRand|DeltaSeconds|GetTimeSeconds)' \
  Source/TruongSinhResolution Source/TruongSinhSimulation; then
  echo "Resolution/simulation must not depend on unseeded randomness or frame time." >&2
  exit 1
fi

check_whitespace() {
  local whitespace_errors
  whitespace_errors="$(git diff --check "$@")"
  if [[ -n "$whitespace_errors" ]]; then
    printf '%s\n' "$whitespace_errors" >&2
    echo "Whitespace validation failed." >&2
    exit 1
  fi
}

# Inspect both worktree changes and staged changes. The latter matters in CI and
# immediately before a commit, when the regular diff may be empty.
check_whitespace
check_whitespace --cached

# git diff cannot see a newly created untracked file. Check these explicitly so
# a first commit receives the same whitespace gate as later commits.
while IFS= read -r -d '' untracked_file; do
  whitespace_errors="$(git diff --no-index --check /dev/null "$untracked_file" || true)"
  if [[ -n "$whitespace_errors" ]]; then
    printf '%s\n' "$whitespace_errors" >&2
    echo "Whitespace validation failed for untracked file: $untracked_file" >&2
    exit 1
  fi
done < <(git ls-files --others --exclude-standard -z)

if rg -n -i --hidden --glob '!.git/**' --glob '!ReferenceVault/**' --glob '!_external/**' '(api[_-]?key|secret[_-]?key|BEGIN (RSA |OPENSSH )?PRIVATE KEY)' .; then
  echo "Potential secret marker found; inspect before commit." >&2
  exit 1
fi

echo "Repository static validation passed. UE build must run on Windows UE 5.8."
