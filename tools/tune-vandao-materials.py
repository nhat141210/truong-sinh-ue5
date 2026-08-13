"""Tune the reusable visual materials in the already-open UE5 editor.

This deliberately edits existing assets in place so level actor references stay
valid and the persistent editor session does not need to be restarted.
"""

import unreal


def node(material, cls, x, y):
    return unreal.MaterialEditingLibrary.create_material_expression(material, cls, x, y)


def connect(material, source, output, target, input_name):
    if target == material:
        properties = {
            "Base Color": unreal.MaterialProperty.MP_BASE_COLOR,
            "Normal": unreal.MaterialProperty.MP_NORMAL,
            "Roughness": unreal.MaterialProperty.MP_ROUGHNESS,
            "Metallic": unreal.MaterialProperty.MP_METALLIC,
            "Specular": unreal.MaterialProperty.MP_SPECULAR,
        }
        unreal.MaterialEditingLibrary.connect_material_property(source, output, properties[input_name])
    else:
        unreal.MaterialEditingLibrary.connect_material_expressions(source, output, target, input_name)


def tune_foliage(path):
    material = unreal.EditorAssetLibrary.load_asset(path)
    if not material:
        raise RuntimeError(f"Missing foliage material: {path}")
    # Default Lit keeps alpha-masked leaves readable under the scene's warm sun
    # and avoids the blue subsurface response of the foliage shading model.
    material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_MASKED)
    material.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_DEFAULT_LIT)
    material.set_editor_property("two_sided", True)
    material.set_editor_property("opacity_mask_clip_value", 0.32)
    material.modify()
    unreal.MaterialEditingLibrary.recompile_material(material)
    unreal.EditorAssetLibrary.save_loaded_asset(material, only_if_is_dirty=False)


def tune_water():
    path = "/Game/VisualTarget/Materials/M_VT_QuietWater"
    material = unreal.EditorAssetLibrary.load_asset(path)
    if not material:
        raise RuntimeError(f"Missing water material: {path}")
    normal = unreal.EditorAssetLibrary.load_asset(
        "/Engine/Functions/Engine_MaterialFunctions02/ExampleContent/Textures/water_n"
    )
    diffuse = unreal.EditorAssetLibrary.load_asset(
        "/Engine/Functions/Engine_MaterialFunctions02/ExampleContent/Textures/water_d"
    )
    if not normal or not diffuse:
        raise RuntimeError("UE5 example water textures are unavailable")

    unreal.MaterialEditingLibrary.delete_all_material_expressions(material)

    texcoord = node(material, unreal.MaterialExpressionTextureCoordinate, -1000, 0)
    texcoord.set_editor_property("u_tiling", 8.0)
    texcoord.set_editor_property("v_tiling", 8.0)

    normal_sample = node(material, unreal.MaterialExpressionTextureSample, -520, -20)
    normal_sample.set_editor_property("texture", normal)
    normal_sample.set_editor_property("sampler_type", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
    connect(material, texcoord, "", normal_sample, "UVs")
    flat_normal = node(material, unreal.MaterialExpressionConstant3Vector, -520, -180)
    flat_normal.set_editor_property("constant", unreal.LinearColor(0.5, 0.5, 1.0, 1.0))
    normal_blend = node(material, unreal.MaterialExpressionLinearInterpolate, -180, -20)
    normal_amount = node(material, unreal.MaterialExpressionConstant, -520, -300)
    normal_amount.set_editor_property("r", 0.22)
    connect(material, flat_normal, "", normal_blend, "A")
    connect(material, normal_sample, "RGB", normal_blend, "B")
    connect(material, normal_amount, "", normal_blend, "Alpha")
    connect(material, normal_blend, "", material, "Normal")

    base = node(material, unreal.MaterialExpressionConstant3Vector, -220, 250)
    base.set_editor_property("constant", unreal.LinearColor(0.012, 0.055, 0.065, 1.0))
    connect(material, base, "", material, "Base Color")

    roughness = node(material, unreal.MaterialExpressionConstant, -180, 500)
    roughness.set_editor_property("r", 0.18)
    connect(material, roughness, "", material, "Roughness")
    metallic = node(material, unreal.MaterialExpressionConstant, -180, 590)
    metallic.set_editor_property("r", 0.04)
    connect(material, metallic, "", material, "Metallic")
    specular = node(material, unreal.MaterialExpressionConstant, -180, 680)
    specular.set_editor_property("r", 0.78)
    connect(material, specular, "", material, "Specular")

    material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_OPAQUE)
    material.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_DEFAULT_LIT)
    material.set_editor_property("two_sided", False)
    material.modify()
    unreal.MaterialEditingLibrary.recompile_material(material)
    unreal.EditorAssetLibrary.save_loaded_asset(material, only_if_is_dirty=False)


for foliage_path in (
    "/Game/VisualTarget/Nature/Materials/M_fern_02",
    "/Game/VisualTarget/Nature/Materials/M_pine_sapling_small_bark",
    "/Game/VisualTarget/Nature/Materials/M_pine_sapling_small_twig",
    "/Game/VisualTarget/Nature/Materials/M_shrub_02",
):
    tune_foliage(foliage_path)
tune_water()
actor_system = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for actor in actor_system.get_all_level_actors():
    if actor.get_actor_label() in ("VT_Pine_00", "VT_Pine_03"):
        actor.set_actor_hidden_in_game(True)
        actor.set_is_temporarily_hidden_in_editor(True)
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).save_current_level()
unreal.log("VANDAO material tuning PASS: foliage=DefaultLit masked; water=normal+panning")
