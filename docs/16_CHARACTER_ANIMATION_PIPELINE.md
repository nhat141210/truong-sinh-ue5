# 16 — Pipeline nhân vật và animation

## Mục tiêu

Nhân vật phải tạo cảm giác khám phá tu tiên third-person mượt, thanh thoát và dễ đọc, nhưng animation chỉ là presentation. Cảnh giới, chiêu thức, hit/result, trạng thái NPC và outcome combat do deterministic simulation quyết định.

Vertical slice cần một player hoàn chỉnh, hai NPC quan trọng, một đối thủ combat và crowd tier. Không sản xuất roster lớn trước khi pipeline shared skeleton, LOD, material variation và performance được khóa.

## Skeleton và modular character contract

### Shared skeleton

- Player, hero NPC, service NPC và basic opponent ưu tiên chung một UE5-compatible humanoid skeleton.
- Bone hierarchy, retarget pose, socket names và physics asset được versioned trong TSCharacterRigDefinition.
- Required sockets: root, head, spine_03, hand_l, hand_r, weapon_r, weapon_l, back, pelvis, foot_l, foot_r, camera_focus, vfx_chest.
- Naming/socket contract không được đổi sau khi có animation asset; thay đổi cần migration checklist và retarget test.
- Non-humanoid creature dùng skeleton riêng nhưng vẫn expose common CombatAnchor sockets qua interface.

### Modular body

Character dùng UTSCharacterVisualComponent với part slots:

~~~text
Body
Head
Hair
UpperGarment
LowerGarment
Footwear
OuterLayer
AccessoryA
AccessoryB
Weapon
BackItem
~~~

Part definition có shared skeleton, mesh LOD, material parameter set, compatible tags, importance tier và optional cloth/hair flags. Runtime save chỉ lưu character visual profile ID + approved cosmetic selection ID; không lưu hard mesh pointer.

## Character classes

| Tier | Dùng cho | Yêu cầu |
|---|---|---|
| Hero | player, nhân vật cốt truyện/camera close-up | 2K material, facial/eye controls, full locomotion, cue set |
| Service | merchant, elder, sect duty NPC | shared mesh/material, dialogue pose, budgeted locomotion |
| Combatant | enemy/duel NPC | combat stance/cue, simplified facial |
| Crowd | background town/sect NPC | instanced/low LOD, no face close-up, low update rate |
| Proxy | distant silhouette | impostor or simple mesh, no interaction/collision |

Một zone High chỉ có tối đa 1 player + 6 Hero/Combatant close + 12 Service visible. Crowd áp budget doc 19.

## Animation architecture

~~~text
ATSPlayerCharacter
  └── UTSCharacterVisualComponent
      ├── UTSAnimInstance
      │   ├── Locomotion State / Motion Matching
      │   ├── Linked Layer: Upper-body interaction
      │   ├── Linked Layer: Combat presentation
      │   ├── Additive Layer: cultivation/status
      │   └── Pose Cache / IK / Foot placement
      └── UTSCharacterCueComponent
          └── consumes replay/presentation cues
~~~

Animation Blueprint chỉ chọn blend state theo approved presentation state. Không compute damage, RNG, cooldown, resource cost, target validity hoặc quest outcome trong AnimBP.

## Locomotion

Game Animation Sample là nguồn kỹ thuật tham khảo/migration candidate. Bản sản phẩm dùng:

- Idle, walk, jog, sprint, stop, turn-in-place.
- Start/stop/turn transition phù hợp third-person camera.
- Slope/foot IK ổn định trên đường đá, bậc, terrain.
- Crouch chỉ bật nếu level interaction thực sự cần.
- Vault/ledge traversal optional, không required cho route parity-critical.
- Sprint không có stamina mechanic trừ khi audit xác nhận một rule tương đương.
- Air control và fall damage là presentation/local navigation rule, không tự thay world calendar.

Motion Matching phù hợp player/Hero nếu data set pass memory/CPU profile. Service/Crowd dùng state-machine locomotion rẻ hơn.

## Presentation state contract

C++ phát enum/state tag, AnimBP consume read-only:

