"""Author the isolated M2B visual-target corridor in Unreal Engine 5.8.

Run with the Editor target already built:
  UnrealEditor-Cmd.exe TruongSinhUE5.uproject \
    -ExecutePythonScript=tools/create-m2b-corridor.py -unattended -NoSplash

This script intentionally uses project-authored flat materials and Engine basic
meshes.  It establishes composition, scale, palette and lighting only; it is not
production environment art and does not replace the M2A smoke/startup map.
"""

import unreal


MAP_PACKAGE = "/Game/Maps/VisualTarget/L_M2B_Corridor"
MATERIAL_PATH = "/Game/VisualTarget/Materials"

CUBE = "/Engine/BasicShapes/Cube.Cube"
CYLINDER = "/Engine/BasicShapes/Cylinder.Cylinder"
SPHERE = "/Engine/BasicShapes/Sphere.Sphere"
CONE = "/Engine/BasicShapes/Cone.Cone"


def set_property(obj, name, value):
    try:
        obj.set_editor_property(name, value)
        return True
    except Exception as exc:
        unreal.log_warning(f"{obj.get_name()}.{name}: {exc}")
        return False


def create_material(name, color, roughness=0.7, metallic=0.0):
    asset_path = f"{MATERIAL_PATH}/{name}"
    material = unreal.load_asset(asset_path)
    if material:
        return material

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    material = asset_tools.create_asset(
        name,
        MATERIAL_PATH,
        unreal.Material,
        unreal.MaterialFactoryNew(),
    )
    if not material:
        raise RuntimeError(f"Could not create material {asset_path}")

    base = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionConstant3Vector, -340, -60
    )
    base.set_editor_property("constant", unreal.LinearColor(*color, 1.0))
    unreal.MaterialEditingLibrary.connect_material_property(
        base, "", unreal.MaterialProperty.MP_BASE_COLOR
    )

    rough = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionConstant, -340, 80
    )
    rough.set_editor_property("r", roughness)
    unreal.MaterialEditingLibrary.connect_material_property(
        rough, "", unreal.MaterialProperty.MP_ROUGHNESS
    )

    metal = unreal.MaterialEditingLibrary.create_material_expression(
        material, unreal.MaterialExpressionConstant, -340, 180
    )
    metal.set_editor_property("r", metallic)
    unreal.MaterialEditingLibrary.connect_material_property(
        metal, "", unreal.MaterialProperty.MP_METALLIC
    )

    unreal.MaterialEditingLibrary.recompile_material(material)
    unreal.EditorAssetLibrary.save_loaded_asset(material, only_if_is_dirty=False)
    return material


def spawn(actor_class, location, rotation=(0.0, 0.0, 0.0), label=None):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        actor_class, unreal.Vector(*location), unreal.Rotator(*rotation)
    )
    if not actor:
        raise RuntimeError(f"Could not spawn {actor_class} at {location}")
    if label:
        actor.set_actor_label(label)
    return actor


def primitive(mesh_path, location, scale, label, material, rotation=(0.0, 0.0, 0.0), collision=True):
    actor = spawn(unreal.StaticMeshActor, location, rotation, label)
    component = actor.get_component_by_class(unreal.StaticMeshComponent)
    component.set_static_mesh(unreal.load_asset(mesh_path))
    component.set_material(0, material)
    component.set_collision_profile_name("BlockAll" if collision else "NoCollision")
    actor.set_actor_scale3d(unreal.Vector(*scale))
    return actor


