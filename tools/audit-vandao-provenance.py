"""Read-only provenance and Blender FBX re-import gate for Vấn Đạo Tông.

Run with Blender 5.2 (the script imports FBX into a factory-reset in-memory
scene and never saves or writes an Unreal asset):

    blender --background --python tools/audit-vandao-provenance.py

The gate validates every VDT manifest hash, re-imports each production FBX,
and checks the single-mesh/unit/material-slot contract.  Derived normalized
FBXs are checked against their source kit dimensions multiplied by 0.01.
"""

from __future__ import annotations

import hashlib
import json
import math
from pathlib import Path

import bpy


ROOT = Path(__file__).resolve().parents[1]
TOLERANCE_RELATIVE = 0.005
TOLERANCE_ABSOLUTE = 0.03


def load_json(path: Path) -> dict:
    if not path.is_file():
        raise RuntimeError(f"Missing manifest: {path}")
    return json.loads(path.read_text(encoding="utf-8"))


def digest(path: Path) -> str:
    hasher = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            hasher.update(block)
    return hasher.hexdigest()


def require_hash(path: Path, expected: str, label: str) -> None:
    if not path.is_file():
        raise RuntimeError(f"Missing provenance file for {label}: {path}")
    actual = digest(path)
    if actual != expected:
        raise RuntimeError(f"Hash mismatch for {label}: expected={expected}, actual={actual}")


def vector_bounds() -> tuple[tuple[float, float, float], tuple[float, float, float]]:
    meshes = [obj for obj in bpy.context.scene.objects if obj.type == "MESH"]
    if not meshes:
        raise RuntimeError("FBX re-import produced no mesh object")
    if len(meshes) != 1:
        raise RuntimeError(f"FBX re-import must produce one joined mesh, got {len(meshes)}")
    if any(obj.type in {"ARMATURE", "CURVE", "SURFACE", "META", "FONT"} for obj in bpy.context.scene.objects):
        raise RuntimeError("FBX re-import produced a non-static object")
    points = [obj.matrix_world @ vertex.co for obj in meshes for vertex in obj.data.vertices]
    minimum = tuple(min(point[index] for point in points) for index in range(3))
    maximum = tuple(max(point[index] for point in points) for index in range(3))
    return minimum, maximum


def import_and_assert(path: Path, label: str, expected_dimensions: tuple[float, float, float], expected_materials: int) -> None:
    bpy.ops.wm.read_factory_settings(use_empty=True)
    result = bpy.ops.import_scene.fbx(filepath=str(path), use_anim=False)
    if "FINISHED" not in result:
        raise RuntimeError(f"Blender FBX import failed for {label}: {result}")
    minimum, maximum = vector_bounds()
    actual = tuple(maximum[index] - minimum[index] for index in range(3))
    for axis, value, expected in zip("XYZ", actual, expected_dimensions):
        tolerance = max(TOLERANCE_ABSOLUTE, abs(expected) * TOLERANCE_RELATIVE)
        if not math.isfinite(value) or abs(value - expected) > tolerance:
            raise RuntimeError(
                f"Dimension contract failed for {label} axis={axis}: actual={value:.6f}, "
                f"expected={expected:.6f}, tolerance={tolerance:.6f}"
            )
    mesh = next(obj for obj in bpy.context.scene.objects if obj.type == "MESH")
    materials = len(mesh.data.materials)
    if materials != expected_materials:
        raise RuntimeError(f"Material-slot contract failed for {label}: actual={materials}, expected={expected_materials}")
    print(
        "VDT_REIMPORT_PASS "
        f"asset={label} dimensions={tuple(round(value, 4) for value in actual)} "
        f"materials={materials}"
    )


def path_from_manifest(manifest_path: Path, relative: str) -> Path:
    return manifest_path.parent / Path(relative)


