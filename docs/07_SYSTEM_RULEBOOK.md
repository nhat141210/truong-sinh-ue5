# System rulebook — simulation deterministic

## 1. Nguyên tắc kiến trúc luật

Simulation nhận state + command + seed và trả result/event log. Nó không đọc widget, input device, `DeltaSeconds`, actor transform, animation, Lumen, Niagara hay timing streaming.

```text
Validate request
→ Preview cost / ask confirmation
→ Begin transaction
→ Consume required resources
→ Resolve deterministic rule(s)
→ Advance time nếu action có cost
→ Run scheduled world updates
→ Commit state + append event log
→ Save/checkpoint policy
→ Notify presentation/UI
```

Nếu transaction fail trước commit, canonical state không đổi. Nếu presentation fail sau commit, result vẫn hợp lệ và reload phải phục hồi được.

## 2. Canonical state

| State | Owner | Yêu cầu |
| --- | --- | --- |
| `FGameClockState` | Chronology | date, tick index, scheduled event cursor |
| `FTruongSinhRngState` | Core | algorithm version + master seed + named stream states |
| `FPlayerCultivationState` | Cultivation | realm, progress, lifespan ledger, roots, dao, techniques |
| `FInventoryState` | Inventory | item stacks, equipment, containers, ownership |
| `FHomeState` | Home | facility slots, production queues, field state |
| `FWorldState` | World | location graph, flags, encounters, global modifiers |
| `FNpcState` | World | age, realm, location, relation, goals, availability |
| `FSectState` | Sect | membership, rank, contribution, permissions/obligations |
| `FQuestState` | Quest | graph node, branch flags, deadline, reward status |
| `FEconomyState` | Economy | market stock/price, auctions, trade routes |
| `FCombatArchive` | Combat | resolved combat result/replay references, never live UI state |

Mọi entity có stable ID; save lưu ID + versioned state, không lưu pointer UObject/Actor.

## 3. Action contract

Mọi action gameplay dùng đúng envelope phản chiếu trong
`Source/TruongSinhCore/Public/Core/TruongSinhTypes.h`:

```cpp
struct FTruongSinhActionCommand {
    FGuid CommandId;                    // correlation/idempotency, không phải RNG
    FTruongSinhStableId ActionId;
    FTruongSinhStableId InstigatorId;
    TArray<FTruongSinhStableId> TargetIds;
    FInstancedStruct Payload;            // typed USTRUCT theo domain
    int64 ExpectedWorldRevision;
    int64 Sequence;
};

struct FTruongSinhActionResult {
    ETruongSinhActionStatus Status;      // Rejected hoặc Committed
    FTruongSinhStableId ActionId;
    FTruongSinhStableId ReasonId;
    int64 PreviousWorldRevision;
    int64 NewWorldRevision;
    TArray<FTruongSinhDomainEvent> Events;
    FString StateHash;
};
```

`ExpectedWorldRevision` ngăn double-click/late UI commit. `CommandId` làm action
idempotent khi UI retry; V1 offline vẫn phải xử lý để save/animation không
double-cost. Mỗi loại action khai báo một `USTRUCT` payload kế thừa
`FTruongSinhActionPayload`; cấm `TMap<FName, FString>` hoặc JSON mơ hồ ở biên
canonical. Command bị reject không đổi revision, lịch hoặc RNG. Command committed
đổi state đúng một lần, tạo event có thứ tự và state hash.

## 4. Thời gian, tuổi thọ và scheduler

### M1 proof hiện có

`FTruongSinhGameSimulation` hiện chỉ triển khai internal action
`core.advance_time` với typed `FTruongSinhAdvanceTimePayload`. Nó lưu
`ElapsedDays`, revision và committed CommandId để chứng minh transaction,
idempotency, event, hash và save round-trip. Đây không phải action trực tiếp cho
người chơi và không phải công thức parity. Mapping tu luyện/travel/craft → time
cost, calendar tháng/năm và scheduler vẫn phải audit trước khi triển khai.

### Clock

- Lịch được biểu diễn bằng integer canonical (`Year`, `Month`, `Day`, `Tick`), có compare/advance không phụ thuộc locale.
- `ActionTimeCost` nằm trong data và phải audit trước khi bật action production.
- `AdvanceTime` không loop từng frame. Nó chạy event queue theo mốc deadline/tick hiệu lực để tăng tốc cả nhiều năm.
- Thứ tự scheduler bắt buộc được lock sau audit: `PreAdvance → Resource/Home → NPC → Sect/Quest → Economy → Encounter → Lifespan → PostEvent`. Nếu benchmark chứng minh thứ tự khác, cập nhật một chỗ trong policy và golden tests.

### Lifespan ledger

Mỗi delta tuổi thọ có record `(sourceTag, date, amount, reasonKey, transactionId)`. Không ghi đè một số `RemainingYears` không có provenance. Khi thọ nguyên chạm ngưỡng, một event canonical được queue; presentation chỉ hiển thị aftermath.

## 5. RNG và determinism

