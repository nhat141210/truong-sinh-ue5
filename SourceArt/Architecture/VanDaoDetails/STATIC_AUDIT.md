# Vấn Đạo Tông Details Kit — Static Audit

Generated headlessly by `tools/create-vandao-details.py` with Blender 5.2.

- Original project-authored geometry: PASS
- External meshes, textures, and generators: none
- Authoring units: metres (`METRIC`, `scale_length=1.0`)
- FBX transform boundary: one joined mesh per asset; location/rotation/scale baked
- Placement contract: XY centred and min-Z grounded at zero; UE import scale 1.0
- Clean Blender 5.2 FBX re-import: PASS (each asset re-opened as one mesh with identity transform and min-Z within 0.00001 m)
- Detail motifs: curved roof eave, carved wood screen, three-lantern cluster, meditation dais, stone approach steps

| Asset | Vertices | Triangles (estimate) | Dimensions (m) | Bounds min (m) | SHA-256 |
|---|---:|---:|---|---|---|
| `SM_VDT_CurvedRoofEave_600.fbx` | 2844 | 5352 | 6.8200 × 2.6350 × 1.3873 | (-3.4100, -1.3175, 0.0000) | `2c4cd5b6a99773267d089c057c1f87a62ed87d609ef680ee2a28eab24b6419b0` |
| `SM_VDT_WoodScreen_480.fbx` | 1984 | 3832 | 5.5200 × 0.6851 × 3.8700 | (-2.7600, -0.3426, 0.0000) | `51d555bc7328ae1dcb6b60349e29be3dd39632deb3b98696ca0bd3d77d2fe620` |
| `SM_VDT_LanternCluster_300.fbx` | 2478 | 4828 | 2.4408 × 1.1600 × 3.7958 | (-1.2204, -0.5800, 0.0000) | `e38b70072b0af473de1e23bd871bfc105fd44d39db725b89ca276db580d031be` |
| `SM_VDT_MeditationDais_500.fbx` | 2136 | 4208 | 4.8731 × 4.8731 × 1.0094 | (-2.4365, -2.4365, 0.0000) | `b065dde58d519dbf0ec7611fda884b40edf2e45f1e0128df8a05d04f22f55e1a` |
| `SM_VDT_StoneSteps_600.fbx` | 2988 | 5872 | 6.5000 × 3.1200 × 2.6500 | (-3.2500, -1.5600, 0.0000) | `6c2f20a7665d733301aeb99bef593c3dacde098bc419fbf258c0f7950004a32b` |
