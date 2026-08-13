"""Create the isolated Vấn Đạo Tông visual map from the validated M2B route.

The native activity anchors are copied intact; this script only replaces the
presentation layer and records reproducible camera markers.
"""

import unreal


SOURCE_MAP = "/Game/Maps/VisualTarget/L_M2B_Corridor"
MAP = "/Game/Maps/VisualTarget/L_VanDaoTong"
PAVILION = "/Game/TruongSinh/Environment/VanDaoTong/Architecture/SM_VDT_Pavilion_Hero.SM_VDT_Pavilion_Hero"
GATE = "/Game/TruongSinh/Environment/VanDaoTong/Architecture/SM_VDT_MountainGate.SM_VDT_MountainGate"
HALL = "/Game/TruongSinh/Environment/VanDaoTong/Architecture/SM_VDT_MainHallFacade.SM_VDT_MainHallFacade"
STEELE = "/Game/TruongSinh/Environment/VanDaoTong/Props/SM_VDT_StoneStele.SM_VDT_StoneStele"
LANTERN = "/Game/TruongSinh/Environment/VanDaoTong/Props/SM_VDT_StoneLantern.SM_VDT_StoneLantern"
BRAZIER = "/Game/TruongSinh/Environment/VanDaoTong/Props/SM_VDT_RitualBrazier.SM_VDT_RitualBrazier"
BRIDGE = "/Game/TruongSinh/Environment/VanDaoTong/Water/SM_VDT_ArchedFootbridge_600.SM_VDT_ArchedFootbridge_600"
RAILING = "/Game/TruongSinh/Environment/VanDaoTong/Props/SM_VDT_BridgeRailing_400.SM_VDT_BridgeRailing_400"
LOTUS = "/Game/TruongSinh/Environment/VanDaoTong/Water/SM_VDT_LotusPadCluster_A.SM_VDT_LotusPadCluster_A"
SHORE = "/Game/TruongSinh/Environment/VanDaoTong/Water/SM_VDT_PondShore_Curve_Quarter.SM_VDT_PondShore_Curve_Quarter"
STEPPING_STONES = "/Game/TruongSinh/Environment/VanDaoTong/Water/SM_VDT_SteppingStoneCluster_A.SM_VDT_SteppingStoneCluster_A"
ISLAND = "/Game/TruongSinh/Environment/VanDaoTong/Terrain/SM_VDT_FloatingIsland.SM_VDT_FloatingIsland"
MASTER_ESTATE = "/Game/TruongSinh/Environment/VanDaoTong/Terrain/SM_VDT_MasterEstate.SM_VDT_MasterEstate"
CUBE = "/Engine/BasicShapes/Cube.Cube"
SHRUBS = tuple(
    f"/Game/VisualTarget/Nature/shrub_02/shrub_02_1k/StaticMeshes/shrub_02_{variant}"
    for variant in ("a", "b", "c", "d")
)
FERNS = tuple(
    f"/Game/VisualTarget/Nature/fern_02/fern_02_1k/StaticMeshes/fern_02_{variant}"
    for variant in ("a", "b", "c", "d")
)
PINES = tuple(
    f"/Game/VisualTarget/Nature/pine_sapling_small/pine_sapling_small_1k/StaticMeshes/pine_sapling_small_{variant}"
    for variant in ("a", "b", "c")
)
ROCK_FACE = "/Game/VisualTarget/Nature/rock_face_01/rock_face_01_2k/StaticMeshes/rock_face_01_2k"
ROCKS = tuple(
    f"/Game/VisualTarget/Nature/rock_moss_set_01/rock_moss_set_01_2k/StaticMeshes/rock_moss_set_01_rock0{variant}"
    for variant in range(1, 7)
)


def set_if(obj, name, value):
    try:
        obj.set_editor_property(name, value)
    except Exception as exc:
        unreal.log_warning(f"{obj.get_name()}.{name}: {exc}")


def spawn_mesh(mesh, location, scale, label, rotation=(0, 0, 0), materials=(), collision=True):
    pitch, yaw, roll = rotation
    actor_rotation = unreal.Rotator(pitch=pitch, yaw=yaw, roll=roll)
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor, unreal.Vector(*location), actor_rotation
    )
    actor.set_actor_label(label)
    component = actor.get_component_by_class(unreal.StaticMeshComponent)
    component.set_static_mesh(mesh)
    component.set_collision_profile_name("BlockAll" if collision else "NoCollision")
    static_materials = mesh.get_editor_property("static_materials")
    for index, slot in enumerate(static_materials):
        slot_name = str(slot.get_editor_property("material_slot_name"))
        material = materials.get(slot_name) if isinstance(materials, dict) else (materials[index] if index < len(materials) else None)
        if material:
            component.set_material(index, material)
    actor.set_actor_scale3d(unreal.Vector(*scale))
    return actor


