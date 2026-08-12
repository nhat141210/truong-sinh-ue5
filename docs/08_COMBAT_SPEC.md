# Combat spec — đấu pháp ngũ hành theo lượt, trình diễn 3D

## 1. Hợp đồng combat

Combat là một mô phỏng theo lượt deterministic, dùng build tu sĩ, resource nguyên tố, technique/ability, equipment, status và seed để giải quyết. UE5 arena, camera, animation, Niagara và âm thanh chỉ phát **combat event log** sau khi rule engine đã quyết định event.

Combat phải cho người chơi cùng loại lựa chọn chiến thuật có chiều sâu như benchmark: quản lý linh khí/ngũ hành, thứ tự action, cost, status, defense, build synergy và đối thủ có policy. Công thức, số lượng action/hand/draw hoặc bảng damage cụ thể chưa được audit phải nằm trong data `REF_PENDING`, không được bịa thành parity.

## 2. Input, output và authority

Tên C++ authoritative nằm trong
`Source/TruongSinhCombat/Public/Combat/TruongSinhCombatSimulation.h`:

- `FTruongSinhCombatSnapshot`: encounter ID, combat revision, turn, acting
  combatant, immutable combatants và RNG snapshot.
- `FTruongSinhCombatCommand`: `FTruongSinhActionCommand`, technique ID và target
  IDs. Lựa chọn phụ về sau phải là typed payload, không là string map.
- `FTruongSinhCombatResult`: committed flag, canonical action result, ordered
  replay events và final snapshot.
- `ITruongSinhCombatSimulation::Resolve`: pure simulation boundary.

Rules engine là authority duy nhất cho `FTruongSinhCombatResult`. UI gửi command;
presentation chỉ đọc replay events. Không actor, ability montage, animation notify
hay Blueprint damage event nào được mutate combatant snapshot. World delta không
được apply trực tiếp từ arena: composition root chuyển combat result thành domain
events rồi commit qua canonical action transaction.

## 3. Combat state

Mỗi combatant tối thiểu có:

- ID, affiliation, realm/build tags và alive/escaped state.
- Vital state: health/injury/shield (tên/category chính xác được audit).
- Resource ledger theo element tags: Kim, Mộc, Thủy, Hỏa, Thổ và neutral/derived slots nếu ruleset yêu cầu.
- Technique loadout, available actions, cooldown/charge.
- Status stack/duration/source, equipment modifiers và combat-local flags.
- AI policy ID hoặc human-controlled flag.

Tất cả số có authoritative fixed point hoặc rounding policy rõ ràng. UI display rounding không được dùng lại vào formula.

## 4. Phase machine

Phase cơ bản được implement từ đầu để hỗ trợ audit, nhưng policy/config mới quyết định bước nào active trong ruleset:

```text
CreateSnapshot
→ Setup
→ TurnStart
→ ResourceRefresh / DrawOrPrepare
→ CommandSelection
→ ValidateCommand
→ ResolveCosts
→ ResolveEffects
→ TriggerReactions
→ StatusTick / Cleanup
→ OutcomeCheck
→ NextTurn hoặc CommitWorldDelta
```

Mỗi phase ghi `CombatEvent` có `sequence`, source, targets, rule ID, random roll ID (nếu có), before/after delta. Thứ tự trigger/reaction được sort stable theo `priority`, `source order`, `effect ID` và test golden.

## 5. Ability resolution

### Definition tối thiểu

| Field | Ý nghĩa |
| --- | --- |
| `AbilityId` | Stable primary asset ID |
| `RequiredTags` | Realm/technique/status/equipment condition |
| `TargetRule` | Self, single, group, encounter target… |
| `ElementCostRule` | Resource requirements/convert/consume behavior |
| `EffectList` | Damage, shield, heal, status, resource, draw/prepare, move… |
| `Timing` | Phase/priority/trigger point |
| `CooldownOrChargeRule` | Audit-driven constraint |
| `PresentationCueId` | Animation/VFX/camera mapping, mechanics-free |
| `AiHints` | Data for policy scoring |

