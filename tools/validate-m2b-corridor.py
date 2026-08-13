"""Read-only structural validation for /Game/Maps/VisualTarget/L_M2B_Corridor."""

import unreal


MAP_PACKAGE = "/Game/Maps/VisualTarget/L_M2B_Corridor"


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

require(unreal.EditorAssetLibrary.does_asset_exist(MAP_PACKAGE), f"Missing {MAP_PACKAGE}")
require(level_subsystem.load_level(MAP_PACKAGE), f"Could not load {MAP_PACKAGE}")

actors = actor_subsystem.get_all_level_actors()
labels = {actor.get_actor_label(): actor for actor in actors}

for required_label in (
    "VT_PlayerStart",
    "VT_MainGate_Lintel",
    "VT_MainGate_JadeDisc",
    "VT_VistaCliff_00",
    "VT_AncestorRockFace",
    "VT_CultivationSite",
    "VT_BreakthroughSite",
    "VT_AlchemySite",
    "VT_FormationSite",
    "VT_ConflictSite",
    "VT_Sun",
    "VT_SkyLight",
    "VT_SkyAtmosphere",
    "VT_HeightFog",
    "VT_PostProcess",
):
    require(required_label in labels, f"Missing required actor {required_label}")

static_mesh_count = sum(isinstance(actor, unreal.StaticMeshActor) for actor in actors)
local_light_count = sum(
    isinstance(actor, unreal.PointLight) and actor.get_actor_label().startswith("VT_WarmLamp_")
    for actor in actors
)
material_count = len(unreal.EditorAssetLibrary.list_assets("/Game/VisualTarget/Materials", recursive=False))

require(static_mesh_count >= 100, f"Expected dense corridor dressing, found {static_mesh_count} static meshes")
require(local_light_count == 3, f"Expected exactly 3 local lights, found {local_light_count}")
require(material_count == 14, f"Expected 14 corridor materials, found {material_count}")

require(
    "VT_NPC_Caretaker_Temporary" not in labels,
    "Temporary white mannequin must not ship in the visual-target corridor",
)

unreal.log(
    "M2B corridor validation PASS: "
    f"actors={len(actors)}, static_meshes={static_mesh_count}, "
    f"local_lights={local_light_count}, materials={material_count}, placeholder_npc=none"
)
