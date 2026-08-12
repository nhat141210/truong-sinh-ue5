# Trạng thái triển khai

## Cập nhật 2026-08-12

M0 product reset đã hoàn tất ở mức source/docs và static validation trên VPS. Repo đã chuyển sang endless sandbox + auto-resolution, nhưng chưa có UE5.8 Windows evidence.

## Có ở source

- Module Core/Data/Simulation/Resolution/Narrative/Save/World/Presentation/UI.
- Typed command, revision/idempotency, deterministic RNG và BLAKE3 state hash.
- Canonical minutes, world layer, lifespan, soul và current vessel state.
- Life rules: effective/remaining lifespan, possession kế thừa identity/property/relations, reset cultivation, emergency vessel fallback.
- Shared activity plan/snapshot/preview/result/factors/presentation beats.
- Pure auto-resolver proof với bounded seeded variation.
- Save JSON schema v2 gồm time/soul/vessel/RNG/commands/pending replay và tamper check.
- Source automation tests cho core/RNG/time/save/resolution/lifespan/possession.
- Third-person legacy-input shell và DX12/SM6/Lumen config.

## Chưa có/không được nói là có

- UHT/UBT/Automation pass.
- Atomic file save, backup hoặc dev v1 migration.
- Smoke map, Enhanced Input, activity UI/commit handler hoặc pending replay runtime.
- NPC scheduler/event/home/economy.
- Blueprint, animation, Niagara, audio, bốn zone hoặc asset.
- PIE/Standalone/package/performance evidence.

## Mốc

| Mốc | Trạng thái | Việc mở khóa |
|---|---|---|
| M0 Product reset | COMPLETE (VPS STATIC) | source/docs mới, dependency tối thiểu và `validate-repo.sh` pass |
| M1 Windows native | NOT STARTED | verify/build/automation/smoke map/input |
| M2A Golden loop | NOT STARTED | activity commit + UI + save/Continue |
| M2B Visual target | NOT STARTED | corridor + one NPC/activity cue + RTX3060 capture |
| M3 Activity framework | SOURCE PROOF | registry/data/handlers/presentation runtime |
| M4 Living world | NOT STARTED | time/NPC/event/social/economy |
| M5 Lifespan/soul | SOURCE PROOF | gameplay integration/content/UI/save file safety |
| M6 Demo | NOT STARTED | four zones, content, both life routes |

## Windows task tiếp theo

1. Pull commit product reset; đọc Windows handoff/AGENTS.
2. Chạy verify, Editor+Development build và toàn bộ `TruongSinh` automation.
3. Sửa UHT/API exact UE5.8 có evidence; không tạo asset trước native pass.
4. Tạo smoke map + Enhanced Input.
5. Kết nối một cultivation plan vào simulation commit và save v2.

## Blocker

- VPS không có UE5.8/UHT/UBT.
- Save v2 file I/O/migration chưa hoàn tất.
- Source mới có thể cần chỉnh exact UE5.8 API/reflection trong build đầu.
