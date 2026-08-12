# M3 conflict avoidance planner evidence — 2026-08-12

## Deliverable

The shared conflict activity now opens one native jade/gold planner before resolution. Keyboard choices are `1` Fight, `2` Negotiate, `3` Compensation, `4` Flee and `5` Sect assistance; locked choices remain visible with a Vietnamese eligibility reason, and Escape cancels the planner before pause handling.

All four avoidance routes are authored inside the existing conflict registry definition. Negotiate requires the authored relationship, compensation requires and atomically consumes one canonical owned asset, flee is always selectable and deterministically resolves, and sect assistance requires the current canonical SectId. Simulation revalidates every prerequisite at commit; stale state rejects without advancing time or consuming an asset.

Conflict records now persist approach, avoided status, consumed asset and assisting sect in the canonical hash and save v2. Avoidance cannot defeat the opponent or apply permanent damage; fight behavior remains unchanged. No currency, action-combat subsystem or combat-only widget framework was introduced.

## Verification

- Native Editor and Development Game builds: PASS.
- Automation: 18/18 Success, including `TruongSinh.Activity.ConflictAvoidanceRoutes`.
- Registry authoring/validator: PASS with 5 definitions and four unique avoidance route IDs.
- Portable Development package: `D:\GameDev\TruongSinhBuilds\Development-20260812-19\Windows\TruongSinhUE5.exe`.
- UAT full cook: 550 packages, `BUILD SUCCESSFUL`; archive has pak/IoStore containers and no project-store dependency.
- Packaged M2B `-NullRHI` smoke stayed alive for 15 seconds, initialized Enhanced Input, loaded all five activity definitions and completed map load with zero Error/Fatal/Ensure/Assertion matches.

## Scope boundary

This proves canonical route selection and consequences. Final dialogue writing, social/economy depth, cinematic replay, character animation, audio and production environment art remain later work.
