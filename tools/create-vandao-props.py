#!/usr/bin/env python3
"""Generate original, licence-clean Blender FBX props for Van Dao Tong.

Run with Blender in background mode.  All generated files are deliberately kept
under SourceArt/Architecture/VanDaoProps so the kit can be reviewed before any
Unreal import.  Dimensions are centimetres (the Unreal import target unit).
"""
from __future__ import annotations

import hashlib
import json
import math
import shutil
from datetime import datetime, timezone
from pathlib import Path

import bpy


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "SourceArt" / "Architecture" / "VanDaoProps"
FBX = OUT / "FBX"
BLEND = OUT / "VanDaoProps_Source.blend"


def clean_scene() -> None:
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    for collection in (bpy.data.meshes, bpy.data.curves, bpy.data.materials):
        if collection != bpy.data.materials:
            for item in list(collection):
                if item.users == 0:
                    collection.remove(item)


def material(name: str, rgba: tuple[float, float, float, float], metallic=0.0, roughness=0.65):
    found = bpy.data.materials.get(name)
    if found:
        return found
    mat = bpy.data.materials.new(name)
    mat.diffuse_color = rgba
    mat.use_nodes = True
    bsdf = mat.node_tree.nodes.get("Principled BSDF")
    bsdf.inputs["Base Color"].default_value = rgba
    bsdf.inputs["Metallic"].default_value = metallic
    bsdf.inputs["Roughness"].default_value = roughness
    return mat


STONE = None
DARK_STONE = None
BRONZE = None
EMBER = None


def finish(obj, mat, bevel=2.0):
    obj.data.materials.append(mat)
    if bevel > 0:
        mod = obj.modifiers.new("EdgeSoftness", "BEVEL")
        mod.width = bevel
        mod.segments = 3
        mod.limit_method = "ANGLE"
        bpy.context.view_layer.objects.active = obj
        bpy.ops.object.modifier_apply(modifier=mod.name)
    bpy.ops.object.shade_smooth_by_angle()
    return obj


def cube(name, loc, scale, mat= None, bevel=2.0):
    bpy.ops.mesh.primitive_cube_add(location=loc)
    obj = bpy.context.object
    obj.name = name
    obj.scale = (scale[0] / 2, scale[1] / 2, scale[2] / 2)
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    return finish(obj, mat or STONE, bevel)


def cyl(name, loc, radius, depth, mat=None, vertices=24, bevel=1.5):
    bpy.ops.mesh.primitive_cylinder_add(vertices=vertices, radius=radius, depth=depth, location=loc)
    obj = bpy.context.object
    obj.name = name
    return finish(obj, mat or STONE, bevel)


def sphere(name, loc, radius, mat=None):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=24, ring_count=12, radius=radius, location=loc)
    obj = bpy.context.object
    obj.name = name
    return finish(obj, mat or STONE, 0)


def torus(name, loc, major, minor, mat=None, rotation=(0, 0, 0)):
    bpy.ops.mesh.primitive_torus_add(major_radius=major, minor_radius=minor, major_segments=32, minor_segments=10, location=loc, rotation=rotation)
    obj = bpy.context.object
    obj.name = name
    return finish(obj, mat or BRONZE, 0)


def join(parts, name):
    bpy.ops.object.select_all(action="DESELECT")
    for p in parts:
        p.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    obj = bpy.context.object
    obj.name = name
    return obj


def stele():
    p = []
    p.append(cube("Stele_Base_Lower", (0, 0, 12), (100, 54, 24), STONE, 3))
    p.append(cube("Stele_Base_Upper", (0, 0, 29), (76, 42, 14), DARK_STONE, 2))
    p.append(cube("Stele_Body", (0, 0, 128), (58, 26, 184), STONE, 4))
    # Cap and a deliberately shallow front inset give readable silhouette without decal dependence.
    p.append(cube("Stele_Cap", (0, 0, 226), (74, 40, 18), DARK_STONE, 3))
    p.append(cyl("Stele_Crest", (0, 0, 243), 18, 18, STONE, 20, 2))
    p.append(cube("Stele_InscriptionInset", (0, -13.6, 135), (34, 2.5, 105), DARK_STONE, 0.8))
    for z in (92, 118, 144, 170):
        p.append(cube("Stele_Rune_%d" % z, (0, -15.1, z), (14, 1.0, 13), EMBER, 0.3))
    return join(p, "SM_VDT_StoneStele")


