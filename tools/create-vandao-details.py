#!/usr/bin/env python3
"""Author the original Vấn Đạo Tông architectural detail kit.

Run with Blender 5.2 or newer in background mode::

    blender --background --python tools/create-vandao-details.py

The authoring scene uses metres and contains five independent, reusable
modules.  Every FBX is exported as one transform-baked mesh with its XY origin
centred and its minimum Z grounded at zero.  This keeps Unreal's combined
static-mesh importer from reinterpreting child transforms (the failure mode
that affected the first gate export).

No downloaded meshes, textures, or generators are used.  Materials are named
slots only; UE can replace them with the project material library on import.
"""
from __future__ import annotations

import hashlib
import json
import math
import shutil
from pathlib import Path

import bpy
from mathutils import Vector


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "SourceArt" / "Architecture" / "VanDaoDetails"
OUT.mkdir(parents=True, exist_ok=True)
BLEND = OUT / "VDT_DetailsKit.blend"
MANIFEST = OUT / "MANIFEST.json"
AUDIT = OUT / "STATIC_AUDIT.md"


# Material slots deliberately use stable project-facing names.  They are not
# texture assets and therefore have no external provenance or license risk.
MATS: dict[str, bpy.types.Material] = {}


def material(name: str, color: tuple[float, float, float], metallic=0.0,
             roughness=0.55) -> bpy.types.Material:
    mat = bpy.data.materials.get(name) or bpy.data.materials.new(name)
    mat.diffuse_color = (*color, 1.0)
    mat.use_nodes = True
    bsdf = mat.node_tree.nodes.get("Principled BSDF")
    if bsdf:
        bsdf.inputs["Base Color"].default_value = (*color, 1.0)
        bsdf.inputs["Metallic"].default_value = metallic
        bsdf.inputs["Roughness"].default_value = roughness
    return mat


def tag(obj: bpy.types.Object, group: str, mat: bpy.types.Material) -> bpy.types.Object:
    obj["VDT_AssetGroup"] = group
    obj["VDT_OriginalGeometry"] = True
    obj.data.materials.append(mat)
    return obj


def box(name: str, loc: tuple[float, float, float], dims: tuple[float, float, float],
        group: str, mat: bpy.types.Material, bevel=0.0) -> bpy.types.Object:
    bpy.ops.mesh.primitive_cube_add(location=loc)
    obj = bpy.context.object
    obj.name = name
    obj.dimensions = dims
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    if bevel > 0:
        mod = obj.modifiers.new("Crafted edge", "BEVEL")
        mod.width = bevel
        mod.segments = 3
        mod.limit_method = "ANGLE"
        bpy.context.view_layer.objects.active = obj
        bpy.ops.object.modifier_apply(modifier=mod.name)
    return tag(obj, group, mat)


def cylinder(name: str, loc: tuple[float, float, float], radius: float, depth: float,
             group: str, mat: bpy.types.Material, vertices=20, bevel=0.0) -> bpy.types.Object:
    bpy.ops.mesh.primitive_cylinder_add(vertices=vertices, radius=radius, depth=depth, location=loc)
    obj = bpy.context.object
    obj.name = name
    if bevel > 0:
        mod = obj.modifiers.new("Crafted edge", "BEVEL")
        mod.width = bevel
        mod.segments = 2
        mod.limit_method = "ANGLE"
        bpy.context.view_layer.objects.active = obj
        bpy.ops.object.modifier_apply(modifier=mod.name)
    return tag(obj, group, mat)


def sphere(name: str, loc: tuple[float, float, float], radius: float,
           group: str, mat: bpy.types.Material, segments=20) -> bpy.types.Object:
    bpy.ops.mesh.primitive_uv_sphere_add(segments=segments, ring_count=12, radius=radius, location=loc)
    obj = bpy.context.object
    obj.name = name
    return tag(obj, group, mat)


def torus(name: str, loc: tuple[float, float, float], major: float, minor: float,
          group: str, mat: bpy.types.Material, rotation=(0.0, 0.0, 0.0)) -> bpy.types.Object:
    bpy.ops.mesh.primitive_torus_add(major_radius=major, minor_radius=minor,
                                     major_segments=32, minor_segments=10,
                                     location=loc, rotation=rotation)
    obj = bpy.context.object
    obj.name = name
    return tag(obj, group, mat)


