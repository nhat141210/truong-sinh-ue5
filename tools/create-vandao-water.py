#!/usr/bin/env python3
"""Create original Vấn Đạo Tông water-garden source meshes with Blender.

This is deliberately an isolated source-art generator.  It does not import into
Unreal or modify a level.  The outputs are a small, reviewable kit: curved
shoreline, a gently arched footbridge, lotus pads, and organic stepping stones.
All dimensions are authored in centimetres for the intended Unreal import.
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
OUT = ROOT / "SourceArt" / "Architecture" / "VanDaoWater"
FBX = OUT / "FBX"
BLEND = OUT / "VanDaoWater_Source.blend"


def mesh(name, vertices, faces, material):
    data = bpy.data.meshes.new(name + "_Mesh")
    data.from_pydata(vertices, [], faces)
    data.materials.append(material)
    obj = bpy.data.objects.new(name, data)
    bpy.context.collection.objects.link(obj)
    for polygon in data.polygons:
        polygon.use_smooth = True
    return obj


def mat(name, color, roughness, metallic=0.0):
    result = bpy.data.materials.new(name)
    result.diffuse_color = (*color, 1.0)
    result.use_nodes = True
    bsdf = result.node_tree.nodes.get("Principled BSDF")
    bsdf.inputs["Base Color"].default_value = (*color, 1.0)
    bsdf.inputs["Roughness"].default_value = roughness
    bsdf.inputs["Metallic"].default_value = metallic
    return result


def add_bevel(obj, width=1.5):
    modifier = obj.modifiers.new("HandSoftenedEdges", "BEVEL")
    modifier.width = width
    modifier.segments = 2
    modifier.limit_method = "ANGLE"
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=modifier.name)


def join(parts, name):
    bpy.ops.object.select_all(action="DESELECT")
    for part in parts:
        part.select_set(True)
    bpy.context.view_layer.objects.active = parts[0]
    bpy.ops.object.join()
    parts[0].name = name
    return parts[0]


def shoreline_curve(stone, moss):
    """A 90-degree hand-cut bank segment: raised outer lip, shallow inner shelf."""
    segments, r_inner, r_outer = 18, 580.0, 760.0
    verts, faces = [], []
    for i in range(segments + 1):
        angle = math.radians(i * 90.0 / segments)
        wav = math.sin(i * 2.17) * 8.0 + math.cos(i * 0.71) * 5.0
        for radius, height in ((r_inner, 2.0), (r_inner + 72, 20.0 + wav), (r_outer, 42.0 + wav * .55)):
            verts.append((math.cos(angle) * radius, math.sin(angle) * radius, height))
    # three concentric rail strips, with non-flat contour from radial profile
    for i in range(segments):
        for rail in range(2):
            a = i * 3 + rail
            b = (i + 1) * 3 + rail
            faces.append((a, b, b + 1, a + 1))
    shore = mesh("SM_VDT_PondShore_Curve_Quarter", verts, faces, stone)
    add_bevel(shore, 2.0)

    # Moss ribbons are deliberately separate material regions, laid only on the outer rise.
    moss_parts = []
    for i in (2, 5, 9, 13, 16):
        angle = math.radians(i * 90.0 / segments)
        radius = r_inner + 118
        x, y = math.cos(angle) * radius, math.sin(angle) * radius
        size = 31 + (i % 3) * 7
        v = [(x - size, y - size * .45, 42), (x + size, y - size * .4, 44),
             (x + size * .7, y + size * .55, 43), (x - size * .6, y + size * .55, 43)]
        moss_parts.append(mesh("MossPatch", v, [(0, 1, 2, 3)], moss))
    return shore, moss_parts


def curved_deck(waterwood, stone):
    """A six-metre arching bridge composed of irregular boards on two curved ribs."""
    pieces = []
    span, half_width, segments = 620.0, 118.0, 24
    # curved deck shell; natural width undulates slightly so it does not read as a box.
    verts, faces = [], []
    for i in range(segments + 1):
        t = i / segments
        x = (t - .5) * span
        z = 34 + math.sin(t * math.pi) * 115
        width = half_width + math.sin(t * math.pi * 5.0) * 4.5
        verts.extend([(x, -width, z), (x, width, z), (x, -width, z - 15), (x, width, z - 15)])
    for i in range(segments):
        a, b = i * 4, (i + 1) * 4
        faces.extend([(a, b, b + 1, a + 1), (a + 2, a + 3, b + 3, b + 2),
                      (a, a + 2, b + 2, b), (a + 1, b + 1, b + 3, a + 3)])
    deck = mesh("BridgeDeck", verts, faces, waterwood)
    add_bevel(deck, 1.8)
    pieces.append(deck)
    # transverse boards score the silhouette, leaving slight gaps to catch highlights.
    for i in range(1, segments):
        t = i / segments
        x = (t - .5) * span
        z = 34 + math.sin(t * math.pi) * 115 + 3
        board = mesh("BridgeBoard", [(x - 3, -116, z), (x + 3, -116, z), (x + 3, 116, z), (x - 3, 116, z)], [(0, 1, 2, 3)], stone)
        pieces.append(board)
    # Twin stone footings have a faceted, tapering silhouette rather than cylinders/cubes.
    for x in (-span / 2 + 32, span / 2 - 32):
        radius = 72
        v, f = [], []
        for level, z in enumerate((0, 22, 70)):
            scale = (1.0, .82, .58)[level]
            for i in range(8):
                a = i * math.tau / 8 + .13 * (level % 2)
                v.append((x + math.cos(a) * radius * scale, math.sin(a) * radius * scale, z))
        for level in range(2):
            for i in range(8):
                n = (i + 1) % 8
                f.append((level * 8 + i, level * 8 + n, (level + 1) * 8 + n, (level + 1) * 8 + i))
        f.extend([tuple(range(7, -1, -1)), tuple(range(16, 24))])
        footing = mesh("BridgeFooting", v, f, stone)
        add_bevel(footing, 2)
        pieces.append(footing)
    return join(pieces, "SM_VDT_ArchedFootbridge_600")


def lotus_cluster(lotus, flower):
    parts = []
    # Three notched, irregular pads with slight individual tilt; front-facing slit sells lily geometry.
    for index, (cx, cy, radius, tilt) in enumerate(((-75, 18, 77, .06), (20, -22, 103, -.04), (100, 48, 65, .08))):
        verts = [(cx, cy, 11)]
        for i in range(15):
            a = math.tau * i / 15
            r = radius * (0.88 + 0.15 * math.sin(i * 2.3 + index))
            # A small radial notch on the west side.
            if i in (7, 8): r *= .52
            verts.append((cx + math.cos(a) * r, cy + math.sin(a) * r, 11 + math.sin(a) * tilt * radius))
        faces = []
        for i in range(15): faces.append((0, i + 1, (i + 1) % 15 + 1))
        pad = mesh("LotusPad", verts, faces, lotus)
        solid = pad.modifiers.new("PadThickness", "SOLIDIFY")
        solid.thickness = 2.5
        bpy.context.view_layer.objects.active = pad
        bpy.ops.object.modifier_apply(modifier=solid.name)
        parts.append(pad)
    # Original radial blossom petals.
    for ring, z, size in ((0, 20, 30), (1, 28, 21)):
        count = 8 if ring == 0 else 6
        for i in range(count):
            a = math.tau * i / count + ring * .23
            v = [(20, -22, z), (20 + math.cos(a - .22) * size, -22 + math.sin(a - .22) * size, z + 1),
                 (20 + math.cos(a) * size * 1.55, -22 + math.sin(a) * size * 1.55, z + (10 if ring == 0 else 16)),
                 (20 + math.cos(a + .22) * size, -22 + math.sin(a + .22) * size, z + 1)]
            parts.append(mesh("LotusPetal", v, [(0, 1, 2, 3)], flower))
    return join(parts, "SM_VDT_LotusPadCluster_A")


def stepping_stones(stone):
    parts = []
    layout = ((0, 0, 78, 58, 18), (145, 70, 63, 46, 22), (282, -15, 88, 61, 17), (430, 58, 56, 43, 20))
    for stone_index, (cx, cy, rx, ry, height) in enumerate(layout):
        rings, sides = 4, 12
        verts = [(cx, cy, 0)]
        for ring in range(1, rings + 1):
            amount = ring / rings
            for i in range(sides):
                a = math.tau * i / sides
                noise = 1 + .09 * math.sin(i * 2.7 + stone_index * 1.9)
                z = math.sin(amount * math.pi * .52) * height * (1 - .12 * math.cos(a * 3))
                verts.append((cx + math.cos(a) * rx * amount * noise, cy + math.sin(a) * ry * amount * noise, z))
        faces = []
        for i in range(sides): faces.append((0, 1 + i, 1 + (i + 1) % sides))
        for ring in range(1, rings):
            base, nxt = 1 + (ring - 1) * sides, 1 + ring * sides
            for i in range(sides): faces.append((base + i, base + (i + 1) % sides, nxt + (i + 1) % sides, nxt + i))
        obj = mesh("SteppingStone", verts, faces, stone)
        parts.append(obj)
    return join(parts, "SM_VDT_SteppingStoneCluster_A")


def export(obj):
    bpy.ops.object.select_all(action="DESELECT")
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    bpy.ops.export_scene.fbx(filepath=str(FBX / (obj.name + ".fbx")), use_selection=True,
        object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", axis_forward="-Z", axis_up="Y",
        mesh_smooth_type="FACE", add_leaf_bones=False)


def digest(path):
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""): h.update(block)
    return h.hexdigest()


def main():
    if FBX.exists(): shutil.rmtree(FBX)
    FBX.mkdir(parents=True, exist_ok=True)
    bpy.ops.object.select_all(action="SELECT")
    bpy.ops.object.delete(use_global=False)
    stone = mat("M_VDT_WetStone", (.105, .17, .15), .52)
    moss = mat("M_VDT_Moss", (.055, .16, .08), .9)
    wood = mat("M_VDT_WaterWood", (.12, .055, .022), .61)
    lotus = mat("M_VDT_LotusLeaf", (.045, .23, .115), .48)
    flower = mat("M_VDT_LotusFlower", (.98, .42, .50), .57)
    shore, mosses = shoreline_curve(stone, moss)
    assets = [shore, curved_deck(wood, stone), lotus_cluster(lotus, flower), stepping_stones(stone)]
    # Moss strips accompany the shore FBX as one authored shoreline source object.
    assets[0] = join([shore] + mosses, "SM_VDT_PondShore_Curve_Quarter")
    records = []
    for obj in assets:
        export(obj)
        path = FBX / (obj.name + ".fbx")
        obj.data.calc_loop_triangles()
        records.append({"asset": obj.name, "file": "FBX/" + path.name, "sha256": digest(path),
                        "vertices": len(obj.data.vertices), "triangles": len(obj.data.loop_triangles),
                        "dimensions_cm": [round(x, 1) for x in obj.dimensions],
                        "materials": [slot.material.name for slot in obj.material_slots if slot.material]})
    bpy.ops.wm.save_as_mainfile(filepath=str(BLEND))
    provenance = {"kit": "VanDaoWater", "authoring": "Original procedural Blender geometry generated locally by tools/create-vandao-water.py.",
                  "licence": "Project-original; copyright assigned to the project owner.",
                  "generated_utc": datetime.now(timezone.utc).replace(microsecond=0).isoformat(),
                  "unit": "centimetres at Unreal import target",
                  "source_blend": {"file": BLEND.name, "sha256": hashlib.sha256(BLEND.read_bytes()).hexdigest()},
                  "assets": records}
    (OUT / "MANIFEST.json").write_text(json.dumps(provenance, indent=2) + "\n", encoding="utf-8")
    report = ["# Vấn Đạo Tông Water Garden — Static Audit", "", "- Source: project-original Blender geometry; no downloaded mesh or texture.", "- Intended Unreal namespace after review: `/Game/TruongSinh/Environment/VanDaoTong/WaterGarden`.", "- The pond is a shoreline/bed mesh only; gameplay water remains an Unreal material or water actor.", "", "| Asset | Triangles | Dimensions (cm) |", "|---|---:|---|"]
    report += ["| %s | %d | %s |" % (item["asset"], item["triangles"], " × ".join(map(str, item["dimensions_cm"]))) for item in records]
    (OUT / "STATIC_AUDIT.md").write_text("\n".join(report) + "\n", encoding="utf-8")
    backup = BLEND.with_suffix(BLEND.suffix + "1")
    if backup.exists(): backup.unlink()
    print("VANDAO_WATER_OK", len(records), "assets", FBX)


if __name__ == "__main__": main()
