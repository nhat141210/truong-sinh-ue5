"""Import the project-owned Vấn Đạo Tông island into the persistent Editor."""

import os
import unreal


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SOURCE = os.path.join(ROOT, "SourceArt", "Architecture", "VanDaoIsland", "SM_VDT_FloatingIsland.fbx")
DESTINATION = "/Game/TruongSinh/Environment/VanDaoTong/Terrain"
NAME = "SM_VDT_FloatingIsland"

if not os.path.isfile(SOURCE):
    raise RuntimeError(f"Missing generated island: {SOURCE}")

unreal.EditorAssetLibrary.make_directory(DESTINATION)
task = unreal.AssetImportTask()
task.set_editor_property("filename", SOURCE)
task.set_editor_property("destination_path", DESTINATION)
task.set_editor_property("destination_name", NAME)
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

path = f"{DESTINATION}/{NAME}.{NAME}"
mesh = unreal.EditorAssetLibrary.load_asset(path)
if not mesh:
    raise RuntimeError(f"Island import failed: {path}")
bounds = mesh.get_bounding_box()
dimensions = bounds.max - bounds.min
if not (8900 <= dimensions.x <= 9100 and 2800 <= dimensions.y <= 3100 and 800 <= dimensions.z <= 1300):
    raise RuntimeError(f"Island unit/bounds contract failed: {dimensions}")
unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False)
unreal.log(f"VDT FLOATING ISLAND IMPORT PASS: {path}; dimensions={dimensions}")