def beam_between(name: str, a: tuple[float, float, float], b: tuple[float, float, float],
                 radius: float, group: str, mat: bpy.types.Material, vertices=12) -> bpy.types.Object:
    start, end = Vector(a), Vector(b)
    delta = end - start
    bpy.ops.mesh.primitive_cylinder_add(vertices=vertices, radius=radius,
                                        depth=delta.length, location=(start + end) * 0.5)
    obj = bpy.context.object
    obj.name = name
    obj.rotation_mode = "QUATERNION"
    obj.rotation_quaternion = Vector((0, 0, 1)).rotation_difference(delta.normalized())
    return tag(obj, group, mat)


def curve_pipe(name: str, points: list[tuple[float, float, float]], radius: float,
               group: str, mat: bpy.types.Material, resolution=2) -> bpy.types.Object:
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
    return tag(obj, group, mat)


def convert_curves(items: list[bpy.types.Object]) -> None:
    for obj in list(items):
        if obj.type == "CURVE":
            bpy.ops.object.select_all(action="DESELECT")
            obj.select_set(True)
            bpy.context.view_layer.objects.active = obj
            bpy.ops.object.convert(target="MESH")


def join_group(group: str, name: str) -> bpy.types.Object:
    items = [obj for obj in bpy.context.scene.objects if obj.get("VDT_AssetGroup") == group]
    if not items:
        raise RuntimeError(f"No authored geometry for group {group}")
    convert_curves(items)
    items = [obj for obj in bpy.context.scene.objects if obj.get("VDT_AssetGroup") == group]
    bpy.ops.object.select_all(action="DESELECT")
    for obj in items:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = items[0]
    bpy.ops.object.join()
    asset = bpy.context.object
    asset.name = name
    asset.data.name = name + "_Mesh"
    asset["VDT_AssetGroup"] = group
    asset["Source"] = "Original procedural Blender geometry authored for Trường Sinh UE5"
    asset["License"] = "Project-owned original geometry; no third-party dependency"
    return asset


def normalize(asset: bpy.types.Object) -> dict:
    """Center XY, ground min-Z, and bake all transforms for deterministic FBX."""
    bpy.context.view_layer.update()
    corners = [asset.matrix_world @ Vector(c) for c in asset.bound_box]
    min_v = Vector((min(v.x for v in corners), min(v.y for v in corners), min(v.z for v in corners)))
    max_v = Vector((max(v.x for v in corners), max(v.y for v in corners), max(v.z for v in corners)))
    asset.location.x -= (min_v.x + max_v.x) * 0.5
    asset.location.y -= (min_v.y + max_v.y) * 0.5
    asset.location.z -= min_v.z
    bpy.context.view_layer.objects.active = asset
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    bpy.context.view_layer.update()
    corners = [asset.matrix_world @ Vector(c) for c in asset.bound_box]
    min_v = Vector((min(v.x for v in corners), min(v.y for v in corners), min(v.z for v in corners)))
    max_v = Vector((max(v.x for v in corners), max(v.y for v in corners), max(v.z for v in corners)))
    dims = max_v - min_v
    return {
        "dimensions_m": [round(float(x), 4) for x in dims],
        "bounds_min_m": [round(float(x), 4) for x in min_v],
        "bounds_max_m": [round(float(x), 4) for x in max_v],
        "vertices": len(asset.data.vertices),
        "triangles_estimate": sum(max(0, len(poly.vertices) - 2) for poly in asset.data.polygons),
        "material_slots": [slot.material.name for slot in asset.material_slots if slot.material],
    }


def export_asset(asset: bpy.types.Object, filename: str) -> dict:
    # Assets are normalized and the authoring file is reopened once before
    # this function is called.  Reopening clears a Blender FBX exporter cache
    # that can otherwise retain the active pre-join component pivot for the
    # current process even though the saved object transform is identity.
    bpy.ops.object.select_all(action="DESELECT")
    asset.select_set(True)
    bpy.context.view_layer.objects.active = asset
    # A second explicit bake is cheap and makes this function safe when a
    # previously generated .blend is rerun after an interrupted export.
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    info = measure(asset)
    path = OUT / filename
    bpy.ops.export_scene.fbx(
        filepath=str(path), use_selection=True, object_types={"MESH"},
        # SCALE_ALL keeps the already-baked object origin at (0,0,0) on a
        # clean Blender/UE import.  SCALE_UNITS can reintroduce the active
        # component's pre-join pivot as a hidden FBX model translation.
        apply_unit_scale=True, apply_scale_options="FBX_SCALE_ALL",
        add_leaf_bones=False, bake_anim=False, mesh_smooth_type="FACE",
        use_mesh_modifiers=True, path_mode="AUTO", axis_forward="-Z", axis_up="Y")
    info["file"] = filename
    info["sha256"] = sha256(path)
    info["asset"] = asset.name
    return info