def spawn_camera(label, location, rotation):
    pitch, yaw, roll = rotation
    actor_rotation = unreal.Rotator(pitch=pitch, yaw=yaw, roll=roll)
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.CineCameraActor, unreal.Vector(*location), actor_rotation
    )
    actor.set_actor_label(label)
    component = actor.get_cine_camera_component()
    component.set_editor_property("current_focal_length", 52.0)
    component.set_editor_property("filmback", component.get_editor_property("filmback"))
    return actor


assets = unreal.EditorAssetLibrary
level = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if not assets.does_asset_exist(PAVILION):
    raise RuntimeError(f"Pavilion asset missing: {PAVILION}")
for required in (GATE, HALL, STEELE, LANTERN, BRAZIER, BRIDGE, RAILING, LOTUS, SHORE, STEPPING_STONES, ISLAND, MASTER_ESTATE):
    if not assets.does_asset_exist(required):
        raise RuntimeError(f"Original Vấn Đạo Tông kit asset missing: {required}")
if not assets.does_asset_exist(MAP):
    raise RuntimeError(
        f"Missing {MAP}. Run tools/rebuild-vandao-tong.ps1 to create the validated foundation first."
    )
current_world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
current_package = current_world.get_outermost().get_name() if current_world else ""
if current_package != MAP and not level.load_level(MAP):
    raise RuntimeError(f"Could not load {MAP}")

actor_system = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for actor in actor_system.get_all_level_actors():
    label = actor.get_actor_label()
    is_m2b_building = label.startswith("VT_B") and len(label) >= 6 and label[4:6].isdigit()
    is_m2b_placeholder = label.startswith(("VT_MainGate_", "VT_Lantern_")) or label in {
        "VT_Bell",
        "VT_BellPlinth",
        "VT_AncestorRockFace",
    }
    if label.startswith("VDT_") or label == "VT_Ground" or is_m2b_building or is_m2b_placeholder:
        actor_system.destroy_actor(actor)

pavilion = unreal.load_asset(PAVILION)
gate = unreal.load_asset(GATE)
hall = unreal.load_asset(HALL)
stele = unreal.load_asset(STEELE)
lantern = unreal.load_asset(LANTERN)
brazier = unreal.load_asset(BRAZIER)
bridge = unreal.load_asset(BRIDGE)
railing = unreal.load_asset(RAILING)
lotus = unreal.load_asset(LOTUS)
shore = unreal.load_asset(SHORE)
stepping_stones = unreal.load_asset(STEPPING_STONES)
island = unreal.load_asset(ISLAND)
master_estate = unreal.load_asset(MASTER_ESTATE)
pbr_stone = unreal.load_asset("/Game/VisualTarget/PBR/Materials/M_PBR_StoneTiles")
pbr_roof = unreal.load_asset("/Game/VisualTarget/PBR/Materials/M_PBR_RoofTiles")
pbr_wood = unreal.load_asset("/Game/VisualTarget/PBR/Materials/M_PBR_OldWood")
realm_grass = unreal.load_asset("/Game/VisualTarget/Materials/M_VT_RealmGrass")
quiet_water = unreal.load_asset("/Game/VisualTarget/Materials/M_VT_QuietWater")
brass = unreal.load_asset("/Game/VisualTarget/Materials/M_VT_AgedBrass")
jade = unreal.load_asset("/Game/VisualTarget/Materials/M_VT_RestrainedJade")
lacquer = unreal.load_asset("/Game/VisualTarget/Materials/M_VT_DyedCloth")
foliage = unreal.load_asset("/Game/VisualTarget/Materials/M_VT_PineDeep")
moss = unreal.load_asset("/Game/VisualTarget/Materials/M_VT_PineMoss")
shrubs = tuple(unreal.load_asset(path) for path in SHRUBS)
ferns = tuple(unreal.load_asset(path) for path in FERNS)
pines = tuple(unreal.load_asset(path) for path in PINES)
rock_face = unreal.load_asset(ROCK_FACE)
rocks = tuple(unreal.load_asset(path) for path in ROCKS)
required_materials = (pbr_stone, pbr_roof, pbr_wood, brass, jade, lacquer, foliage, moss, realm_grass, quiet_water, rock_face, *shrubs, *ferns, *pines, *rocks)
if not all(required_materials):
    raise RuntimeError("Vấn Đạo Tông material profile is incomplete")
