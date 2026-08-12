# Ma trận parity chức năng

> Đây là nguồn trạng thái bắt buộc. Mỗi row là một capability có thể kiểm tra, không phải một ý tưởng chung. Chi tiết cách audit nằm ở [04_REFERENCE_AUDIT_GUIDE.md](04_REFERENCE_AUDIT_GUIDE.md); định nghĩa trạng thái nằm ở [02_PARITY_CONTRACT.md](02_PARITY_CONTRACT.md).

## Quy tắc cập nhật

- Giữ ID cố định; không xóa row khi đổi thiết kế. Đánh dấu `SUPERSEDED` và mở row thay thế nếu cần.
- `Reference question` và `evidence ID` không chứa lời thoại, tên/content hay asset của benchmark.
- `Own requirement` dùng terminology/content của Trường Sinh UE5.
- `Test oracle` là tên Automation test hoặc Manual Scenario ID có thể chạy lại.
- Mọi row ban đầu là `P0-NOT_AUDITED`; đó là trạng thái trung thực, không phải lỗi.

## Dashboard khởi tạo

| Nhóm | Tổng CAP | P0 | P1 | P2 | P3 | P4 | Blocked |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Core chronology/save | 7 | 7 | 0 | 0 | 0 | 0 | 0 |
| Cultivation/progression | 8 | 8 | 0 | 0 | 0 | 0 | 0 |
| Combat | 8 | 8 | 0 | 0 | 0 | 0 | 0 |
| Production/home | 6 | 6 | 0 | 0 | 0 | 0 | 0 |
| World/social/quest | 9 | 9 | 0 | 0 | 0 | 0 | 0 |
| Economy/travel/events | 7 | 7 | 0 | 0 | 0 | 0 | 0 |
| 3D presentation/UX | 7 | 7 | 0 | 0 | 0 | 0 | 0 |
| **Tổng** | **52** | **52** | **0** | **0** | **0** | **0** | **0** |

## Core chronology và save

| ID | Capability / câu hỏi audit | Own requirement | Oracle | Trạng thái |
| --- | --- | --- | --- | --- |
| `CAP-CORE-001` | Lịch game biểu diễn và chuyển ngày/tháng/năm thế nào? | `FTruongSinhGameDate` canonical, không dùng wall clock | `Automation.Core.CalendarRollOver` | `P0-NOT_AUDITED` |
| `CAP-CORE-002` | Action nào tiêu tốn thời gian và có preview gì? | Mọi `GameplayAction` trả `FActionCostPreview` trước confirm | `Automation.Core.ActionCostPreview` | `P0-NOT_AUDITED` |
| `CAP-CORE-003` | Khi time advance, scheduler chạy thứ tự nào? | Event tick stable, ordered và replayable | `Automation.Core.TimeAdvanceOrder` | `P0-NOT_AUDITED` |
| `CAP-CORE-004` | RNG ảnh hưởng state nào? | PRNG streams có seed/lưu state; không random theo FPS | `Automation.Core.SeededReplay` | `P0-NOT_AUDITED` |
| `CAP-SAVE-001` | Save gồm state nào và load khôi phục gì? | Snapshot versioned, atomic, checksum + backup | `Automation.Save.RoundTripHash` | `P0-NOT_AUDITED` |
| `CAP-SAVE-002` | Hành vi save corrupt/old schema? | Reject/fallback rõ ràng, migration được test | `Automation.Save.CorruptAndMigrate` | `P0-NOT_AUDITED` |
| `CAP-CORE-005` | Pause/alt-tab/streaming ảnh hưởng simulation ra sao? | Không simulation tick khi pause; load zone không đổi state | `Manual.CORE.PauseAndStreaming` | `P0-NOT_AUDITED` |

## Tu luyện và tiến trình