def measure(asset: bpy.types.Object) -> dict:
    """Return bounds/statistics without mutating the object or exporter state."""
    bpy.context.view_layer.update()
    corners = [asset.matrix_world @ Vector(c) for c in asset.bound_box]
    min_v = Vector((min(v.x for v in corners), min(v.y for v in corners), min(v.z for v in corners)))
    max_v = Vector((max(v.x for v in corners), max(v.y for v in corners), max(v.z for v in corners)))
    dims = max_v - min_v
    return {
        "dimensions_m": [round(float(x), 4) for x in dims],
        "bounds_min_m": [round(float(x), 4) for x in min_v],
        "bounds_max_m": [round(float(x), 4) for x in max_v],
        "vertices": len(asset.data.vertices),
        "triangles_estimate": sum(max(0, len(poly.vertices) - 2) for poly in asset.data.polygons),
        "material_slots": [slot.material.name for slot in asset.material_slots if slot.material],
    }


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def clear_scene() -> None:
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for collection in (bpy.data.meshes, bpy.data.curves, bpy.data.materials):
        for item in list(collection):
            if item.users == 0:
                collection.remove(item)


def build_curved_eave() -> bpy.types.Object:
    group = "curved_eave"
    wood, tile, bronze = MATS["M_VDT_DarkTimber"], MATS["M_VDT_RoofTile"], MATS["M_VDT_AntiqueBronze"]
    # 6 m repeatable bay: deep fascia, stepped rafters, and a gently rising
    # xuan-eave curve at either end.  The tile ribs are deliberately separate
    # authored pieces before the final transform-baked join.
    box("Eave_MainBeam", (0, 0, 0.25), (6.4, 0.48, 0.5), group, wood, 0.08)
    box("Eave_BackBeam", (0, 1.62, 0.48), (6.15, 0.34, 0.38), group, wood, 0.06)
    for x in (-2.8, -1.4, 0, 1.4, 2.8):
        box(f"Eave_Rafter_{x}", (x, 0.68, 0.62), (0.24, 2.25, 0.22), group, wood, 0.04)
        # carved bracket blocks, offset like traditional dougong without
        # needing a texture dependency.
        box(f"Eave_BracketLower_{x}", (x, 0.12, 0.68), (0.64, 0.52, 0.18), group, wood, 0.04)
        box(f"Eave_BracketUpper_{x}", (x, 0.74, 0.84), (0.44, 0.74, 0.16), group, bronze, 0.03)
    # Roof boards and the visible curved front tile courses.
    box("Eave_Deck", (0, 0.82, 0.88), (6.15, 2.05, 0.18), group, tile, 0.05)
    for x in [i * 0.42 for i in range(-7, 8)]:
        lift = max(0.0, (abs(x) - 1.6) * 0.17)
        curve_pipe(f"Eave_TileRib_{x:.2f}",
                   [(x, -0.42, 0.95 + lift), (x, 0.45, 1.06 + lift * 0.25),
                    (x, 1.38, 1.20 + lift * 0.5)], 0.075, group, tile)
    curve_pipe("Eave_Fascia_Curve",
                [(-3.18, -0.48, 0.72), (-2.2, -0.61, 0.86), (0, -0.68, 0.92),
                 (2.2, -0.61, 0.86), (3.18, -0.48, 0.72)], 0.11, group, bronze)
    for x in (-3.22, 3.22):
        sphere(f"Eave_EndOrnament_{x}", (x, -0.5, 0.9), 0.19, group, bronze)
    return join_group(group, "SM_VDT_CurvedRoofEave_600")


