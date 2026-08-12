"""Create the authored registry for the shared activity pipeline.

Run with UnrealEditor-Cmd after the native Editor target is built. The asset
contains setup data only; neither outcomes nor mutable state live here.
"""

import unreal


ASSET_PATH = "/Game/Data/DA_ActivityRegistry"
DESTINATION_PATH = "/Game/Data"


def stable_id(value):
    result = unreal.TruongSinhStableId()
    result.set_editor_property("value", value)
    return result


def conflict_route(route_id, duration, difficulty, relationship_id="", asset_id="", requires_sect=False):
    result = unreal.TruongSinhConflictRouteDefinition()
    result.set_editor_property("route_id", stable_id(route_id))
    result.set_editor_property("duration_minutes", duration)
    result.set_editor_property("difficulty_or_target_power", difficulty)
    if relationship_id:
        result.set_editor_property("required_relationship_id", stable_id(relationship_id))
    if asset_id:
        result.set_editor_property("required_owned_asset_id", stable_id(asset_id))
    result.set_editor_property("requires_sect_membership", requires_sect)
    return result


def definition(activity_id, facility_id, resolver_id, duration, minimum, difficulty,
               technique, preparation, environment, output_id="", maximum_output=0,
               formation_effect_id="", formation_duration=0, conflict_opponent_id=""):
    result = unreal.TruongSinhActivityDefinition()
    result.set_editor_property("activity_id", stable_id(activity_id))
    result.set_editor_property("facility_id", stable_id(facility_id))
    result.set_editor_property("method_id", stable_id("method.five_elements_breathing"))
    result.set_editor_property("location_id", stable_id("zone.lower_realm.dev_smoke"))
    result.set_editor_property("resolver_id", resolver_id)
    result.set_editor_property("duration_minutes", duration)
    result.set_editor_property("minimum_cultivation_units", minimum)
    result.set_editor_property("difficulty_or_target_power", difficulty)
    result.set_editor_property("technique_modifier_units", technique)
    result.set_editor_property("preparation_modifier_units", preparation)
    result.set_editor_property("environment_modifier_units", environment)
    if output_id:
        result.set_editor_property("output_id", stable_id(output_id))
        result.set_editor_property("maximum_output_units", maximum_output)
    if formation_effect_id:
        result.set_editor_property("formation_effect_id", stable_id(formation_effect_id))
        result.set_editor_property("formation_duration_minutes", formation_duration)
    if conflict_opponent_id:
        result.set_editor_property("conflict_opponent_id", stable_id(conflict_opponent_id))
        result.set_editor_property("conflict_avoidance_routes", [
            conflict_route("conflict.route.negotiate", 20, 6000,
                           relationship_id="relationship.cloud_palm_disciple.acquainted"),
            conflict_route("conflict.route.pay", 10, 0,
                           asset_id="asset.compensation.cloud_palm_pouch"),
            conflict_route("conflict.route.flee", 15, 5200),
            conflict_route("conflict.route.sect_assist", 30, 0, requires_sect=True),
        ])
    return result


unreal.EditorAssetLibrary.make_directory(DESTINATION_PATH)
registry = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
if not registry:
    factory = unreal.DataAssetFactory()
    factory.set_editor_property(
        "data_asset_class", unreal.TruongSinhActivityRegistryDataAsset
    )
    registry = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        "DA_ActivityRegistry",
        DESTINATION_PATH,
        unreal.TruongSinhActivityRegistryDataAsset,
        factory,
    )

if not registry:
    raise RuntimeError(f"Could not create {ASSET_PATH}")

registry.set_editor_property("definitions", [
    definition(
        "activity.cultivation.breathing_cycle",
        "facility.cultivation.dev_smoke",
        "cultivation",
        480,
        0,
        6500,
        350,
        300,
        450,
    ),
    definition(
        "activity.breakthrough.foundation",
        "facility.breakthrough.foundation",
        "breakthrough",
        720,
        800,
        6500,
        350,
        300,
        450,
    ),
    definition(
        "activity.alchemy.qingxin_pill",
        "facility.alchemy.qingxin",
        "alchemy",
        360,
        0,
        6500,
        400,
        350,
        300,
        "pill.qingxin",
        3,
    ),
    definition(
        "activity.formation.spirit_gathering",
        "facility.formation.spirit_gathering",
        "formation",
        240,
        350,
        6400,
        350,
        500,
        400,
        formation_effect_id="effect.formation.spirit_gathering",
        formation_duration=10080,
    ),
    definition(
        "activity.conflict.cloud_palm_trial",
        "facility.conflict.cloud_palm_trial",
        "conflict",
        30,
        500,
        7200,
        550,
        250,
        150,
        conflict_opponent_id="npc.rival.cloud_palm_disciple",
    ),
])

unreal.EditorAssetLibrary.save_loaded_asset(registry, only_if_is_dirty=False)
unreal.log(f"Created shared activity registry: {ASSET_PATH}")
