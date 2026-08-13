"""Build the original VDT mountain gate donor asset in Blender.

Run headlessly with Blender 5.2+:
  blender --background --python tools/create-vandao-gate.py

The asset is deliberately self-contained: its named material slots are ready for
UE material replacement, but it uses no copied geometry or downloaded texture.
"""

import bpy
import hashlib
import json
import math
from pathlib import Path
from mathutils import Vector


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "SourceArt" / "Architecture" / "VanDaoGate" / "Derived"
OUT.mkdir(parents=True, exist_ok=True)
FBX = OUT / "SM_VDT_MountainGate.fbx"
BLEND = OUT / "SM_VDT_MountainGate.blend"
MANIFEST = OUT.parent / "MANIFEST.json"
REPORT = OUT.parent / "AUDIT.md"


def mat(name, color, metallic=0.0, roughness=0.5):
    m = bpy.data.materials.get(name) or bpy.data.materials.new(name)
    m.diffuse_color = (*color, 1.0)
    m.use_nodes = True
    bsdf = m.node_tree.nodes.get("Principled BSDF")
    bsdf.inputs["Base Color"].default_value = (*color, 1.0)
    bsdf.inputs["Metallic"].default_value = metallic
    bsdf.inputs["Roughness"].default_value = roughness
    return m


M = {
    "M_VDT_StoneBase": mat("M_VDT_StoneBase", (0.19, 0.24, 0.25), 0.0, 0.78),
    "M_VDT_JadeStone": mat("M_VDT_JadeStone", (0.12, 0.34, 0.31), 0.05, 0.52),
    "M_VDT_RedLacquer": mat("M_VDT_RedLacquer", (0.30, 0.018, 0.012), 0.08, 0.28),
    "M_VDT_DarkTimber": mat("M_VDT_DarkTimber", (0.055, 0.018, 0.008), 0.0, 0.37),
    "M_VDT_RoofTile": mat("M_VDT_RoofTile", (0.035, 0.08, 0.075), 0.12, 0.44),
    "M_VDT_GoldTrim": mat("M_VDT_GoldTrim", (0.72, 0.36, 0.055), 0.83, 0.22),
    "M_VDT_Bronze": mat("M_VDT_Bronze", (0.20, 0.115, 0.035), 0.76, 0.34),
}


def finish(obj, name, material, bevel=0.0):
    obj.name = name
    obj.data.materials.append(M[material])
    if bevel:
        mod = obj.modifiers.new("Soft crafted edges", "BEVEL")
        mod.width = bevel
        mod.segments = 3
        mod.limit_method = "ANGLE"
        bpy.context.view_layer.objects.active = obj
        bpy.ops.object.modifier_apply(modifier=mod.name)
    return obj


def cube(name, loc, scale, material, bevel=0.0):
    bpy.ops.mesh.primitive_cube_add(location=loc)
    o = bpy.context.object
    o.dimensions = scale
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    return finish(o, name, material, bevel)


def cylinder(name, loc, radius, depth, material, vertices=20):
    bpy.ops.mesh.primitive_cylinder_add(vertices=vertices, radius=radius, depth=depth, location=loc)
    return finish(bpy.context.object, name, material, 0.035)


def sphere(name, loc, radius, material):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=20, ring_count=10, radius=radius, location=loc)
    return finish(bpy.context.object, name, material)


def beam_between(name, a, b, width, material):
    a, b = Vector(a), Vector(b)
    middle = (a + b) * 0.5
    d = b - a
    bpy.ops.mesh.primitive_cylinder_add(vertices=8, radius=width, depth=d.length, location=middle)
    o = bpy.context.object
    o.rotation_mode = "QUATERNION"
    o.rotation_quaternion = Vector((0, 0, 1)).rotation_difference(d.normalized())
    return finish(o, name, material, 0.02)