def build_specs() -> tuple[list[tuple[Path, str, tuple[float, float, float], int]], int]:
    specs: list[tuple[Path, str, tuple[float, float, float], int]] = []
    hash_count = 0

    gate_manifest_path = ROOT / "SourceArt/Architecture/VanDaoGate/MANIFEST.json"
    gate = load_json(gate_manifest_path)
    gate_relative = gate.get("file", f"Derived/{gate['asset']}")
    gate_path = path_from_manifest(gate_manifest_path, gate_relative)
    require_hash(gate_path, gate["sha256"], "VanDaoGate")
    source_blend = gate["source_blend"]
    require_hash(path_from_manifest(gate_manifest_path, source_blend["file"]), source_blend["sha256"], "VanDaoGate source blend")
    hash_count += 2
    specs.append((gate_path, "VanDaoGate", tuple(gate["dimensions_m"]), len(gate["materials"])))

    hall_manifest_path = ROOT / "SourceArt/Architecture/VanDaoHall/PROVENANCE.json"
    hall = load_json(hall_manifest_path)
    source_blend = hall["source_blend"]
    require_hash(path_from_manifest(hall_manifest_path, source_blend["file"]), source_blend["sha256"], "VanDaoHall source blend")
    hash_count += 1
    hall_dimensions = {
        "SM_VDT_MainHallFacade": (18.4, 7.3342, 9.35),
        "SM_VDT_RoofEave_Module": (5.8095, 1.495, 1.1),
        "SM_VDT_Dougong_Cluster": (1.5, 1.58, 0.87),
        "SM_VDT_MoonWindowPanel": (2.04, 2.04, 1.45),
    }
    hall_materials = {
        "SM_VDT_MainHallFacade": 5,
        "SM_VDT_RoofEave_Module": 2,
        "SM_VDT_Dougong_Cluster": 2,
        "SM_VDT_MoonWindowPanel": 2,
    }
    for item in hall["assets"]:
        path = path_from_manifest(hall_manifest_path, item["file"])
        require_hash(path, item["sha256"], f"VanDaoHall/{item['asset']}")
        hash_count += 1
        specs.append((path, f"VanDaoHall/{item['asset']}", hall_dimensions[item["asset"]], hall_materials[item["asset"]]))

    island_manifest_path = ROOT / "SourceArt/Architecture/VanDaoIsland/MANIFEST.json"
    island = load_json(island_manifest_path)
    island_path = path_from_manifest(island_manifest_path, island["asset"])
    require_hash(island_path, island["sha256"], "VanDaoIsland")
    source_blend = island["source_blend"]
    require_hash(path_from_manifest(island_manifest_path, source_blend["file"]), source_blend["sha256"], "VanDaoIsland source blend")
    hash_count += 2
    specs.append((island_path, "VanDaoIsland", tuple(island["dimensions_m"]), len(island["material_slots"])))

    estate_manifest_path = ROOT / "SourceArt/Architecture/VanDaoMasterEstate/MANIFEST.json"
    estate = load_json(estate_manifest_path)
    estate_path = path_from_manifest(estate_manifest_path, estate["asset"])
    require_hash(estate_path, estate["sha256"], "VanDaoMasterEstate")
    source_blend = estate["source_blend"]
    require_hash(path_from_manifest(estate_manifest_path, source_blend["file"]), source_blend["sha256"], "VanDaoMasterEstate source blend")
    hash_count += 2
    specs.append((estate_path, "VanDaoMasterEstate", tuple(estate["dimensions_m"]), len(estate["material_slots"])))

    for kit_name in ("VanDaoProps", "VanDaoWater"):
        manifest_path = ROOT / f"SourceArt/Architecture/{kit_name}/manifest.json"
        manifest = load_json(manifest_path)
        source_blend = manifest["source_blend"]
        require_hash(path_from_manifest(manifest_path, source_blend["file"]), source_blend["sha256"], f"{kit_name} source blend")
        hash_count += 1
        normalized_path = manifest_path.parent / "Normalized/MANIFEST.json"
        normalized = load_json(normalized_path)
        normalized_hashes = normalized["assets"]
        for item in manifest["assets"]:
            source_path = path_from_manifest(manifest_path, item["file"])
            require_hash(source_path, item["sha256"], f"{kit_name}/{item['asset']}")
            hash_count += 1
            source_dimensions = tuple(float(value) for value in item["dimensions_cm"])
            specs.append((source_path, f"{kit_name}/{item['asset']}", source_dimensions, len(item["materials"])))

            normalized_file = Path(item["file"]).name
            normalized_asset = manifest_path.parent / "Normalized" / normalized_file
            expected_normalized_hash = normalized_hashes.get(normalized_file)
            if not expected_normalized_hash:
                raise RuntimeError(f"Normalized manifest has no entry for {kit_name}/{normalized_file}")
            require_hash(normalized_asset, expected_normalized_hash, f"{kit_name}/Normalized/{normalized_file}")
            hash_count += 1
            specs.append(
                (
                    normalized_asset,
                    f"{kit_name}/Normalized/{item['asset']}",
                    tuple(value * 0.01 for value in source_dimensions),
                    len(item["materials"]),
                )
            )

    pavilion_manifest_path = ROOT / "SourceArt/Architecture/VVayToyekChinesePavilion/manifest.json"
    pavilion = load_json(pavilion_manifest_path)
    archive_path = path_from_manifest(pavilion_manifest_path, pavilion["archive"]["path"])
    require_hash(archive_path, pavilion["archive"]["sha256"], "VVayToyek pavilion archive")
    hash_count += 1
    pavilion_relative = pavilion["derived"]["normalized"]["path"]
    pavilion_path = path_from_manifest(pavilion_manifest_path, pavilion_relative)
    require_hash(pavilion_path, pavilion["derived"]["normalized"]["sha256"], "VVayToyek pavilion normalized FBX")
    hash_count += 1
    specs.append(
        (
            pavilion_path,
            "VVayToyekChinesePavilion/Normalized",
            tuple(value * 0.01 for value in pavilion["derived"]["normalized"]["ue_bounds_centimeters"]),
            len(pavilion["derived"]["geometry_review"]["material_slots"]),
        )
    )
    return specs, hash_count


def main() -> None:
    specs, hash_count = build_specs()
    for path, label, dimensions, materials in specs:
        import_and_assert(path, label, dimensions, materials)
    print(f"VDT_PROVENANCE_AUDIT_PASS assets={len(specs)} hashes={hash_count} blender={bpy.app.version_string}")


if __name__ == "__main__":
    main()
