# Vấn Đạo Tông gate transform audit

## Finding

The detached pieces visible in VDT7 are gate geometry, not Unreal Editor
sprites. Their silhouettes match the authored lantern roofs, roof tile courses,
finial wings and ridge ornaments. The original FBX retained 121 independently
transformed objects, allowing Unreal's combined static-mesh import path to
reinterpret object rotations/origins.

The authored model itself is spatially coherent. A clean Blender 5.2 import of
the original FBX measured a single compact envelope:

- Mesh objects: 121
- World bounds: `(-9.3176, -2.6087, 0.0)` to `(9.3176, 2.6087, 9.6931)` m
- Dimensions: `18.6353 x 5.2175 x 9.6931` m
- Objects outside the design envelope: 0

## Correction and validation

`tools/create-vandao-gate.py` now retains the editable multi-object `.blend`,
then converts the plaque text, joins all export geometry, and bakes location,
rotation and scale before writing the FBX.

A clean Blender 5.2 re-import of the corrected FBX measured:

- Mesh objects: 1
- Vertices: 22,756
- World bounds: `(-9.3034, -2.5969, 0.0)` to `(9.3034, 2.5969, 9.6887)` m
- Dimensions: `18.6068 x 5.1938 x 9.6887` m
- Object location: `(0, 0, 0)`
- Object rotation: approximately `(0, 0, 0)`
- Object scale: `(1, 1, 1)`
- Named material slots preserved: 7

This removes per-piece FBX transforms from the Unreal import boundary while
preserving the intended assembled geometry and materials.