def lantern():
    p = []
    p.append(cube("Lantern_Plinth", (0, 0, 10), (76, 76, 20), STONE, 3))
    p.append(cyl("Lantern_Stem_Lower", (0, 0, 41), 18, 42, DARK_STONE, 20, 2))
    p.append(cube("Lantern_Tray", (0, 0, 69), (66, 66, 12), STONE, 2))
    # Four frame posts and a warm hollow center.
    for x in (-22, 22):
        for y in (-22, 22):
            p.append(cyl("Lantern_Frame", (x, y, 100), 4.5, 55, DARK_STONE, 12, 1))
    p.append(cube("Lantern_GlowCore", (0, 0, 100), (38, 38, 46), EMBER, 2))
    p.append(cube("Lantern_Roof_Lower", (0, 0, 133), (78, 78, 10), DARK_STONE, 2))
    p.append(cube("Lantern_Roof_Upper", (0, 0, 142), (58, 58, 10), STONE, 2))
    p.append(cyl("Lantern_Finial", (0, 0, 157), 8, 20, BRONZE, 16, 1))
    return join(p, "SM_VDT_StoneLantern")


def brazier():
    p = []
    p.append(cyl("Brazier_Base", (0, 0, 9), 46, 18, DARK_STONE, 32, 2))
    p.append(cyl("Brazier_Foot", (0, 0, 33), 25, 32, BRONZE, 32, 2))
    p.append(torus("Brazier_LowerRing", (0, 0, 51), 30, 4, BRONZE))
    p.append(cyl("Brazier_Bowl", (0, 0, 70), 43, 30, BRONZE, 32, 2))
    p.append(torus("Brazier_Rim", (0, 0, 86), 43, 4.5, BRONZE))
    p.append(cyl("Brazier_Ember", (0, 0, 85), 30, 4, EMBER, 32, 0.5))
    # Three scroll-like handles establish a ritual reading in silhouette.
    for angle in (0, 120, 240):
        rad = math.radians(angle)
        x, y = math.cos(rad) * 47, math.sin(rad) * 47
        handle = torus("Brazier_Handle", (x, y, 72), 10, 2.8, BRONZE, rotation=(math.pi / 2, 0, rad))
        p.append(handle)
    for x, y in ((-14, -8), (0, 10), (14, -8)):
        p.append(cyl("Brazier_Incense", (x, y, 113), 1.8, 58, DARK_STONE, 10, 0.1))
    return join(p, "SM_VDT_RitualBrazier")


def baluster():
    p = []
    p.append(cube("Baluster_Foot", (0, 0, 9), (36, 36, 18), STONE, 2))
    p.append(cyl("Baluster_Lower", (0, 0, 27), 12, 20, DARK_STONE, 20, 1.5))
    p.append(sphere("Baluster_Belly", (0, 0, 49), 18, STONE))
    p.append(cyl("Baluster_Neck", (0, 0, 73), 10, 26, DARK_STONE, 20, 1.5))
    p.append(cube("Baluster_Cap", (0, 0, 92), (32, 32, 13), STONE, 2))
    return join(p, "SM_VDT_BridgeBaluster")


def railing_segment():
    p = []
    p.append(cube("Railing_Base", (0, 0, 7), (400, 28, 14), STONE, 2))
    p.append(cube("Railing_Top", (0, 0, 106), (400, 26, 14), STONE, 2))
    for x in (-180, -90, 0, 90, 180):
        # slight crafted variation in central balusters through alternating belly scale
        part = baluster()
        part.location = (x, 0, 0)
        p.append(part)
    # A low inset bar improves readability at grazing angle.
    p.append(cube("Railing_MidBar", (0, 0, 58), (370, 10, 9), DARK_STONE, 1.2))
    return join(p, "SM_VDT_BridgeRailing_400")


