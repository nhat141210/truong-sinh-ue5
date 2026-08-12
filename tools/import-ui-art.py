from pathlib import Path

import unreal


project_root = Path(unreal.Paths.project_dir()).resolve()
source_file = project_root / "SourceArt" / "UI" / "Generated" / "T_UI_JadeFrame.png"
destination_path = "/Game/UI/Generated"
asset_path = f"{destination_path}/T_UI_JadeFrame"

if not source_file.is_file():
    raise RuntimeError(f"Missing generated UI source: {source_file}")

task = unreal.AssetImportTask()
task.set_editor_property("automated", True)
task.set_editor_property("destination_name", "T_UI_JadeFrame")
task.set_editor_property("destination_path", destination_path)
task.set_editor_property("filename", str(source_file))
task.set_editor_property("replace_existing", True)
task.set_editor_property("save", True)
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

texture = unreal.EditorAssetLibrary.load_asset(asset_path)
if not texture:
    raise RuntimeError(f"UI texture import did not create {asset_path}")

# UI art must retain its alpha and avoid streaming/mip shimmer at HUD scale.
texture.set_editor_property("srgb", True)
if hasattr(unreal.TextureCompressionSettings, "TC_USER_INTERFACE2D"):
    texture.set_editor_property(
        "compression_settings", unreal.TextureCompressionSettings.TC_USER_INTERFACE2D
    )
if hasattr(unreal.TextureGroup, "TEXTUREGROUP_UI"):
    texture.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_UI)
if hasattr(unreal.TextureMipGenSettings, "TMGS_NO_MIPMAPS"):
    texture.set_editor_property(
        "mip_gen_settings", unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS
    )

unreal.EditorAssetLibrary.save_loaded_asset(texture, only_if_is_dirty=False)
unreal.log(f"Imported generated UI frame: {asset_path}")