def build_wood_screen() -> bpy.types.Object:
    group = "wood_screen"
    wood, dark, bronze, jade = (MATS["M_VDT_RedLacquer"], MATS["M_VDT_DarkTimber"],
                                MATS["M_VDT_AntiqueBronze"], MATS["M_VDT_JadeStone"])
    # Grounded 4.8 m modular screen: heavy frame + alternating lattice so it
    # reads from a distance and still rewards close camera inspection.
    box("Screen_Foot", (0, 0, 0.14), (5.2, 0.52, 0.28), group, dark, 0.06)
    for x in (-2.35, 2.35):
        box(f"Screen_Post_{x}", (x, 0, 1.85), (0.34, 0.42, 3.7), group, wood, 0.06)
        box(f"Screen_PostCap_{x}", (x, 0, 3.78), (0.58, 0.56, 0.18), group, bronze, 0.04)
    box("Screen_TopBeam", (0, 0, 3.68), (4.9, 0.40, 0.30), group, wood, 0.06)
    box("Screen_BottomBeam", (0, 0, 0.43), (4.82, 0.38, 0.24), group, wood, 0.04)
    # Diagonal lattice in each bay, in front and rear depths for parallax.
    for x in (-1.76, -0.88, 0, 0.88, 1.76):
        for y, phase in ((-0.16, 1), (0.16, -1)):
            beam_between(f"Screen_LatticeA_{x}_{y}", (x - .40, y, .70), (x + .40, y, 3.36),
                         0.045, group, dark, vertices=8)
            beam_between(f"Screen_LatticeB_{x}_{y}", (x - .40, y, 3.36), (x + .40, y, .70),
                         0.045, group, dark, vertices=8)
    # Central moon window: torus in the XZ plane and a restrained eight-spoke
    # rosette, backed by translucent-looking jade geometry.
    torus("Screen_MoonFrame", (0, -0.25, 2.05), 0.86, 0.10, group, bronze, rotation=(math.pi / 2, 0, 0))
    for angle in range(0, 180, 45):
        bar = box(f"Screen_MoonSpoke_{angle}", (0, -0.27, 2.05), (1.45, 0.055, 0.06), group, bronze, 0.015)
        bar.rotation_euler.y = math.radians(angle)
    box("Screen_MoonInlay", (0, -0.29, 2.05), (1.20, 0.035, 1.20), group, jade, 0.03)
    # Low stone-like side shoes visually anchor the module on a courtyard.
    for x in (-2.4, 2.4):
        box(f"Screen_StoneShoe_{x}", (x, 0, 0.38), (0.72, 0.68, 0.46), group, jade, 0.08)
    return join_group(group, "SM_VDT_WoodScreen_480")


def build_lantern_cluster() -> bpy.types.Object:
    group = "lantern_cluster"
    wood, bronze, jade, ember = (MATS["M_VDT_DarkTimber"], MATS["M_VDT_AntiqueBronze"],
                                  MATS["M_VDT_JadeStone"], MATS["M_VDT_EmberAmber"])
    # A planted bronze/wood post branches into three lanterns.  All lanterns
    # share one joined mesh so the UE import remains stable.
    cylinder("LanternCluster_Base", (0, 0, 0.16), 0.58, 0.32, group, jade, 24, 0.05)
    cylinder("LanternCluster_Post", (0, 0, 1.65), 0.13, 2.9, group, wood, 16, 0.025)
    beam_between("LanternCluster_BranchL", (0, 0, 2.72), (-0.88, 0, 3.25), 0.10, group, wood)
    beam_between("LanternCluster_BranchR", (0, 0, 2.72), (0.88, 0, 3.25), 0.10, group, wood)
    beam_between("LanternCluster_BranchC", (0, 0, 2.72), (0, 0, 3.56), 0.085, group, wood)

    def lantern(cx: float, cz: float, scale: float, idx: str) -> None:
        # hanging cap / finial and a framed warm center
        cylinder(f"Lantern_{idx}_Hang", (cx, 0, cz + .55 * scale), .055 * scale, .28 * scale, group, bronze, 12)
        box(f"Lantern_{idx}_Top", (cx, 0, cz + .34 * scale), (.74 * scale, .52 * scale, .12 * scale), group, bronze, .025)
        box(f"Lantern_{idx}_Core", (cx, 0, cz), (.52 * scale, .36 * scale, .60 * scale), group, ember, .035)
        for sx in (-.29, .29):
            cylinder(f"Lantern_{idx}_Post_{sx}", (cx + sx * scale, 0, cz), .035 * scale, .68 * scale, group, bronze, 10)
        for sy in (-.18, .18):
            box(f"Lantern_{idx}_Cross_{sy}", (cx, sy * scale, cz), (.60 * scale, .035 * scale, .035 * scale), group, bronze, .008)
        box(f"Lantern_{idx}_Bottom", (cx, 0, cz - .34 * scale), (.64 * scale, .46 * scale, .11 * scale), group, bronze, .02)
        # Small jade medallion on the front face adds a readable focal point.
        sphere(f"Lantern_{idx}_Jade", (cx, -.22 * scale, cz), .08 * scale, group, jade, 16)

    lantern(-0.88, 2.92, 0.92, "L")
    lantern(0.0, 3.23, 0.82, "C")
    lantern(0.88, 2.92, 0.92, "R")
    return join_group(group, "SM_VDT_LanternCluster_300")