| ID | Capability / câu hỏi audit | Own requirement | Oracle | Trạng thái |
| --- | --- | --- | --- | --- |
| `CAP-CULT-001` | Character start có lựa chọn căn cơ/linh căn nào? | Tạo nhân vật data-driven, selection ảnh hưởng build hợp lệ | `Automation.Cultivation.CharacterCreation` | `P0-NOT_AUDITED` |
| `CAP-CULT-002` | Tu vi/progress tăng từ action nào, bị giới hạn ra sao? | `CultivationProgress` có source, cap, modifier và log | `Automation.Cultivation.ProgressSources` | `P0-NOT_AUDITED` |
| `CAP-CULT-003` | Điều kiện đột phá và outcome class là gì? | Requirement/cost/risk data-driven, preflight rõ | `Automation.Cultivation.BreakthroughPreflight` | `P0-NOT_AUDITED` |
| `CAP-CULT-004` | Tuổi thọ tăng/giảm bởi event nào? | Lifespan ledger có provenance từng delta | `Automation.Cultivation.LifespanLedger` | `P0-NOT_AUDITED` |
| `CAP-CULT-005` | Thất bại đột phá để lại state/hậu quả nào? | Failure branch deterministic và saveable | `Automation.Cultivation.BreakthroughFailure` | `P0-NOT_AUDITED` |
| `CAP-CULT-006` | Công pháp/thần thông mở khóa, học, đổi build thế nào? | Technique loadout validate realm/tag/cost | `Automation.Cultivation.TechniqueLoadout` | `P0-NOT_AUDITED` |
| `CAP-CULT-007` | Đạo/ngộ đạo có điều kiện và hiệu ứng gì? | Dao state là data + modifier, không là flavor-only | `Automation.Cultivation.DaoEffects` | `P0-NOT_AUDITED` |
| `CAP-CULT-008` | Thiên kiếp trigger/resolve/hậu quả thế nào? | Event snapshot + outcome log + presentation cue | `Automation.Cultivation.TribulationReplay` | `P0-NOT_AUDITED` |

## Combat

| ID | Capability / câu hỏi audit | Own requirement | Oracle | Trạng thái |
| --- | --- | --- | --- | --- |
| `CAP-COMBAT-001` | Combat snapshot lấy player/NPC/world state nào? | Immutable `FTruongSinhCombatSnapshot` có schema/hash | `Automation.Combat.SnapshotHash` | `P0-NOT_AUDITED` |
| `CAP-COMBAT-002` | Thứ tự turn/phase/resource refresh? | Phase machine data-configurable, event log đầy đủ | `Automation.Combat.PhaseOrder` | `P0-NOT_AUDITED` |
| `CAP-COMBAT-003` | Linh khí/ngũ hành tạo, tiêu và chuyển đổi thế nào? | Element resource ledger theo tag/source | `Automation.Combat.ElementLedger` | `P0-NOT_AUDITED` |
| `CAP-COMBAT-004` | Action hợp lệ/invalid và cost/cooldown? | Command validator độc lập UI | `Automation.Combat.CommandValidation` | `P0-NOT_AUDITED` |
| `CAP-COMBAT-005` | Damage, shield, status và duration resolve thứ tự nào? | Rule pipeline + combat event replay | `Automation.Combat.EffectResolution` | `P0-NOT_AUDITED` |
| `CAP-COMBAT-006` | AI chọn action theo state nào? | Seeded policy, trace score mỗi lựa chọn | `Automation.Combat.AIPolicyReplay` | `P0-NOT_AUDITED` |
| `CAP-COMBAT-007` | Kết thúc combat áp reward/loss vào world thế nào? | Commit transaction một lần, idempotent | `Automation.Combat.WorldCommit` | `P0-NOT_AUDITED` |
| `CAP-COMBAT-008` | 3D replay/skip có đổi result không? | Presentation đọc log, không ghi mechanics | `Automation.Combat.PresentationIsolation` | `P0-NOT_AUDITED` |

## Chế tạo, động phủ và tài nguyên

