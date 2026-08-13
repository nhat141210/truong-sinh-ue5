"""Normalize an FBX authored in centimetre numbers to UE's metre source rule.

Contract: Blender geometry is modelled in metres, then UE imports FBX to
centimetres at uniform scale 1.0.  Older Vấn Đạo Tông props were authored with
centimetre values in Blender; this converts their geometry once by 0.01 without
touching the provenance original.
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

import bpy
from mathutils import Vector


def arguments() -> argparse.Namespace:
    raw = sys.argv[sys.argv.index("--") + 1 :] if "--" in sys.argv else []
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    return parser.parse_args(raw)


def main() -> None:
    args = arguments()
    output = args.output.resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=str(args.input.resolve()), use_anim=False)
    meshes = [item for item in bpy.context.scene.objects if item.type == "MESH"]
    if not meshes:
        raise RuntimeError("No mesh found in source FBX")
    for item in meshes:
        item.scale = (0.01, 0.01, 0.01)
        item.select_set(True)
    # UE's Combine Meshes path can mis-handle transforms of FBXs containing
    # many child objects. Joining in Blender bakes every object's world matrix
    # into one mesh while preserving all material slots.
    bpy.context.view_layer.objects.active = meshes[0]
    bpy.ops.object.join()
    normalized = bpy.context.object
    normalized.name = args.output.stem
    normalized.data.name = args.output.stem + "_Mesh"
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    coordinates = [normalized.matrix_world @ vertex.co for vertex in normalized.data.vertices]
    min_x = min(value.x for value in coordinates)
    max_x = max(value.x for value in coordinates)
    min_y = min(value.y for value in coordinates)
    max_y = max(value.y for value in coordinates)
    min_z = min(value.z for value in coordinates)
    origin_offset = Vector((-(min_x + max_x) * 0.5, -(min_y + max_y) * 0.5, -min_z))
    for vertex in normalized.data.vertices:
        vertex.co += origin_offset
    normalized.data.update()
    bpy.context.scene.unit_settings.system = "METRIC"
    bpy.context.scene.unit_settings.scale_length = 1.0
    bpy.ops.object.select_all(action="DESELECT")
    normalized.select_set(True)
    bpy.context.view_layer.objects.active = normalized
    bpy.ops.export_scene.fbx(
        filepath=str(output),
        use_selection=True,
        apply_unit_scale=True,
        apply_scale_options="FBX_SCALE_UNITS",
        mesh_smooth_type="FACE",
        add_leaf_bones=False,
        bake_anim=False,
    )
    print(f"NORMALIZED_FBX_METRE_CONTRACT_OK {output}")


if __name__ == "__main__":
    main()
