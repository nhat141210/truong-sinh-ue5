# 11 — Kiến trúc kỹ thuật UE5.8

## Quyết định kiến trúc

Project dùng Unreal Engine 5.8 C++ làm nguồn sự thật. Blueprint chỉ dùng để ghép presentation, widget layout, animation graph, Niagara, level composition và data asset instance; không đặt rule simulation trong Blueprint.

Simulation deterministic, tick theo command/lịch game, không theo FPS. Presentation 3D tiêu thụ snapshot/replay event, không được sửa kết quả. Điều này cho phép góc nhìn thứ ba, Lumen, animation hoặc skip cinematic không ảnh hưởng parity.

Không nhúng hoặc phân phối nội dung độc quyền của game tham chiếu. Mọi rule và data cần parity được ghi từ audit có nguồn, confidence và test oracle riêng.

## Dependency graph

~~~text
consumer → dependency được phép

Data          → Core
Combat        → Core, Data
Simulation    → Core; thêm Data/Combat chỉ khi handler thật dùng
Quest         → Core, Data, Simulation
Save          → Core, Data, Simulation, Combat, Quest
World         → Core, Data, Simulation, Quest
Presentation  → Core, Combat; thêm World snapshot adapter khi thật dùng
UI            → Core, Data, Simulation, Combat, Quest
TruongSinhUE5 → composition root của các runtime module
Tests         → module đang được kiểm thử, chỉ Development/Editor
~~~

Đây là DAG tối đa được phép, không phải lý do khai báo dependency chưa dùng.
`Build.cs` là bằng chứng dependency hiện tại: chỉ thêm module khi header/source thật
sự cần. Không thêm cạnh ngược tạo cycle. Runtime module được khai báo rõ trong
`.uproject`; test module là `Developer`, không thuộc Shipping dependency chain.

Quy tắc phụ thuộc:

- Core chỉ phụ thuộc `Core`, `CoreUObject`, `StructUtils`; không phụ thuộc Engine
  world, Gameplay Tags, UMG, actor, mesh hay asset visual.
- Simulation bắt đầu từ Core và chỉ thêm Data/Combat khi domain handler thật dùng;
  không gọi UWorld, AActor, Niagara, widget hoặc async asset load.
- Combat là thư viện simulation có interface hẹp, không biết camera/VFX.
- World/Presentation/UI chỉ đọc snapshot và gửi typed command về simulation.
- Save serializes state owned bởi Simulation; presentation chỉ lưu checkpoint 3D hợp lệ thông qua save DTO.

## Module ownership

| Module | Trách nhiệm | Không được làm |
|---|---|---|
| TruongSinhCore | ID, deterministic RNG, date, result/error, event envelope | biết asset/UI/actor |
| TruongSinhData | PrimaryAsset registry, schema validation, String Table key, import JSON | thay state runtime |
| TruongSinhSimulation | lịch, nhân vật, NPC, cultivation, economy, world event | tạo visual actor |
| TruongSinhCombat | snapshot combat, resolve turn, AI policy, replay log | đọc widget/collision |
| TruongSinhQuest | quest state, predicates, reward command | tự spawn NPC |
| TruongSinhWorld | zone descriptor, spawn manifests, interaction bridge, streaming | tính resource/tuổi |
| TruongSinhPresentation | camera, cutscene, combat replay, actor visual state | quyết định combat |
| TruongSinhUI | CommonUI, input routing, ViewModel | mutate data struct |
| TruongSinhSave | schema, checksum, atomic write, migration | lưu raw UObject graph |
| TruongSinhTests | automation, golden tests, data validation | chứa production rule |

## Core runtime objects

### GameInstance subsystems

- UContentRegistrySubsystem: load/validate definition registry trước New Game hoặc Load Game.
- `UTruongSinhGameSimulationFacade`: composition/write gateway; M1 giữ
  `ElapsedDays`, revision, RNG và idempotency. Calendar month/year chỉ thêm sau audit.
