# M3 auto-conflict activity slice evidence — 2026-08-12

## Deliverable

The shared activity pipeline now supports `activity.conflict.cloud_palm_trial` at `facility.conflict.cloud_palm_trial`, targeting `npc.rival.cloud_palm_disciple`. Conflict uses the same deterministic Plan → Resolve → Commit → Present/save path as the other four activities; it does not add action-combat input, hitboxes, combos, dodge/parry or a combat Behavior Tree.

The resolver has an explicit Conflict branch, so it cannot fall through to cultivation rewards. Bounded seeded variation cannot reverse a large power gap. The result contains opponent, outcome, whether the opponent was defeated and permanent lifespan damage: zero on Great Success/Success, 7 days on Partial Success and 30 days on Failure.

Simulation advances the authored 30 minutes, applies lifespan damage and appends one canonical conflict record. The record participates in BLAKE3 state hashing and save v2 JSON round-trip. The committed-command gate rejects retries before a second consequence can be applied.

## Verification

- Native Editor and Development Game builds: PASS.
- Automation: 17/17 Success, including `TruongSinh.Activity.ConflictCommitAndSave` and the strong/weak bounded-RNG checks in `TruongSinh.Resolution.DeterministicAutoActivity`.
- Registry validator: `definitions=5, resolvers=cultivation,breakthrough,alchemy,formation,conflict`.
- M2B structural validator: PASS with 170 actors, 155 static meshes, 3 local lights and 14 materials.
- Portable Development package: `D:\GameDev\TruongSinhBuilds\Development-20260812-18\Windows\TruongSinhUE5.exe`.
- UAT full cook: 550 packages, `BUILD SUCCESSFUL`; archive has 1 pak, 2 utoc, 2 ucas and no `ue.projectstore`.
- Packaged M2B `-NullRHI` smoke stayed alive for 15 seconds, initialized Enhanced Input, loaded the five-definition registry and completed map load with zero Error/Fatal/Ensure/Assertion matches.

## Scope boundary

This verifies automatic conflict resolution and canonical consequences. Negotiation, payment, fleeing and sect-assistance routes (`CONFLICT-002`) are not implemented by this slice. Current encounter geometry is a functional native marker, not final character art, animation or VFX.
