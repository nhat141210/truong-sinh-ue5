"""Import the approved Poly Haven Ninomaru Teien HDRI as a UE TextureCube."""

import os
import unreal


SOURCE_FILE = os.path.abspath(
    os.path.join(
        os.path.dirname(__file__),
        "..",
        "SourceArt",
        "Environment",
        "PolyHaven",
        "hdri",
        "ninomaru_teien_4k.hdr",
    )
)
DESTINATION_PATH = "/Game/VisualTarget/HDRI"
DESTINATION_NAME = "TC_NinomaruTeien_4K"
ASSET_PATH = f"{DESTINATION_PATH}/{DESTINATION_NAME}"


if not os.path.isfile(SOURCE_FILE):
    raise RuntimeError(f"Missing approved HDRI source: {SOURCE_FILE}")

unreal.EditorAssetLibrary.make_directory(DESTINATION_PATH)
task = unreal.AssetImportTask()
task.set_editor_property("filename", SOURCE_FILE)
task.set_editor_property("destination_path", DESTINATION_PATH)
task.set_editor_property("destination_name", DESTINATION_NAME)
task.set_editor_property("automated", True)
task.set_editor_property("replace_existing", True)
task.set_editor_property("save", True)
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

asset = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
if not asset:
    raise RuntimeError(f"HDRI import did not create {ASSET_PATH}")
if not isinstance(asset, unreal.TextureCube):
    raise RuntimeError(f"Expected TextureCube at {ASSET_PATH}, got {asset.get_class().get_name()}")

asset.set_editor_property("srgb", False)
asset.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_HDR)
asset.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_SKYBOX)
asset.set_editor_property("max_texture_size", 4096)
asset.post_edit_change()
unreal.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False)
unreal.log(f"Poly Haven HDRI import PASS: {ASSET_PATH} class=TextureCube source=4K HDR")
