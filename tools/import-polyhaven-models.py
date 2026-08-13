"""Import the approved Poly Haven glTF nature kit through UE Interchange."""

import os
import unreal


SOURCE_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "SourceArt", "Environment", "PolyHaven", "model"))
MODELS = (
    ("pine_sapling_small", "1k"),
    ("shrub_02", "1k"),
    ("fern_02", "1k"),
    ("rock_face_01", "2k"),
    ("rock_moss_set_01", "2k"),
)


for asset_id, resolution in MODELS:
    source = os.path.join(SOURCE_ROOT, asset_id, f"{asset_id}_{resolution}.gltf")
    if not os.path.isfile(source):
        raise RuntimeError(f"Missing downloaded model: {source}")
    destination = f"/Game/VisualTarget/Nature/{asset_id}"
    unreal.EditorAssetLibrary.make_directory(destination)
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", source)
    task.set_editor_property("destination_path", destination)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("save", True)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    if not task.get_editor_property("imported_object_paths"):
        raise RuntimeError(f"Interchange imported no objects for {asset_id}")
    unreal.log(f"Imported {asset_id}: {len(task.get_editor_property('imported_object_paths'))} objects")

unreal.log("Poly Haven nature import PASS: models=5")
