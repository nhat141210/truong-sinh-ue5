# 18 — Trình bày combat 3D theo lượt

## Mục tiêu

Combat giữ luật turn-based, linh khí/ngũ hành/công pháp và outcome parity theo audit. UE5 chỉ nâng lớp nhìn/nghe: arena 3D, nhân vật, animation, Niagara, camera, âm thanh và replay rõ ràng.

Không chuyển V1 sang action combat. Reflex, camera distance, animation timing hoặc khả năng nhấn nhanh không được tăng/giảm chance, damage, target validity hoặc result.

## Hai lớp tách biệt

~~~text
FTruongSinhCombatSnapshot + FTruongSinhCombatCommand
  → ITruongSinhCombatSimulation::Resolve
  → FTruongSinhCombatResult + ordered replay events (canonical)
  → UTruongSinhCombatPresentationDirector
  → animation / camera / Niagara / audio / UI
~~~

Combat simulation có thể chạy complete upfront hoặc resolve each accepted turn. Dù chọn cách nào, mỗi accepted combat command phải commit canonical state trước khi visual cue bắt đầu. Presentation có thể pause/skip/fast-forward replay nhưng không re-resolve action.

## Combat state machine

~~~text
Explore
  → EncounterPending
  → CombatTransitionIn
  → CombatPlanning
  → CombatResolve
  → CombatReplay
  → CombatPlanning (next turn)
  → CombatResult
  → CombatTransitionOut
  → Explore
~~~

- EncounterPending build snapshot from world state and locks unrelated world interaction.
- CombatPlanning accepts only validated typed combat command.
- CombatResolve runs deterministic simulation and emits events.
- CombatReplay consumes ordered log. Input remains limited to speed/skip until safe boundary.
- CombatResult writes reward/defeat/quest changes once, then offers return transition.
- On presentation failure, show summary/log and still allow CombatResult continuation.

## APIs

### Simulation interface

~~~text
class ITruongSinhCombatSimulation {
  FTruongSinhCombatResult Resolve(
    const FTruongSinhCombatSnapshot& Snapshot,
    const FTruongSinhCombatCommand& Command);
};
~~~

`FTruongSinhCombatCommand` mang revision guard qua action envelope, technique ID và
target IDs. Typed choice payload sẽ được thêm khi audit xác định loại lựa chọn hợp
lệ. Nó không chứa damage, random roll, animation name hoặc presentation preference.

### Replay types

~~~text
FTruongSinhCombatReplayEvent {
  FTruongSinhStableId EventId;
  FTruongSinhStableId SourceId;
  FTruongSinhStableId TargetId;
  FTruongSinhStableId PresentationCueId;
  int64 Sequence;
  TArray<FTruongSinhCombatValue> QuantizedValues;
}
~~~

Duration, transform hint và camera profile nằm trong `PresentationCue` data, không
trong event canonical. Một action event không được thiếu cue fallback. Presentation
DTO có thể bổ sung dữ liệu visual đã resolve nhưng không được sửa replay event gốc.

### Presentation interface

~~~text
ICombatPresentation {
  BeginCombat(const FCombatPresentationContext& Context);
  PlayReplay(const FCombatReplayLog& Log, EReplaySpeed Speed);
  SkipToSafeBoundary();
  EndCombat(const FTruongSinhCombatResult& Result);
  OnCueFailed(const FCombatReplayEvent& Event);
}
~~~

## Arena design

Arena là presentation profile, không quyết định mechanics:

- Cỡ mặc định 22–36 m đường kính visual; đủ camera orbit nhẹ và displacement cosmetic.
- Ground có contact/readability, low clutter center 60%.
- Source/target placement derived từ snapshot formation + stable entity ordering.
- Environment tag đổi terrain, sky, prop, ambient và cue variation, không đổi formula trừ khi snapshot rule nói rõ.
- Có entry/exit anchor, camera-safe volume, no hidden collision on center line.
- Một encounter có thể reuse arena profile; không bắt buộc tạo map riêng mỗi trận.

