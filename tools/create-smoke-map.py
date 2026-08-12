"""Create the authored M2A smoke map in an open Unreal Engine 5.8 editor.

Run after the native Editor target builds, with the Python Editor Script Plugin enabled:
  UnrealEditor.exe TruongSinhUE5.uproject -ExecutePythonScript=tools/create-smoke-map.py
"""

import unreal


MAP_PACKAGE = "/Game/Maps/Dev/L_Dev_Smoke"


def set_editor_property_if_present(obj, name, value):
    try:
        obj.set_editor_property(name, value)
    except Exception as exc:  # UE properties vary slightly between point releases.
        unreal.log_warning(f"{obj.get_name()}.{name}: {exc}")


def spawn(actor_class, location, rotation=(0.0, 0.0, 0.0), label=None):
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        actor_class,
        unreal.Vector(*location),
        unreal.Rotator(*rotation),
    )
    if label:
        actor.set_actor_label(label)
    return actor


def add_primitive(mesh_path, location, scale, label, rotation=(0.0, 0.0, 0.0)):
    actor = spawn(unreal.StaticMeshActor, location, rotation, label)
    component = actor.static_mesh_component
    component.set_static_mesh(unreal.load_asset(mesh_path))
    component.set_collision_profile_name("BlockAll")
    actor.set_actor_scale3d(unreal.Vector(*scale))
    return actor


def build_map():
    level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PACKAGE):
        unreal.log(f"Replacing existing smoke map: {MAP_PACKAGE}")
        if not level_subsystem.load_level(MAP_PACKAGE):
            raise RuntimeError(f"Could not load existing level {MAP_PACKAGE}")
        for existing_actor in actor_subsystem.get_all_level_actors():
            actor_subsystem.destroy_actor(existing_actor)
    elif not level_subsystem.new_level(MAP_PACKAGE):
        raise RuntimeError(f"Could not create level {MAP_PACKAGE}")

    # Clean, generous play surface with a raised ceremonial platform.
    add_primitive(
        "/Engine/BasicShapes/Cube.Cube", (0.0, 0.0, -50.0),
        (16.0, 12.0, 1.0), "SMK_Floor")
    add_primitive(
        "/Engine/BasicShapes/Cylinder.Cylinder", (650.0, 0.0, 10.0),
        (3.8, 3.8, 0.35), "SMK_CultivationDais")
    add_primitive(
        "/Engine/BasicShapes/Cylinder.Cylinder", (650.0, 0.0, 45.0),
        (2.8, 2.8, 0.18), "SMK_CultivationDais_Inset")

    # A restrained gateway and stone rhythm create a readable destination silhouette.
    for y in (-270.0, 270.0):
        add_primitive(
            "/Engine/BasicShapes/Cube.Cube", (350.0, y, 180.0),
            (0.55, 0.55, 3.6), f"SMK_GatePillar_{int(y)}")
    add_primitive(
        "/Engine/BasicShapes/Cube.Cube", (350.0, 0.0, 380.0),
        (0.55, 6.0, 0.35), "SMK_GateLintel")
    for index, y in enumerate((-440.0, -320.0, 320.0, 440.0)):
        add_primitive(
            "/Engine/BasicShapes/Cylinder.Cylinder", (720.0, y, 65.0),
            (0.55, 0.55, 1.3), f"SMK_BoundaryStone_{index}")

    player_start = spawn(unreal.PlayerStart, (-650.0, 0.0, 120.0), label="SMK_PlayerStart")
    player_start.set_actor_rotation(unreal.Rotator(0.0, 0.0, 0.0), False)

    sun = spawn(unreal.DirectionalLight, (0.0, 0.0, 850.0), (-38.0, -32.0, 0.0), "SMK_Sun")
    sun_component = sun.get_component_by_class(unreal.DirectionalLightComponent)
    set_editor_property_if_present(sun_component, "intensity", 7.5)
    set_editor_property_if_present(sun_component, "light_color", unreal.Color(255, 224, 188))
    set_editor_property_if_present(sun_component, "atmosphere_sun_light", True)

    sky = spawn(unreal.SkyLight, (0.0, 0.0, 500.0), label="SMK_SkyLight")
    sky_component = sky.get_component_by_class(unreal.SkyLightComponent)
    set_editor_property_if_present(sky_component, "intensity", 1.15)
    set_editor_property_if_present(sky_component, "real_time_capture", True)

    spawn(unreal.SkyAtmosphere, (0.0, 0.0, 0.0), label="SMK_SkyAtmosphere")
    fog = spawn(unreal.ExponentialHeightFog, (0.0, 0.0, 0.0), label="SMK_HeightFog")
    fog_component = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
    set_editor_property_if_present(fog_component, "fog_density", 0.012)
    set_editor_property_if_present(fog_component, "fog_height_falloff", 0.22)
    set_editor_property_if_present(fog_component, "enable_volumetric_fog", True)

    site_class = unreal.load_class(None, "/Script/TruongSinhUE5.TruongSinhCultivationSite")
    if not site_class:
        raise RuntimeError("Native TruongSinhCultivationSite class is unavailable; build the Editor target first")
    spawn(site_class, (650.0, 0.0, 105.0), label="SMK_CultivationSite")

    breakthrough_class = unreal.load_class(None, "/Script/TruongSinhUE5.TruongSinhBreakthroughSite")
    if not breakthrough_class:
        raise RuntimeError("Native TruongSinhBreakthroughSite class is unavailable; build the Editor target first")
    spawn(breakthrough_class, (650.0, 540.0, 105.0), label="SMK_BreakthroughSite")

    level_subsystem.save_current_level()
    unreal.EditorAssetLibrary.save_asset(MAP_PACKAGE, only_if_is_dirty=False)
    unreal.log(f"Created and saved {MAP_PACKAGE}")


build_map()
