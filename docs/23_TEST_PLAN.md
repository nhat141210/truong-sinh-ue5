# Test plan

## Source automation hiện có

- Stable ID.
- RNG golden vectors/isolation.
- Canonical time command/idempotency/deterministic replay.
- Save v2 round-trip/tamper detection.
- Auto-resolution determinism và bounded variation.
- Lifespan monotonicity.
- Possession identity/property/relations/reset/fallback.

Không gọi PASS cho tới khi chạy qua UE5.8 UHT/UBT/Automation trên Windows.

## Bắt buộc bổ sung

- Hybrid time 30/60/120 FPS cùng quantum; pause/menu/offline không tiến.
- Activity preview không tiêu RNG; confirm/reload/skip không reroll.
- 1x/2x/skip/missing cue cùng state hash.
- Realm/technique/pill/damage lifespan contributions và diminishing returns.
- NPC active/offscreen batch hội tụ cùng state.
- Authored event condition/cooldown/role/seed qua save/load.
- Atomic save/current→backup/corruption/migration.
- Soak 100 năm không duplicate NPC, property hoặc deadlock.

## Integration/visual

- New Game → interact → cultivation auto → save/Continue.
- Travel bốn zone chỉ load một map và trừ đúng time.
- Possession removes target NPC actor/AI and respawns player identity safely.
- RTX3060: 1080p High avg60/1%45, ≤10 full NPC, VRAM/RAM <8 GB.
- Medium/Low giữ đủ gameplay/thông tin.

## Release smoke

Demo 3–5 giờ có route trường thọ và đoạt xá, một conflict tránh được/auto, tiếp tục sau authored content; package chạy offline trên Windows sạch.
