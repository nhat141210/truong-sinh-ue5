"""Author the isolated M2B visual-target corridor in Unreal Engine 5.8.

Run with the Editor target already built:
  UnrealEditor-Cmd.exe TruongSinhUE5.uproject \
    -ExecutePythonScript=tools/create-m2b-corridor.py -unattended -NoSplash

This script authors the visual-target composition with approved PBR surfaces,
scanned nature assets and the verified UE 5.8 High LevelPrototyping architecture
kit.  This map does not replace the M2A smoke/startup map.
"""

import os

import unreal


MAP_PACKAGE = os.environ.get(
    "TRUONGSINH_VISUAL_MAP_PACKAGE",
    "/Game/Maps/VisualTarget/L_M2B_Corridor",
)
if not MAP_PACKAGE.startswith("/Game/Maps/VisualTarget/"):
    raise RuntimeError(f"Refusing visual-map target outside /Game/Maps/VisualTarget: {MAP_PACKAGE}")
MATERIAL_PATH = "/Game/VisualTarget/Materials"

CUBE = "/Engine/BasicShapes/Cube.Cube"
CYLINDER = "/Engine/BasicShapes/Cylinder.Cylinder"

ARCH_MESH_ROOT = "/Game/LevelPrototyping/Meshes"
ARCH_CHAMFER = f"{ARCH_MESH_ROOT}/SM_ChamferCube.SM_ChamferCube"
ARCH_CYLINDER = f"{ARCH_MESH_ROOT}/SM_Cylinder.SM_Cylinder"
ARCH_PLANE = f"{ARCH_MESH_ROOT}/SM_Plane.SM_Plane"
ARCH_QUARTER = f"{ARCH_MESH_ROOT}/SM_QuarterCylinder.SM_QuarterCylinder"
ARCH_QUARTER_OUTER = f"{ARCH_MESH_ROOT}/SM_QuarterCylinderOuter.SM_QuarterCylinderOuter"

PINE_MESHES = tuple(
    f"/Game/VisualTarget/Nature/pine_sapling_small/pine_sapling_small_1k/StaticMeshes/pine_sapling_small_{variant}"
    for variant in ("a", "b", "c")
)
SHRUB_MESHES = tuple(
    f"/Game/VisualTarget/Nature/shrub_02/shrub_02_1k/StaticMeshes/shrub_02_{variant}"
    for variant in ("a", "b", "c", "d")
)
FERN_MESHES = tuple(
    f"/Game/VisualTarget/Nature/fern_02/fern_02_1k/StaticMeshes/fern_02_{variant}"
    for variant in ("a", "b", "c", "d")
)
ROCK_MESHES = tuple(
    f"/Game/VisualTarget/Nature/rock_moss_set_01/rock_moss_set_01_2k/StaticMeshes/rock_moss_set_01_rock{index:02d}"
    for index in range(1, 7)
)
ROCK_FACE_MESH = "/Game/VisualTarget/Nature/rock_face_01/rock_face_01_2k/StaticMeshes/rock_face_01_2k"


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
    pitch, yaw, roll = rotation
    actor_rotation = unreal.Rotator(pitch=pitch, yaw=yaw, roll=roll)
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        actor_class, unreal.Vector(*location), actor_rotation
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


def required_mesh(mesh_path):
    mesh = unreal.load_asset(mesh_path)
    if not mesh or not isinstance(mesh, unreal.StaticMesh):
        raise RuntimeError(f"Required production nature mesh is missing: {mesh_path}")
    return mesh


def nature_mesh(mesh, location, scale, label, rotation=(0.0, 0.0, 0.0), collision=False):
    actor = spawn(unreal.StaticMeshActor, location, rotation, label)
    component = actor.get_component_by_class(unreal.StaticMeshComponent)
    component.set_static_mesh(mesh)
    component.set_collision_profile_name("BlockAll" if collision else "NoCollision")
    actor.set_actor_scale3d(unreal.Vector(*scale))
    return actor


