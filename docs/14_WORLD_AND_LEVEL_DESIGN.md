# 14 — Thiết kế world và level open-zone

## Mục tiêu

Thế giới có cảm giác tu tiên 3D để người chơi đi bộ, giao tiếp, quan sát và dùng động phủ trực tiếp; nhưng world simulation vẫn tuân thủ travel/time/economy/encounter của reference audit. Không làm seamless world khổng lồ trước khi vertical slice đạt visual và performance gate.

Không tái tạo nguyên xi map, tên riêng, kiến trúc, asset, NPC hoặc câu chuyện của game tham chiếu. Bản đồ và level kit là thiết kế mới, trong khi parity matrix kiểm chứng hành vi gameplay.

## Cấu trúc world

~~~text
World Map Simulation
  ├── Location definitions + travel edges + action-time costs
  ├── Zone Presentation Map (one or more UE maps per location)
  └── Dynamic state / quest / ownership / time-of-day patches

Open-zone UE maps
  ├── Town
  ├── Sect
  ├── Wilderness
  ├── Cave Residence
  ├── Sea / Island
  ├── Secret Realm
  ├── Combat Arena
  └── Tribulation Arena
~~~

World Map là source of travel logic. Zone actor là source of local interaction affordance. Running quanh zone không consume game date, trừ explicit audited command.

## Zone contract

Mỗi UTSLocationDefinition trỏ đến một FZoneDescriptor:

~~~text
LocationId
ZoneMapSoftPath
EntrySpawnTags
TravelGateDefinitions
InteractionManifest
NpcSpawnManifest
DataLayerRules
LightingProfileId
WorldStatePatchRules
PerformanceTier
~~~

Mỗi map có ATSZoneRoot actor:

- ZoneId phải match descriptor.
- Entry point components tagged TS.Entry.*.
- Interaction anchors implement `ITruongSinhInteractionProvider`.
- Không canonical player/NPC state trên level actor.
- BeginPlay: ZonePresentationSubsystem asks canonical snapshot và applies spawn/patch.
- Unload: actor state discarded; thay đổi chỉ persist qua accepted simulation command.

## Vertical slice composition

Vertical slice phải là chất lượng final target, không graybox cuối:

1. Tiền cảnh thành thị: đường đá, chợ nhỏ, service, NPC hội thoại, cổng travel.
2. Sân tông môn: cổng núi, bảng nhiệm vụ, trưởng lão/nhiệm vụ, contribution service.
3. Hoang dã: đường mòn, điểm hái linh thảo, encounter trigger, vật thể che tầm nhìn.
4. Nội thất động phủ/luyện đan: lò đan, kho, bàn tu luyện, một lô linh điền.
5. Combat arena: nền đủ đọc silhouette/element/VFX, camera space rõ.
6. Một khu thiên kiếp nhỏ hoặc weather/event staging area.

Lộ trình thử:

~~~text
Town → nhận một mục tiêu → Sect courtyard → wilderness interaction/encounter
→ combat arena → return → cultivation/alchemy → first breakthrough
~~~

Một route 10–15 phút phải chứng minh: third-person movement, dialogue, interaction, map travel, deterministic combat, time advance, save/load, Lumen/Nanite/PCG quality và performance target.

## Level construction standards

### World Partition và streaming

- World Partition cho zone ngoài trời đủ lớn; small interiors/arena dùng map riêng hoặc Level Instance.
- Cell size initial: 128 m; loading range initial: 256–384 m, sau đó profile trên RTX 3060.
- HLOD cho cụm architecture/rock/foliage xa; manual HLOD review cho landmark.
- PCG chỉ spawn decor/foliage/harvest presentation, không ownership logic hoặc quest-critical state.
- Quest-critical actor có persistent anchor/StableSpawnId và spawn từ manifest.
- Loading screen dùng transition subsystem, không block GameThread bằng sync load hàng loạt asset.

### Data Layers

Data Layer dùng cho presentation state: ngày/đêm variants, sự kiện, ownership, repair/destroyed set dressing, seasonal/quest visual patch.

