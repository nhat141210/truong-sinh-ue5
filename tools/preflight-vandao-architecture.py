"""UE5.8 sandbox import gate for Vấn Đạo Tông hero architecture."""

from __future__ import annotations

import hashlib
import os

import unreal


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
DESTINATION = "/Game/External/VDTArchitecturePreflight"
TOLERANCE = 0.03
ASSETS = (
    (
        "SourceArt/Architecture/VanDaoGate/Derived/SM_VDT_MountainGate.fbx",
        "SM_VDT_MountainGate",
        (1860.68, 519.38, 968.87),
        7,
        "e7bbb9d83ce171d81b00793bb37c8398351b651fb2ceac4530fad7ab0b82620c",
    ),
    (
        "SourceArt/Architecture/VanDaoHall/VDT_MainHallFacade.fbx",
        "SM_VDT_MainHallFacade",
        (1840.0, 733.418, 935.0),
        5,
        "d89fed7faf0c29a004f6687624f4aa99fdd001fe1ef63baefabf29eb0beac716",
    ),
    (
        "SourceArt/Architecture/VVayToyekChinesePavilion/Derived/Normalized/SM_VDT_Pavilion_Hero.fbx",
        "SM_VDT_Pavilion_Hero",
        (971.666, 971.666, 892.268),
        5,
        "ab13b5648d1a614e682a6e830b13a5c8ea5f3ad5777aea04b995f3d4a11cd34b",
    ),
    (
        "SourceArt/Architecture/VanDaoMasterEstate/SM_VDT_MasterEstate.fbx",
        "SM_VDT_MasterEstate",
        (29902.6, 20647.1, 1820.0),
        2,
        "9af953f4ecf2ac8f476161d4d0f912cd116fec3aa5447f4c7387b7289caaf0cb",
    ),
)


def sha256(path: str) -> str:
    digest = hashlib.sha256()
    with open(path, "rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def import_and_assert(relative_source: str, name: str, expected_values: tuple[float, float, float], material_count: int, expected_hash: str) -> None:
    source = os.path.join(ROOT, *relative_source.split("/"))
    if not os.path.isfile(source):
        raise RuntimeError(f"Missing architecture source: {source}")
    actual_hash = sha256(source)
    if actual_hash != expected_hash:
        raise RuntimeError(f"Source hash mismatch for {name}: {actual_hash}")

    task = unreal.AssetImportTask()
    task.set_editor_property("filename", source)
    task.set_editor_property("destination_path", DESTINATION)
    task.set_editor_property("destination_name", f"{name}_Preflight")
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("save", True)
    options = unreal.FbxImportUI()
    options.set_editor_property("import_mesh", True)
    options.set_editor_property("import_as_skeletal", False)
    options.static_mesh_import_data.set_editor_property("combine_meshes", True)
    options.static_mesh_import_data.set_editor_property("import_uniform_scale", 1.0)
    options.static_mesh_import_data.set_editor_property("auto_generate_collision", False)
    task.set_editor_property("options", options)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

    object_path = f"{DESTINATION}/{name}_Preflight.{name}_Preflight"
    mesh = unreal.EditorAssetLibrary.load_asset(object_path)
    if not mesh:
        raise RuntimeError(f"Architecture preflight produced no mesh: {object_path}")
    expected = unreal.Vector(*expected_values)
    bounds = mesh.get_bounding_box()
    actual = bounds.max - bounds.min
    ratios = (actual.x / expected.x, actual.y / expected.y, actual.z / expected.z)
    if any(abs(value - 1.0) > TOLERANCE for value in ratios):
        raise RuntimeError(f"Architecture unit contract failed for {name}: actual={actual}, expected={expected}, ratios={ratios}")
    static_materials = mesh.get_editor_property("static_materials")
    if len(static_materials) != material_count:
        raise RuntimeError(f"Architecture material-slot contract failed for {name}: actual={len(static_materials)}, expected={material_count}")
    unreal.log(f"VDT ARCHITECTURE PREFLIGHT PASS: asset={name}; actual={actual}; ratios={ratios}; materials={len(static_materials)}")


unreal.EditorAssetLibrary.make_directory(DESTINATION)
for asset in ASSETS:
    import_and_assert(*asset)
unreal.log(f"VDT ARCHITECTURE PREFLIGHT SUITE PASS: assets={len(ASSETS)}")


