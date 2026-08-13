"""Import the approved Poly Haven 2K PBR kit and build UE materials."""

import os
import unreal


SOURCE_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "SourceArt", "Environment", "PolyHaven"))
TEXTURE_PATH = "/Game/VisualTarget/PBR/Textures"
MATERIAL_PATH = "/Game/VisualTarget/PBR/Materials"
SETS = {
    "StoneTiles": ("stone_tiles", 5.0),
    "RoofTiles": ("roof_tiles_14", 2.5),
    "OldWood": ("wooden_planks", 2.0),
    "ClayPlaster": ("clay_plaster", 3.0),
}


def import_texture(filename, destination_name):
    destination = f"{TEXTURE_PATH}/{destination_name}"
    existing = unreal.EditorAssetLibrary.load_asset(destination) if unreal.EditorAssetLibrary.does_asset_exist(destination) else None
    if existing:
        return existing
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", os.path.join(SOURCE_ROOT, filename))
    task.set_editor_property("destination_path", TEXTURE_PATH)
    task.set_editor_property("destination_name", destination_name)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("save", True)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    result = unreal.EditorAssetLibrary.load_asset(destination)
    if not result:
        raise RuntimeError(f"Failed to import {filename}")
    return result


def configure_texture(texture, kind):
    if kind == "normal":
        texture.set_editor_property("srgb", False)
        texture.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_NORMALMAP)
    elif kind == "arm":
        texture.set_editor_property("srgb", False)
        texture.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_MASKS)
    unreal.EditorAssetLibrary.save_loaded_asset(texture, only_if_is_dirty=False)


def build_material(display_name, source_id, tiling):
    material_name = f"M_PBR_{display_name}"
    material_path = f"{MATERIAL_PATH}/{material_name}"
    material = unreal.EditorAssetLibrary.load_asset(material_path) if unreal.EditorAssetLibrary.does_asset_exist(material_path) else None
    if not material:
        material = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
            material_name, MATERIAL_PATH, unreal.Material, unreal.MaterialFactoryNew()
        )
    if not material:
        raise RuntimeError(f"Failed to create {material_path}")

    unreal.MaterialEditingLibrary.delete_all_material_expressions(material)
    diffuse = import_texture(f"{source_id}_diff_2k.jpg", f"T_{display_name}_D")
    normal = import_texture(f"{source_id}_nor_dx_2k.jpg", f"T_{display_name}_N")
    arm = import_texture(f"{source_id}_arm_2k.jpg", f"T_{display_name}_ARM")
    configure_texture(normal, "normal")
    configure_texture(arm, "arm")

    coordinate = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionTextureCoordinate, -720, 40
    )
    coordinate.set_editor_property("u_tiling", tiling)
    coordinate.set_editor_property("v_tiling", tiling)

    diffuse_sample = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionTextureSample, -450, -170
    )
    diffuse_sample.set_editor_property("texture", diffuse)
    normal_sample = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionTextureSample, -450, 20
    )
    normal_sample.set_editor_property("texture", normal)
    normal_sample.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
    arm_sample = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionTextureSample, -450, 230
    )
    arm_sample.set_editor_property("texture", arm)
    arm_sample.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_MASKS)

    for sample in (diffuse_sample, normal_sample, arm_sample):
        unreal.MaterialEditingLibrary.connect_material_expressions(coordinate, "", sample, "UVs")
    unreal.MaterialEditingLibrary.connect_material_property(diffuse_sample, "RGB", unreal.MaterialProperty.MP_BASE_COLOR)
    unreal.MaterialEditingLibrary.connect_material_property(normal_sample, "RGB", unreal.MaterialProperty.MP_NORMAL)
    unreal.MaterialEditingLibrary.connect_material_property(arm_sample, "R", unreal.MaterialProperty.MP_AMBIENT_OCCLUSION)
    unreal.MaterialEditingLibrary.connect_material_property(arm_sample, "G", unreal.MaterialProperty.MP_ROUGHNESS)
    unreal.MaterialEditingLibrary.connect_material_property(arm_sample, "B", unreal.MaterialProperty.MP_METALLIC)
    unreal.MaterialEditingLibrary.recompile_material(material)
    unreal.EditorAssetLibrary.save_loaded_asset(material, only_if_is_dirty=False)
    return material


unreal.EditorAssetLibrary.make_directory(TEXTURE_PATH)
unreal.EditorAssetLibrary.make_directory(MATERIAL_PATH)
for name, (source, tiling) in SETS.items():
    build_material(name, source, tiling)
unreal.log("Poly Haven environment import PASS: 4 PBR materials, 12 textures")
