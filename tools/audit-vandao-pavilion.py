"""Audit and render a neutral preview of the Vấn Đạo Tông pavilion donor.

Run with Blender in background mode, for example:
  blender --background --python tools/audit-vandao-pavilion.py -- \
    --input path/to/model.fbx --output-dir Artifacts/VisualTarget/VDT0/PavilionAudit
"""

from __future__ import annotations

import argparse
import json
import math
import sys
from pathlib import Path

import bmesh
import bpy
from mathutils import Vector


def parse_args() -> argparse.Namespace:
    args = sys.argv[sys.argv.index("--") + 1 :] if "--" in sys.argv else []
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    return parser.parse_args(args)


def look_at(camera: bpy.types.Object, target: Vector) -> None:
    camera.rotation_euler = (target - camera.location).to_track_quat("-Z", "Y").to_euler()


def audit_mesh(obj: bpy.types.Object) -> dict[str, object]:
    mesh = obj.data
    mesh.calc_loop_triangles()
    bm = bmesh.new()
    bm.from_mesh(mesh)
    non_manifold = sum(1 for edge in bm.edges if not edge.is_manifold)
    bm.free()
    return {
        "name": obj.name,
        "vertices": len(mesh.vertices),
        "polygons": len(mesh.polygons),
        "triangles": len(mesh.loop_triangles),
        "non_manifold_edges": non_manifold,
        "material_slots": [slot.material.name if slot.material else None for slot in obj.material_slots],
        "dimensions": [round(value, 4) for value in obj.dimensions],
    }


def create_neutral_material() -> bpy.types.Material:
    material = bpy.data.materials.new("M_Audit_NeutralClay")
    material.diffuse_color = (0.28, 0.14, 0.07, 1.0)
    material.use_nodes = True
    bsdf = material.node_tree.nodes.get("Principled BSDF")
    bsdf.inputs["Base Color"].default_value = (0.24, 0.10, 0.045, 1.0)
    bsdf.inputs["Roughness"].default_value = 0.62
    return material


def main() -> None:
    options = parse_args()
    source = options.input.resolve()
    output = options.output_dir.resolve()
    output.mkdir(parents=True, exist_ok=True)

    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=str(source), use_anim=False)
    meshes = [obj for obj in bpy.context.scene.objects if obj.type == "MESH"]
    if not meshes:
        raise RuntimeError(f"FBX imported no mesh objects: {source}")

    corners = [obj.matrix_world @ Vector(corner) for obj in meshes for corner in obj.bound_box]
    minimum = Vector((min(p.x for p in corners), min(p.y for p in corners), min(p.z for p in corners)))
    maximum = Vector((max(p.x for p in corners), max(p.y for p in corners), max(p.z for p in corners)))
    center = (minimum + maximum) * 0.5
    extent = maximum - minimum
    radius = max(extent.length * 0.62, 1.0)

    original_materials = sorted({slot.material.name for obj in meshes for slot in obj.material_slots if slot.material})
    mesh_audits = [audit_mesh(obj) for obj in meshes]
    audit = {
        "source": str(source),
        "blender_version": bpy.app.version_string,
        "mesh_count": len(meshes),
        "object_count": len(bpy.context.scene.objects),
        "materials": original_materials,
        "bounds_min": [round(value, 4) for value in minimum],
        "bounds_max": [round(value, 4) for value in maximum],
        "bounds_extent": [round(value, 4) for value in extent],
        "totals": {
            "vertices": sum(len(obj.data.vertices) for obj in meshes),
            "polygons": sum(len(obj.data.polygons) for obj in meshes),
            "triangles": sum(item["triangles"] for item in mesh_audits),
            "non_manifold_edges": sum(item["non_manifold_edges"] for item in mesh_audits),
        },
        "meshes": mesh_audits,
    }

    neutral = create_neutral_material()
    for obj in meshes:
        obj.data.materials.clear()
        obj.data.materials.append(neutral)

    bpy.ops.mesh.primitive_plane_add(size=max(extent.x, extent.y) * 3.0, location=(center.x, center.y, minimum.z))
    ground = bpy.context.object
    ground_mat = bpy.data.materials.new("M_Audit_Ground")
    ground_mat.diffuse_color = (0.035, 0.045, 0.04, 1.0)
    ground.data.materials.append(ground_mat)

    bpy.ops.object.light_add(type="SUN", location=center + Vector((radius, -radius, radius * 1.4)))
    key = bpy.context.object
    key.data.energy = 3.5
    key.data.angle = math.radians(18.0)
    look_at(key, center)

    bpy.ops.object.light_add(type="AREA", location=center + Vector((-radius, radius * 0.3, radius * 0.7)))
    fill = bpy.context.object
    fill.data.energy = 4500.0
    fill.data.size = radius
    look_at(fill, center)

    bpy.ops.object.camera_add(location=center + Vector((radius * 1.6, -radius * 1.8, radius * 1.05)))
    camera = bpy.context.object
    camera.data.lens = 55.0
    look_at(camera, center + Vector((0.0, 0.0, extent.z * 0.08)))
    bpy.context.scene.camera = camera

    world = bpy.context.scene.world or bpy.data.worlds.new("World")
    bpy.context.scene.world = world
    world.use_nodes = True
    background = world.node_tree.nodes.get("Background")
    background.inputs["Color"].default_value = (0.055, 0.07, 0.065, 1.0)
    background.inputs["Strength"].default_value = 0.4
    scene = bpy.context.scene
    scene.render.engine = "BLENDER_EEVEE"
    scene.render.resolution_x = 1280
    scene.render.resolution_y = 720
    scene.render.resolution_percentage = 100
    scene.render.image_settings.file_format = "PNG"
    scene.render.filepath = str(output / "pavilion-neutral.png")
    scene.render.film_transparent = False
    bpy.ops.render.render(write_still=True)

    audit["render"] = str((output / "pavilion-neutral.png").resolve())
    (output / "audit.json").write_text(json.dumps(audit, indent=2, ensure_ascii=False), encoding="utf-8")
    print(json.dumps(audit["totals"], sort_keys=True))


if __name__ == "__main__":
    main()
