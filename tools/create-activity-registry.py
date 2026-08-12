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


def definition(activity_id, facility_id, resolver_id, duration, minimum, difficulty,
               technique, preparation, environment):
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
])

unreal.EditorAssetLibrary.save_loaded_asset(registry, only_if_is_dirty=False)
unreal.log(f"Created shared activity registry: {ASSET_PATH}")