def require_architecture_kit():
    required = (ARCH_CHAMFER, ARCH_CYLINDER, ARCH_PLANE, ARCH_QUARTER, ARCH_QUARTER_OUTER)
    missing = [asset_path for asset_path in required if not unreal.load_asset(asset_path)]
    if missing:
        raise RuntimeError(
            "Verified UE 5.8 architecture kit is missing. Run "
            "tools/install-ue58-architecture-kit.ps1. Missing: " + ", ".join(missing)
        )


def tiled_eave(prefix, center, length, depth, tile_count, material, along_x=True):
    """Build two curved tiled edges with raised corner caps and a ridge."""
    x, y, z = center
    primary_step = length / max(tile_count - 1, 1)
    for side in (-1, 1):
        for tile_index in range(tile_count):
            primary = -length * 0.5 + tile_index * primary_step
            end_lift = 20.0 if tile_index in (0, tile_count - 1) else 0.0
            if along_x:
                location = (x + primary, y + side * depth * 0.5, z + end_lift)
                # Quarter-cylinder length is local Z; pitch it onto world X.
                scale = (0.82, 0.62 + end_lift / 90.0, primary_step / 100.0)
                rotation = (0.0, 90.0, 0.0 if side < 0 else 180.0)
            else:
                location = (x + side * depth * 0.5, y + primary, z + end_lift)
                # Roll the same local Z length onto world Y.
                scale = (0.82, 0.62 + end_lift / 90.0, primary_step / 100.0)
                rotation = (90.0 if side < 0 else -90.0, 0.0, 0.0)
            primitive(
                ARCH_QUARTER_OUTER,
                location,
                scale,
                f"{prefix}_Eave_{side}_{tile_index:02d}",
                material,
                rotation,
                collision=False,
            )

    ridge_count = max(5, tile_count // 2)
    ridge_step = length / max(ridge_count - 1, 1)
    for ridge_index in range(ridge_count):
        primary = -length * 0.5 + ridge_index * ridge_step
        location = (x + primary, y, z + 92.0) if along_x else (x, y + primary, z + 92.0)
        scale = (0.38, 0.34, ridge_step / 100.0)
        primitive(
            ARCH_QUARTER,
            location,
            scale,
            f"{prefix}_Ridge_{ridge_index:02d}",
            material,
            (0.0, 90.0, 0.0) if along_x else (90.0, 0.0, 0.0),
            collision=False,
        )


def building(index, x, y, facing_yaw, mats):
    side = f"B{index:02d}"
    front_y = y + (475 if y < 0 else -475)
    sign_y = y + (535 if y < 0 else -535)
    # Beveled masonry and timber catch Lumen highlights without razor-edged cubes.
    primitive(ARCH_CHAMFER, (x, y, 45), (8.2, 5.2, 0.9), f"VT_{side}_Base", mats["stone_dark"])
    primitive(ARCH_CHAMFER, (x, y, 205), (7.4, 4.5, 2.35), f"VT_{side}_Wall", mats["plaster"])
    for offset in (-330, 0, 330):
        primitive(ARCH_CYLINDER, (x + offset, front_y, 230), (0.18, 0.18, 3.7), f"VT_{side}_Post_{offset}", mats["wood"])
        primitive(ARCH_CHAMFER, (x + offset, front_y, 420), (0.38, 0.42, 0.20), f"VT_{side}_Bracket_{offset}", mats["wood"], collision=False)
    primitive(ARCH_CHAMFER, (x, front_y, 405), (7.9, 0.28, 0.28), f"VT_{side}_HeaderBeam", mats["wood"], collision=False)
    # A narrow inner deck keeps rain cover while repeated curved profiles carry
    # the visible silhouette and lift the four corners.
    primitive(ARCH_CHAMFER, (x, y, 448), (7.9, 4.65, 0.16), f"VT_{side}_RoofDeck", mats["tile_wet"], collision=False)
    tiled_eave(f"VT_{side}_Roof", (x, y, 468), 820.0, 590.0, 9, mats["tile"], along_x=True)
    # Brass sign and dyed hanging banners face the central route.
    primitive(ARCH_CHAMFER, (x, sign_y, 335), (1.15, 0.08, 0.42), f"VT_{side}_Sign", mats["brass"], collision=False)
    for banner_x in (-230, 230):
        primitive(ARCH_PLANE, (x + banner_x, sign_y, 225), (0.76, 0.1, 2.1), f"VT_{side}_Banner_{banner_x}", mats["cloth"], (90.0, 0.0, facing_yaw), collision=False)
    if facing_yaw:
        # Buildings on the opposite side are authored mirrored by their Y position;
        # labels retain the requested facing for later art replacement.
        pass


def tree(index, x, y, scale, nature):
    """Place a real pine plus deterministic understorey, without graybox foliage."""
    pine_scale = 4.15 * scale
    nature_mesh(
        nature["pines"][index % len(nature["pines"])],
        (x, y, -2),
        (pine_scale, pine_scale, pine_scale),
        f"VT_Pine_{index:02d}",
        (0, (index * 67 + 19) % 360, 0),
        collision=False,
    )

    # Sparse, asymmetric ground cover avoids repeated asset silhouettes while
    # retaining a deterministic authored result on every map regeneration.
    understorey = (
        (-155, -92, 0.62, 31),
        (128, -118, 0.48, 173),
        (-78, 148, 0.54, 281),
    )
    for plant_index, (offset_x, offset_y, plant_scale, yaw) in enumerate(understorey):
        if plant_index == 0:
            mesh = nature["shrubs"][(index + plant_index) % len(nature["shrubs"])]
            prefix = "Shrub"
        else:
            mesh = nature["ferns"][(index * 2 + plant_index) % len(nature["ferns"])]
            prefix = "Fern"
        final_scale = plant_scale * (0.88 + 0.06 * (index % 3))
        nature_mesh(
            mesh,
            (x + offset_x * scale, y + offset_y * scale, -1),
            (final_scale, final_scale, final_scale),
            f"VT_{prefix}_{index:02d}_{plant_index}",
            (0, (yaw + index * 43) % 360, 0),
        )


def lantern(index, x, y, mats):
    primitive(CYLINDER, (x, y, 115), (0.16, 0.16, 2.3), f"VT_Lantern_{index}_Post", mats["stone"])
    primitive(CUBE, (x, y, 235), (0.36, 0.36, 0.36), f"VT_Lantern_{index}_Housing", mats["brass"], collision=False)
    primitive(CUBE, (x, y, 285), (0.48, 0.48, 0.10), f"VT_Lantern_{index}_Cap", mats["tile"], collision=False)


def build_map():
    require_architecture_kit()
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
        "realm_grass": create_material("M_VT_RealmGrass", (0.095, 0.19, 0.105), 0.93),
        "water": create_material("M_VT_QuietWater", (0.025, 0.14, 0.17), 0.16, 0.05),
        "cloth": create_material("M_VT_DyedCloth", (0.28, 0.035, 0.025), 0.72),
        "brass": create_material("M_VT_AgedBrass", (0.31, 0.16, 0.045), 0.38, 0.72),
        "jade": create_material("M_VT_RestrainedJade", (0.045, 0.30, 0.22), 0.27, 0.08),
        "mountain": create_material("M_VT_MountainSilhouette", (0.045, 0.065, 0.07), 0.95),
    }
    pbr = {
        "stone": unreal.load_asset("/Game/VisualTarget/PBR/Materials/M_PBR_StoneTiles"),
        "roof": unreal.load_asset("/Game/VisualTarget/PBR/Materials/M_PBR_RoofTiles"),
        "wood": unreal.load_asset("/Game/VisualTarget/PBR/Materials/M_PBR_OldWood"),
        "plaster": unreal.load_asset("/Game/VisualTarget/PBR/Materials/M_PBR_ClayPlaster"),
    }
    if all(pbr.values()):
        mats["stone"] = pbr["stone"]
        mats["stone_dark"] = pbr["stone"]
        mats["stone_wet"] = pbr["stone"]
        mats["tile"] = pbr["roof"]
        mats["tile_wet"] = pbr["roof"]
        mats["wood"] = pbr["wood"]
        mats["plaster"] = pbr["plaster"]
    else:
        unreal.log_warning("Approved PBR kit not imported; using foundation materials")

    nature = {
        "pines": tuple(required_mesh(path) for path in PINE_MESHES),
        "shrubs": tuple(required_mesh(path) for path in SHRUB_MESHES),
        "ferns": tuple(required_mesh(path) for path in FERN_MESHES),
        "rocks": tuple(required_mesh(path) for path in ROCK_MESHES),
        "rock_face": required_mesh(ROCK_FACE_MESH),
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
            rock_scale = 0.29 + 0.025 * ((index + (0 if y < 0 else 2)) % 3)
            nature_mesh(
                nature["rocks"][(index + (0 if y < 0 else 3)) % len(nature["rocks"])],
                (x, y + (-18 + index * 9 if y < 0 else 18 - index * 9), -4),
                (rock_scale * 1.22, rock_scale, rock_scale * 0.28),
                f"VT_SteppingStone_{y}_{index}",
                (0, (index * 53 + (17 if y > 0 else 0)) % 360, 0),
                collision=True,
            )

    # Three building pairs create foreground/midground compression along the vista.
    for index, (x, y) in enumerate(((350, -1500), (350, 1500), (2100, -1500), (2100, 1500), (3750, -1500), (3750, 1500))):
        building(index, x, y, 0 if y < 0 else 180, mats)

    # Primary landmark: a restrained three-tier sect gate and jade moon-disc.
    for y in (-510, 510):
        primitive(ARCH_CYLINDER, (5550, y, 360), (0.72, 0.72, 7.2), f"VT_MainGate_Pillar_{y}", mats["wood"])
        primitive(ARCH_CHAMFER, (5550, y, 115), (1.15, 1.15, 0.55), f"VT_MainGate_Foot_{y}", mats["stone_dark"])
        primitive(ARCH_CHAMFER, (5550, y, 715), (1.16, 1.28, 0.26), f"VT_MainGate_Bracket_{y}", mats["wood"], collision=False)
    primitive(ARCH_CHAMFER, (5550, 0, 720), (1.0, 11.3, 0.42), "VT_MainGate_Lintel", mats["wood"])
    primitive(ARCH_CHAMFER, (5550, 0, 794), (1.14, 11.8, 0.20), "VT_MainGate_RoofDeck", mats["tile_wet"], collision=False)
    tiled_eave("VT_MainGate_Roof", (5550, 0, 824), 1260.0, 300.0, 13, mats["tile"], along_x=False)
    primitive(ARCH_CYLINDER, (5520, 0, 610), (2.25, 2.25, 0.20), "VT_MainGate_JadeDisc", mats["jade"], (90, 0, 0), collision=False)
    primitive(ARCH_CYLINDER, (5510, 0, 610), (1.35, 1.35, 0.22), "VT_MainGate_DiscCore", mats["stone_dark"], (90, 0, 0), collision=False)

    # Secondary landmarks: bell plinth, ancestor stone and old pines.
    primitive(CYLINDER, (2900, -760, 70), (1.4, 1.4, 0.55), "VT_BellPlinth", mats["stone"])
    primitive(CYLINDER, (2900, -760, 235), (0.75, 0.75, 1.8), "VT_Bell", mats["brass"], collision=False)
    nature_mesh(
        nature["rock_face"],
        (1180, 720, -6),
        (0.44, 0.22, 0.62),
        "VT_AncestorRockFace",
        (0, 102, 0),
        collision=True,
    )

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
        tree(index, x, y, scale, nature)
    for index, x in enumerate((100, 1050, 2000, 2950, 3900, 4850)):
        lantern(index * 2, x, -600, mats)
        lantern(index * 2 + 1, x, 600, mats)

    # Layer irregular scanned cliff faces for the distant skyline.  The varied
    # rotations and non-uniform scales break repetition but remain deterministic.
    mountain_clusters = (
        (13500, -6200, 4.8, 4.0, 6.2, 18),
        (14800, -3300, 5.6, 4.4, 7.4, 61),
        (16500, 400, 6.2, 4.8, 8.2, 104),
        (15000, 3600, 5.2, 4.1, 6.8, 147),
        (13300, 6500, 4.4, 3.8, 5.8, 203),
    )
    for index, (x, y, sx, sy, sz, yaw) in enumerate(mountain_clusters):
        nature_mesh(
            nature["rock_face"],
            (x, y, -480),
            (sx, sy, sz),
            f"VT_VistaCliff_{index:02d}",
            (0, yaw, 0),
            collision=False,
        )

    # Midground boulder groups stitch architecture into the garden and hide the
    # water-band ends without introducing any visible Engine primitive rocks.
    boulders = (
        (460, -1120, 0.72, 23),
        (980, 1115, 0.58, 139),
        (1740, -1090, 0.64, 278),
        (2670, 1100, 0.76, 331),
        (3510, -1110, 0.67, 72),
        (4380, 1090, 0.82, 196),
        (5100, -820, 0.92, 247),
    )
    for index, (x, y, scale, yaw) in enumerate(boulders):
        nature_mesh(
            nature["rocks"][index % len(nature["rocks"])],
            (x, y, -12),
            (scale * 1.12, scale * 0.88, scale),
            f"VT_GardenBoulder_{index:02d}",
            (0, yaw, 0),
            collision=False,
        )

    # Keep spawn on the collision slab.  The previous negative X spawn was
    # outside the authored foundation and let the pawn fall during packaged play.
    player_start = spawn(unreal.PlayerStart, (200, 0, 115), label="VT_PlayerStart")
    player_start.set_actor_rotation(unreal.Rotator(0, 0, 0), False)

    # Late-afternoon key, neutral sky fill, one volumetric layer and three warm lamps.
    sun = spawn(unreal.DirectionalLight, (0, 0, 1600), (-26, -34, -8), "VT_Sun")
    sun_component = sun.get_component_by_class(unreal.DirectionalLightComponent)
    set_property(sun_component, "intensity", 5.2)
    set_property(sun_component, "light_color", unreal.Color(255, 210, 162))
    set_property(sun_component, "atmosphere_sun_light", True)
    set_property(sun_component, "cast_volumetric_shadow", True)

    sky = spawn(unreal.SkyLight, (0, 0, 900), label="VT_SkyLight")
    sky_component = sky.get_component_by_class(unreal.SkyLightComponent)
    garden_hdri = unreal.load_asset("/Game/VisualTarget/HDRI/TC_NinomaruTeien_4K")
    if not garden_hdri:
        raise RuntimeError("Missing approved Ninomaru Teien HDRI; run tools/import-polyhaven-hdri.py")
    set_property(sky_component, "source_type", unreal.SkyLightSourceType.SLS_SPECIFIED_CUBEMAP)
    set_property(sky_component, "cubemap", garden_hdri)
    set_property(sky_component, "source_cubemap_angle", 205.0)
    set_property(sky_component, "cubemap_resolution", 1024)
    set_property(sky_component, "intensity", 0.65)
    set_property(sky_component, "real_time_capture", False)
    set_property(sky_component, "lower_hemisphere_is_solid_color", True)
    set_property(sky_component, "lower_hemisphere_color", unreal.LinearColor(0.004, 0.008, 0.006, 1.0))
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
