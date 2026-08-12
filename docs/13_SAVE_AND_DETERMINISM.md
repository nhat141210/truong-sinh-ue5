# 13 — Save, replay và determinism

## Hợp đồng deterministic

Với cùng:

- Version game/content manifest.
- New Game seed hoặc save snapshot.
- Chuỗi `FTruongSinhActionCommand` hợp lệ theo thứ tự.

simulation phải cho cùng:

- StateHash ở mỗi command boundary.
- Combat result và replay log.
- Lịch game, tuổi thọ, NPC/quest/economy state.
- Resource delta và event list.

FPS, frame pacing, Lumen, actor spawn order, animation, input UI timing sau command commit, skip cutscene và quality preset không được làm khác state.

## RNG

Dùng `FTruongSinhDeterministicRng` trong Core. Contract v1 đã khóa:

- PRNG stream: SplitMix64.
- Derive stream: `MasterSeed XOR FNV1a64(UTF8(StreamId)) XOR 0x9E3779B97F4A7C15`.
- `AlgorithmVersion = 1`; đổi thuật toán hoặc derive rule bắt buộc có save migration và golden vector mới.
- Bounded integer dùng rejection sampling, không modulo bias.

Không dùng `FMath::Rand`, `FRand`, `std::rand`, thời gian hệ thống, GUID, pointer
address hay order container làm random source gameplay.

Session state có:

~~~text
AlgorithmVersion        int32
MasterSeed              int64
NamedStreams[]          { Stable StreamId, uint64 State, int64 DrawCount }
~~~

`State` là UPROPERTY `uint64` để save bảo toàn đủ bit nhưng cố ý không expose thành
Blueprint pin. Blueprint/presentation không được đọc hoặc sửa RNG canonical.
Mỗi purpose dùng stable stream ID riêng như `world.events`, `combat.<combat-id>`,
`craft.<station-id>`. Nếu cần entity/counter thì chúng là thành phần của chuỗi ID
canonical, không phải display text. Streams phải serialize theo lexical ID order.
Mỗi result log ghi stream ID, draw count bắt đầu/kết thúc và audit value ở
Development build. Command reject không consume RNG.

## Lịch và event order

`FTruongSinhGameDate` là integer calendar. Time advance chỉ xảy ra qua simulation command:

- Travel.
- Cultivate / seclusion.
- Craft / refine.
- Quest action.
- Event resolution.
- Reference-audited action khác.

Không tick lịch theo real time. Khi calendar advance:

1. Validate time delta.
2. Cập nhật date.
3. Lấy due events.
4. Sort theo Date, Priority, StableEventId.
5. Resolve từng event với named RNG stream.
6. Apply state/event journal.
7. Publish một StateRevision.

NPC update batch cũng sort stable entity ID. Không iterate TMap/TSet để quyết định outcome.

## Fixed-point rules

- Money, item count, contribution: signed int64.
- Tu vi, percent, attribute: int64 scaled; scale nằm trong schema.
- Tỷ lệ: integer basis points, 0–10_000.
- Damage/defense: quantized integer; round rule được đặt tên và test.
- Thời gian: day integer, không float day.

Float chỉ dùng transform, camera, interpolation và VFX. Float không được serialize vào canonical gameplay state trừ khi quantize trước.

## Simulation journal

Mỗi committed command tạo `FJournalEntry`:

~~~text
JournalSequence
PreviousStateHash
CommandType
CanonicalPayloadHash
TimeDelta
RngAudit
ResultCode
NewStateHash
Events (type ID + sequence + typed payload hash)
~~~

Journal là debug/recovery aid, không thay save snapshot lâu dài. Development save giữ N entry gần nhất; Shipping có thể giới hạn kích thước nhưng phải giữ đủ để report lỗi command cuối.

## Save format

### M1 codec hiện có

`FTruongSinhSaveGameV1` và `FTruongSinhSaveJsonCodec` hiện chứng minh versioned
DTO, canonical integer-as-string JSON, RNG bit preservation, committed CommandId
và canonical BLAKE3-256 payload hash validation bằng API Core của UE5.8. Codec chưa
tự ghi file, chưa có `.tmp/.bak`, flush,
atomic replace hoặc migration implementation. Chỉ đánh dấu save foundation hoàn
thành sau khi lớp file I/O được kiểm trên Windows và corruption recovery test pass.

