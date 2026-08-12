# 12 — Schema dữ liệu và content pipeline

## Mục tiêu

Game phải mở rộng từ hàng chục lên hàng nghìn công pháp, NPC, vật phẩm và sự kiện mà không sửa logic hoặc Blueprint cho từng entry. Dữ liệu là authored, versioned, validated và có stable ID.

Schema mô tả dữ liệu của game mới; không copy bảng, chuỗi, icon hoặc content độc quyền từ game tham chiếu. Mọi item/rule lấy từ audit ghi ReferenceEvidenceId và confidence, sau đó có test oracle trước khi được coi là parity.

## Quy ước định danh

- `FTruongSinhStableId`: canonical lowercase ASCII, chỉ `[a-z0-9._-]`, không thay
  đổi sau khi publish. Ví dụ: `realm.qi_refining.early`. Không dựa vào casing hoặc
  internal index của `FName`; wrapper lưu `FString` để casing/serialization không
  phụ thuộc FName pool. Mọi sort/hash serialize từ chuỗi canonical.
- FPrimaryAssetId: Type = TSRealm, TSTechnique, TSItem, TSNpc, TSSect, TSLocation, TSQuest, TSEvent, TSPresentationCue.
- GameplayTag: TS.Realm.*, TS.Element.*, TS.Technique.*, TS.State.*, TS.Action.*, TS.Zone.*, TS.Result.*.
- Display text: String Table key, ví dụ UI.Realm.QiRefiningEarly.Name.
- Không dùng localized display name làm key logic.
- ID xóa phải chuyển Deprecated=true và migration map, không re-use cho content khác.

## Common metadata

Mọi UPrimaryDataAsset gameplay có FTSContentMetadata:

~~~text
Id                    FName
SchemaVersion         int32
DisplayNameKey        FName
DescriptionKey        FName
Tags                  FGameplayTagContainer
AuditStatus           Draft / Observed / Inferred / Verified
ReferenceEvidenceIds  TArray<FName>
ParityRequirementId   FName
Deprecated            bool
~~~

Data Validator fail nếu Id, String Table key, tag, reference asset hoặc version không hợp lệ.

## Primitive value types

Simulation dùng fixed unit, không dùng float để quyết định rule:

~~~text
FTruongSinhGameDate {
  int32 Year;
  int32 Month;
  int32 Day;
}

FQuantizedValue {
  int64 Raw;       // scale do field định nghĩa, ví dụ 1_000 = 1.000
  int32 Scale;
}

FResourceAmount {
  FPrimaryAssetId ResourceDefinition;
  int64 Quantity;
}

FTruongSinhStableId {
  FString Value;  // canonical lowercase ASCII [a-z0-9._-]
}
~~~

Instance cần identity qua save sẽ dùng một type GUID riêng khi schema đó được
triển khai; không nhồi GUID runtime vào authored `FTruongSinhStableId`.

Tất cả arithmetic overflow phải check. Giá trị invalid không tự clamp im lặng;
command trả `FTruongSinhActionResult` trạng thái Rejected với `ReasonId`.

## Definition assets

### Realm và cultivation

UTSRealmDefinition:

~~~text
Metadata
SortOrder
PrerequisiteRealmId
MinimumCultivation
MaximumCultivation
LifespanDelta
BreakthroughRuleId
AllowedTechniqueTags
TribulationProfileId
PresentationProfileId
~~~

UTSCultivationProfileDefinition mô tả linh căn, affinity ngũ hành, aptitude, dao affinity, valid starting state và modifier. Player/NPC instance lưu values runtime, không mutate asset.

### Technique, ability và effect

UTSTechniqueDefinition:

~~~text
Metadata
TechniqueTags
RequiredRealmId
ElementTags
LearnCost
UnlockConditions
CombatActions
PassiveEffects
UpgradeTracks
SourceRules
PresentationCueIds
~~~

Combat action có ActionId, target policy, energy cost, timing rule, formula ID, effect list và cue ID. Formula gọi registry C++ bằng FormulaId; không serialize lambda, Blueprint graph hoặc text expression thực thi tùy ý.

### Item, recipe và artifact

UTSItemDefinition:

~~~text
Metadata
ItemKind                 Herb / Pill / Material / Artifact / Equipment / Quest
StackLimit
WeightOrCapacityCost
QualityRuleId
UseActionId
EquipSlot
Tradable
WorldTags
PresentationProfileId
~~~

UTSRecipeDefinition:

~~~text
Metadata
RecipeKind               Alchemy / Refining / Array / Building
RequiredStationTags
RequiredRealmId
Inputs                   array of item ID + quantity
TimeCostRuleId
SuccessRuleId
Outputs                  deterministic output table
FailureOutputs
PresentationCueId
~~~