- `UTruongSinhCultivationSubsystem`: player cultivation state, breakthrough và tribulation command.
- `UTruongSinhWorldSimulationSubsystem`: NPC/sect/economy/world snapshot.
- `UTruongSinhQuestSubsystem`: predicate, progression, deadline.
- `UTruongSinhCombatServiceSubsystem`: tạo `FTruongSinhCombatSnapshot` và gọi `ITruongSinhCombatSimulation`.
- `UTruongSinhSaveSubsystem`: save/load, migration, backup.
- `UTruongSinhInputRouterSubsystem`: Enhanced Input mapping context và modal restore.

Các subsystem public qua interface/service locator đã kiểm soát; gameplay actor không cache raw subsystem pointer qua map change. GameInstance tồn tại xuyên open-zone.

### World subsystems

- UZonePresentationSubsystem: chuyển FZonePresentationSnapshot thành spawn/despawn/update actor.
- UInteractionRouterSubsystem: phát hiện interactable, request UI/action, validation context.
- UCameraDirectorSubsystem: explore/dialogue/combat/cinematic camera.
- UWorldTransitionSubsystem: loading screen, travel transition, spawn checkpoint.

World subsystem bị hủy khi map unload; nó không giữ canonical state.

## Command → Result pipeline

~~~text
Player input / actor interaction / UI
  → FTruongSinhActionCommand (stable ID, typed payload, expected revision)
  → UTruongSinhGameSimulationFacade::Execute()
  → validate state + data + cost + prerequisites
  → deterministic state transition
  → FTruongSinhActionResult + ordered domain events + new revision + state hash
  → Save dirty marker + UI ViewModel refresh
  → Presentation dispatcher / Combat replay / zone patch
~~~

`FTruongSinhActionCommand` tối thiểu có:

~~~text
CommandId           correlation/idempotency ID, không dùng làm RNG
ActionId            stable action ID
InstigatorId        stable actor/entity ID
TargetIds           stable target IDs
Payload             FInstancedStruct chứa typed domain USTRUCT
ExpectedRevision   int64 optimistic concurrency guard
Sequence            deterministic caller order
~~~

`FTruongSinhActionResult` có:

~~~text
Status             Rejected / Committed
ActionId / ReasonId stable IDs
PreviousWorldRevision / NewWorldRevision
Events              event type ID + sequence + typed payload
StateHash           canonical post-commit hash
~~~

Command bị reject không được advance RNG, lịch hay state. Command committed luôn
append journal entry trước khi presentation bắt đầu. `CommandId` chỉ dùng chống
double commit; GUID không được dùng làm gameplay entropy. Payload typed cho từng
action kế thừa `FTruongSinhActionPayload`, không thay bằng JSON hoặc string map.

`UTruongSinhGameSimulationFacade` thuộc composition root, là API ghi state duy
nhất cho World/UI. Nó route command đến domain handler, quản lý transaction,
idempotency, journal và publish snapshot. Các domain subsystem không gọi lẫn nhau
qua service locator tùy tiện.

## Snapshot boundary

Simulation state không cấp direct mutable reference cho UI/actor. Nó xuất:

- FPlayerSimulationSnapshot.
- FWorldSimulationSnapshot.
- `FTruongSinhCombatSnapshot`.
- FZonePresentationSnapshot.
- FGameSaveSnapshot.

Snapshot immutable theo revision. Presentation có thể chạy chậm hơn simulation; khi đó nó phải coalesce state update an toàn hoặc chuyển sang latest snapshot, không replay sai result.

## Deterministic execution

- Mọi random qua FDeterministicRng owned bởi session/world state.
- Thứ tự entity là stable ID lexical order, không dựa TMap iteration, actor spawn order hay pointer address.
- Time advance gọi ProcessDueEvents theo event sort key: date, priority, stable event ID.
- Combat resolve single-threaded theo snapshot; background task chỉ chuẩn bị data immutable, không commit state.
- Floating point không dùng cho rule quan trọng. Dùng integer fixed units hoặc rational/quantized value.
- Save/load có StateHash trước và sau round-trip.

