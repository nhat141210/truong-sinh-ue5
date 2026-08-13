"""Validate the fixed Poly Haven material/mesh contract."""

import unreal


ROOT = "/Game/VisualTarget/Nature"
registry = unreal.AssetRegistryHelpers.get_asset_registry()
registry.scan_paths_synchronous([ROOT], force_rescan=True)
mesh_count = 0

for data in registry.get_assets_by_path(ROOT, recursive=True):
    mesh = data.get_asset()
    if not isinstance(mesh, unreal.StaticMesh):
        continue
    mesh_count += 1
    path = mesh.get_path_name().lower()
    foliage = any(token in path for token in ("/fern_02/", "/pine_sapling_small/", "/shrub_02/"))
    nanite = mesh.get_editor_property("nanite_settings").get_editor_property("enabled")
    if nanite == foliage:
        raise RuntimeError(f"Wrong Nanite policy: {mesh.get_path_name()} nanite={nanite}")
    for slot in range(mesh.get_num_sections(0)):
        material = mesh.get_material(slot)
        if not material or not material.get_path_name().startswith(ROOT + "/Materials/M_"):
            raise RuntimeError(f"Mesh does not use fixed material: {mesh.get_path_name()} slot={slot}")
        material_path = material.get_path_name().lower()
        masked_foliage = any(
            token in material_path
            for token in ("m_fern_02", "m_pine_sapling_small_twig", "m_shrub_02")
        )
        if masked_foliage:
            if material.get_editor_property("blend_mode") != unreal.BlendMode.BLEND_MASKED:
                raise RuntimeError(f"Foliage material is not masked: {material.get_path_name()}")
            if not material.get_editor_property("two_sided"):
                raise RuntimeError(f"Foliage material is not two-sided: {material.get_path_name()}")
        elif not foliage and not material.get_editor_property("used_with_nanite"):
            raise RuntimeError(f"Rock material lacks Nanite usage: {material.get_path_name()}")

if mesh_count != 18:
    raise RuntimeError(f"Expected 18 nature meshes, got {mesh_count}")
unreal.log("Poly Haven nature material validation PASS: meshes=18")
