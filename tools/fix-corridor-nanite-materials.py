"""Enable the Nanite shader permutation on corridor materials used by Nanite meshes."""

import unreal


MATERIALS = (
    "/Game/VisualTarget/PBR/Materials/M_PBR_StoneTiles",
    "/Game/VisualTarget/PBR/Materials/M_PBR_ClayPlaster",
    "/Game/VisualTarget/PBR/Materials/M_PBR_OldWood",
    "/Game/VisualTarget/PBR/Materials/M_PBR_RoofTiles",
    "/Game/VisualTarget/Materials/M_VT_AgedBrass",
    "/Game/VisualTarget/Materials/M_VT_DyedCloth",
    "/Game/VisualTarget/Materials/M_VT_RestrainedJade",
)

for path in MATERIALS:
    material = unreal.EditorAssetLibrary.load_asset(path)
    if not isinstance(material, unreal.Material):
        raise RuntimeError(f"Missing corridor material: {path}")
    material.set_editor_property("used_with_nanite", True)
    unreal.MaterialEditingLibrary.recompile_material(material)
    unreal.EditorAssetLibrary.save_loaded_asset(material, only_if_is_dirty=False)
    unreal.log(f"CORRIDOR_NANITE_FIX material={path}")

unreal.log(f"Corridor Nanite material fix PASS: materials={len(MATERIALS)}")