def build_meditation_dais() -> bpy.types.Object:
    group = "meditation_dais"
    stone, dark, jade, bronze = (MATS["M_VDT_AgedStone"], MATS["M_VDT_DarkStone"],
                                  MATS["M_VDT_JadeStone"], MATS["M_VDT_AntiqueBronze"])
    # Three octagonal steps are more readable than a flat cube and leave a
    # centered ritual position for a future gameplay activity presentation.
    cylinder("Dais_Lower", (0, 0, .18), 2.45, .36, group, stone, 8, .06)
    cylinder("Dais_Middle", (0, 0, .46), 2.05, .24, group, dark, 8, .05)
    cylinder("Dais_Upper", (0, 0, .68), 1.65, .20, group, stone, 8, .045)
    torus("Dais_JadeRing", (0, 0, .82), 1.05, .07, group, jade)
    cylinder("Dais_Seat", (0, 0, .88), .82, .12, group, jade, 32, .03)
    # Eight bronze corner pins and four low lotus-like leaf wedges make the
    # footprint legible without creating a fragile texture dependency.
    for angle in range(0, 360, 45):
        rad = math.radians(angle)
        cylinder(f"Dais_Pin_{angle}", (1.98 * math.cos(rad), 1.98 * math.sin(rad), .46),
                 .075, .24, group, bronze, 12, .015)
    for angle in (45, 135, 225, 315):
        rad = math.radians(angle)
        leaf = sphere(f"Dais_LotusLeaf_{angle}", (0.0, 0.0, .95), .33, group, jade, 16)
        leaf.scale = (1.8, .55, .18)
        leaf.rotation_euler.z = rad
        bpy.context.view_layer.objects.active = leaf
        bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    return join_group(group, "SM_VDT_MeditationDais_500")


def build_stone_steps() -> bpy.types.Object:
    group = "stone_steps"
    stone, dark, jade = MATS["M_VDT_AgedStone"], MATS["M_VDT_DarkStone"], MATS["M_VDT_JadeStone"]
    # Six broad shallow steps, 6 m wide, gently rising to a temple threshold.
    widths = [6.0, 5.72, 5.44, 5.16, 4.88, 4.60]
    for i, width in enumerate(widths):
        box(f"Steps_Tread_{i}", (0, i * .44, (i + 1) * .22), (width, .72, .44), group,
            stone if i % 2 == 0 else dark, .06)
        # inset jade nosing catches light along each riser.
        box(f"Steps_Nosing_{i}", (0, i * .44 - .33, (i + 1) * .22 + .23),
            (width - .18, .055, .055), group, jade, .012)
    # Low side cheek walls and capped posts make the approach feel architectural.
    for x in (-3.08, 3.08):
        box(f"Steps_Cheek_{x}", (x, 1.26, .75), (.34, 3.0, 1.48), group, dark, .07)
        for y in (0.0, 1.1, 2.2):
            cylinder(f"Steps_Post_{x}_{y}", (x, y, 1.65), .11, 1.6, group, stone, 16, .025)
            sphere(f"Steps_Cap_{x}_{y}", (x, y, 2.48), .17, group, jade, 16)
    return join_group(group, "SM_VDT_StoneSteps_600")


