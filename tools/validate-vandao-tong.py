"""Structural and transform validation for the Vấn Đạo Tông visual map."""

import unreal


MAP = "/Game/Maps/VisualTarget/L_VanDaoTong"


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


level = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
assets = unreal.EditorAssetLibrary
require(assets.does_asset_exist(MAP), f"Missing {MAP}")
require(level.load_level(MAP), f"Could not load {MAP}")

all_actors = actors.get_all_level_actors()
labels = {actor.get_actor_label(): actor for actor in all_actors}
for label in (
    "VDT_MasterEstate",
    "VDT_Arrival_MountainGate",
    "VDT_MainHall_Facade",
    "VDT_MainHall_Pavilion",
    "VDT_WestTeaPavilion",
    "VDT_EastTeaPavilion",
    "VDT_VolumetricCloud",
    "VT_PlayerStart",
    "VT_CultivationSite",
    "VT_BreakthroughSite",
    "VT_AlchemySite",
    "VT_FormationSite",
    "VT_ConflictSite",
):
    require(label in labels, f"Missing required Vấn Đạo actor: {label}")

require("VT_Ground" not in labels, "Rectangular M2B graybox foundation must be replaced")
require(
    sum(label.startswith("VDT_VistaIsland_") for label in labels) == 3,
    "Expected exactly three non-gameplay vista islands",
)
require(
    sum(label.startswith("VDT_SectStele_") for label in labels) == 6,
    "Expected six sect steles",
)
require(
    sum(label.startswith("VDT_StoneLantern_") for label in labels) == 6,
    "Expected six grounded stone lanterns",
)
require(
    sum(label.startswith("VDT_SiteShell_") for label in labels) == 5,
    "Expected one original-art shell for each of five native gameplay sites",
)
require(
    sum(label.startswith("VDT_BridgeRail_") for label in labels) == 8,
    "Expected eight authored bridge-railing segments",
)
require(
    sum(label.startswith("VDT_GardenPlant_") for label in labels) == 6,
    "Expected six scanned garden plant clusters",
)
estate_mesh = unreal.load_asset("/Game/TruongSinh/Environment/VanDaoTong/Terrain/SM_VDT_MasterEstate")
estate_bounds = estate_mesh.get_bounding_box()
estate_dimensions = estate_bounds.max - estate_bounds.min
require(29500 <= estate_dimensions.x <= 30500, f"Master-estate width contract failed: {estate_dimensions}")
require(20500 <= estate_dimensions.y <= 21500, f"Master-estate depth contract failed: {estate_dimensions}")
require(1200 <= estate_dimensions.z <= 2200, f"Master-estate cliff contract failed: {estate_dimensions}")

for path in (
    "/Game/TruongSinh/Environment/VanDaoTong/Props/SM_VDT_StoneStele",
    "/Game/TruongSinh/Environment/VanDaoTong/Props/SM_VDT_StoneLantern",
    "/Game/TruongSinh/Environment/VanDaoTong/Props/SM_VDT_RitualBrazier",
    "/Game/TruongSinh/Environment/VanDaoTong/Water/SM_VDT_ArchedFootbridge_600",
    "/Game/TruongSinh/Environment/VanDaoTong/Water/SM_VDT_LotusPadCluster_A",
    "/Game/TruongSinh/Environment/VanDaoTong/Terrain/SM_VDT_MasterEstate",
):
    mesh = unreal.load_asset(path)
    require(mesh is not None, f"Missing validated VDT mesh: {path}")
    bounds = mesh.get_bounding_box()
    require(abs(bounds.min.z) <= 0.1, f"Mesh is not grounded at local min-Z=0: {path}: {bounds.min}")
    center_x = (bounds.min.x + bounds.max.x) * 0.5
    center_y = (bounds.min.y + bounds.max.y) * 0.5
    require(abs(center_x) <= 0.1 and abs(center_y) <= 0.1, f"Mesh XY pivot is not centred: {path}")

unreal.log(
    "VANDAO MAP VALIDATION PASS: "
    f"actors={len(all_actors)}; master_estate={estate_dimensions}; vista_islands=3; native_sites=5; grounded_props=5"
)