~~~text
TS.Presentation.Explore.Idle
TS.Presentation.Explore.Move
TS.Presentation.Interact
TS.Presentation.Dialogue
TS.Presentation.Cultivate
TS.Presentation.Alchemy
TS.Presentation.Refine
TS.Presentation.Combat.Ready
TS.Presentation.Combat.Resolve
TS.Presentation.HitReact
TS.Presentation.Defeat
TS.Presentation.Tribulation
TS.Presentation.Cinematic
~~~

UTSCharacterCueComponent API:

~~~text
PlayCue(FPrimaryAssetId CueId, const FPresentationCueContext& Context)
StopCue(FGuid CueInstanceId)
SetPresentationState(FGameplayTag StateTag)
BindReplayEvent(const FCombatReplayEvent& Event)
~~~

Cue context có entity stable IDs, target transform snapshot, element tag, intensity tier, replay sequence. Không có raw calculation payload mà animation có thể sửa.

## Combat animation

- Combat resolved theo event log ở 18_3D_COMBAT_PRESENTATION.md.
- Mỗi action family có start, active, recovery presentation windows, nhưng state simulation đã commit trước.
- Hit react và defeat chọn từ deterministic event variant index, không reroll random trong AnimBP.
- Weapon trail, hand VFX, projectile socket và root motion cue phải có fallback khi target/ground unavailable.
- Root motion chỉ dùng cosmetic/camera-safe steps. Không dùng root motion để định vị entity phải khớp simulation grid/range.

## Cultivation, alchemy và social animation

Các action dài phải có loop và exit sạch:

- Meditate: enter → breathing loop → resolution pulse → exit.
- Alchemy/refining: station approach → work loop → result cue → exit.
- Dialogue: idle/talk/listen gestures theo priority, không motion quá mạnh làm camera khó đọc.
- Build/place: preview hand gesture; result spawn chỉ sau command accepted.
- Tribulation: stance, wind response, hit/recover/resolve, với reduced-motion alternative.

Khi UI được đóng/mở, animation phải return safe idle, không leave character frozen.

## LOD và update policy

| Tier | Skeletal LOD | Anim update | Cloth/Groom | Khoảng cách gợi ý |
|---|---|---|---|---|
| Player | LOD0–2 | every frame | selective | 0–25 m |
| Hero | LOD0–2 | every frame gần, URO xa | limited | 0–30 m |
| Service | LOD1–3 | URO ngoài 15 m | off ngoài 12 m | 0–35 m |
| Crowd | LOD2–4 | 10–15 Hz or VAT/proxy | off | 15–60 m |
| Proxy | billboard/simple | none | off | >60 m |

Khoảng cách được profile theo zone, không hard-lock nếu player camera/density khác. Skeletal mesh có LOD screen size explicit; không để Auto LOD không kiểm tra.

## Pipeline authoring

1. Tạo/import mesh vào staging, validate scale 1 Unreal Unit = 1 cm.
2. Bind/retarget vào shared skeleton.
3. Generate LOD/material instances/physics asset.
4. Tạo TSCharacterVisualProfile và assign tier.
5. Tạo/retarget locomotion and action clips.
6. Wire AnimBP linked layers và cue table.
7. Run character validation map: move, slope, stairs, dialogue, all combat cue, LOD transition.
8. Record visual/performance review trước khi merge content.

Control Rig dùng cho chỉnh pose, IK và cinematic bake. Runtime Control Rig chỉ dành cho ít character important and only after performance profiling.

## Acceptance và test

- Player đi/chạy/quay/dừng trên town, slope, stairs không foot slide đáng thấy hoặc camera snap.
- Tất cả socket bắt buộc tồn tại; Data Validation fail nếu thiếu.
- LOD transition không pop lớn ở camera explore.
- Animation cue fail vẫn trả control về safe state và không block simulation/UI.
- Save/load ở interaction/cultivation/combat boundary spawn đúng profile/pose safe.
- 20-minute soak trong vertical slice không memory leak, anim blueprint warning hoặc cloth/Groom spike vượt budget.
- High/Medium trên RTX 3060 đạt budget doc 19.
