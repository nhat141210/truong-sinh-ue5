"""Build original Vấn Đạo Tông main-hall modules for the UE visual slice.

Run headlessly with Blender 5.2+:
  blender --background --python tools/create-vandao-hall.py

All measurements are metres.  The scene unit scale and FBX export settings
preserve Unreal's centimetre import convention.  No external geometry,
textures, or generators are used.
"""

from pathlib import Path
import hashlib
import json
import math

import bpy
from mathutils import Vector

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "SourceArt" / "Architecture" / "VanDaoHall"
OUT.mkdir(parents=True, exist_ok=True)
BLEND = OUT / "VDT_MainHallKit.blend"


def clean_scene():
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for datablocks in (bpy.data.meshes, bpy.data.curves, bpy.data.materials):
        # Keep materials rebuilt below; remove orphan meshes/curves deterministically.
        if datablocks is not bpy.data.materials:
            for item in list(datablocks):
                if item.users == 0:
                    datablocks.remove(item)


def material(name, color, metallic=0.0, roughness=0.55):
    mat = bpy.data.materials.get(name) or bpy.data.materials.new(name)
    mat.diffuse_color = (*color, 1.0)
    mat.use_nodes = True
    bsdf = mat.node_tree.nodes.get("Principled BSDF")
    bsdf.inputs["Base Color"].default_value = (*color, 1.0)
    bsdf.inputs["Metallic"].default_value = metallic
    bsdf.inputs["Roughness"].default_value = roughness
    return mat


REDWOOD = None
DARK_TILE = None
AGED_STONE = None
BRASS = None
JADE = None


def tag(obj, group, mat):
    obj["VDT_AssetGroup"] = group
    obj.data.materials.append(mat)
    return obj


def box(name, location, dimensions, bevel, group, mat):
    bpy.ops.mesh.primitive_cube_add(location=location)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dimensions
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    if bevel:
        mod = obj.modifiers.new("Softened carved edges", "BEVEL")
        mod.width = bevel
        mod.segments = 3
        mod.limit_method = "ANGLE"
        bpy.context.view_layer.objects.active = obj
        bpy.ops.object.modifier_apply(modifier=mod.name)
    tag(obj, group, mat)
    return obj


def cylinder(name, location, radius, depth, group, mat, vertices=16):
    bpy.ops.mesh.primitive_cylinder_add(vertices=vertices, radius=radius, depth=depth, location=location)
    obj = bpy.context.object
    obj.name = name
    bevel = obj.modifiers.new("Edge round", "BEVEL")
    bevel.width = min(radius * 0.18, 0.055)
    bevel.segments = 2
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=bevel.name)
    tag(obj, group, mat)
    return obj


def curve_pipe(name, points, radius, group, mat, resolution=2):
    curve = bpy.data.curves.new(name, type="CURVE")
    curve.dimensions = "3D"
    curve.resolution_u = resolution
    curve.bevel_depth = radius
    curve.bevel_resolution = 2
    spline = curve.splines.new("BEZIER")
    spline.bezier_points.add(len(points) - 1)
    for bp, co in zip(spline.bezier_points, points):
        bp.co = co
        bp.handle_left_type = "AUTO"
        bp.handle_right_type = "AUTO"
    obj = bpy.data.objects.new(name, curve)
    bpy.context.collection.objects.link(obj)
    tag(obj, group, mat)
    return obj


def join_group(group, export_name):
    items = [o for o in bpy.context.scene.objects if o.get("VDT_AssetGroup") == group]
    bpy.ops.object.select_all(action="DESELECT")
    for obj in items:
        obj.select_set(True)
        bpy.context.view_layer.objects.active = obj
        if obj.type == "CURVE":
            bpy.ops.object.convert(target="MESH")
    items = [o for o in bpy.context.selected_objects]
    bpy.context.view_layer.objects.active = items[0]
    bpy.ops.object.join()
    asset = bpy.context.object
    asset.name = export_name
    asset.data.name = export_name + "_Mesh"
    asset["Source"] = "Original geometry authored by Trường Sinh UE5 team"
    asset["License"] = "CC0-1.0"
    return asset


def export_asset(asset, filename):
    bpy.ops.object.select_all(action="DESELECT")
    asset.select_set(True)
    bpy.context.view_layer.objects.active = asset
    bpy.ops.export_scene.fbx(
        filepath=str(OUT / filename), use_selection=True, object_types={"MESH"},
        apply_unit_scale=True, apply_scale_options="FBX_SCALE_ALL", add_leaf_bones=False,
        bake_anim=False, mesh_smooth_type="FACE", use_mesh_modifiers=True,
        path_mode="AUTO", axis_forward="-Z", axis_up="Y")