materials = {
    "M_VDT_GoldTrim": brass,
    "M_VDT_Bronze": brass,
    "M_VDT_AgedBrass": brass,
    "M_VDT_AntiqueBrass": brass,
    "M_VDT_Bronze_Aged": brass,
    "M_VDT_Ember_Amber": brass,
    "M_VDT_DarkTimber": pbr_wood,
    "M_VDT_Redwood": pbr_wood,
    "M_VDT_AncientWood": pbr_wood,
    "M_VDT_DarkWood": pbr_wood,
    "M_VDT_WaterWood": pbr_wood,
    "M_VDT_RoofTile": pbr_roof,
    "M_VDT_DarkTile": pbr_roof,
    "M_VDT_RedLacquer": lacquer,
    "M_VDT_LacquerRed": lacquer,
    "M_VDT_StoneBase": pbr_stone,
    "M_VDT_AgedStone": pbr_stone,
    "M_VDT_Stone_Dark": pbr_stone,
    "M_VDT_WetStone": pbr_stone,
    "M_VDT_Stone_Jade": pbr_stone,
    "M_VDT_JadeStone": moss,
    "M_VDT_JadePlaque": jade,
    "M_VDT_LotusLeaf": foliage,
    "M_VDT_LotusFlower": lacquer,
    "M_VDT_Moss": moss,
    "M_VDT_IslandTop": pbr_stone,
    "M_VDT_IslandCliff": pbr_stone,
    "M_VDT_MasterEstateTop": realm_grass,
    "M_VDT_MasterEstateCliff": pbr_stone,
}

# Replace the rectangular foundation with a single irregular, tapered island.
# Its flat top preserves the authored route while the 8–12 m underside supplies
# the floating-realm silhouette in every wide shot.
spawn_mesh(master_estate, (3000, 0, -5), (1.0, 1.0, 1.0), "VDT_MasterEstate", materials=materials)

# Reserve eight quiet expansion parcels around the central route. These flush,
# non-colliding green pads keep future building locations stable without making
# the current scene look like a construction grid.
expansion_lots = (
    (900, -3900, 42, 24), (2850, -4000, 42, 24), (5050, -3850, 42, 24),
    (900, 3650, 42, 24), (2920, 3900, 42, 24), (5200, 3700, 42, 24),
    (-4300, -2200, 36, 22), (-4100, 2500, 36, 22),
)
for index, (x, y, sx, sy) in enumerate(expansion_lots):
    lot = spawn_mesh(
        unreal.load_asset(CUBE), (x, y, -1), (sx, sy, 0.08),
        f"VDT_ExpansionLot_{index + 1:02d}", materials=(), collision=False,
    )
    lot.get_component_by_class(unreal.StaticMeshComponent).set_material(0, realm_grass)

# Two broad, overlapping water gardens sit beyond the central activity route.
# Overlap and rotate the banks so the estate reads as a living landscape rather
# than a pair of rectangular test strips.
ponds = (
    ((-1250, -2800, -2), (24, 8.5, 0.06), 8),
    ((-50, -3000, -2), (18, 7.0, 0.06), -12),
    ((6100, 2550, -2), (23, 8.0, 0.06), -8),
    ((7200, 2850, -2), (16, 6.5, 0.06), 14),
)
for index, (location, scale, yaw) in enumerate(ponds):
    pond = spawn_mesh(unreal.load_asset(CUBE), location, scale, f"VDT_WaterGarden_Expansion_{index:02d}", (0, yaw, 0), materials=(), collision=False)
    pond.get_component_by_class(unreal.StaticMeshComponent).set_material(0, quiet_water)

# Layered satellite islands turn the uniform sky into a readable immortal-realm
# vista. They sit beyond gameplay and have no collision or simulation role.
for index, (location, scale, yaw) in enumerate((
    ((12700, -7600, -1750), (1.75, 1.25, 1.45), 14),
    ((15100, 6500, -2300), (2.05, 1.45, 1.7), -22),
    ((18500, -500, -2850), (2.35, 1.7, 1.95), 5),
)):
    spawn_mesh(
        island,
        location,
        scale,
        f"VDT_VistaIsland_{index:02d}",
        (0, yaw, 0),
        materials,
        collision=False,
    )

