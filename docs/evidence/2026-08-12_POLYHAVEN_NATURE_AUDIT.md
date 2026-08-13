# Poly Haven nature provenance and UE 5.8 quality audit

Date: 2026-08-12
Engine: Unreal Engine 5.8.1, changelist 56057345
Scope: `/Game/VisualTarget/Nature` and `SourceArt/Environment/PolyHaven`

## Provenance gate

Command:

```powershell
.\tools\validate-polyhaven-provenance.ps1
```

Result:

```text
Poly Haven provenance validation PASS: files=40, hashes=40, license=CC0 1.0 Universal
```

The manifest covers all 12 approved surface files and all 28 glTF nature source files. Recomputed SHA256 values matched every entry; there were no missing, mismatched, or orphaned hashes.

## UE import quality gate

Command:

```powershell
& 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe' `
  'D:\GameDev\truong-sinh-ue5\TruongSinhUE5.uproject' `
  '-ExecutePythonScript=D:\GameDev\truong-sinh-ue5\tools\audit-polyhaven-nature.py' `
  -unattended -NoSplash -NoSound -NullRHI
```

Result: `Poly Haven nature quality audit PASS`.

All six imported `*_nor_gl` textures are configured as `TC_NORMALMAP`, have `sRGB=False`, and have `flip_green_channel=True`. This is the expected UE conversion from the glTF/OpenGL green-channel convention to Unreal's DirectX convention.

## Imported static meshes

The UE 5.8 Python API exposed the imported render vertex count but not a triangle-count call. `t=UNAVAILABLE` in the commandlet output is therefore reported honestly rather than inferred from source glTF accessors. All 18 meshes have one imported LOD and Nanite enabled.

| Exact object path | Bounds XYZ (cm) | LOD0 vertices | LODs | Nanite |
|---|---:|---:|---:|---|
| `/Game/VisualTarget/Nature/fern_02/fern_02_1k/StaticMeshes/fern_02_a.fern_02_a` | 55.19 × 61.34 × 28.68 | 369 | 1 | Yes |
| `/Game/VisualTarget/Nature/fern_02/fern_02_1k/StaticMeshes/fern_02_b.fern_02_b` | 98.97 × 89.37 × 42.77 | 819 | 1 | Yes |
| `/Game/VisualTarget/Nature/fern_02/fern_02_1k/StaticMeshes/fern_02_c.fern_02_c` | 87.43 × 76.52 × 34.90 | 952 | 1 | Yes |
| `/Game/VisualTarget/Nature/fern_02/fern_02_1k/StaticMeshes/fern_02_d.fern_02_d` | 57.28 × 59.33 × 21.31 | 384 | 1 | Yes |
| `/Game/VisualTarget/Nature/pine_sapling_small/pine_sapling_small_1k/StaticMeshes/pine_sapling_small_a.pine_sapling_small_a` | 78.77 × 72.74 × 129.90 | 1,947 | 1 | Yes |
| `/Game/VisualTarget/Nature/pine_sapling_small/pine_sapling_small_1k/StaticMeshes/pine_sapling_small_b.pine_sapling_small_b` | 52.88 × 73.03 × 104.56 | 1,821 | 1 | Yes |
| `/Game/VisualTarget/Nature/pine_sapling_small/pine_sapling_small_1k/StaticMeshes/pine_sapling_small_c.pine_sapling_small_c` | 51.28 × 58.00 × 96.13 | 1,758 | 1 | Yes |
| `/Game/VisualTarget/Nature/rock_face_01/rock_face_01_2k/StaticMeshes/rock_face_01_2k.rock_face_01_2k` | 495.35 × 382.73 × 356.40 | 2,070 | 1 | Yes |
| `/Game/VisualTarget/Nature/rock_moss_set_01/rock_moss_set_01_2k/StaticMeshes/rock_moss_set_01_rock01.rock_moss_set_01_rock01` | 223.76 × 336.88 × 147.27 | 2,087 | 1 | Yes |
| `/Game/VisualTarget/Nature/rock_moss_set_01/rock_moss_set_01_2k/StaticMeshes/rock_moss_set_01_rock02.rock_moss_set_01_rock02` | 265.54 × 326.37 × 126.20 | 1,632 | 1 | Yes |
| `/Game/VisualTarget/Nature/rock_moss_set_01/rock_moss_set_01_2k/StaticMeshes/rock_moss_set_01_rock03.rock_moss_set_01_rock03` | 210.86 × 207.96 × 109.97 | 1,177 | 1 | Yes |
| `/Game/VisualTarget/Nature/rock_moss_set_01/rock_moss_set_01_2k/StaticMeshes/rock_moss_set_01_rock04.rock_moss_set_01_rock04` | 212.76 × 196.47 × 176.83 | 1,487 | 1 | Yes |
| `/Game/VisualTarget/Nature/rock_moss_set_01/rock_moss_set_01_2k/StaticMeshes/rock_moss_set_01_rock05.rock_moss_set_01_rock05` | 181.73 × 299.87 × 120.55 | 1,195 | 1 | Yes |
| `/Game/VisualTarget/Nature/rock_moss_set_01/rock_moss_set_01_2k/StaticMeshes/rock_moss_set_01_rock06.rock_moss_set_01_rock06` | 212.60 × 275.73 × 126.19 | 1,311 | 1 | Yes |
| `/Game/VisualTarget/Nature/shrub_02/shrub_02_1k/StaticMeshes/shrub_02_a.shrub_02_a` | 131.20 × 164.10 × 168.90 | 1,375 | 1 | Yes |
| `/Game/VisualTarget/Nature/shrub_02/shrub_02_1k/StaticMeshes/shrub_02_b.shrub_02_b` | 111.02 × 127.99 × 150.63 | 1,100 | 1 | Yes |
| `/Game/VisualTarget/Nature/shrub_02/shrub_02_1k/StaticMeshes/shrub_02_c.shrub_02_c` | 176.68 × 228.51 × 131.48 | 1,599 | 1 | Yes |
| `/Game/VisualTarget/Nature/shrub_02/shrub_02_1k/StaticMeshes/shrub_02_d.shrub_02_d` | 108.02 × 109.80 × 122.92 | 1,053 | 1 | Yes |

Quality note: one LOD is acceptable for this imported slice because Nanite is enabled, but masked foliage cost and overdraw still require an RTX 3060 profile in the actual corridor before the visual target can pass `PERF-001`.