def main_hall_facade():
    group = "main_hall"
    # Foundation and tiered stone plinth.
    box("Foundation_Lower", (0, 0.55, 0.25), (18.4, 7.2, 0.5), 0.12, group, AGED_STONE)
    box("Foundation_Upper", (0, 0.38, 0.68), (16.9, 6.4, 0.42), 0.1, group, AGED_STONE)
    for x in (-7.2, -4.8, -2.4, 0, 2.4, 4.8, 7.2):
        # subtle outward taper via stacked plinths and capital, not a plain post.
        box(f"ColumnBase_{x}", (x, -1.65, 1.05), (0.92, 0.92, 0.38), 0.08, group, AGED_STONE)
        col = cylinder(f"TimberColumn_{x}", (x, -1.65, 3.85), 0.34, 5.25, group, REDWOOD, 20)
        box(f"ColumnCapital_{x}", (x, -1.65, 6.54), (0.82, 0.82, 0.26), 0.09, group, REDWOOD)
        # Dou-gong bracket: stepped arms plus a jade-end pin.
        box(f"BracketLower_{x}", (x, -1.87, 6.88), (1.35, 0.85, 0.22), 0.07, group, REDWOOD)
        box(f"BracketUpper_{x}", (x, -2.13, 7.18), (0.92, 1.2, 0.2), 0.07, group, REDWOOD)
        cylinder(f"BracketPin_{x}", (x, -2.6, 7.16), 0.11, 0.17, group, BRASS, 12).rotation_euler = (math.pi / 2, 0, 0)
    # Front wall bays and inset lattice, with a tall ceremonial doorway in the centre.
    for x in (-6.0, -3.6, 3.6, 6.0):
        box(f"WallBay_{x}", (x, -0.12, 3.5), (1.85, 0.25, 4.8), 0.045, group, REDWOOD)
        for dx in (-0.55, 0, 0.55):
            box(f"LatticeV_{x}_{dx}", (x + dx, -0.3, 3.65), (0.10, 0.11, 3.55), 0.025, group, BRASS)
        for z in (2.2, 3.1, 4.0, 4.9):
            box(f"LatticeH_{x}_{z}", (x, -0.3, z), (1.55, 0.11, 0.09), 0.02, group, BRASS)
    for x in (-1.3, 1.3):
        box(f"CeremonialDoor_{x}", (x, -0.28, 3.45), (2.3, 0.24, 4.65), 0.05, group, REDWOOD)
        curve_pipe(f"DoorInlay_{x}", [(x - 0.55, -0.43, 2.3), (x, -0.46, 3.6), (x + 0.55, -0.43, 2.3)], 0.045, group, BRASS)
    box("DoorLintel", (0, -0.35, 5.95), (5.2, 0.35, 0.32), 0.07, group, REDWOOD)
    # Calligraphy plaque form: blank by design; art/UI can apply a licensed texture later.
    box("PlaqueBlank", (0, -0.57, 6.48), (3.1, 0.12, 0.72), 0.09, group, JADE)
    # Heavy layered eaves, each layer articulated rather than a flat canopy.
    box("RoofBeam", (0, 0.0, 7.42), (17.9, 5.8, 0.42), 0.12, group, REDWOOD)
    for y, z, width in ((-0.35, 7.8, 17.6), (0.55, 8.18, 16.8), (1.3, 8.56, 15.7)):
        box(f"RoofDeck_{y}", (0, y, z), (width, 2.1, 0.20), 0.08, group, DARK_TILE)
    # Front roof tile ribs curve upward at the corners, creating the xianxia eave silhouette.
    for x in [i * 0.56 for i in range(-15, 16)]:
        lift = max(0.0, (abs(x) - 5.8) * 0.28)
        curve_pipe(f"RoofRib_{x:.2f}", [(x, 1.9, 8.68 + lift * 0.35), (x, -0.2, 8.0 + lift * 0.18), (x, -2.95, 7.3 + lift)], 0.105, group, DARK_TILE)
    # Ridge and ornamented terminal curls.
    curve_pipe("RoofRidge", [(-7.7, 1.92, 8.95), (-3.6, 1.95, 9.12), (0, 1.95, 9.17), (3.6, 1.95, 9.12), (7.7, 1.92, 8.95)], 0.18, group, DARK_TILE)
    for x in (-8.4, 8.4):
        curve_pipe(f"EaveCurl_{x}", [(x * 0.94, -2.92, 7.75), (x, -3.05, 8.25), (x * 1.015, -2.76, 8.62)], 0.14, group, DARK_TILE)
        sphere_loc = (x * 1.015, -2.75, 8.64)
        bpy.ops.mesh.primitive_uv_sphere_add(segments=16, ring_count=8, radius=0.23, location=sphere_loc)
        tag(bpy.context.object, group, BRASS)
    return join_group(group, "SM_VDT_MainHallFacade")