Đặc tả chi tiết tại 13_SAVE_AND_DETERMINISM.md.

## C++ và Blueprint boundary

| Chủ đề | C++ | Blueprint |
|---|---|---|
| Rule, formula, command validation | bắt buộc | cấm |
| Save DTO/migration/hash | bắt buộc | cấm |
| Data schema/validator | bắt buộc | instance asset |
| AI combat/NPC policy | bắt buộc | chọn Behavior/StateTree asset |
| Character movement | C++ base | config/AnimBP |
| Interaction | C++ interface/router | actor visual setup |
| UI screen stack/ViewModel | C++ base | layout/style |
| Combat replay event | C++ data | cue animation/camera/VFX |
| Zone state patch | C++ controller | data layer/level art |

Nếu Blueprint cần thêm event, nó chỉ subscribe vào delegate của C++ service. Không logic duplicated giữa C++ và Blueprint.

## Engine/plugin configuration

Bật theo nhu cầu và xác minh tồn tại trong UE 5.8: CommonUI, Enhanced Input,
GameplayAbilities, PCG, StateTree, Python Editor Script Plugin và Editor-only
`ModelContextProtocol` + `AllToolsets`. Gameplay Tags/Tasks, Asset Manager,
Automation, World Partition, Niagara, Lumen, Nanite và VSM là engine systems/module,
không được ghi bừa thành plugin descriptor nếu Editor không cung cấp plugin đó.
MassEntity chỉ bật khi density NPC thật sự cần.

Không bật multiplayer, EOS, Steam OnlineSubsystem hoặc replication cho core V1. Gameplay Ability System dùng local single-player prediction off; mọi ability result do deterministic simulation/service xác nhận.

Renderer Windows được pin Deferred + DX12 + SM6, Lumen GI/reflections, Mesh Distance
Fields, TSR và Virtual Shadow Maps. Hardware ray tracing tắt ở baseline và không là
requirement; chỉ mở bằng decision/performance gate mới.

## Source layout

~~~text
Source/
  TruongSinhCore/
  TruongSinhData/
  TruongSinhSimulation/
  TruongSinhCombat/
  TruongSinhQuest/
  TruongSinhWorld/
  TruongSinhPresentation/
  TruongSinhUI/
  TruongSinhSave/
  TruongSinhTests/
Content/
  Data/           data assets, tables, string tables
  Characters/     shared skeleton, AnimBP, modular parts
  World/          zone maps, level instances, PCG
  UI/             widget blueprints and theme
  Presentation/   camera, cue, Niagara, material
~~~

Không commit DerivedDataCache, Intermediate, Saved, Binaries hoặc donor source trực tiếp vào project root.

## Editor MCP và automation

UE5.8 native MCP (plugin identifier `ModelContextProtocol`, friendly name Unreal MCP) chỉ dùng localhost. Agent dùng MCP theo thứ tự:

1. Read-only: project state, source control state, log, map/actor inventory.
2. Tạo/sửa asset presentation trên branch/checkpoint.
3. Chạy PIE/test/screenshot.
4. Mọi thay đổi rule hoặc source vẫn qua C++ files và build.

MCP không là source of truth, không được mở port public, và không được dùng để import/redistribute reference proprietary content.

MCP chỉ bật cho target Editor trong `.uproject`; không gọi lệnh start server ở packaged build. Trên Windows, để Editor tự tạo config local bằng `ModelContextProtocol.GenerateClientConfig Codex`; `.codex/` bị Git ignore.

## Build gates

- Development Editor build sạch trước mỗi merge logic.
- Automation Unit + Functional test pass.
- Cook/Package Development Windows định kỳ từ clean workspace.
- Data validator chạy commandlet, fail nếu ID duplicate, reference hỏng, missing string key, invalid tag hoặc asset thiếu presentation fallback.
- Shipping gate chạy trên Windows test machine; performance tại 19_PERFORMANCE_BUDGET.md.
