# M3 shared activity registry evidence — 2026-08-12

## Deliverable

`/Game/Data/DA_ActivityRegistry` is a native `UDataAsset` containing authored, immutable setup for the running activities:

| Facility ID | Activity ID | Resolver ID | Duration |
|---|---|---|---:|
| `facility.cultivation.dev_smoke` | `activity.cultivation.breathing_cycle` | `cultivation` | 480 min |
| `facility.breakthrough.foundation` | `activity.breakthrough.foundation` | `breakthrough` | 720 min |
| `facility.alchemy.qingxin` | `activity.alchemy.qingxin_pill` | `alchemy` | 360 min |
| `facility.formation.spirit_gathering` | `activity.formation.spirit_gathering` | `formation` | 240 min |
| `facility.conflict.cloud_palm_trial` | `activity.conflict.cloud_palm_trial` | `conflict` | 30 min |

`ATruongSinhPlayerController` finds the definition from the interaction offer's stable facility ID and reads its activity/method/location, duration, thresholds and resolver key. An unavailable or invalid registry disables interaction; it does not substitute a hard-coded gameplay path.

The registry has no mutable save state and never resolves or commits an outcome. Resolution and Simulation retain those authorities.

## Authoring and validation

- Authoring script: `tools/create-activity-registry.py`.
- Structural validator: `tools/validate-activity-registry.py`.
- Editor validator output: `Activity registry validation PASS: definitions=5, resolvers=cultivation,breakthrough,alchemy,formation,conflict`.
- Native automation: `TruongSinh.Data.ActivityRegistry` verifies valid lookup and rejects duplicate facility IDs. Full automation: 17/17 Success, 0 warning/fail/not-run.
- Editor and Development native targets pass after the registry change.

## Packaged Windows evidence

- `Config/DefaultGame.ini` explicitly cooks `/Game/Data`; this is required because the registry is loaded by native CDO setup rather than a map hard reference.
- Portable Development package: `D:\GameDev\TruongSinhBuilds\Development-20260812-18\Windows\TruongSinhUE5.exe`; UAT `BUILD SUCCESSFUL` with pak/IoStore containers.
- 15-second packaged `-NullRHI` smoke loaded `/Game/Maps/VisualTarget/L_M2B_Corridor` and logged `Shared activity registry loaded: 5 definitions` before `Load map complete`, with no Error/Fatal/Ensure/Assertion matches.

## Scope remaining

The registry contains all five initially required verified activity definitions. Avoid-conflict routes remain separate gameplay work and are not claimed here.
