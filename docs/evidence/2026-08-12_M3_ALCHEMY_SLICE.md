# M3 alchemy slice evidence — 2026-08-12

## Playable shared-pipeline activity

`facility.alchemy.qingxin` is a native furnace interaction in `/Game/Maps/Dev/L_Dev_Smoke` and `/Game/Maps/VisualTarget/L_M2B_Corridor`. Its authored registry definition resolves through the existing shared pipeline as `activity.alchemy.qingxin_pill`; no minigame, controller-specific resolver or second UI framework is introduced.

For a committed alchemy action, `FTruongSinhAutoResolver` deterministically produces:

- `pill.qingxin` output ID;
- exact output quantity;
- quality basis points;
- impurity basis points;
- normal time/replay/outcome information.

`FTruongSinhResolvedActivityCommitPayload` transfers those results into simulation. Simulation validates the range/identity, persists an `FTruongSinhActivityOutputRecord`, includes it in the BLAKE3 canonical hash and saves/restores it in the v2 envelope. The output record is keyed by command GUID and cannot be committed twice.

## Verification

- Editor/Game native builds pass.
- Full Windows automation: 15/15 Success, 0 warning/fail/not-run.
- `TruongSinh.Activity.AlchemyOutputAndSave` verifies deterministic recipe output, output quantity/quality/impurity, shared commit, canonical ledger and v2 save round trip.
- Registry editor validator: `definitions=3, resolvers=cultivation,breakthrough,alchemy`.
- M2B structural validator passes after adding `VT_AlchemySite`.
- Development archive: `D:\GameDev\TruongSinhBuilds\Development-20260812-16\Windows\TruongSinhUE5.exe`; UAT `BUILD SUCCESSFUL`.
- A 15-second packaged `-NullRHI` run browsed and completed `/Game/Maps/VisualTarget/L_M2B_Corridor`, logged `Shared activity registry loaded: 3 definitions.`, and had no Error/Fatal/Ensure/Assertion matches.

## Not claimed

There is not yet a resource inventory/consumption economy, recipe catalogue UI, production animation/VFX/audio or a formation/conflict activity. The slice proves canonical recipe output, not the complete crafting system.