| ID | Capability / câu hỏi audit | Own requirement | Oracle | Trạng thái |
| --- | --- | --- | --- | --- |
| `CAP-PROD-001` | Luyện đan có input, cost, outcome/failure nào? | Recipe transaction atomic và audit log | `Automation.Production.AlchemyTransaction` | `P0-NOT_AUDITED` |
| `CAP-PROD-002` | Luyện khí có material/quality/tool/location rule nào? | Refining recipe data-driven | `Automation.Production.RefiningTransaction` | `P0-NOT_AUDITED` |
| `CAP-PROD-003` | Item stack, quality, durability/equipment xử lý thế nào? | Inventory/equipment canonical, stable ID | `Automation.Production.InventoryInvariant` | `P0-NOT_AUDITED` |
| `CAP-HOME-001` | Động phủ mở/nâng cấp/đặt facility thế nào? | Home state, slot/layout và unlock theo data | `Automation.Home.FacilityUnlock` | `P0-NOT_AUDITED` |
| `CAP-HOME-002` | Linh điền/production tick khi nào và cho output gì? | Scheduled production, không phụ thuộc loaded level | `Automation.Home.ProductionTick` | `P0-NOT_AUDITED` |
| `CAP-HOME-003` | Bế quan có risk/cost/outcome gì? | Retreat action có preflight và checkpoint | `Automation.Home.RetreatOutcome` | `P0-NOT_AUDITED` |

## World, social và quest

| ID | Capability / câu hỏi audit | Own requirement | Oracle | Trạng thái |
| --- | --- | --- | --- | --- |
| `CAP-WORLD-001` | NPC có age, realm, location, relation state nào? | NPC snapshot deterministic/offscreen | `Automation.World.NPCLifecycle` | `P0-NOT_AUDITED` |
| `CAP-WORLD-002` | NPC action/world event scheduler tương tác ra sao? | Ordered event queue, conflict policy ghi rõ | `Automation.World.EventConflictOrder` | `P0-NOT_AUDITED` |
| `CAP-SOCIAL-001` | Relation thay đổi bởi action và ngưỡng mở gì? | Relation ledger/threshold data | `Automation.Social.RelationThresholds` | `P0-NOT_AUDITED` |
| `CAP-SECT-001` | Tông môn có membership/rank/obligation/reward gì? | Sect membership state machine | `Automation.Sect.MembershipState` | `P0-NOT_AUDITED` |
| `CAP-SECT-002` | Tán tu có route/resource/risk riêng gì? | Independent route không bị fake bằng sect UI | `Manual.SECT.IndependentRoute` | `P0-NOT_AUDITED` |
| `CAP-QUEST-001` | Quest precondition/accept/complete/fail ra sao? | Branch graph + immutable quest event log | `Automation.Quest.StateTransitions` | `P0-NOT_AUDITED` |
| `CAP-QUEST-002` | Deadline/time advance/abandon có hậu quả gì? | Scheduler/world patch testable | `Automation.Quest.Deadline` | `P0-NOT_AUDITED` |
| `CAP-WORLD-003` | Bí cảnh/encounter mở, resolve và reset thế nào? | Encounter instance state + seed | `Automation.World.EncounterLifecycle` | `P0-NOT_AUDITED` |
| `CAP-SOCIAL-002` | Quan hệ sâu/đạo lữ/luận đạo có state gì? | Relationship feature gated by audited rules | `Automation.Social.DeepRelation` | `P0-NOT_AUDITED` |

## Economy, travel và event

