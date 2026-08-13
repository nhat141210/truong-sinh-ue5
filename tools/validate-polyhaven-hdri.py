"""Validate the imported Ninomaru Teien HDRI and UE texture settings."""

import os
import unreal


ASSET_PATH = "/Game/VisualTarget/HDRI/TC_NinomaruTeien_4K"
EXPECTED_SOURCE = os.path.normcase(
    os.path.abspath(
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
)

asset = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
if not asset:
    raise RuntimeError(f"Missing HDRI asset: {ASSET_PATH}")
if not isinstance(asset, unreal.TextureCube):
    raise RuntimeError(f"Expected TextureCube, got {asset.get_class().get_name()}")
if asset.get_editor_property("srgb"):
    raise RuntimeError("HDRI TextureCube must not use sRGB sampling")
compression = asset.get_editor_property("compression_settings")
if compression not in (
    unreal.TextureCompressionSettings.TC_HDR,
    unreal.TextureCompressionSettings.TC_HDR_COMPRESSED,
    unreal.TextureCompressionSettings.TC_HDR_F32,
):
    raise RuntimeError(f"HDRI TextureCube must use an HDR compression mode, got {compression}")
lod_group = asset.get_editor_property("lod_group")
if lod_group not in (
    unreal.TextureGroup.TEXTUREGROUP_SKYBOX,
    unreal.TextureGroup.TEXTUREGROUP_WORLD,
):
    raise RuntimeError(f"HDRI TextureCube has an unexpected LOD group: {lod_group}")

import_data = asset.get_editor_property("asset_import_data")
source_file = os.path.normcase(os.path.abspath(import_data.get_first_filename()))
if source_file != EXPECTED_SOURCE:
    raise RuntimeError(f"Unexpected HDRI import source: {source_file}")

unreal.log(
    "Poly Haven HDRI validation PASS: "
    f"asset={ASSET_PATH}, class=TextureCube, compression={compression}, lod_group={lod_group}, source=4K HDR"
)
