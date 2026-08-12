# Data schemas

## ID và units

- Stable ID: lowercase `[a-z0-9._-]`.
- Canonical số nguyên: phút, ngày, units, basis points; không float cho outcome.
- Player text: String Table key.
- Asset reference: Primary Asset ID/soft reference; save không hard-reference Actor.

## Activity definition

`UTSActivityDefinition`:

- ActivityId, Type, ResolverId.
- AllowedMethod/Facility/Strategy IDs.
- Duration/cost policy IDs.
- Eligibility predicates.
- Result table/factor policy.
- Replay profile và fallback string key.

`FActivityPlan`: command, activity/method/facility/location/resources/duration/strategy.

`FAutoResolutionResult`: outcome/reason, final/target score, time delta, factor list, ordered beats.

## Life state

`FLifespanState`: biological age, base, realm, technique, pill/resource và permanent damage days.

`FSoulState`: SoulId, integrity, known techniques, memories, scars/karma và vessel history.

`FVesselState`: vessel/identity/root/realm, cultivation, lifespan, sect, relations và property.

## NPC/event

`FNpcState`: identity/vessel/life, location, schedule state, occupation, goals, relations, faction và flags. Actor transform không canonical ngoài checkpoint cần thiết.

`UTSAuthoredEventDefinition`: EventId, conditions, role slots, zone/time window, cooldown, priority, choices, consequences và presentation beats.

## Zone/home

`UTSZoneDefinition`: ZoneId, map soft path, size class, travel edges/time, weather/lighting profiles, spawn anchors và scalability limits.

`UTSFacilityDefinition`: ID, slots, build cost/time, allowed activity/resolver IDs, modifiers và visual variants.

## Validation

Fail khi duplicate/invalid ID, missing string key, missing resolver/cue fallback, dangling relation/asset, negative canonical value, activity không có test fixture hoặc asset thiếu provenance.