def building(index, x, y, facing_yaw, mats):
    side = f"B{index:02d}"
    front_y = y + (475 if y < 0 else -475)
    sign_y = y + (535 if y < 0 else -535)
    # Raised stone base, warm plaster volume and exposed aged timber rhythm.
    primitive(CUBE, (x, y, 45), (8.2, 5.2, 0.9), f"VT_{side}_Base", mats["stone_dark"])
    primitive(CUBE, (x, y, 205), (7.4, 4.5, 2.35), f"VT_{side}_Wall", mats["plaster"])
    for offset in (-330, 0, 330):
        primitive(CYLINDER, (x + offset, front_y, 230), (0.18, 0.18, 3.7), f"VT_{side}_Post_{offset}", mats["wood"])
    # A shallow double roof gives the corridor a readable East-Asian silhouette.
    primitive(CUBE, (x, y, 455), (8.7, 5.8, 0.24), f"VT_{side}_RoofLow", mats["tile"])
    primitive(CUBE, (x, y, 492), (7.7, 4.8, 0.18), f"VT_{side}_RoofHigh", mats["tile_wet"])
    # Brass sign and dyed hanging banners face the central route.
    primitive(CUBE, (x, sign_y, 335), (1.15, 0.08, 0.42), f"VT_{side}_Sign", mats["brass"], collision=False)
    for banner_x in (-230, 230):
        primitive(CUBE, (x + banner_x, sign_y, 225), (0.38, 0.05, 1.05), f"VT_{side}_Banner_{banner_x}", mats["cloth"], collision=False)
    if facing_yaw:
        # Buildings on the opposite side are authored mirrored by their Y position;
        # labels retain the requested facing for later art replacement.
        pass


def tree(index, x, y, scale, mats):
    primitive(CYLINDER, (x, y, 180 * scale), (0.28 * scale, 0.28 * scale, 3.5 * scale), f"VT_Tree_{index}_Trunk", mats["wood"], collision=False)
    primitive(SPHERE, (x, y, 460 * scale), (1.5 * scale, 1.25 * scale, 1.65 * scale), f"VT_Tree_{index}_CrownA", mats["foliage"], collision=False)
    primitive(SPHERE, (x + 85 * scale, y - 25 * scale, 515 * scale), (1.05 * scale, 0.95 * scale, 1.15 * scale), f"VT_Tree_{index}_CrownB", mats["foliage_alt"], collision=False)


def lantern(index, x, y, mats):
    primitive(CYLINDER, (x, y, 115), (0.16, 0.16, 2.3), f"VT_Lantern_{index}_Post", mats["stone"])
    primitive(CUBE, (x, y, 235), (0.36, 0.36, 0.36), f"VT_Lantern_{index}_Housing", mats["brass"], collision=False)
    primitive(CUBE, (x, y, 285), (0.48, 0.48, 0.10), f"VT_Lantern_{index}_Cap", mats["tile"], collision=False)


