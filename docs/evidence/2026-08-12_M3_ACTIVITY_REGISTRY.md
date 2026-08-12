# M3 shared activity registry evidence — 2026-08-12

## Deliverable

`/Game/Data/DA_ActivityRegistry` is a native `UDataAsset` containing authored, immutable setup for the running activities:

| Facility ID | Activity ID | Resolver ID | Duration |
|---|---|---|---:|
| `facility.cultivation.dev_smoke` | `activity.cultivation.breathing_cycle` | `cultivation` | 480 min |
| `facility.breakthrough.foundation` | `activity.breakthrough.foundation` | `breakthrough` | 720 min |

`ATruongSinhPlayerController` finds the definition from the interaction offer's stable facility ID and reads its activity/method/location, duration, thresholds and resolver key. An unavailable or invalid registry disables interaction; it does not substitute a hard-coded gameplay path.

The registry has no mutable save state and never resolves or commits an outcome. Resolution and Simulation retain those authorities.

## Authoring and validation

- Authoring script: `tools/create-activity-registry.py`.
- Structural validator: `tools/validate-activity-registry.py`.
- Editor validator output: `Activity registry validation PASS: definitions=2, resolvers=cultivation,breakthrough`.
- Native automation: `TruongSinh.Data.ActivityRegistry` verifies valid lookup and rejects duplicate facility IDs. Full automation: 14/14 Success, 0 warning/fail/not-run.
- Editor and Development native targets pass after the registry change.

## Packaged Windows evidence

- `Config/DefaultGame.ini` explicitly cooks `/Game/Data`; this is required because the registry is loaded by native CDO setup rather than a map hard reference.
- Portable Development package: `D:\GameDev\TruongSinhBuilds\Development-20260812-14\Windows\TruongSinhUE5.exe`; UAT `BUILD SUCCESSFUL` with pak/IoStore containers.
- 15-second packaged `-NullRHI` smoke loaded `/Game/Maps/Dev/L_Dev_Smoke` and logged `Shared activity registry loaded: 2 definitions.` before `Load map complete`, with no Error/Fatal/Ensure/Assertion matches.

## Scope remaining

The registry contains only the two verified activities. Alchemy, formation and conflict still need their own authored definitions and resolver/commit implementations; this evidence does not claim those activities are playable.
