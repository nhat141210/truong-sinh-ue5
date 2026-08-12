"""Validate the shared runtime activity registry after editor authoring."""

import unreal


ASSET_PATH = "/Game/Data/DA_ActivityRegistry"
registry = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
if not registry:
    raise RuntimeError(f"Missing {ASSET_PATH}")

definitions = registry.get_editor_property("definitions")
if len(definitions) != 2:
    raise RuntimeError(f"Expected exactly two authored M3 definitions, got {len(definitions)}")

by_facility = {entry.get_editor_property("facility_id").get_editor_property("value"): entry for entry in definitions}
for facility, resolver, duration in (
    ("facility.cultivation.dev_smoke", "cultivation", 480),
    ("facility.breakthrough.foundation", "breakthrough", 720),
):
    entry = by_facility.get(facility)
    if not entry:
        raise RuntimeError(f"Missing registry definition for {facility}")
    if str(entry.get_editor_property("resolver_id")) != resolver:
        raise RuntimeError(f"Wrong resolver for {facility}")
    if entry.get_editor_property("duration_minutes") != duration:
        raise RuntimeError(f"Wrong duration for {facility}")

activity_ids = set()
for entry in definitions:
    activity_id = entry.get_editor_property("activity_id").get_editor_property("value")
    facility_id = entry.get_editor_property("facility_id").get_editor_property("value")
    method_id = entry.get_editor_property("method_id").get_editor_property("value")
    location_id = entry.get_editor_property("location_id").get_editor_property("value")
    if not all((activity_id, facility_id, method_id, location_id)):
        raise RuntimeError("Registry contains an empty stable ID")
    if activity_id in activity_ids:
        raise RuntimeError(f"Duplicate activity ID: {activity_id}")
    activity_ids.add(activity_id)

unreal.log("Activity registry validation PASS: definitions=2, resolvers=cultivation,breakthrough")