# Main hall sits beyond all native activities: a real dimensional terminus for
# the player route, with the retained M2B gate as its foregate.
spawn_mesh(gate, (800, 0, 30), (0.72, 0.72, 0.72), "VDT_Arrival_MountainGate", (0, -90, 0), materials)
spawn_mesh(hall, (7000, 0, 38), (1.0, 1.0, 1.0), "VDT_MainHall_Facade", (0, -90, 0), materials)
# Pavilion passed the same scale-1.0 architecture preflight as gate and hall.
terrace = spawn_mesh(unreal.load_asset(CUBE), (6400, 0, -42), (18.0, 20.0, 1.0), "VDT_MainHall_Terrace")
terrace.get_component_by_class(unreal.StaticMeshComponent).set_material(0, pbr_stone)
spawn_mesh(pavilion, (6250, 0, 42), (1.0, 1.0, 1.0), "VDT_MainHall_Pavilion", (0, -90, 0), materials)
spawn_mesh(pavilion, (4720, -980, 20), (0.46, 0.46, 0.46), "VDT_WestTeaPavilion", (0, 180, 0), materials)
spawn_mesh(pavilion, (4720, 980, 20), (0.46, 0.46, 0.46), "VDT_EastTeaPavilion", (0, 0, 0), materials)

for index, (x, y, yaw) in enumerate(((1550, -710, 0), (1550, 710, 180), (4450, -820, 0), (4450, 820, 180), (6000, -980, 0), (6000, 980, 180))):
    spawn_mesh(stele, (x, y, 20), (0.9, 0.9, 0.9), f"VDT_SectStele_{index:02d}", (0, yaw, 0), materials)
    spawn_mesh(lantern, (x + 180, y, 10), (0.9, 0.9, 0.9), f"VDT_StoneLantern_{index:02d}", (0, yaw, 0), materials)

# Break the long pond rectangles with authored stepping-stone clusters. These
# are decorative and non-colliding; the native water actors remain authoritative
# for the route and interaction footprint.
for index, (x, y, yaw, scale) in enumerate(((1950, -860, 8, 0.72), (2350, -860, 8, 0.62), (2750, -860, 8, 0.70), (2350, 860, 188, 0.64))):
    spawn_mesh(stepping_stones, (x, y, 6), (scale, scale, scale), f"VDT_SteppingStones_{index:02d}", (0, yaw, 0), materials, collision=False)
for index, (x, y) in enumerate(((3000, -720), (3000, 720), (5800, -500), (5800, 500))):
    spawn_mesh(brazier, (x, y, 18), (0.85, 0.85, 0.85), f"VDT_RitualBrazier_{index:02d}", (0, index * 90, 0), materials)

# Bridge and uneven curved shore props terminate the old rectangular water read.
for index, y in enumerate((-860, 860)):
    spawn_mesh(bridge, (2850, y, 12), (1.0, 1.0, 1.0), f"VDT_ArchedBridge_{index:02d}", (0, 90, 0), materials)
    for side in (-1, 1):
        for segment in (-1, 1):
            spawn_mesh(
                railing,
                (2850 + side * 108, y + segment * 165, 58),
                (0.92, 0.92, 0.92),
                f"VDT_BridgeRail_{index}_{side}_{segment}",
                (0, 90, 0),
                materials,
                collision=False,
            )
    for side in (-1, 1):
        spawn_mesh(shore, (2450 + side * 580, y + side * 150, -4), (1.45, 1.45, 1.0), f"VDT_CurvedShore_{index}_{side}", (0, 90 if side > 0 else -90, 0), materials)
    for lotus_index, offset in enumerate((-420, -120, 180, 470)):
        spawn_mesh(lotus, (2600 + offset, y + (110 if lotus_index % 2 else -110), 4), (0.62, 0.62, 0.62), f"VDT_Lotus_{index}_{lotus_index}", (0, lotus_index * 57, 0), materials)

