# Vấn Đạo Tông — runtime/UX static audit

Audit scope: startup routing, native player shell, HUD creation, input/mouse
capture, interaction targeting/planner, and PIE/Standalone risk review. This
audit is static-only; it does not claim a fresh PIE or Standalone run.

## Static evidence

| Area | Evidence | Result |
|---|---|---|
| Startup map | `Config/DefaultEngine.ini`: `EditorStartupMap` and `GameDefaultMap` both point to `/Game/Maps/VisualTarget/L_VanDaoTong` | PASS |
| Cook routing | `Config/DefaultGame.ini`: `L_VanDaoTong` is in `MapsToCook` | PASS |
| Game mode | `Source/TruongSinhUE5/Private/Gameplay/TruongSinhGameMode.cpp:6-10` assigns the native character and player controller | PASS |
| HUD creation | `Source/TruongSinhUE5/Private/Gameplay/TruongSinhPlayerController.cpp:170-174` creates the native HUD and adds it to the player screen | PASS |
| Enhanced Input | `Source/TruongSinhUE5/Private/Gameplay/TruongSinhCharacter.cpp:102-115` maps WASD, mouse, Space, E, Esc and conflict keys 1-5; `:150-161` installs the mapping context on pawn restart | PASS |
| Mouse capture | `TruongSinhPlayerController.cpp:241-252` applies game-only input, hides the cursor, captures permanently and locks the viewport; `:652-664` restores a visible cursor/input mode on pause and reapplies gameplay capture on resume | PASS (static) |
| Interaction prompt | `TruongSinhPlayerController.cpp:209-238` traces from the follow camera and updates the contextual HUD prompt; `:597-630` filters offers through the shared deterministic selector and registry | PASS (static) |
| Conflict planner | `TruongSinhPlayerController.cpp:314-356` computes canonical eligibility and presents five routes; `:563-595` validates selection and handles close/retry | PASS (static) |

## Risk/gap list

- There is no title/start-button screen. The startup route is direct-to-play;
  therefore a hidden cursor at gameplay launch is intentional. If product
  requires a click-to-start splash, it must be added as a separate UI state.
- A fresh PIE and Standalone input/Continue capture remains a manual gate. This
  audit does not convert earlier smoke/package logs into Vấn Đạo Tông runtime
  evidence.
- Interaction depends on a camera `ECC_Visibility` line trace hitting the native
  provider's collision. Visual shells are non-colliding by construction, but a
  final manual pass should verify the prompt appears at each of the five sites.
- Remote Editor Python was not available during this audit (`No Unreal Python
  node found`), so no map mutation or editor-writer operation was performed.

## Static commands

```text
git diff --check                         PASS
startup/cook/native-file checks          PASS
native runtime source files changed      NONE
```