USaveSubsystem serialize DTO versioned, không serialize raw UObject graph:

~~~text
FTSSaveHeader {
  Magic = TSGS
  SaveSchemaVersion
  GameBuildId
  ContentManifestHash
  CreatedUtc
  UpdatedUtc
  SlotId
  PayloadSha256
}

FTSSavePayload {
  MasterSeed + RNG state
  World state + date + revision
  Player state
  NPC / sect / economy / quest state
  Inventory, cave residence and zone patches
  Current location + valid 3D checkpoint
  Active combat snapshot/replay checkpoint when supported
  Settings references only, not device-specific window state
}
~~~

Save filename:

~~~text
Saved/SaveGames/slot_<id>.tsgs
Saved/SaveGames/slot_<id>.bak
Saved/SaveGames/slot_<id>.tmp
~~~

## Atomic write

1. Serialize payload to memory.
2. Validate DTO and compute payload SHA-256.
3. Write temp file in same directory.
4. Flush/close temp file.
5. Read temp back and verify header/hash.
6. Move existing main to .bak.
7. Atomic rename temp to main where platform supports it.
8. Retain last known good backup.

Crash giữa step 6–7 phải offer recovery from backup; không silent overwrite. Corrupt hash không load partially.

## Load và migration

Load:

1. Read header, magic, version và hash.
2. Attempt main; invalid thì attempt backup.
3. Compare content manifest compatibility.
4. Migrate DTO step-by-step chỉ forward.
5. Validate stable IDs và required definitions.
6. Rebuild runtime indexes from DTO.
7. Recompute StateHash.
8. Spawn presentation from FZonePresentationSnapshot.

Migration function signature:

~~~text
bool MigrateSave(int32 FromVersion, FTSSavePayload& InOutPayload, FText& OutError)
~~~

Migration không access actor, widget hoặc async asset. Missing deprecated ID phải map qua data migration table hoặc produce clear incompatible-save error với backup retained.

## State hash

StateHash dùng canonical serialization:

- fields emitted in schema order;
- collections sorted by stable ID;
- UTF-8 normalized string keys only;
- no UObject pointer/path volatile state;
- no wall-clock timestamps;
- integer little-endian documented.

Hash là SHA-256 trong save integrity path. Có thể dùng fast 64-bit canonical hash per command cho debug, nhưng parity test compare full SHA-256 at checkpoints.

## Combat replay

Combat simulation emit FCombatReplayLog:

~~~text
CombatId
CombatSeed
InitialCombatSnapshotHash
TurnEvents ordered by Turn, Phase, EventSequence
ActionCommands
RngAudit
FinalCombatSnapshotHash
~~~

Presentation chỉ consume replay events. Nếu presentation fail hoặc player skip,
service vẫn commit `FTruongSinhCombatResult` đã resolve. Mid-combat save chỉ allowed
at stable turn boundary; còn lại UI disable save và ghi reason.

## Autosave policy

Autosave tại:

- thành công load New Game / Continue;
- accepted travel result sau destination snapshot valid;
- combat resolved;
- quest milestone;
- breakthrough/tribulation result;
- successful crafting hoặc cave build action;
- trước high-risk transition nếu reference flow yêu cầu.

Không autosave every frame. Manual save disabled chỉ during atomic command resolve, map unload, non-resumable cinematic hoặc combat presentation giữa command boundary.

## Test suite

- PRNG test vectors fixed trong source.
- Same seed + command script repeated 100 lần yields identical StateHash.
- Save/load round-trip tại New Game, travel, crafting, quest, combat boundary và late game.
- Corrupt main recovery dùng backup; corrupt cả hai giữ slot và report error.
- Migration fixture cho mọi prior schema version.
- Reordered TMap insertion có canonical hash/outcome như nhau.
- 30/60/120 FPS presentation run cùng command script cho cùng hash.
- Skip và watch full combat replay cho cùng final hash.
- Interrupted temp save không phá last valid save.