def build_map():
    unreal.EditorAssetLibrary.make_directory(MATERIAL_PATH)
    mats = {
        "stone": create_material("M_VT_StoneGrey", (0.22, 0.25, 0.25), 0.92),
        "stone_dark": create_material("M_VT_StoneDark", (0.08, 0.105, 0.11), 0.86),
        "stone_wet": create_material("M_VT_StoneWet", (0.11, 0.15, 0.16), 0.34),
        "plaster": create_material("M_VT_AgedPlaster", (0.48, 0.45, 0.36), 0.82),
        "wood": create_material("M_VT_OldWood", (0.15, 0.065, 0.035), 0.78),
        "tile": create_material("M_VT_BlueBlackTile", (0.035, 0.075, 0.085), 0.48),
        "tile_wet": create_material("M_VT_BlueBlackTileWet", (0.025, 0.095, 0.11), 0.24),
        "foliage": create_material("M_VT_PineDeep", (0.035, 0.12, 0.075), 0.88),
        "foliage_alt": create_material("M_VT_PineMoss", (0.09, 0.18, 0.095), 0.9),
        "water": create_material("M_VT_QuietWater", (0.025, 0.14, 0.17), 0.16, 0.05),
        "cloth": create_material("M_VT_DyedCloth", (0.28, 0.035, 0.025), 0.72),
        "brass": create_material("M_VT_AgedBrass", (0.31, 0.16, 0.045), 0.38, 0.72),
        "jade": create_material("M_VT_RestrainedJade", (0.045, 0.30, 0.22), 0.27, 0.08),
        "mountain": create_material("M_VT_MountainSilhouette", (0.045, 0.065, 0.07), 0.95),
    }

    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PACKAGE):
        if not level_subsystem.load_level(MAP_PACKAGE):
            raise RuntimeError(f"Could not load existing map {MAP_PACKAGE}")
        for actor in actor_subsystem.get_all_level_actors():
            actor_subsystem.destroy_actor(actor)
    elif not level_subsystem.new_level(MAP_PACKAGE):
        raise RuntimeError(f"Could not create map {MAP_PACKAGE}")

    # Walkable corridor: 80 m long, 22 m wide, with irregular wet paving bands.
    primitive(CUBE, (2600, 0, -55), (42, 13, 1.0), "VT_Ground", mats["stone_dark"])
    for index in range(16):
        x = -900 + index * 450
        y = (-1 if index % 2 else 1) * (18 + (index % 3) * 9)
        material = mats["stone_wet"] if index % 4 == 0 else mats["stone"]
        primitive(CUBE, (x, y, 8 + (index % 2) * 2), (4.1, 4.8, 0.14), f"VT_PathSlab_{index:02d}", material)

    # Shallow garden-water bands and stepping stones keep the lived-in route dense.
    for y in (-860, 860):
        primitive(CUBE, (2250, y, -2), (22, 2.8, 0.08), f"VT_WaterGarden_{y}", mats["water"], collision=False)
        for index, x in enumerate((700, 1500, 2300, 3100, 3900)):
            primitive(CYLINDER, (x, y, 15), (0.65, 0.65, 0.12), f"VT_SteppingStone_{y}_{index}", mats["stone_wet"])

    # Three building pairs create foreground/midground compression along the vista.
    for index, (x, y) in enumerate(((350, -1500), (350, 1500), (2100, -1500), (2100, 1500), (3750, -1500), (3750, 1500))):
        building(index, x, y, 0 if y < 0 else 180, mats)

    # Primary landmark: a restrained three-tier sect gate and jade moon-disc.
    for y in (-510, 510):
        primitive(CUBE, (5550, y, 360), (0.8, 0.8, 7.2), f"VT_MainGate_Pillar_{y}", mats["wood"])
        primitive(CUBE, (5550, y, 115), (1.15, 1.15, 0.55), f"VT_MainGate_Foot_{y}", mats["stone_dark"])
    primitive(CUBE, (5550, 0, 720), (1.0, 11.3, 0.42), "VT_MainGate_Lintel", mats["wood"])
    primitive(CUBE, (5550, 0, 805), (1.4, 13.0, 0.28), "VT_MainGate_EaveLow", mats["tile"])
    primitive(CUBE, (5550, 0, 875), (1.05, 10.5, 0.22), "VT_MainGate_EaveHigh", mats["tile_wet"])
    primitive(CYLINDER, (5520, 0, 610), (2.25, 2.25, 0.20), "VT_MainGate_JadeDisc", mats["jade"], (90, 0, 0), collision=False)
    primitive(CYLINDER, (5510, 0, 610), (1.35, 1.35, 0.22), "VT_MainGate_DiscCore", mats["stone_dark"], (90, 0, 0), collision=False)

    # Secondary landmarks: bell plinth, ancestor stone and old pines.
    primitive(CYLINDER, (2900, -760, 70), (1.4, 1.4, 0.55), "VT_BellPlinth", mats["stone"])
    primitive(CYLINDER, (2900, -760, 235), (0.75, 0.75, 1.8), "VT_Bell", mats["brass"], collision=False)
    primitive(CUBE, (1180, 720, 180), (0.75, 0.35, 3.5), "VT_AncestorStone", mats["stone_wet"])

    # The visual target remains a real Golden Loop route: it reuses the native
    # interaction provider and never owns cultivation outcome in the level.
    cultivation_site_class = unreal.load_class(
        None, "/Script/TruongSinhUE5.TruongSinhCultivationSite"
    )
    if not cultivation_site_class:
        raise RuntimeError("Could not load native TruongSinhCultivationSite class")
    cultivation_site = spawn(
        cultivation_site_class, (3220, 0, 48), (0, 0, 0), "VT_CultivationSite"
    )
    cultivation_site.set_actor_scale3d(unreal.Vector(1.25, 1.25, 1.25))

    breakthrough_site_class = unreal.load_class(
        None, "/Script/TruongSinhUE5.TruongSinhBreakthroughSite"
    )
    if not breakthrough_site_class:
        raise RuntimeError("Could not load native TruongSinhBreakthroughSite class")
    breakthrough_site = spawn(
        breakthrough_site_class, (4050, 0, 48), (0, 0, 0), "VT_BreakthroughSite"
    )
    breakthrough_site.set_actor_scale3d(unreal.Vector(1.25, 1.25, 1.25))

    alchemy_site_class = unreal.load_class(None, "/Script/TruongSinhUE5.TruongSinhAlchemySite")
    if not alchemy_site_class:
        raise RuntimeError("Could not load native TruongSinhAlchemySite class")
    alchemy_site = spawn(alchemy_site_class, (2350, 0, 48), (0, 0, 0), "VT_AlchemySite")
    alchemy_site.set_actor_scale3d(unreal.Vector(1.25, 1.25, 1.25))

    formation_site_class = unreal.load_class(None, "/Script/TruongSinhUE5.TruongSinhFormationSite")
    if not formation_site_class:
        raise RuntimeError("Could not load native TruongSinhFormationSite class")
    formation_site = spawn(formation_site_class, (1500, 0, 48), (0, 0, 0), "VT_FormationSite")

    conflict_site_class = unreal.load_class(None, "/Script/TruongSinhUE5.TruongSinhConflictSite")
    if not conflict_site_class:
        raise RuntimeError("Could not load native TruongSinhConflictSite class")
    conflict_site = spawn(conflict_site_class, (1050, 0, 48), (0, 0, 0), "VT_ConflictSite")
    formation_site.set_actor_scale3d(unreal.Vector(1.25, 1.25, 1.25))

    for index, (x, y, scale) in enumerate(((820, -920, 0.85), (1450, 1030, 1.15), (2800, 1040, 0.95), (4200, -1010, 1.25), (4650, 970, 0.8))):
        tree(index, x, y, scale, mats)
    for index, x in enumerate((100, 1050, 2000, 2950, 3900, 4850)):
        lantern(index * 2, x, -600, mats)
        lantern(index * 2 + 1, x, 600, mats)

    # Distant mountain masses frame the gate instead of enlarging the playable map.
    for index, (x, y, sx, sy, sz) in enumerate(((9300, -3800, 34, 34, 45), (10400, 1200, 44, 44, 60), (9000, 4300, 30, 30, 41))):
        primitive(CONE, (x, y, 1750), (sx, sy, sz), f"VT_VistaMountain_{index}", mats["mountain"], collision=False)

    # Keep spawn on the collision slab.  The previous negative X spawn was
    # outside the authored foundation and let the pawn fall during packaged play.
    player_start = spawn(unreal.PlayerStart, (200, 0, 115), label="VT_PlayerStart")
    player_start.set_actor_rotation(unreal.Rotator(0, 0, 0), False)

    # One temporary ambient figure reuses the already-manifested UE5.8 Manny.
    npc = spawn(unreal.SkeletalMeshActor, (1950, -520, 90), (0, 145, 0), "VT_NPC_Caretaker_Temporary")
    npc_component = npc.get_component_by_class(unreal.SkeletalMeshComponent)
    npc_mesh = unreal.load_asset("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple")
    if npc_component and npc_mesh:
        try:
            npc_component.set_skeletal_mesh_asset(npc_mesh)
        except Exception:
            set_property(npc_component, "skeletal_mesh_asset", npc_mesh)
        idle = unreal.load_asset("/Game/Characters/Mannequins/Anims/Unarmed/MM_Idle")
        if idle:
            try:
                npc_component.set_animation_mode(unreal.AnimationMode.ANIMATION_SINGLE_NODE)
                npc_component.set_animation(idle)
            except Exception as exc:
                unreal.log_warning(f"Could not assign caretaker idle preview: {exc}")

    # Late-afternoon key, neutral sky fill, one volumetric layer and three warm lamps.
    sun = spawn(unreal.DirectionalLight, (0, 0, 1600), (-26, -34, -8), "VT_Sun")
    sun_component = sun.get_component_by_class(unreal.DirectionalLightComponent)
    set_property(sun_component, "intensity", 5.2)
    set_property(sun_component, "light_color", unreal.Color(255, 210, 162))
    set_property(sun_component, "atmosphere_sun_light", True)
    set_property(sun_component, "cast_volumetric_shadow", True)

    sky = spawn(unreal.SkyLight, (0, 0, 900), label="VT_SkyLight")
    sky_component = sky.get_component_by_class(unreal.SkyLightComponent)
    set_property(sky_component, "intensity", 0.8)
    set_property(sky_component, "real_time_capture", True)
    spawn(unreal.SkyAtmosphere, (0, 0, 0), label="VT_SkyAtmosphere")

    fog = spawn(unreal.ExponentialHeightFog, (0, 0, -80), label="VT_HeightFog")
    fog_component = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
    set_property(fog_component, "fog_density", 0.018)
    set_property(fog_component, "fog_height_falloff", 0.18)
    set_property(fog_component, "fog_inscattering_color", unreal.LinearColor(0.20, 0.28, 0.32, 1.0))
    set_property(fog_component, "enable_volumetric_fog", True)
    set_property(fog_component, "volumetric_fog_scattering_distribution", 0.42)
    set_property(fog_component, "volumetric_fog_extinction_scale", 0.75)

    for index, (x, y) in enumerate(((350, -1040), (2100, 1040), (3750, -1040))):
        light = spawn(unreal.PointLight, (x, y, 360), label=f"VT_WarmLamp_{index}")
        component = light.get_component_by_class(unreal.PointLightComponent)
        set_property(component, "intensity", 520.0)
        set_property(component, "attenuation_radius", 650.0)
        set_property(component, "light_color", unreal.Color(255, 154, 79))
        set_property(component, "cast_shadows", index == 1)

    post = spawn(unreal.PostProcessVolume, (2500, 0, 300), label="VT_PostProcess")
    set_property(post, "unbound", True)
    settings = post.get_editor_property("settings")
    for name, value in (
        ("override_bloom_intensity", True),
        ("bloom_intensity", 0.22),
        ("override_vignette_intensity", True),
        ("vignette_intensity", 0.18),
        ("override_auto_exposure_bias", True),
        ("auto_exposure_bias", -0.25),
    ):
        try:
            settings.set_editor_property(name, value)
        except Exception as exc:
            unreal.log_warning(f"PostProcessSettings.{name}: {exc}")
    post.set_editor_property("settings", settings)

    if not level_subsystem.save_current_level():
        raise RuntimeError(f"Could not save map {MAP_PACKAGE}")
    unreal.EditorAssetLibrary.save_asset(MAP_PACKAGE, only_if_is_dirty=False)
    unreal.EditorAssetLibrary.save_directory(MATERIAL_PATH, only_if_is_dirty=False, recursive=True)
    unreal.log(f"M2B corridor foundation created and saved: {MAP_PACKAGE}")


build_map()