### Pipeline

1. Validate turn owner, target, required tags, cooldown and resources.
2. Reserve/consume cost according to rule.
3. Resolve ability effects in declared/stable trigger order.
4. Emit deltas and reactions, checking outcome after each configured boundary.
5. Record all generated event IDs before notifying presentation.
6. On finish, resolve reward/loss/escape/world patch as one `CombatCommit` transaction.

Invalid command never consumes resource. A rejected command returns a localised reason key and leaves snapshot unchanged.

## 6. Ngũ hành và build system

- Element interactions use Gameplay Tags (`Element.Metal`, `Element.Wood`, `Element.Water`, `Element.Fire`, `Element.Earth`) and data tables, not nested `switch` statements in UI.
- Sinh/khắc, conversion, bonus, resistance, overflow, draw/prepare behavior and rounding are configuration data, then locked by audit tests.
- A technique can add tags, transform resource rules, grant ability sets or modify event priorities. It must state its source and realm requirement.
- Equipment/pill/status apply modifiers through ordered modifier layers: base → persistent build → encounter → temporary combat → final rounding. The exact audited ordering is written into golden tests.

## 7. AI policy

AI is a deterministic scorer, not a frame-ticking Behavior Tree:

```text
Enumerate valid commands
→ score commands from state + AiHints + policy weights
→ tie-break through Combat RNG stream
→ emit AI decision trace
```

The trace records candidate IDs, scores, filters and chosen command. Difficulty may use a different policy data asset, never hidden stat boosts unless audit says so.

## 8. 3D presentation contract

1. Encounter requests `CreateCombatSnapshot` and freezes the relevant world interaction.
2. `CombatArenaDirector` loads/spawns an arena visual matching `ArenaTags`.
3. `CombatReplayController` consumes ordered events; it never asks rules engine to reroll.
4. UI may queue command selection only in `CommandSelection`; fast-forward affects playback speed only.
5. At replay end, `CombatCommit` applies `WorldDelta` once; the player returns to the rebuilt zone state.

If a cue/asset fails, show a fallback marker and continue replay. Do not abandon a resolved result or let a cinematic block save recovery.

## 9. Combat audit matrix khởi tạo

| ID | Câu hỏi cần audit | Project proof cần có | Trạng thái |
| --- | --- | --- | --- |
| `CMB-A01` | setup snapshot và đầu turn gồm gì? | `Automation.Combat.PhaseOrder` fixture | `UNKNOWN` |
| `CMB-A02` | resource nào refresh/draw/retain? | ledger before/after test | `UNKNOWN` |
| `CMB-A03` | command target/cost validation? | invalid command test | `UNKNOWN` |
| `CMB-A04` | elemental interactions và rounding order? | formula golden table | `UNKNOWN` |
| `CMB-A05` | status/timing/reaction priority? | trigger ordering replay | `UNKNOWN` |
| `CMB-A06` | win/loss/escape/resolve route? | world commit test | `UNKNOWN` |
| `CMB-A07` | enemy policy/difficulty behavior? | AI trace fixture | `UNKNOWN` |
| `CMB-A08` | reward/injury/cooldown persistent state? | save/load combat result fixture | `UNKNOWN` |

## 10. Vertical slice combat definition

Slice chứa content gốc, có:

- Một player build cấp đầu, một opponent archetype và một arena ngoài trời.
- Ít nhất năm ability khác loại: tạo resource, tiêu resource, defense/status, direct effect, utility/conditional effect.
- Hai type element interaction để chứng minh data-driven rule và cue mapping.
- Một AI policy readable qua debug trace.
- Win, loss và exit/escape branch nếu audit chứng minh branch tương ứng cần có.
- Deterministic replay test: cùng snapshot + command list cho cùng event/result hash, kể cả khi skip replay.

Slice không được dùng hand-authored Blueprint damage hoặc scripted enemy outcome.