Recipes có thể conditional nhưng phải enumerate condition tags và result table. RNG outcome gọi deterministic RNG stream được chỉ định, không gọi FMath::Rand.

### NPC, sect và relationship

UTSNpcArchetypeDefinition:

~~~text
Metadata
SpawnProfileId
InitialCultivationProfileId
TechniquePoolIds
FactionOrSectIds
LifecycleRuleId
RelationshipDefaults
PortraitOrCharacterProfileId
DialogueProfileId
~~~

Runtime FNpcState:

~~~text
PersistentId
ArchetypeId
NameKeyOrGeneratedNameSeed
BirthDate
CurrentRealmId
CultivationValue
LifespanState
InventoryState
TechniqueState
Relationships by StableEntityId
CurrentActivity
CurrentLocationId
Flags
~~~

UTSSectDefinition giữ ranks, contribution rules, duty pool, exchange catalog, location ID, reputation policy, AI policy và presentation profile.

### Location, zone và world event

UTSLocationDefinition:

~~~text
Metadata
LocationType
WorldMapPosition
ZoneMapSoftPath
TravelEdges
EntryRequirements
TravelTimeRuleId
EncounterTables
SpawnManifestId
ZoneStateRules
~~~

World map position chỉ phục vụ presentation. Travel edge và time rule là source of truth simulation.

UTSWorldEventDefinition:

~~~text
Metadata
TriggerPredicateId
ScheduleRuleId
Priority
ParticipantSelectorId
Effects
QuestLinks
PresentationCueId
~~~

### Quest và encounter

UTSQuestDefinition có state graph node IDs, transition IDs, prerequisite predicates, deadline/time rule, objective definitions, reward command list, failure consequence list, location/NPC bindings và presentation beat IDs.

UTSEncounterDefinition gồm combat setup, valid participants, environment tags, reward/failure transition và combat arena presentation profile.

## Runtime state versus definition

| Loại | Immutable asset | Runtime/save state |
|---|---|---|
| Cảnh giới | ngưỡng, requirement, tags | cultivation value, unlocked realm |
| Công pháp | formula, cost, cue | known rank, cooldown, disabled state |
| Item | definition, stack limit | instance ID, stack, quality, binding |
| NPC | archetype, spawn profile | tuổi, state, relation, inventory |
| Location | edge, travel rule, zone path | discovered, patch flags, spawn state |
| Quest | graph, objective definition | node hiện tại, deadline, outcome |

Không ghi mutable state vào asset. Không lưu hard object reference đến level actor trong canonical state.

## JSON import pipeline

JSON dùng nguồn authoring/bulk import, không phải source runtime:

~~~text
External JSON
  → UTSContentImportCommandlet
  → schema validation + stable-ID validation
  → generate/update Data Asset or DataTable
  → Content Registry scan
  → Data Validation report
  → commit generated assets and source JSON together
~~~

Import phải idempotent. Field unknown là error; field deprecated đưa warning; missing mandatory field fail. JSON schema version không tự đoán.

Ví dụ shape tối thiểu:

~~~json
{
  "schemaVersion": 1,
  "id": "technique.example.metal_guard",
  "displayNameKey": "Technique.Example.MetalGuard.Name",
  "requiredRealmId": "realm.qi_refining.early",
  "elements": ["TS.Element.Metal"],
  "combatActions": ["action.example.metal_guard"]
}
~~~

## Provenance và audit evidence

Parity item có optional nhưng strongly-required fields:

~~~text
EvidenceId
ReferenceBuildId
ObservationDate
ObservationMethod       manual-play / recorded test / readable-data audit
Confidence              observed / inferred / verified
OracleTestId
KnownDifferenceNote
~~~

Không lưu copied source files, art, audio, dialogue or content dump của reference trong Git. Evidence ID trỏ tới metadata/hash cục bộ trong ReferenceVault.

## Data validation gates

Commandlet validate:

- duplicate/reused stable ID;
- missing display/description key;
- missing or invalid gameplay tag;
- dangling referenced asset;
- circular prerequisite không được phép;
- unbounded result table;
- formula ID chưa đăng ký;
- content required by parity matrix thiếu;
- actor presentation profile thiếu fallback;
- deprecated ID bị dùng bởi new content;
- localized Vietnamese string trống.

Gate M2A yêu cầu content golden loop valid; M2B yêu cầu toàn bộ presentation
reference của visual target valid. Gate M7 yêu cầu toàn bộ catalog valid.
