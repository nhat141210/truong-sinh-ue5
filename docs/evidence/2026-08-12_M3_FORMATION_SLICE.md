# M3 formation activity slice evidence — 2026-08-12

## Deliverable

The shared activity pipeline now supports `activity.formation.spirit_gathering` at `facility.formation.spirit_gathering`. The resolver produces a deterministic outcome and, on a non-failure result, an effect ID, integrity in basis points and a seven-day game-time duration.

Simulation commits the formation atomically with its 240-minute construction cost. Canonical formation state contains the originating command, blueprint, effect, integrity and absolute expiry minute. It participates in the canonical hash, save v2 round-trip and deterministic time-based expiry. Retry safety remains inherited from the committed command ID gate.

Both `/Game/Maps/Dev/L_Dev_Smoke` and `/Game/Maps/VisualTarget/L_M2B_Corridor` contain the native formation interaction site. PlayerController selects the formation definition from the shared registry and presents the effect, integrity, duration and construction time through the existing HUD result path.

## Verification

- Native Editor and Development Game builds: PASS.
- Automation: 16/16 Success, including `TruongSinh.Activity.FormationStateAndExpiry`.
- Registry validator: `definitions=4, resolvers=cultivation,breakthrough,alchemy,formation`.
- M2B structural validator: PASS with 169 actors, 155 static meshes, 3 local lights and 14 materials.
- Portable Development package: `D:\GameDev\TruongSinhBuilds\Development-20260812-17\Windows\TruongSinhUE5.exe`.
- UAT full cook: 550 packages, `BUILD SUCCESSFUL`; archive has pak/IoStore containers and no project-store dependency.
- Packaged M2B `-NullRHI` smoke stayed alive for 15 seconds, initialized Enhanced Input, loaded the four-definition registry and completed map load with zero Error/Fatal/Ensure/Assertion matches.

## Scope boundary

This is a verified native gameplay slice, not final formation art or VFX. The current site uses restrained engine primitives so canonical behavior and packaging remain testable while production environment art is still pending.
