"""Generate the original Vấn Đạo Tông floating-island foundation in Blender."""

from __future__ import annotations

import hashlib
import json
from pathlib import Path

import bpy


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "SourceArt" / "Architecture" / "VanDaoIsland"
FBX = OUT / "SM_VDT_FloatingIsland.fbx"
BLEND = OUT / "SM_VDT_FloatingIsland.blend"
MANIFEST = OUT / "MANIFEST.json"


def material(name: str, color: tuple[float, float, float], roughness: float):
    value = bpy.data.materials.new(name)
    value.diffuse_color = (*color, 1.0)
    value.use_nodes = True
    bsdf = value.node_tree.nodes.get("Principled BSDF")
    bsdf.inputs["Base Color"].default_value = (*color, 1.0)
    bsdf.inputs["Roughness"].default_value = roughness
    return value


def main() -> None:
    bpy.ops.wm.read_factory_settings(use_empty=True)
    OUT.mkdir(parents=True, exist_ok=True)
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 1.0

    top_material = material("M_VDT_IslandTop", (0.14, 0.18, 0.16), 0.82)
    cliff_material = material("M_VDT_IslandCliff", (0.055, 0.07, 0.065), 0.94)

    stations = 19
    vertices: list[tuple[float, float, float]] = []
    top_left: list[int] = []
    top_right: list[int] = []
    lower_left: list[int] = []
    lower_right: list[int] = []
    bottom: list[int] = []

    widths = (11.0, 12.6, 13.5, 14.1, 13.7, 14.8, 14.3, 15.1, 14.6,
              15.0, 14.2, 14.8, 13.9, 14.5, 13.4, 13.8, 12.7, 11.9, 10.4)
    for index in range(stations):
        x = -45.0 + index * 5.0
        width = widths[index]
        lower_z = -2.8 - (index % 4) * 0.42
        bottom_z = -8.4 - ((index * 7) % 5) * 0.72
        for collection, point in (
            (top_left, (x, -width, 0.0)),
            (top_right, (x, width, 0.0)),
            (lower_left, (x, -width * 0.78, lower_z)),
            (lower_right, (x, width * 0.78, lower_z - 0.35)),
            (bottom, (x, (index % 3 - 1) * 0.38, bottom_z)),
        ):
            collection.append(len(vertices))
            vertices.append(point)

    faces: list[tuple[int, ...]] = []
    material_indices: list[int] = []

    def face(indices: tuple[int, ...], material_index: int) -> None:
        faces.append(indices)
        material_indices.append(material_index)

    for index in range(stations - 1):
        nxt = index + 1
        face((top_left[index], top_left[nxt], top_right[nxt], top_right[index]), 0)
        face((top_left[index], lower_left[index], lower_left[nxt], top_left[nxt]), 1)
        face((top_right[index], top_right[nxt], lower_right[nxt], lower_right[index]), 1)
        face((lower_left[index], bottom[index], bottom[nxt], lower_left[nxt]), 1)
        face((bottom[index], lower_right[index], lower_right[nxt], bottom[nxt]), 1)

    face((top_left[0], top_right[0], lower_right[0], bottom[0], lower_left[0]), 1)
    face((top_right[-1], top_left[-1], lower_left[-1], bottom[-1], lower_right[-1]), 1)

    mesh = bpy.data.meshes.new("SM_VDT_FloatingIsland_Mesh")
    mesh.from_pydata(vertices, [], faces)
    mesh.materials.append(top_material)
    mesh.materials.append(cliff_material)
    for polygon, material_index in zip(mesh.polygons, material_indices):
        polygon.material_index = material_index
        polygon.use_smooth = material_index == 1
    mesh.update()

    island = bpy.data.objects.new("SM_VDT_FloatingIsland", mesh)
    bpy.context.collection.objects.link(island)
    bpy.context.view_layer.objects.active = island
    island.select_set(True)

    bevel = island.modifiers.new("Weathered island rim", "BEVEL")
    bevel.width = 0.18
    bevel.segments = 3
    bevel.limit_method = "ANGLE"
    bpy.ops.object.modifier_apply(modifier=bevel.name)
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
    MANIFEST.write_text(
        json.dumps(
            {
                "asset": FBX.name,
                "sha256": digest,
                "source": "Original procedural Blender geometry authored for this project",
                "license": "Project-owned original geometry",
                "blender": bpy.app.version_string,
                "source_blend": {
                    "file": BLEND.name,
                    "sha256": hashlib.sha256(BLEND.read_bytes()).hexdigest(),
                },
                "dimensions_m": [round(v, 3) for v in island.dimensions],
                "vertices": len(mesh.vertices),
                "triangles": len(mesh.loop_triangles),
                "material_slots": ["M_VDT_IslandTop", "M_VDT_IslandCliff"],
                "ue_import": {"uniform_scale": 1.0, "collision": "auto convex"},
            },
            ensure_ascii=False,
            indent=2,
        ) + "\n",
        encoding="utf-8",
    )
    print(f"VDT_FLOATING_ISLAND_PASS {FBX} {digest}")


if __name__ == "__main__":
    main()
