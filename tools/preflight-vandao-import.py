"""UE5.8 sandbox import gate for the normalised Vấn Đạo Tông prop kits.

It must pass before these assets are imported to their production path or
referenced by a map. The explicit contract prevents actor-scale workarounds.
"""

from __future__ import annotations

import hashlib
import os

import unreal


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
DESTINATION = "/Game/External/VDTPreflight"
TOLERANCE = 0.30
ASSETS = (
    ("VanDaoProps", "SM_VDT_StoneStele", (100.0, 54.0, 252.0), 3, "87aa98d39b080ff9ef8e64fe77c2b932156b2723d2627e85f9f30fc14a3477c4"),
    ("VanDaoProps", "SM_VDT_StoneLantern", (78.0, 78.0, 167.0), 4, "7bd1249b58a54354cf1494ddac1efb1314882d567d2d932fcfe6feeead60e1be"),
    ("VanDaoProps", "SM_VDT_RitualBrazier", (107.3, 104.3, 142.0), 3, "592dbbe07e9a1abefb132a228b59b81c363c2b3d962722d1ed48b8ed2b847be0"),
    ("VanDaoProps", "SM_VDT_BridgeBaluster", (36.0, 36.0, 98.5), 2, "b2055ae28c7da919f7553b98dfbee89d104805146db99f7a557914189a144d4a"),
    ("VanDaoProps", "SM_VDT_BridgeRailing_400", (400.0, 36.0, 122.0), 2, "2ad2b0bae3b7e182a9fc2c307b093f73ee768e8938897f3e6f796bc010d423ac"),
    ("VanDaoWater", "SM_VDT_PondShore_Curve_Quarter", (760.0, 760.0, 47.0), 2, "44cb8921ee26f5acd48fe8a70bc12fe94fed80747b85c7e2b05809c86c584d12"),
    ("VanDaoWater", "SM_VDT_ArchedFootbridge_600", (697.6, 245.0, 152.0), 2, "3be78ad6da5ba857baccb6be43e341d472f516b1909f356b73685abdba092031"),
    ("VanDaoWater", "SM_VDT_LotusPadCluster_A", (304.9, 233.6, 40.7), 2, "a22209d0b78afa6cf1d6ef90cb59d7d249181f46c459d5062cf7ebcfc8468182"),
    ("VanDaoWater", "SM_VDT_SteppingStoneCluster_A", (557.9, 190.7, 24.6), 1, "05e66db267a88ae7263861447c3e4dab63ef762c76b7482dc7a009280553aca1"),
)


def import_and_assert(group: str, name: str, expected_values: tuple[float, float, float], material_count: int, source_hash: str) -> None:
    source = os.path.join(ROOT, "SourceArt", "Architecture", group, "Normalized", f"{name}.fbx")
    if not os.path.isfile(source):
        raise RuntimeError(f"Missing preflight source: {source}")
    actual_hash = hashlib.sha256(open(source, "rb").read()).hexdigest()
    if actual_hash != source_hash:
        raise RuntimeError(f"Derivative hash mismatch for {name}: {actual_hash}")

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
    options.static_mesh_import_data.set_editor_property("auto_generate_collision", True)
    task.set_editor_property("options", options)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

    path = f"{DESTINATION}/{name}_Preflight.{name}_Preflight"
    mesh = unreal.EditorAssetLibrary.load_asset(path)
    if not mesh:
        raise RuntimeError(f"Preflight import produced no mesh: {path}")
    expected = unreal.Vector(*expected_values)
    box = mesh.get_bounding_box()
    actual = box.max - box.min
    ratios = (actual.x / expected.x, actual.y / expected.y, actual.z / expected.z)
    if any(abs(value - 1.0) > TOLERANCE for value in ratios):
        raise RuntimeError(f"FBX unit contract failed for {name}: actual={actual}, expected~={expected}, ratios={ratios}")
    static_materials = mesh.get_editor_property("static_materials")
    if len(static_materials) != material_count:
        raise RuntimeError(f"FBX material-slot contract failed for {name}: actual={len(static_materials)}, expected={material_count}")
    unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False)
    unreal.log(f"VDT IMPORT PREFLIGHT PASS: asset={name}; actual={actual}; ratios={ratios}; materials={len(static_materials)}")


unreal.EditorAssetLibrary.make_directory(DESTINATION)
for asset in ASSETS:
    import_and_assert(*asset)
unreal.log(f"VDT IMPORT PREFLIGHT SUITE PASS: assets={len(ASSETS)}")
