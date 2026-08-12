"""Validate the shared runtime activity registry after editor authoring."""

import unreal


ASSET_PATH = "/Game/Data/DA_ActivityRegistry"
registry = unreal.EditorAssetLibrary.load_asset(ASSET_PATH)
if not registry:
    raise RuntimeError(f"Missing {ASSET_PATH}")

definitions = registry.get_editor_property("definitions")
if len(definitions) != 5:
    raise RuntimeError(f"Expected exactly five authored M3 definitions, got {len(definitions)}")

by_facility = {entry.get_editor_property("facility_id").get_editor_property("value"): entry for entry in definitions}
for facility, resolver, duration in (
    ("facility.cultivation.dev_smoke", "cultivation", 480),
    ("facility.breakthrough.foundation", "breakthrough", 720),
    ("facility.alchemy.qingxin", "alchemy", 360),
    ("facility.formation.spirit_gathering", "formation", 240),
    ("facility.conflict.cloud_palm_trial", "conflict", 30),
):
    entry = by_facility.get(facility)
    if not entry:
        raise RuntimeError(f"Missing registry definition for {facility}")
    if str(entry.get_editor_property("resolver_id")) != resolver:
        raise RuntimeError(f"Wrong resolver for {facility}")
    if entry.get_editor_property("duration_minutes") != duration:
        raise RuntimeError(f"Wrong duration for {facility}")

alchemy = by_facility["facility.alchemy.qingxin"]
if alchemy.get_editor_property("output_id").get_editor_property("value") != "pill.qingxin":
    raise RuntimeError("Alchemy output ID is missing")
if alchemy.get_editor_property("maximum_output_units") != 3:
    raise RuntimeError("Alchemy maximum output is wrong")

formation = by_facility["facility.formation.spirit_gathering"]
if formation.get_editor_property("formation_effect_id").get_editor_property("value") != "effect.formation.spirit_gathering":
    raise RuntimeError("Formation effect ID is missing")
if formation.get_editor_property("formation_duration_minutes") != 10080:
    raise RuntimeError("Formation duration is wrong")

conflict = by_facility["facility.conflict.cloud_palm_trial"]
if conflict.get_editor_property("conflict_opponent_id").get_editor_property("value") != "npc.rival.cloud_palm_disciple":
    raise RuntimeError("Conflict opponent ID is missing")
routes = conflict.get_editor_property("conflict_avoidance_routes")
route_ids = {route.get_editor_property("route_id").get_editor_property("value") for route in routes}
if route_ids != {"conflict.route.negotiate", "conflict.route.pay", "conflict.route.flee", "conflict.route.sect_assist"}:
    raise RuntimeError(f"Conflict avoidance routes are wrong: {route_ids}")

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

unreal.log("Activity registry validation PASS: definitions=5, resolvers=cultivation,breakthrough,alchemy,formation,conflict")