| ID | Capability / câu hỏi audit | Own requirement | Oracle | Trạng thái |
| --- | --- | --- | --- | --- |
| `CAP-TRAVEL-001` | Travel liên vùng cost/requirement/encounter? | World-map link action với preview/transaction | `Automation.Travel.LinkCost` | `P0-NOT_AUDITED` |
| `CAP-TRAVEL-002` | Đi bộ/interact trong zone có đổi lịch không? | 3D exploration never advances calendar | `Automation.Travel.LocalExplorationNoTime` | `P0-NOT_AUDITED` |
| `CAP-ECON-001` | Shop stock/price/buy/sell cập nhật thế nào? | Market snapshot/transaction ledger | `Automation.Economy.MarketTransaction` | `P0-NOT_AUDITED` |
| `CAP-ECON-002` | Auction có bid, deadline, rival, settle rules nào? | Auction instance state machine | `Automation.Economy.AuctionSettlement` | `P0-NOT_AUDITED` |
| `CAP-ECON-003` | Trade/caravan lợi nhuận/rủi ro/time cost ra sao? | Trade route action + seeded encounter | `Automation.Economy.TradeRoute` | `P0-NOT_AUDITED` |
| `CAP-EVENT-001` | World event trigger/priority/cooldown? | Event eligibility + queue test | `Automation.Events.TriggerPriority` | `P0-NOT_AUDITED` |
| `CAP-EVENT-002` | Endgame/phi thăng requirement và branches? | Full-run prerequisite validator | `Automation.Events.AscensionPreflight` | `P0-NOT_AUDITED` |

## 3D presentation và UX

| ID | Capability / câu hỏi audit | Own requirement | Oracle | Trạng thái |
| --- | --- | --- | --- | --- |
| `CAP-3D-001` | Player di chuyển/interaction phù hợp game state? | Actor 3D chỉ gửi validated action | `Automation.Presentation.InteractionGateway` | `P0-NOT_AUDITED` |
| `CAP-3D-002` | Zone load/stream restore actor state thế nào? | World presentation rebuild từ snapshot | `Manual.3D.ZoneReload` | `P0-NOT_AUDITED` |
| `CAP-3D-003` | Combat arena/camera/animation có isolated? | Replay-only adapter, skip safe | `Automation.Presentation.CombatReplay` | `P0-NOT_AUDITED` |
| `CAP-3D-004` | Lumen/VFX/animation scalability không che UI? | Quality presets preserve readability | `Manual.3D.Scalability` | `P0-NOT_AUDITED` |
| `CAP-UX-001` | Player thấy cost, invalid reason, outcome rõ? | Standard action confirmation/result surface | `Manual.UX.ActionFeedback` | `P0-NOT_AUDITED` |
| `CAP-UX-002` | Keyboard/mouse, pause, focus loss, save safe? | CommonUI stack/input mode contract | `Manual.UX.FocusAndPause` | `P0-NOT_AUDITED` |
| `CAP-LOC-001` | Chuỗi UI/data được localize, không literal logic? | String Table + localization validation | `Automation.Localization.NoLiteralPlayerText` | `P0-NOT_AUDITED` |

## Mẫu cập nhật completion cho một row

| Field | Giá trị mẫu (dùng nội dung gốc) |
| --- | --- |
| ID | `CAP-CORE-002` |
| Audit evidence | `REF-CHRONO-TRAVEL-COST-001`, confidence `HIGH` |
| Own requirement | `TravelAction` preview `days`, `currency`, `risk`; confirm tạo transaction duy nhất |
| Tests | `Automation.Core.ActionCostPreview`; `Automation.Travel.LinkCost` |
| Playable proof | `SCN-TRAVEL-001` và package log hash |
| Save proof | `SAVE-ROUNDTRIP-TRAVEL-001` |
| Performance proof | Không cần / link `PERF-*` nếu có presentation |
| P4 decision | Chỉ khi coverage tất cả class travel đã audit và no regression |

## Ma trận completion release

| Yêu cầu | Pass khi |
| --- | --- |
| Logic | Tất cả CAP required ≥ P2, test suite xanh |
| Gameplay | Tất cả CAP required ≥ P3, manual scenario tái lập |
| Content | Mỗi capability có content original đủ full-run và catalog validate |
| Presentation | Tất cả CAP-3D/UX ≥ P3, target FPS/UX pass |
| Release | Toàn bộ 52 CAP đạt P4 hoặc được thay bằng row đã phê duyệt; không `UNKNOWN`, `BLOCKED`, stub hay feature flag `RefPending` trên critical path |
