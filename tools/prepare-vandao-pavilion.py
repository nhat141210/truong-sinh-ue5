"""Prepare the approved CC0 pavilion donor for Unreal Engine.

The source remains intact in SourceArt.  This script produces a single,
10-metre hero pavilion with five stable material slots and applied transforms.
Run with Blender:
  blender --background --python tools/prepare-vandao-pavilion.py -- \
    --input <source.fbx> --output <derived.fbx> --report <report.json>
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

import bpy


def options() -> argparse.Namespace:
    values = sys.argv[sys.argv.index("--") + 1 :] if "--" in sys.argv else []
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--report", type=Path, required=True)
    return parser.parse_args(values)


def material_class(name: str) -> str:
    if any(token in name for token in ("瓦", "脊", "兽")):
        return "M_VDT_RoofTile"
    if any(token in name for token in ("牌匾", "对联")):
        return "M_VDT_LacquerRed"
    if any(token in name for token in ("金", "雷公")):
        return "M_VDT_AgedBrass"
    if any(token in name for token in ("凳", "扶手")):
        return "M_VDT_DarkWood"
    return "M_VDT_AncientWood"


def main() -> None:
    args = options()
    output = args.output.resolve()
    report = args.report.resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    report.parent.mkdir(parents=True, exist_ok=True)

    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=str(args.input.resolve()), use_anim=False)
    meshes = [obj for obj in bpy.context.scene.objects if obj.type == "MESH"]
    if not meshes:
        raise RuntimeError("Source FBX contains no mesh.")

    slot_materials: dict[str, bpy.types.Material] = {}
    for obj in meshes:
        for slot in obj.material_slots:
            if not slot.material:
                continue
            destination = material_class(slot.material.name)
            if destination not in slot_materials:
                material = bpy.data.materials.new(destination)
                material.diffuse_color = (0.5, 0.5, 0.5, 1.0)
                slot_materials[destination] = material
            slot.material = slot_materials[destination]

    bpy.ops.object.select_all(action="DESELECT")
    for obj in meshes:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = meshes[0]
    bpy.ops.object.join()
    pavilion = bpy.context.object
    pavilion.name = "SM_VDT_Pavilion_Hero"
    pavilion.data.name = "SM_VDT_Pavilion_Hero"

    # Original bounds are approximately 40 Blender units; scale to a 10m hero prop.
    pavilion.scale = (0.25, 0.25, 0.25)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    min_z = min((pavilion.matrix_world @ vertex.co).z for vertex in pavilion.data.vertices)
    pavilion.location.z -= min_z
    bpy.ops.object.transform_apply(location=True, rotation=False, scale=False)

    for polygon in pavilion.data.polygons:
        polygon.use_smooth = True
    modifier = pavilion.modifiers.new("WeightedNormals", "WEIGHTED_NORMAL")
    modifier.keep_sharp = True
    bpy.context.view_layer.objects.active = pavilion
    bpy.ops.object.modifier_apply(modifier=modifier.name)

    bpy.ops.object.shade_smooth_by_angle()
    bpy.ops.object.select_all(action="DESELECT")
    pavilion.select_set(True)
    bpy.context.view_layer.objects.active = pavilion
    bpy.ops.export_scene.fbx(
        filepath=str(output),
        use_selection=True,
        apply_unit_scale=True,
        apply_scale_options="FBX_SCALE_ALL",
        mesh_smooth_type="FACE",
        add_leaf_bones=False,
        bake_anim=False,
        path_mode="AUTO",
    )

    pavilion.data.calc_loop_triangles()
    report.write_text(
        json.dumps(
            {
                "source": str(args.input.resolve()),
                "output": str(output),
                "blender_version": bpy.app.version_string,
                "vertices": len(pavilion.data.vertices),
                "triangles": len(pavilion.data.loop_triangles),
                "dimensions": [round(value, 4) for value in pavilion.dimensions],
                "material_slots": [slot.material.name if slot.material else None for slot in pavilion.material_slots],
            },
            indent=2,
        ),
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()
