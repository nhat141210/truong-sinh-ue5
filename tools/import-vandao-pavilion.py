"""Import the Blender-prepared CC0 Vấn Đạo Tông hero pavilion into UE5."""

import os
import unreal


PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SOURCE = os.path.join(
    PROJECT_ROOT,
    "SourceArt",
    "Architecture",
    "VVayToyekChinesePavilion",
    "Derived",
    "Normalized",
    "SM_VDT_Pavilion_Hero.fbx",
)
DESTINATION = "/Game/TruongSinh/Environment/VanDaoTong/Architecture"

if not os.path.isfile(SOURCE):
    raise RuntimeError(f"Missing Blender-derived pavilion: {SOURCE}")

unreal.EditorAssetLibrary.make_directory(DESTINATION)
task = unreal.AssetImportTask()
task.set_editor_property("filename", SOURCE)
task.set_editor_property("destination_path", DESTINATION)
task.set_editor_property("destination_name", "SM_VDT_Pavilion_Hero")
task.set_editor_property("automated", True)
task.set_editor_property("replace_existing", True)
task.set_editor_property("save", True)
options = unreal.FbxImportUI()
options.set_editor_property("import_mesh", True)
options.set_editor_property("import_as_skeletal", False)
options.static_mesh_import_data.set_editor_property("combine_meshes", True)
options.static_mesh_import_data.set_editor_property("import_uniform_scale", 1.0)
options.static_mesh_import_data.set_editor_property("generate_lightmap_u_vs", True)
options.static_mesh_import_data.set_editor_property("auto_generate_collision", True)
task.set_editor_property("options", options)
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

imported = task.get_editor_property("imported_object_paths")
if not imported:
    raise RuntimeError("Unreal imported no pavilion assets.")
mesh_path = f"{DESTINATION}/SM_VDT_Pavilion_Hero.SM_VDT_Pavilion_Hero"
mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
if not mesh:
    raise RuntimeError(f"Imported mesh missing: {mesh_path}")

try:
    mesh.nanite_settings.enabled = True
    unreal.StaticMeshEditorSubsystem().set_lods_with_notification(mesh, 1, None)
except Exception as exc:
    unreal.log_warning(f"Nanite/LOD configuration deferred: {exc}")
unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False)
unreal.log(f"VANDAO pavilion import PASS: {mesh_path}; objects={len(imported)}")