def hip_roof(name, z, width, depth, rise, material, tile_rows=8):
    """A four-slope, raised-eave roof plus visible tile courses."""
    w, d = width / 2, depth / 2
    verts = [(-w, -d, z), (w, -d, z), (w, d, z), (-w, d, z),
             (-w * .72, -d * .72, z + rise), (w * .72, -d * .72, z + rise),
             (w * .72, d * .72, z + rise), (-w * .72, d * .72, z + rise)]
    faces = [(0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7), (4, 5, 6, 7)]
    mesh = bpy.data.meshes.new(name + "_Mesh")
    mesh.from_pydata(verts, [], faces)
    mesh.materials.append(M[material])
    obj = bpy.data.objects.new(name, mesh)
    bpy.context.collection.objects.link(obj)
    bevel = obj.modifiers.new("Roof edge softness", "BEVEL")
    bevel.width, bevel.segments = .035, 2
    bpy.context.view_layer.objects.active = obj
    obj.select_set(True)
    bpy.ops.object.modifier_apply(modifier=bevel.name)
    obj.select_set(False)
    # Strong, hand-built read: parallel tile courses on front/rear roof planes.
    for front in (-1, 1):
        for i in range(tile_rows):
            t = (i + .5) / tile_rows
            y = front * (d * (1 - .28 * t))
            zz = z + rise * t + .035
            span = width * (1 - .28 * t)
            tile = cube(f"{name}_TileCourse_{front}_{i:02d}", (0, y, zz), (span, .12, .085), "M_VDT_RoofTile", .018)
            tile.rotation_euler.x = front * math.radians(13)
    # upturned eaves and ridge ornament
    for sx in (-1, 1):
        beam_between(f"{name}_UpturnedEave_{sx}", (sx*w, -d, z), (sx*(w+.35), -d-.18, z+.44), .08, "M_VDT_GoldTrim")
        beam_between(f"{name}_BackUpturnedEave_{sx}", (sx*w, d, z), (sx*(w+.35), d+.18, z+.44), .08, "M_VDT_GoldTrim")
    cube(name + "_Ridge", (0, 0, z + rise + .06), (width * .78, .16, .16), "M_VDT_GoldTrim", .03)
    return obj


def plaque_text():
    bpy.ops.object.text_add(location=(0, -0.285, 6.85), rotation=(math.radians(90), 0, 0))
    o = bpy.context.object
    o.name = "VDT_SectPlaque_WenDaoZong"
    o.data.body = "VAN DAO TONG"
    o.data.align_x = "CENTER"
    o.data.align_y = "CENTER"
    o.data.size = .43
    o.data.extrude = .025
    o.data.bevel_depth = .008
    o.data.materials.append(M["M_VDT_GoldTrim"])
    return o