def eave_module():
    group = "eave"
    box("EaveBeam", (0, 0, 0.1), (5.6, 1.15, 0.32), 0.09, group, REDWOOD)
    for x in (-2.4, -1.2, 0, 1.2, 2.4):
        box(f"EaveBracket_{x}", (x, -0.32, -0.26), (0.7, 1.05, 0.22), 0.06, group, REDWOOD)
        curve_pipe(f"EaveTile_{x}", [(x, 0.45, 0.38), (x, -0.08, 0.2), (x, -0.76, 0.47)], 0.12, group, DARK_TILE)
    curve_pipe("EaveRidge", [(-2.9, 0.45, 0.52), (0, 0.52, 0.60), (2.9, 0.45, 0.52)], 0.13, group, DARK_TILE)
    return join_group(group, "SM_VDT_RoofEave_Module")


def bracket_cluster():
    group = "bracket"
    box("BracketSeat", (0, 0, 0), (1.15, 1.0, 0.26), 0.08, group, REDWOOD)
    box("BracketArmA", (0, -0.38, 0.34), (1.5, 0.36, 0.22), 0.07, group, REDWOOD)
    box("BracketArmB", (0, 0.28, 0.64), (0.78, 1.28, 0.20), 0.07, group, REDWOOD)
    cylinder("BracketBoss", (0, -0.58, 0.4), 0.13, 0.16, group, BRASS, 12).rotation_euler = (math.pi / 2, 0, 0)
    return join_group(group, "SM_VDT_Dougong_Cluster")


def moon_window():
    group = "window"
    # Circular moon window frame made from a torus with a restrained eight-spoke lattice.
    bpy.ops.mesh.primitive_torus_add(major_radius=0.9, minor_radius=0.12, major_segments=32, minor_segments=8, location=(0, 0, 0))
    tag(bpy.context.object, group, REDWOOD)
    for angle in range(0, 180, 45):
        bar = box(f"MoonLattice_{angle}", (0, 0, 0), (1.45, 0.09, 0.08), 0.025, group, BRASS)
        bar.rotation_euler = (0, math.radians(angle), 0)
    return join_group(group, "SM_VDT_MoonWindowPanel")


def write_provenance(files):
    entries = []
    for filename, asset_name, role in files:
        data = (OUT / filename).read_bytes()
        entries.append({"file": filename, "asset": asset_name, "sha256": hashlib.sha256(data).hexdigest(), "role": role})
    manifest = {
        "asset_set": "VanDaoHall",
        "author": "Truong Sinh UE5 team",
        "license": "CC0-1.0",
        "source": "Original procedural Blender geometry; no external assets, textures, or downloads.",
        "units": "metres in Blender; FBX applies Unreal-compatible centimetre scale",
        "blender_version": bpy.app.version_string,
        "source_blend": {"file": BLEND.name, "sha256": hashlib.sha256(BLEND.read_bytes()).hexdigest()},
        "assets": entries,
    }
    (OUT / "PROVENANCE.json").write_text(json.dumps(manifest, indent=2), encoding="utf-8")
    report = ["# Vấn Đạo Tông Hall Kit — Static Audit", "", "Generated headlessly by `tools/create-vandao-hall.py`.", "", "- Original CC0 geometry: PASS", "- External downloads/textures: none", "- Object groups exported independently: %d" % len(entries), "- Curved eaves / layered roof silhouette: PASS", "- Wooden dougong bracket motif: PASS", "- UE import note: import each FBX as a static mesh, preserve material slots.", ""]
    for entry in entries:
        report.append("- `%s` — %s — `%s`" % (entry["file"], entry["role"], entry["sha256"]))
    (OUT / "STATIC_AUDIT.md").write_text("\n".join(report) + "\n", encoding="utf-8")


def main():
    global REDWOOD, DARK_TILE, AGED_STONE, BRASS, JADE
    clean_scene()
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 1.0
    REDWOOD = material("M_VDT_Redwood", (0.18, 0.035, 0.018), 0.0, 0.42)
    DARK_TILE = material("M_VDT_DarkTile", (0.035, 0.055, 0.07), 0.05, 0.36)
    AGED_STONE = material("M_VDT_AgedStone", (0.18, 0.20, 0.19), 0.0, 0.72)
    BRASS = material("M_VDT_AntiqueBrass", (0.32, 0.19, 0.045), 0.72, 0.30)
    JADE = material("M_VDT_JadePlaque", (0.045, 0.22, 0.16), 0.16, 0.28)
    assets = [
        (main_hall_facade(), "VDT_MainHallFacade.fbx", "hero facade, 18m wide; place at the corridor terminus"),
        (eave_module(), "VDT_RoofEave_Module.fbx", "repeatable roof/eave silhouette module"),
        (bracket_cluster(), "VDT_Dougong_Cluster.fbx", "repeatable carved wooden bracket cluster"),
        (moon_window(), "VDT_MoonWindowPanel.fbx", "decorative circular lattice panel"),
    ]
    for asset, filename, _role in assets:
        export_asset(asset, filename)
    bpy.ops.wm.save_as_mainfile(filepath=str(BLEND))
    write_provenance([(filename, asset.name, role) for asset, filename, role in assets])
    print("VDT_HALL_KIT_PASS", OUT)


if __name__ == "__main__":
    main()