- Một root seed được tạo khi New Game và lưu trong save.
- Tách named streams: `World`, `Combat`, `Craft`, `Npc`, `Encounter`, `Loot`; không dùng stream chung theo thứ tự gọi UI.
- Combat snapshot tạo child seed từ `CombatId`; replay dùng child seed/log đó.
- Mọi random decision ghi `RollId`, stream, range/weight source ID và result vào event log.
- Cùng save snapshot + command log phải cho cùng `ResultHash` trên máy khác cùng build. Floating-point chỉ dùng khi policy/rounding được khóa; ưu tiên fixed-point/int cho economic/combat rules.

## 6. Tu luyện và cảnh giới

`Cultivation` là action/request có thể phát sinh progress, modifier, risk hoặc event; không phải progress bar tăng theo thời gian thật.

```text
Eligibility
→ cost/location/environment validation
→ resolve progress modifier
→ apply progress ledger
→ check realm threshold
→ offer/resolve breakthrough when player chooses
→ queue consequences and time advance
```

- Realm, threshold, requirement, cost, linh căn/đạo modifier và eligibility phải là data assets.
- Công thức cụ thể dùng `REF_PENDING` cho đến khi audit đạt confidence đủ; không hard-code số liệu tham chiếu.
- Breakthrough tạo `FBreakthroughSnapshot` trước RNG; success/failure đều tạo event log và có state hậu quả.
- Thiên kiếp là system event kết hợp progression, combat/encounter và presentation; ánh sáng/bầu trời không là nguồn kết quả.

## 7. Technique, ability và equipment

- `TechniqueDefinition` mô tả unlock, required tags/realm, passive modifiers, ability grants và source type.
- `AbilityDefinition` mô tả command contract, target rule, cost, effects, cooldown, presentation cue key. Nó không chứa logic Blueprint độc quyền.
- `EquipmentDefinition` mô tả slot, modifier, durability/charge nếu benchmark cần, và compatibility tags.
- Loadout validation chạy khi equip, đổi technique, load save và trước combat. Invalid state phải repair/reject có log, không crash.

## 8. Inventory, crafting, home

- Inventory chuyển item qua transaction: validate ownership/capacity → reserve inputs → resolve → commit outputs → event log. Không destroy/spawn item trực tiếp trong world actor.
- Alchemy/refining recipe gồm input rule, facility/tool requirement, time cost, quality/outcome table, failure branch, output rule. Các value audit chưa xong là `REF_PENDING`.
- Home/facility là entity state; 3D décor/transform map từ `FacilityInstanceId` nhưng gameplay effect chỉ đọc canonical slot/config.
- Production queue chạy trong scheduler, bao gồm lúc home zone không loaded.

## 9. World, NPC, sect, quest và economy

### World/NPC

- NPC có state tối thiểu: identity ID, age, realm, location, availability, relation ledger, faction/sect, goal queue và flags.
- NPC offscreen chỉ mô phỏng theo event/time boundaries; không tick AI per frame.
- World event có eligibility, priority, cooldown, seed stream, resolve action và world patch. Event conflict phải có stable order (`priority`, `scheduled date`, `event ID`).

### Sect/quest/social

- Membership dùng state machine `None → Candidate → Member → Restricted/Departed` hoặc states audit chứng minh; consequence được event hóa.
- Contribution, permission và obligation là ledger, không phải widget counter.
- Quest là directed graph có precondition, choice, deadline, state mutation, reward/penalty. Quest complete/fail phải idempotent.
- Relation là ledger có source; threshold evaluation chạy sau transaction, rồi queue event thay vì trigger level Blueprint trực tiếp.

### Economy/travel

- Market state mang `LocationId`, stock, price rule, refresh tick và transaction history.
- Auction/trade route dùng instance ID + deadline + participants/state, không fake bằng cutscene.
- World map graph link có eligibility, travel time, cost, risk/event table. Local exploration không gọi `AdvanceTime`.

## 10. Save và recovery

- Save snapshot schema versioned; ghi temp → fsync/close theo platform API → atomic replace → giữ backup gần nhất.
- Lưu `StateRevision`, root seed, stream state, content version manifest và hash. Không serialize widget/actor pointer.
- Load validate content IDs, schema, checksum và invariant trước khi spawn presentation. Corrupt save giữ nguyên file gốc, báo lỗi hữu ích và offer backup/new game.
- Khi content đổi, migration transform canonical state; migration có fixture test cho từng version.

## 11. Invariant và test bắt buộc

| Invariant | Oracle |
| --- | --- |
| Resource không âm trừ khi definition cho phép debt | `Automation.Core.ResourceInvariant` |
| Một request ID không bị commit hai lần | `Automation.Core.IdempotentRequest` |
| Time only advances through accepted action/event | `Automation.Core.TimeAuthority` |
| Save-load hash không đổi với snapshot hợp lệ | `Automation.Save.RoundTripHash` |
| Same seed/command log → same result hash | `Automation.Core.SeededReplay` |
| Presentation không sửa canonical state | `Automation.Presentation.ReadOnlyAdapter` |
| Dangling ID/content tag bị chặn trước PIE | `Automation.Data.RegistryValidation` |