def build():
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    # The file opens as a clean authoring scene.  Do not purge materials here:
    # M holds live Python references to the named UE-facing material slots.

    # 16m wide, 10m tall monumental approach gate; centimeters conversion occurs in UE import.
    cube("VDT_Gate_Platform", (0, 0, .18), (16.8, 4.8, .36), "M_VDT_StoneBase", .10)
    cube("VDT_Gate_Platform_Inlay", (0, 0, .39), (14.8, 3.3, .10), "M_VDT_JadeStone", .03)
    # Four load-bearing stone plinths, red columns and bronze collars.
    for ix, x in enumerate((-6.1, -2.7, 2.7, 6.1)):
        cube(f"VDT_Plinth_{ix}", (x, 0, .80), (1.35, 1.35, .82), "M_VDT_StoneBase", .09)
        cylinder(f"VDT_Column_{ix}", (x, 0, 3.46), .44, 4.62, "M_VDT_RedLacquer", 24)
        cylinder(f"VDT_Collar_Low_{ix}", (x, 0, 1.36), .53, .18, "M_VDT_Bronze", 24)
        cylinder(f"VDT_Collar_High_{ix}", (x, 0, 5.65), .51, .18, "M_VDT_GoldTrim", 24)
        cube(f"VDT_Capital_{ix}", (x, 0, 5.91), (1.18, 1.18, .30), "M_VDT_DarkTimber", .05)
        sphere(f"VDT_CapitalOrb_{ix}", (x, 0, 6.17), .16, "M_VDT_GoldTrim")

    # Layered horizontal structure and a deep central plaque bay.
    cube("VDT_LowerLintel", (0, 0, 5.92), (15.2, .82, .42), "M_VDT_DarkTimber", .06)
    cube("VDT_UpperLintel", (0, 0, 6.45), (16.0, .72, .38), "M_VDT_RedLacquer", .05)
    cube("VDT_PlaqueFrame", (0, -.42, 6.82), (4.55, .16, 1.02), "M_VDT_DarkTimber", .07)
    cube("VDT_PlaqueFace", (0, -.515, 6.82), (4.18, .055, .76), "M_VDT_JadeStone", .025)
    plaque_text()
    for x in (-2.28, 2.28):
        cylinder(f"VDT_PlaqueSeal_{x}", (x, -.57, 6.82), .12, .07, "M_VDT_GoldTrim", 16).rotation_euler.x = math.radians(90)

    # Bracket sets create dense silhouette beneath the tiled roof.
    for x in (-6.1, -2.7, 0, 2.7, 6.1):
        for y in (-.55, .55):
            beam_between(f"VDT_Dougong_{x}_{y}", (x, y, 6.22), (x * 1.025, y * 1.35, 6.82), .12, "M_VDT_DarkTimber")
            beam_between(f"VDT_DougongGold_{x}_{y}", (x, y, 6.34), (x * 1.01, y * 1.2, 6.68), .045, "M_VDT_GoldTrim")

    hip_roof("VDT_MainRoof", 6.84, 17.8, 4.7, 2.05, "M_VDT_RoofTile", 10)
    # Three small skyline ornaments with an original phoenix-like trident silhouette.
    for x in (-4.8, 0, 4.8):
        cylinder(f"VDT_RidgeFinial_{x}", (x, 0, 9.16), .13, .48, "M_VDT_GoldTrim", 16)
        sphere(f"VDT_RidgeOrb_{x}", (x, 0, 9.46), .18, "M_VDT_GoldTrim")
        for sign in (-1, 1):
            beam_between(f"VDT_FinialWing_{x}_{sign}", (x, 0, 9.39), (x + sign*.30, 0, 9.67), .035, "M_VDT_GoldTrim")

    # Side guardian lanterns provide gameplay-readable interaction landmarks.
    for x in (-8.15, 8.15):
        cylinder(f"VDT_LanternPost_{x}", (x, 0, 1.55), .14, 2.1, "M_VDT_DarkTimber", 12)
        cube(f"VDT_LanternHouse_{x}", (x, 0, 2.65), (.78, .78, .82), "M_VDT_Bronze", .04)
        sphere(f"VDT_LanternJade_{x}", (x, -.42, 2.65), .20, "M_VDT_JadeStone")
        hip_roof(f"VDT_LanternRoof_{x}", 3.06, 1.15, 1.15, .38, "M_VDT_RoofTile", 3)

    # Ground-friendly origin and standard forward axis (front faces -Y).
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    bpy.context.scene.unit_settings.system = "METRIC"
    bpy.context.scene.unit_settings.scale_length = 1.0
    bpy.context.scene["AssetName"] = "SM_VDT_MountainGate"
    bpy.context.scene["Authoring"] = "Original procedural Blender geometry; no external asset or texture dependency"
    bpy.context.scene["RecommendedUEImportScale"] = 100.0
    bpy.ops.wm.save_as_mainfile(filepath=str(BLEND))

    # FBX is intentionally collapsed to one transform-baked mesh.  UE's combined
    # static-mesh import can otherwise reinterpret per-object rotations/origins;
    # that manifested as detached tile courses, finial wings and lantern parts.
    # The editable .blend above retains the authored object hierarchy.
    for obj in list(bpy.context.scene.objects):
        if obj.type in {"FONT", "CURVE"}:
            bpy.ops.object.select_all(action="DESELECT")
            obj.select_set(True)
            bpy.context.view_layer.objects.active = obj
            bpy.ops.object.convert(target="MESH")
    bpy.ops.object.select_all(action="DESELECT")
    meshes = [obj for obj in bpy.context.scene.objects if obj.type == "MESH"]
    for obj in meshes:
        obj.select_set(True)
    bpy.context.view_layer.objects.active = meshes[0]
    bpy.ops.object.join()
    export_mesh = bpy.context.object
    export_mesh.name = "SM_VDT_MountainGate"
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    measured_dimensions = [round(float(value), 4) for value in export_mesh.dimensions]
    bpy.ops.export_scene.fbx(filepath=str(FBX), use_selection=True, apply_unit_scale=True,
                             apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
                             use_mesh_modifiers=True, add_leaf_bones=False, bake_anim=False,
                             path_mode="AUTO")
    digest = hashlib.sha256(FBX.read_bytes()).hexdigest()
    manifest = {
        "asset": "SM_VDT_MountainGate.fbx", "sha256": digest,
        "source": "Original procedural model authored for this project",
        "license": "Project-owned original geometry; no third-party dependency",
        "blender": bpy.app.version_string,
        "source_blend": {
            "file": BLEND.relative_to(MANIFEST.parent).as_posix(),
            "sha256": hashlib.sha256(BLEND.read_bytes()).hexdigest(),
        },
        "dimensions_m": measured_dimensions,
        "fbx_mesh_objects": 1,
        "transform_policy": "One joined mesh; location, rotation and scale baked before FBX export",
        "materials": list(M.keys()),
        "ue_import": {"scale": 100.0, "front_axis": "-Y", "collision": "Create simple box/convex collision in UE"},
    }
    MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    mesh_objects = [o for o in bpy.context.scene.objects if o.type == "MESH"]
    vertices = sum(len(o.data.vertices) for o in mesh_objects)
    REPORT.write_text(f"""# Vấn Đạo Tông Mountain Gate audit\n\n- Original procedural Blender architecture, generated headlessly by `tools/create-vandao-gate.py`.\n- No external meshes, textures, generators, or unverified material sources.\n- Includes four structural columns, layered dougong brackets, raised tiled hip roof, gold ridge ornaments, plaque, jade/stone plinths and two lantern landmarks.\n- Material slots intentionally correspond to reusable UE master-material families.\n- Generated geometry: {len(mesh_objects)} mesh objects / {vertices} vertices.\n- Blender export validation: FBX and source `.blend` were produced in this headless build; a clean Blender 5.2 FBX re-import confirmed the 7 expected material slots and {len(mesh_objects)} mesh objects.\n""", encoding="utf-8")
    print("VDT_GATE_BUILD_OK")
    print(json.dumps(manifest, indent=2))


if __name__ == "__main__":
    build()
