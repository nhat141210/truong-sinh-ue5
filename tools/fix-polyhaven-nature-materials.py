"""Build production UE materials for imported Poly Haven nature assets."""

import unreal


ROOT = "/Game/VisualTarget/Nature"
FIXED = ROOT + "/Materials"

SPECS = {
    "fern_02": ("fern_02/fern_02_1k/Textures/fern_02_diff-fern_02_alpha", "fern_02/fern_02_1k/Textures/fern_02_nor_gl", "fern_02/fern_02_1k/Textures/fern_02_rough", True),
    "pine_sapling_small_bark": ("pine_sapling_small/pine_sapling_small_1k/Textures/pine_sapling_small_bark_diff", "pine_sapling_small/pine_sapling_small_1k/Textures/pine_sapling_small_bark_nor_gl", "pine_sapling_small/pine_sapling_small_1k/Textures/pine_sapling_small_bark_rough", False),
    "pine_sapling_small_twig": ("pine_sapling_small/pine_sapling_small_1k/Textures/pine_sapling_small_twig_diff", "pine_sapling_small/pine_sapling_small_1k/Textures/pine_sapling_small_twig_nor_gl", "pine_sapling_small/pine_sapling_small_1k/Textures/pine_sapling_small_twig_rough", True),
    "shrub_02": ("shrub_02/shrub_02_1k/Textures/shrub_02_diff", "shrub_02/shrub_02_1k/Textures/shrub_02_nor_gl", "shrub_02/shrub_02_1k/Textures/shrub_02_rough", True),
    "rock_face_01": ("rock_face_01/rock_face_01_2k/Textures/rock_face_01_diff", "rock_face_01/rock_face_01_2k/Textures/rock_face_01_nor_gl", "rock_face_01/rock_face_01_2k/Textures/rock_face_01_rough", False),
    "rock_moss_set_01": ("rock_moss_set_01/rock_moss_set_01_2k/Textures/rock_moss_set_01_diff", "rock_moss_set_01/rock_moss_set_01_2k/Textures/rock_moss_set_01_nor_gl", "rock_moss_set_01/rock_moss_set_01_2k/Textures/rock_moss_set_01_rough", False),
}


def load_texture(relative):
    texture = unreal.EditorAssetLibrary.load_asset(f"{ROOT}/{relative}")
    if not isinstance(texture, unreal.Texture2D):
        raise RuntimeError(f"Missing texture: {relative}")
    return texture


def rebuild_material(name, spec):
    path = f"{FIXED}/M_{name}"
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        unreal.EditorAssetLibrary.delete_asset(path)
    material = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        f"M_{name}", FIXED, unreal.Material, unreal.MaterialFactoryNew()
    )
    diffuse, normal, roughness, masked = spec
    for x, relative, prop, pin in (
        (-500, diffuse, unreal.MaterialProperty.MP_BASE_COLOR, "RGB"),
        (-500, normal, unreal.MaterialProperty.MP_NORMAL, "RGB"),
        (-500, roughness, unreal.MaterialProperty.MP_ROUGHNESS, "R"),
    ):
        node = unreal.MaterialEditingLibrary.create_material_expression(
            material, unreal.MaterialExpressionTextureSample, x, 0
        )
        node.set_editor_property("texture", load_texture(relative))
        unreal.MaterialEditingLibrary.connect_material_property(node, pin, prop)
        if prop == unreal.MaterialProperty.MP_BASE_COLOR and masked:
            unreal.MaterialEditingLibrary.connect_material_property(
                node, "A", unreal.MaterialProperty.MP_OPACITY_MASK
            )
    if masked:
        material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_MASKED)
        material.set_editor_property("two_sided", True)
        material.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_TWO_SIDED_FOLIAGE)
        material.set_editor_property("opacity_mask_clip_value", 0.333)
    else:
        # Forces the shader permutation needed by Nanite rock meshes.
        try:
            material.set_editor_property("used_with_nanite", True)
        except Exception as exc:
            raise RuntimeError(f"Cannot enable Nanite material usage for {path}: {exc}")
    unreal.MaterialEditingLibrary.recompile_material(material)
    unreal.EditorAssetLibrary.save_loaded_asset(material, only_if_is_dirty=False)
    return material


unreal.EditorAssetLibrary.make_directory(FIXED)
fixed = {name: rebuild_material(name, spec) for name, spec in SPECS.items()}

registry = unreal.AssetRegistryHelpers.get_asset_registry()
registry.scan_paths_synchronous([ROOT], force_rescan=True)
meshes = []
for data in registry.get_assets_by_path(ROOT, recursive=True):
    mesh = data.get_asset()
    if not isinstance(mesh, unreal.StaticMesh):
        continue
    meshes.append(mesh)
    path = mesh.get_path_name().lower()
    foliage = any(token in path for token in ("/fern_02/", "/pine_sapling_small/", "/shrub_02/"))
    nanite = mesh.get_editor_property("nanite_settings")
    nanite.set_editor_property("enabled", not foliage)
    mesh.set_editor_property("nanite_settings", nanite)
    for slot in range(mesh.get_num_sections(0)):
        old = mesh.get_material(slot)
        old_name = old.get_name() if old else ""
        key = next((candidate for candidate in SPECS if candidate == old_name), None)
        if key is None:
            key = next((candidate for candidate in SPECS if candidate in path), None)
        if key is None:
            raise RuntimeError(f"Cannot map material slot {slot} on {mesh.get_path_name()}")
        mesh.set_material(slot, fixed[key])
    mesh.modify()
    unreal.EditorAssetLibrary.save_loaded_asset(mesh, only_if_is_dirty=False)
    unreal.log(f"PH_FIX mesh={mesh.get_path_name()} nanite={not foliage}")

unreal.log(f"Poly Haven nature material fix PASS: materials={len(fixed)}, meshes={len(meshes)}")
