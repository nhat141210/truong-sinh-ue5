# Vấn Đạo Tông — visual audit (VDT32)

Date: 2026-08-13  
Scope: read-only review of the latest deterministic captures and tracked
authoring scripts. No Unreal Editor writer, map mutation, or asset mutation was
used for this audit.

## Evidence reviewed

- `Artifacts/VisualTarget/VDT32/CAM_VDT_01_ArrivalVista.png`
- `Artifacts/VisualTarget/VDT32/CAM_VDT_02_SectGate.png`
- `Artifacts/VisualTarget/VDT32/CAM_VDT_03_Forecourt.png`
- `Artifacts/VisualTarget/VDT32/CAM_VDT_04_WaterGarden.png`
- `Artifacts/VisualTarget/VDT32/CAM_VDT_05_MainHall.png`
- `tools/create-vandao-tong-visual.py`
- `tools/create-m2b-corridor.py`
- `tools/create-vandao-water.py`
- `tools/validate-vandao-tong.py`

The structural validator and provenance audit are useful evidence for asset
presence, transforms, and actor counts. They do not prove visual quality, PIE,
Standalone, or packaged-runtime parity.

## Findings, ordered by visual impact

1. **World integration is not yet believable.** The upper half of the wide
   views is a deep, mostly uniform blue. Satellite islands read as isolated
   cut-outs because aerial perspective and horizon fade are weak.
2. **The floating-island edge reads as a hard platform.** Large flat tile slabs,
   stepped rectangular patches, and abrupt dark void edges dominate the
   foreground. There is no continuous soil, rock, moss, or vegetation contact
   transition.
3. **The water garden reads as a material test.** The native water cube is a
   narrow rectangle with straight borders; the lotus clusters are flat and
   visibly clipped in the foreground hero shot. `SM_VDT_SteppingStoneCluster_A`
   exists in the validated water kit but is not currently placed by the visual
   map script.
4. **Foreground artifacts are compositionally distracting.** In
   `CAM_VDT_03_Forecourt`, the near water/lotus geometry reaches the bottom
   frame as bright cyan cutouts. The source coordinates show that
   `VT_WaterGarden_-900` spans approximately `x=1150..3350`, `y=-1180..-620`,
   directly under this camera.
5. **Vegetation is too sparse for a lived-in sect.** The scene has six small
   shrub/fern clusters and the near pines are hidden after a cyan material cue;
   the grounds therefore lack canopy, grass, and contact variation.
6. **The gate has a procedural low-poly roof read.** Repetitive parallel tile
   courses and dark/blue under-eave strips are prominent in the gate shots.
7. **Architecture is stronger than its surroundings, but values remain flat.**
   Main Hall and pavilion silhouettes have useful detail; gray stone platforms
   and hard seams still dominate the read.
8. **Hero cameras are too wide.** `spawn_camera` sets 38 mm and the capture
   manifest records 90° FOV. Gate and hall close views would be more cinematic
   around 50–58 mm with a small camera pull-back.

## Safe next authoring changes

These are recommendations for the next deterministic script pass; they were
not applied by this audit:

- Restore the foundation fog's volumetric settings from
  `create-m2b-corridor.py` (`fog_inscattering_color`, volumetric scattering
  distribution, and extinction scale) before experimenting with density.
- Replace or visually mask the straight native water cube with overlapping
  pond/shore meshes; place one or more existing stepping-stone clusters along
  the edge. Keep native collision/provider actors intact.
- Add authored rock/shore/vegetation contact around the island perimeter, using
  collision-disabled presentation actors, and move near lotus clusters outside
  the hero camera frustum.
- Keep one or two pines only in distant/background composition after foliage
  material verification; avoid hiding all canopy without a replacement.
- Set hero camera focal length to roughly 50–58 mm and recapture all five shots.

## Gate status

**Not a final visual/art gate.** VDT32 is a valid structural milestone, not a
claim of photorealism or production completion. Fresh PIE, Standalone, and
packaged-runtime captures are still required after the next visual pass.

