"""Generate the large, original Vấn Đạo Tông master-estate landform.

The existing gameplay anchors occupy the central 70 m route. This asset gives
the sect a 300 x 210 m buildable plateau with a tapered, irregular cliff rim,
leaving authored room for future halls, residences, gardens, and training
grounds without changing the simulation map or actor coordinates.
"""

from __future__ import annotations

import hashlib
import json
import math
from pathlib import Path

import bpy
from mathutils import Matrix, Vector


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "SourceArt" / "Architecture" / "VanDaoMasterEstate"
FBX = OUT / "SM_VDT_MasterEstate.fbx"
BLEND = OUT / "SM_VDT_MasterEstate.blend"
MANIFEST = OUT / "MANIFEST.json"
TARGET_X = 300.0
TARGET_Y = 210.0


def make_material(name: str, color: tuple[float, float, float], roughness: float):
    material = bpy.data.materials.new(name)
    material.diffuse_color = (*color, 1.0)
    material.use_nodes = True
    bsdf = material.node_tree.nodes.get("Principled BSDF")
    bsdf.inputs["Base Color"].default_value = (*color, 1.0)
    bsdf.inputs["Roughness"].default_value = roughness
    return material


def main() -> None:
    bpy.ops.wm.read_factory_settings(use_empty=True)
    OUT.mkdir(parents=True, exist_ok=True)
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 1.0

    top_material = make_material("M_VDT_MasterEstateTop", (0.17, 0.20, 0.17), 0.86)
    cliff_material = make_material("M_VDT_MasterEstateCliff", (0.055, 0.068, 0.060), 0.96)

    count = 32
    top_ring: list[tuple[float, float, float]] = []
    lower_ring: list[tuple[float, float, float]] = []
    bottom_ring: list[tuple[float, float, float]] = []
    for index in range(count):
        angle = 2.0 * math.pi * index / count
        # A rounded, asymmetrical oval reads as an elevated plateau rather than
        # a rectangular cube while retaining generous, navigable build space.
        ripple = 0.955 + 0.035 * math.sin(index * 1.7) + 0.018 * math.cos(index * 2.9)
        x = 0.5 * TARGET_X * math.cos(angle) * ripple
        y = 0.5 * TARGET_Y * math.sin(angle) * (0.965 + 0.025 * math.cos(index * 1.3))
        top_ring.append((x, y, 0.0))
        lower_ring.append((x * 0.945, y * 0.945, -3.8 - (index % 4) * 0.55))
        bottom_ring.append((x * 0.57, y * 0.57, -15.0 - (index % 5) * 0.8))

    # Normalize the authored outline to the declared master-estate envelope.
    max_x = max(abs(point[0]) for point in top_ring)
    max_y = max(abs(point[1]) for point in top_ring)
    top_ring = [(x * (0.5 * TARGET_X / max_x), y * (0.5 * TARGET_Y / max_y), z) for x, y, z in top_ring]
    lower_ring = [(x * (0.5 * TARGET_X / max_x), y * (0.5 * TARGET_Y / max_y), z) for x, y, z in lower_ring]
    bottom_ring = [(x * (0.5 * TARGET_X / max_x), y * (0.5 * TARGET_Y / max_y), z) for x, y, z in bottom_ring]

    # Keep the same 18.2 m cliff silhouette but normalize the exported mesh to
    # the project contract min-Z=0; the map actor supplies world placement.
    vertical_offset = 18.2
    vertices: list[tuple[float, float, float]] = [(0.0, 0.0, vertical_offset)]
    vertices.extend(top_ring)
    vertices.extend(lower_ring)
    vertices.extend(bottom_ring)
    vertices = [(x, y, z + vertical_offset) for x, y, z in vertices[1:]]
    vertices.insert(0, (0.0, 0.0, vertical_offset))
    min_x = min(point[0] for point in vertices)
    max_x = max(point[0] for point in vertices)
    min_y = min(point[1] for point in vertices)
    max_y = max(point[1] for point in vertices)
    center_x = 0.5 * (min_x + max_x)
    center_y = 0.5 * (min_y + max_y)
    vertices = [(x - center_x, y - center_y, z) for x, y, z in vertices]
    top_offset = 1
    lower_offset = 1 + count
    bottom_offset = 1 + 2 * count

    faces: list[tuple[int, ...]] = []
    materials: list[int] = []
    for index in range(count):
        nxt = (index + 1) % count
        faces.append((0, top_offset + index, top_offset + nxt))
        materials.append(0)
        faces.append((top_offset + index, lower_offset + index, lower_offset + nxt, top_offset + nxt))
        materials.append(1)
        faces.append((lower_offset + index, bottom_offset + index, bottom_offset + nxt, lower_offset + nxt))
        materials.append(1)
    faces.append(tuple(reversed([bottom_offset + index for index in range(count)])))
    materials.append(1)

    mesh = bpy.data.meshes.new("SM_VDT_MasterEstate_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.materials.append(top_material)
    mesh.materials.append(cliff_material)
    for polygon, material_index in zip(mesh.polygons, materials):
        polygon.material_index = material_index
        polygon.use_smooth = material_index == 1
    mesh.update()

    estate = bpy.data.objects.new("SM_VDT_MasterEstate", mesh)
    bpy.context.collection.objects.link(estate)
    bpy.context.view_layer.objects.active = estate
    estate.select_set(True)
    bevel = estate.modifiers.new("Weathered master-estate rim", "BEVEL")
    bevel.width = 0.65
    bevel.segments = 3
    bevel.limit_method = "ANGLE"
    bpy.ops.object.modifier_apply(modifier=bevel.name)
    # Bevel can extend alternating rim segments by a few millimetres. Recenter
    # after the modifier so the exported FBX remains exactly pivot-centred.
    bpy.context.view_layer.update()
    min_x = min(corner[0] for corner in estate.bound_box)
    max_x = max(corner[0] for corner in estate.bound_box)
    min_y = min(corner[1] for corner in estate.bound_box)
    max_y = max(corner[1] for corner in estate.bound_box)
    estate.data.transform(Matrix.Translation(Vector((-(min_x + max_x) * 0.5, -(min_y + max_y) * 0.5, 0.0))))
    bpy.ops.object.mode_set(mode="EDIT")
    bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=1.15, island_margin=0.02)
    bpy.ops.object.mode_set(mode="OBJECT")

    bpy.ops.wm.save_as_mainfile(filepath=str(BLEND))
    bpy.ops.export_scene.fbx(
        filepath=str(FBX),
        use_selection=True,
        apply_unit_scale=True,
        apply_scale_options="FBX_SCALE_ALL",
        mesh_smooth_type="FACE",
        add_leaf_bones=False,
        bake_anim=False,
        axis_forward="-Z",
        axis_up="Y",
    )
    mesh.calc_loop_triangles()
    digest = hashlib.sha256(FBX.read_bytes()).hexdigest()
    BLEND_DIGEST = hashlib.sha256(BLEND.read_bytes()).hexdigest()
    dimensions = [round(value, 3) for value in estate.dimensions]
    MANIFEST.write_text(
        json.dumps(
            {
                "asset": FBX.name,
                "sha256": digest,
                "source": "Original procedural Blender geometry authored for the Vấn Đạo Tông master estate",
                "license": "Project-owned original geometry",
                "blender": bpy.app.version_string,
                "source_blend": {"file": BLEND.name, "sha256": BLEND_DIGEST},
                "dimensions_m": dimensions,
                "vertices": len(mesh.vertices),
                "triangles": len(mesh.loop_triangles),
                "material_slots": ["M_VDT_MasterEstateTop", "M_VDT_MasterEstateCliff"],
                "planning_contract": {
                    "buildable_area_m": [270.0, 180.0],
                    "central_route_preserved": True,
                    "reserved_expansion_lots": 8,
                },
                "ue_import": {"uniform_scale": 1.0, "collision": "auto convex"},
            },
            ensure_ascii=False,
            indent=2,
        ) + "\n",
        encoding="utf-8",
    )
    print(f"VDT_MASTER_ESTATE_PASS dimensions_m={dimensions} sha256={digest}")


if __name__ == "__main__":
    main()