# Native activity actors keep their interaction collision, light and canonical
# command providers. Only their Engine BasicShape renderers are hidden, then a
# non-colliding original-art shell is placed at the same stable location.
site_labels = {
    "VT_ConflictSite": (stele, (1050, 0, 8), (0.72, 0.72, 0.72), 90),
    "VT_FormationSite": (lantern, (1500, 0, 8), (1.05, 1.05, 1.05), 0),
    "VT_AlchemySite": (brazier, (2350, 0, 8), (1.12, 1.12, 1.12), 0),
    "VT_CultivationSite": (lotus, (3220, 0, 10), (0.92, 0.92, 0.92), 18),
    "VT_BreakthroughSite": (stele, (4050, 0, 8), (0.88, 0.88, 0.88), -90),
}
for site_label, (shell_mesh, shell_location, shell_scale, shell_yaw) in site_labels.items():
    site = next((actor for actor in actor_system.get_all_level_actors() if actor.get_actor_label() == site_label), None)
    if not site:
        raise RuntimeError(f"Native gameplay site missing while dressing Vấn Đạo Tông: {site_label}")
    for component in site.get_components_by_class(unreal.StaticMeshComponent):
        component.set_visibility(False, True)
        component.set_hidden_in_game(True)
    spawn_mesh(
        shell_mesh,
        shell_location,
        shell_scale,
        f"VDT_SiteShell_{site_label[3:]}",
        (0, shell_yaw, 0),
        materials,
        collision=False,
    )

# Garden courts use scanned shrubs/ferns in restrained, deterministic clusters.
# The old pine nearest the gameplay light is hidden because the blue cue made it
# read as emissive rather than foliage.
for actor in actor_system.get_all_level_actors():
    if actor.get_actor_label() in ("VT_Pine_00", "VT_Pine_02", "VT_Pine_03"):
        actor.set_actor_hidden_in_game(True)
        actor.set_is_temporarily_hidden_in_editor(True)

garden_clusters = (
    ((1750, -1120, 0), shrubs[0], 0.82, 14),
    ((1900, -1030, 0), ferns[1], 0.90, 83),
    ((2650, 1120, 0), shrubs[2], 0.72, 147),
    ((3450, -1080, 0), shrubs[1], 0.78, 211),
    ((4320, 1040, 0), ferns[3], 1.05, 286),
    ((5650, -1050, 0), shrubs[3], 0.84, 331),
)
for index, (location, mesh, scale, yaw) in enumerate(garden_clusters):
    spawn_mesh(
        mesh,
        location,
        (scale, scale, scale),
        f"VDT_GardenPlant_{index:02d}",
        (0, yaw, 0),
        materials=(),
        collision=False,
    )

# A broad natural ring gives the plateau visual scale: pine groves and mossy
# boulders frame the buildable land while remaining outside the native route.
perimeter_pines = (
    ((-4200, -5200, 0), 5.8, 20), ((-1500, -5650, 0), 4.6, 110),
    ((1800, -5650, 0), 5.2, 185), ((5200, -5200, 0), 6.2, 250),
    ((8200, -3400, 0), 5.5, 315), ((8700, 100, 0), 4.8, 25),
    ((8200, 3500, 0), 6.4, 150), ((5200, 5200, 0), 5.4, 210),
    ((1200, 5550, 0), 4.9, 285), ((-1800, 5200, 0), 5.8, 350),
    ((-4800, 3300, 0), 5.1, 70), ((-5400, -700, 0), 6.0, 140),
)
for index, (location, scale, yaw) in enumerate(perimeter_pines):
    spawn_mesh(pines[index % len(pines)], location, (scale, scale, scale), f"VDT_Pine_Expansion_{index:02d}", (0, yaw, 0), collision=False)

perimeter_rocks = (
    ((-5200, -4300, -20), (8.0, 8.0, 6.0), 20), ((-3000, -5700, -15), (6.0, 7.0, 5.0), 75),
    ((900, -6100, -10), (9.0, 6.0, 7.0), 140), ((4600, -5900, -15), (7.0, 8.0, 6.0), 220),
    ((8500, -4200, -20), (8.0, 7.0, 7.0), 290), ((9000, 3000, -15), (10.0, 7.0, 8.0), 15),
    ((4800, 5900, -10), (7.0, 9.0, 6.0), 95), ((-800, 6100, -20), (9.0, 7.0, 7.0), 190),
    ((-5100, 4200, -10), (7.0, 8.0, 6.0), 275), ((-5900, 900, -15), (8.0, 6.0, 7.0), 340),
)
for index, (location, scale, yaw) in enumerate(perimeter_rocks):
    mesh = rock_face if index % 3 == 0 else rocks[index % len(rocks)]
    spawn_mesh(mesh, location, scale, f"VDT_MountainRock_{index:02d}", (0, yaw, 0), collision=False)

