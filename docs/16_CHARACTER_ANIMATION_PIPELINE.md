# Character và animation pipeline

## Phạm vi

- Một skeleton humanoid chung.
- Player + ba body archetype NPC.
- Modular clothing; hero hair chất lượng cao, NPC dùng hair cards.
- 12–20 NPC canonical nhưng chỉ 6–10 full animation visible.

## Locomotion

Game Animation Sample/Motion Matching là nền cho idle, walk, run, turn, jump, slope/stairs và mantle tại ledge đánh dấu. Không free climbing, parkour hệ thống, bơi lặn sâu hoặc ngự kiếm điều khiển tự do bản đầu.

## Activity animation

Montage không gắn formula. `FPresentationBeat` tra cue theo:

```text
activity + element + delivery + impact + scale + symbol
```

Shared montage family: prepare, channel, release, receive/counter, result, recover. Đấu pháp auto chỉ phát replay; không hitbox gameplay hoặc root-motion quyết định outcome.

## NPC budget

- Animation Budget Allocator từ visual slice.
- NPC gần: full update/facial preset.
- NPC vừa: giảm update/interpolate.
- NPC xa: proxy/idle rẻ hoặc ẩn.
- Offscreen: không Skeletal Mesh/Actor.

## Validation

Một character test map kiểm locomotion, mantle, dialogue pose, activity montage, missing-cue fallback, LOD/cloth/hair và 10 NPC budget. Save/load spawn safe pose, không resume root motion giữa chừng.