## Camera

UTSCombatCameraDirector dùng camera state data, không Sequencer-only logic:

| State | Camera | Giới hạn |
|---|---|---|
| Planning | 3/4 elevated, 8–13 m | target + UI luôn đọc được |
| Cast | push/dolly ngắn source → target | <= 1.2 s cho standard action |
| Impact | medium framing target | không mất source/target context |
| Hero | controlled rail/cut 1–2 shot | <= 3.0 s, cho phép skip |
| Result | stable wide | show victory/defeat + UI |
| Accessibility | fixed tactical camera | giảm motion, no shake |

Camera không rotate nhanh quá 90°/s trong default replay, không roll, không first-person forced. Combat player luôn biết lượt nào, target nào và result nào xảy ra.

## Action timing

Presentation speed targets:

| Cue tier | Enter + cast + impact + recover | Mục đích |
|---|---:|---|
| Micro/status | 0.15–0.40 s | không kéo dài turn |
| Standard action | 0.70–1.40 s | action rõ, giữ nhịp |
| Multi-hit | 1.2–2.4 s | gộp visual events khi safe |
| Hero technique | 2.0–3.5 s | hiếm, có skip |
| Breakthrough/tribulation | 4–12 s segmented | event lớn, reduced flash |

Fast x2 scales animation/camera/audio fade, not simulation. Skip jumps to latest completed command boundary and applies no extra state.

## Character positioning và hit representation

- Formation position cosmetic uses deterministic stable slot so replay/load nhìn consistent.
- Knockback, dash, airborne are cosmetic bounded inside arena; simulation range/state does not follow physics result.
- Hit can spawn montage, material flash, decal/VFX and floating number; visual damage number comes from replay DisplayMagnitude only.
- Death/defeat pose does not destroy canonical entity until combat result commit. If corpse needs world presence, it is a world patch from result.
- Projectile uses source/target hints, has max flight time and fallback impact if line trace misses due to level geometry.

## Elemental language

- Kim: precise linear shards/rings, short sharp impact.
- Mộc: growth/coil/healing lattice, slower expanding movement.
- Thủy: flow/arc/ripple, directional motion.
- Hỏa: flare/heat trail, controlled brightness.
- Thổ: ground rise/weight/dust, strong vertical mass.

Every element also has icon/pattern/text on UI; color-only signaling is forbidden.

## UI integration

Combat screen receives FCombatViewModel after every resolution. It displays:

- Active actor and phase.
- Legal actions and why an action is disabled.
- Resources and statuses from snapshot.
- Selected targets and preview only when simulation can validate it.
- Combat log with replay event sequence.
- Replay speed: 1x, 2x, Skip. Auto only if parity audit defines an AI policy.

No input creates local optimistic damage. UI can visually pre-highlight valid
range/target but must submit `FTruongSinhCombatCommand` to service.

## Failure and recovery

- Missing animation: play neutral pose + UI/log result.
- Missing Niagara/audio: skip cue, do not delay result.
- Arena streaming failure: use fallback neutral arena profile; preserve snapshot/result.
- Actor absent: spawn proxy at transform hint; no new simulation entity.
- Player alt-tabs/pauses: pause visual time; no command commits until explicit resume.
- Crash after command resolve: autosave/journal resumes at next safe boundary, never repeats command based on UI guess.

## Combat test plan

- Golden cases for each audited action/element/status compare turn result, state hash and replay ordering.
- Watch at 1x, watch at 2x, skip, and run low quality: final hash identical.
- 30/60/120 FPS playback: event sequence/camera safe-boundary behavior stable.
- Every cue profile has missing-asset fallback functional test.
- Target selection rejects stale revision/invalid target without RNG draw.
- Arena enter/exit leaves no orphan UI, actor, Niagara or input context.
- 20 sequential combat soak test stays within CPU/GPU/memory budget.