# Curated shots stay in the map and are used for deterministic review captures.
spawn_camera("VDT_CAM_01_ArrivalVista", (-700, -900, 190), (6.2, 31.6, 0))
spawn_camera("VDT_CAM_02_SectGate", (-200, -800, 180), (7.6, 38.7, 0))
spawn_camera("VDT_CAM_03_Forecourt", (3200, -1050, 220), (2.4, 19.9, 0))
spawn_camera("VDT_CAM_04_WaterGarden", (1700, -1100, 160), (1.2, 18.4, 0))
spawn_camera("VDT_CAM_05_MainHall", (5200, -900, 160), (7.8, 34.7, 0))

# Lock exposure for reliable review rather than letting auto exposure wash out
# the lacquer/roof contrast in every capture.
for actor in actor_system.get_all_level_actors():
    if actor.get_actor_label() == "VT_PostProcess":
        settings = actor.get_editor_property("settings")
        set_if(settings, "override_auto_exposure_bias", True)
        set_if(settings, "auto_exposure_bias", -0.35)
        set_if(settings, "override_bloom_intensity", True)
        set_if(settings, "bloom_intensity", 0.12)
        actor.set_editor_property("settings", settings)
    elif actor.get_actor_label() == "VT_Sun":
        sun_component = actor.get_component_by_class(unreal.DirectionalLightComponent)
        set_if(sun_component, "intensity", 4.2)
        set_if(sun_component, "light_color", unreal.Color(255, 196, 148))
    elif actor.get_actor_label() == "VT_SkyAtmosphere":
        atmosphere = actor.get_component_by_class(unreal.SkyAtmosphereComponent)
        set_if(atmosphere, "rayleigh_scattering_scale", 0.0331)
        set_if(atmosphere, "mie_scattering_scale", 0.003996)
        set_if(atmosphere, "mie_anisotropy", 0.80)
        set_if(atmosphere, "ground_albedo", unreal.Color(170, 170, 170))
        set_if(
            atmosphere,
            "sky_and_aerial_perspective_luminance_factor",
            unreal.LinearColor(1.0, 1.0, 1.0, 1.0),
        )
    elif actor.get_actor_label() == "VT_HeightFog":
        fog = actor.get_component_by_class(unreal.ExponentialHeightFogComponent)
        set_if(fog, "fog_density", 0.018)
        set_if(fog, "fog_height_falloff", 0.18)
        set_if(fog, "fog_inscattering_luminance", unreal.LinearColor(0.20, 0.28, 0.32, 1.0))
        set_if(fog, "enable_volumetric_fog", True)
        set_if(fog, "volumetric_fog_distance", 12000.0)
        set_if(fog, "volumetric_fog_extinction_scale", 0.75)
        set_if(fog, "volumetric_fog_albedo", unreal.Color(170, 190, 205))
        set_if(fog, "start_distance", 0.0)

site_light_palette = {
    "VT_ConflictSite": unreal.Color(255, 100, 70),
    "VT_FormationSite": unreal.Color(104, 202, 164),
    "VT_AlchemySite": unreal.Color(255, 148, 58),
    "VT_CultivationSite": unreal.Color(92, 184, 150),
    "VT_BreakthroughSite": unreal.Color(255, 194, 104),
}
for site_label, light_color in site_light_palette.items():
    site = next((actor for actor in actor_system.get_all_level_actors() if actor.get_actor_label() == site_label), None)
    if site:
        for component in site.get_components_by_class(unreal.PointLightComponent):
            set_if(component, "intensity", 100.0)
            set_if(component, "attenuation_radius", 260.0)
            set_if(component, "light_color", light_color)

cloud_material = unreal.load_asset(
    "/Engine/EngineSky/VolumetricClouds/m_SimpleVolumetricCloud_Inst"
)
if not cloud_material:
    raise RuntimeError("UE5.8 simple volumetric cloud material is missing")
cloud = unreal.EditorLevelLibrary.spawn_actor_from_class(
    unreal.VolumetricCloud, unreal.Vector(0, 0, 0), unreal.Rotator()
)
cloud.set_actor_label("VDT_VolumetricCloud")
cloud_component = cloud.get_component_by_class(unreal.VolumetricCloudComponent)
cloud_component.set_material(cloud_material)
set_if(cloud_component, "layer_bottom_altitude", 0.75)
set_if(cloud_component, "layer_height", 2.6)
set_if(cloud_component, "view_sample_count_scale", 0.75)
set_if(cloud_component, "shadow_view_sample_count_scale", 0.65)

if not level.save_current_level():
    raise RuntimeError(f"Could not save {MAP}")
assets.save_asset(MAP, only_if_is_dirty=False)
unreal.log(f"VANDAO visual map PASS: {MAP}; pavilion_instances=3; cameras=5")
