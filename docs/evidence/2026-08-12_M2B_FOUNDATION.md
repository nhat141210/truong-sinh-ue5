# M2B visual-target foundation evidence — 2026-08-12

## Scope and truthful status

This is a runnable visual foundation, not production-final art. It establishes a separate authored corridor map, a visible temporary NPC, a native cultivation interaction site and a skinned native HUD without moving the shipping startup map away from `/Game/Maps/Dev/L_Dev_Smoke`.

The corridor deliberately uses UE Engine primitive meshes and project-authored base materials. It must not be presented as the final environment, character, animation, audio or VFX pass.

## Authored content

- Map: `/Game/Maps/VisualTarget/L_M2B_Corridor`.
- Corridor composition: main gate/jade disc, stepped path, water bands, facade dressing, mountain vista, three warm local lights, fog/sky/post-process treatment.
- Runtime actors: `VT_PlayerStart`, `VT_NPC_Caretaker_Temporary`, `VT_CultivationSite` and the required sky/lighting actors.
- Structural validator: `tools/validate-m2b-corridor.py`.
- Structural result: 166 actors, 155 static-mesh actors, 3 named local lights and 14 project corridor materials; temporary caretaker uses `SKM_Manny_Simple`.
- HUD frame: `/Game/UI/Generated/T_UI_JadeFrame`, imported by `tools/import-ui-art.py`; source/provenance is recorded as `GEN-UI-JADE-FRAME-001` in `docs/20_ASSET_AND_REFERENCE_INDEX.md`.

## Validation

- `ResavePackages -Verify` for the corridor map: success, 0 errors, 0 warnings.
- Unreal Asset Validation for the map and its 14 materials: pass, no Python error.
- `Config/DefaultGame.ini` explicitly cooks `/Game/Maps/VisualTarget/L_M2B_Corridor`.
- Portable Development archive: `D:\GameDev\TruongSinhBuilds\Development-20260812-7\Windows\TruongSinhUE5.exe`.
- A 15-second packaged `-NullRHI` run remained alive and logged:
  - `Browse: /Game/Maps/VisualTarget/L_M2B_Corridor?Name=Player`
  - `Load map complete /Game/Maps/VisualTarget/L_M2B_Corridor`
- A packaged D3D window capture confirmed the corrected PlayerStart renders the corridor, player mesh, temporary NPC and HUD rather than the earlier empty-fog view.

Runtime artifacts are intentionally untracked under `Artifacts/Runtime/`.

## Gates still open

- Production East-Asian/xianxia environment, character, animation, audio and VFX assets with reviewed provenance.
- Activity cue presentation and manual interaction/Continue capture.
- RTX 3060 performance evidence: `stat unit`, `stat gpu`, ProfileGPU, Insights and `memreport` at the documented High/Medium targets.
- Clean-machine and Shipping validation.