Data Layer không là source of truth. WorldSimulation state tạo patch set; ZonePresentationSubsystem activation/deactivation layer theo patch. Sau Load Game, layer activation được rebuild từ snapshot.

### Level Instances

Dùng cho cổng, nhà, tiệm, động phủ room, cầu, shrine, ruin modular. Mỗi instance giữ visual/interaction anchor IDs, không mutable inventory/quest state trực tiếp.

## Interaction design trong 3D

`ITruongSinhInteractionProvider` expose:

~~~text
GetInteractionOffers(InstigatorId) → TArray<FTruongSinhInteractionOffer>
BuildInteractionCommand(CandidateId, InstigatorId, ExpectedRevision, Sequence)
  → FTruongSinhActionCommand
~~~

Offer chỉ chứa candidate ID, String Table key, disabled reason, priority, range và
enabled flag. Cost preview và eligibility cuối cùng do simulation tính; actor chỉ
có thể đưa gợi ý sớm. Interface C++ phải `Blueprintable`, function dùng
`BlueprintNativeEvent` để Blueprint actor implement mà không nắm authority.

Router chọn candidate theo:

1. Player aim/camera center.
2. Distance.
3. Interaction priority.
4. Stable anchor ID tie-breaker.

Interaction range defaults:

- NPC/service: 250 cm.
- Station/door: 225 cm.
- Gather point: 200 cm.
- Travel gate: 350 cm.

Các số là initial visual usability, không phải time cost. Prompt hiển thị action verb rõ, không chỉ icon.

## NPC presentation

- Canonical FNpcState quyết định ai sống, location, activity, faction/quest state.
- Spawn manifest tạo hero NPC, service NPC, crowd NPC hoặc proxy theo distance/importance.
- Hero NPC: full skeletal mesh, dialogue/interact component, 2–6 gần player trong slice.
- Service NPC: mesh full quality nhưng animation budget thấp hơn, tối đa 12 visible.
- Crowd: MassEntity/instanced proxy, không individual collision/quest logic, tối đa 40 visible in town at High.
- Khi NPC canonical state thay, actor visual update qua stable persistent ID; không random respawn làm sai relation/quest.

## Combat transition

Encounter không phải combat result. Khi command hợp lệ:

1. World scene locks interaction và creates combat snapshot.
2. Camera/streaming transition sang arena hoặc combat layer trong current zone.
3. Presentation gửi player combat command.
4. Combat resolves và emits replay/result.
5. Result applies world patch/reward/defeat state.
6. Return anchor do encounter definition chọn.

Không đặt random enemy state trong actor; encounter definition + seed phải tái tạo từ save.

## Cave residence / construction

Động phủ là zone riêng hoặc interior Level Instance:

- Building slots/anchors mang StableBuildSlotId.
- Preview actor không collision query ngoài build mode; không save.
- FBuildCommand validates item/cost/prerequisite/slot trong simulation.
- Result trả BuildInstanceId + presentation patch.
- Presentation spawn level instance/mesh tại slot.
- Upgrade/demolish dùng command và giữ migration map cho old build ID.

Công trình ảnh hưởng simulation qua definition effect, không qua overlap/collision actor.

## Navigation, collision và camera

- Explore character capsule dùng common standardized dimensions; mọi door/stairs test at run, walk, crouch.
- Navmesh support NPC presentation navigation; simulation AI không depend navmesh để calculate world result.
- Traversal zone (vault/ledge) authored, optional và không required cho parity-critical path.
- Camera collision spring arm tránh close wall clipping; không forced first-person squeeze.
- Mọi route cần fallback safe respawn tại last valid anchor sau out-of-world; debug teleport chỉ Development.

## Zone state validation

Automated functional test per zone:

- Load zone từ new session và late-game save.
- Spawn required interaction anchors.
- Apply valid Data Layer patch combination.
- Walk canonical route không collision trap.
- Enter/exit combat, return anchor và verify world revision.
- Unload/reload zone và compare presentation snapshot.
- Validate map soft path, entry tags, manifest ID và performance tier.