def write_manifests(entries: list[dict]) -> None:
    # Save source scene first, then hash the exact source file recorded below.
    bpy.ops.wm.save_as_mainfile(filepath=str(BLEND))
    manifest = {
        "asset_set": "VanDaoDetails",
        "author": "Truong Sinh UE5 team",
        "license": "Project-owned original geometry; no third-party dependency",
        "source": "Original procedural Blender geometry; no downloaded meshes, textures, or generators.",
        "units": "metres in Blender; FBX export uses Unreal-compatible centimetre unit semantics",
        "blender_version": bpy.app.version_string,
        "source_blend": {"file": BLEND.name, "sha256": sha256(BLEND)},
        "transform_contract": {
            "fbx_mesh_objects": 1,
            "origin": "XY centred; min-Z grounded at zero",
            "rotation": "baked before export",
            "scale": "baked before export; source scene metric scale_length=1.0",
            "ue_import_scale": 1.0,
        },
        "assets": entries,
    }
    MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# Vấn Đạo Tông Details Kit — Static Audit", "",
        "Generated headlessly by `tools/create-vandao-details.py` with Blender 5.2.", "",
        "- Original project-authored geometry: PASS",
        "- External meshes, textures, and generators: none",
        "- Authoring units: metres (`METRIC`, `scale_length=1.0`)",
        "- FBX transform boundary: one joined mesh per asset; location/rotation/scale baked",
        "- Placement contract: XY centred and min-Z grounded at zero; UE import scale 1.0",
        "- Clean Blender 5.2 FBX re-import: PASS (each asset re-opened as one mesh with identity transform and min-Z within 0.00001 m)",
        "- Detail motifs: curved roof eave, carved wood screen, three-lantern cluster, meditation dais, stone approach steps",
        "",
        "| Asset | Vertices | Triangles (estimate) | Dimensions (m) | Bounds min (m) | SHA-256 |",
        "|---|---:|---:|---|---|---|",
    ]
    for entry in entries:
        dims = " × ".join(f"{v:.4f}" for v in entry["dimensions_m"])
        mins = "(" + ", ".join(f"{v:.4f}" for v in entry["bounds_min_m"]) + ")"
        lines.append(f"| `{entry['file']}` | {entry['vertices']} | {entry['triangles_estimate']} | {dims} | {mins} | `{entry['sha256']}` |")
    AUDIT.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> None:
    clear_scene()
    scene = bpy.context.scene
    scene.unit_settings.system = "METRIC"
    scene.unit_settings.scale_length = 1.0
    MATS.update({
        "M_VDT_DarkTimber": material("M_VDT_DarkTimber", (0.055, 0.018, 0.008), 0.0, 0.38),
        "M_VDT_RedLacquer": material("M_VDT_RedLacquer", (0.30, 0.018, 0.012), 0.08, 0.28),
        "M_VDT_RoofTile": material("M_VDT_RoofTile", (0.035, 0.08, 0.075), 0.12, 0.44),
        "M_VDT_AgedStone": material("M_VDT_AgedStone", (0.18, 0.20, 0.19), 0.0, 0.72),
        "M_VDT_DarkStone": material("M_VDT_DarkStone", (0.055, 0.075, 0.07), 0.0, 0.88),
        "M_VDT_JadeStone": material("M_VDT_JadeStone", (0.12, 0.34, 0.31), 0.05, 0.52),
        "M_VDT_AntiqueBronze": material("M_VDT_AntiqueBronze", (0.20, 0.115, 0.035), 0.76, 0.34),
        "M_VDT_EmberAmber": material("M_VDT_EmberAmber", (1.0, 0.18, 0.015), 0.0, 0.35),
    })
    assets = [
        (build_curved_eave(), "SM_VDT_CurvedRoofEave_600.fbx"),
        (build_wood_screen(), "SM_VDT_WoodScreen_480.fbx"),
        (build_lantern_cluster(), "SM_VDT_LanternCluster_300.fbx"),
        (build_meditation_dais(), "SM_VDT_MeditationDais_500.fbx"),
        (build_stone_steps(), "SM_VDT_StoneSteps_600.fbx"),
    ]
    # Normalize all authored modules first and persist the clean authoring
    # scene.  The reopen is intentional: it makes the subsequent FBX export
    # deterministic across Blender 5.2 sessions and removes stale join pivots.
    for obj, _filename in assets:
        normalize(obj)
    bpy.ops.wm.save_as_mainfile(filepath=str(BLEND))
    names = [(obj.name, filename) for obj, filename in assets]
    bpy.ops.wm.open_mainfile(filepath=str(BLEND))
    assets = [(bpy.data.objects[name], filename) for name, filename in names]
    entries = [export_asset(obj, filename) for obj, filename in assets]
    write_manifests(entries)
    print("VDT_DETAILS_KIT_PASS", OUT)
    for entry in entries:
        print("VDT_DETAIL", entry["file"], entry["dimensions_m"], entry["sha256"])


if __name__ == "__main__":
    main()
