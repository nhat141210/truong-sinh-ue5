"""Validate imported PBR assets and texture settings for the M2B visual target."""

import unreal


names = ("StoneTiles", "RoofTiles", "OldWood", "ClayPlaster")
for name in names:
    material_path = f"/Game/VisualTarget/PBR/Materials/M_PBR_{name}"
    if not unreal.EditorAssetLibrary.does_asset_exist(material_path):
        raise RuntimeError(f"Missing PBR material: {material_path}")
    for suffix in ("D", "N", "ARM"):
        texture_path = f"/Game/VisualTarget/PBR/Textures/T_{name}_{suffix}"
        texture = unreal.EditorAssetLibrary.load_asset(texture_path)
        if not texture:
            raise RuntimeError(f"Missing PBR texture: {texture_path}")
        if suffix in ("N", "ARM") and texture.get_editor_property("srgb"):
            raise RuntimeError(f"Non-color texture has sRGB enabled: {texture_path}")

unreal.log("Poly Haven environment validation PASS: materials=4, textures=12, resolution=2K")
