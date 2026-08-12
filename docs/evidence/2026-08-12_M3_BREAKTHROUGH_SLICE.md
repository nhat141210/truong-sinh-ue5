# M3 breakthrough slice evidence — 2026-08-12

## Implemented slice

`facility.breakthrough.foundation` is a second native interaction provider in both `/Game/Maps/Dev/L_Dev_Smoke` and `/Game/Maps/VisualTarget/L_M2B_Corridor`.

The player must first obtain 800 cultivation units through the existing cultivation activity. The breakthrough then follows the same runtime route:

```text
interaction offer → activity plan → deterministic resolver → typed simulation commit
→ autosave/pending replay → native HUD summary
```

It does not add a new UI framework or simulation subsystem. A success moves the vessel from `realm.mortal` to `realm.foundation` and grants a realm-lifespan contribution; the HUD renders this as `TRÚC CƠ`.

## Canonical safety

- `FTruongSinhResolvedActivityCommitPayload` is the typed hand-off from resolution to simulation.
- It carries an `ActivityId`, exact time/progress/lifespan deltas, outcome/replay IDs, target realm and `RequiredCurrentRealmId`.
- Simulation refuses malformed payloads and refuses a payload when its required realm does not match the canonical vessel realm.
- Therefore a later command cannot repeat the `realm.mortal → realm.foundation` transition to farm its lifespan reward. This is enforced by simulation, not the HUD or map actor.

## Windows evidence

- `tools/build-windows.ps1`: Editor and Development targets passed after the slice.
- `tools/run-tests.ps1`: 13/13 Success, 0 warning/fail/not-run. New automation: `TruongSinh.Activity.BreakthroughCommitAndLifespan` verifies deterministic resolve, realm/lifespan commit, duplicate rejection and later precondition rejection.
- Editor map validator: `M2B corridor validation PASS: actors=167, static_meshes=155, local_lights=3, materials=14, npc_mesh=SKM_Manny_Simple`.
- Portable Development archive: `D:\GameDev\TruongSinhBuilds\Development-20260812-10\Windows\TruongSinhUE5.exe`; UAT `BUILD SUCCESSFUL`, with one pak/two utoc/two ucas containers.
- 15-second packaged `-NullRHI` smoke directly browsed and completed load of `/Game/Maps/VisualTarget/L_M2B_Corridor`, with no Error/Fatal/Ensure/Assertion matches in the smoke log.

## Still outside this slice

Data Asset resolver registry, alchemy, formation and conflict are not implemented as playable activities yet. This is M3 progress, not M3 completion or a release-quality progression system.