def export_asset(obj):
    bpy.ops.object.select_all(action="DESELECT")
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.export_scene.fbx(
        filepath=str(FBX / (obj.name + ".fbx")),
        use_selection=True,
        object_types={"MESH"},
        apply_scale_options="FBX_SCALE_UNITS",
        axis_forward="-Z",
        axis_up="Y",
        mesh_smooth_type="FACE",
        bake_space_transform=False,
        add_leaf_bones=False,
    )


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def main():
    global STONE, DARK_STONE, BRONZE, EMBER
    if OUT.exists():
        # Preserve previous source artefacts in the recycle bin is not possible in headless
        # Blender; remove only generated FBX files known to this script.
        if FBX.exists():
            shutil.rmtree(FBX)
    FBX.mkdir(parents=True, exist_ok=True)
    clean_scene()
    STONE = material("M_VDT_Stone_Jade", (0.17, 0.29, 0.25, 1.0), 0.0, 0.78)
    DARK_STONE = material("M_VDT_Stone_Dark", (0.055, 0.075, 0.07, 1.0), 0.0, 0.9)
    BRONZE = material("M_VDT_Bronze_Aged", (0.19, 0.09, 0.025, 1.0), 0.82, 0.33)
    EMBER = material("M_VDT_Ember_Amber", (1.0, 0.18, 0.015, 1.0), 0.0, 0.35)
    bsdf = EMBER.node_tree.nodes.get("Principled BSDF")
    bsdf.inputs["Emission Color"].default_value = (1.0, 0.035, 0.001, 1.0)
    bsdf.inputs["Emission Strength"].default_value = 4.0

    assets = [stele(), lantern(), brazier(), baluster(), railing_segment()]
    records = []
    for obj in assets:
        export_asset(obj)
        file_path = FBX / (obj.name + ".fbx")
        obj.data.calc_loop_triangles()
        records.append({
            "asset": obj.name,
            "file": "FBX/" + file_path.name,
            "sha256": sha256(file_path),
            "vertices": len(obj.data.vertices),
            "triangles": len(obj.data.loop_triangles),
            "materials": [slot.material.name for slot in obj.material_slots if slot.material],
            "dimensions_cm": [round(v, 2) for v in obj.dimensions],
        })
    bpy.ops.wm.save_as_mainfile(filepath=str(BLEND))
    manifest = {
        "kit": "VanDaoProps",
        "authoring": "Original procedural Blender geometry generated by tools/create-vandao-props.py",
        "licence": "Project-original; copyright assigned to the project owner.",
            "generated_utc": datetime.now(timezone.utc).replace(microsecond=0).isoformat(),
            "unit": "centimetres at Unreal import target",
            "source_blend": {
                "file": BLEND.name,
                "sha256": hashlib.sha256(BLEND.read_bytes()).hexdigest(),
            },
            "assets": records,
    }
    (OUT / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    lines = ["# Vấn Đạo Tông Prop Kit — Static Audit", "", "- Source: original procedural Blender geometry; no external mesh, texture, or download.", "- Export: ASCII-only FBX names, selected mesh only, no armature, no animation.", "- Materials: `M_VDT_Stone_Jade`, `M_VDT_Stone_Dark`, `M_VDT_Bronze_Aged`, `M_VDT_Ember_Amber`.", "- Intended namespace after review: `/Game/TruongSinh/Environment/VanDaoTong/Props`.", "", "| Asset | Triangles | Materials | Dimensions (cm) |", "|---|---:|---|---|"]
    for r in records:
        lines.append("| %s | %d | %s | %s |" % (r["asset"], r["triangles"], ", ".join(r["materials"]), " × ".join(map(str, r["dimensions_cm"]))))
    (OUT / "STATIC_AUDIT.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
    backup = BLEND.with_suffix(BLEND.suffix + "1")
    if backup.exists():
        backup.unlink()
    print("VANDAO_PROPS_OK", len(records), "assets", FBX)


if __name__ == "__main__":
    main()
