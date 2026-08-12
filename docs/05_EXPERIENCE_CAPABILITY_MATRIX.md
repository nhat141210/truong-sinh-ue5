# Ma trận capability trải nghiệm

Trạng thái: `NOT_STARTED`, `SOURCE_ONLY`, `WINDOWS_VERIFIED`, `SLICE_PASS`, `RELEASE_PASS`.

| ID | Capability | Acceptance chính | Trạng thái |
|---|---|---|---|
| CORE-001 | Command deterministic/idempotent | Retry không commit hai lần; cùng seed/input cùng hash | WINDOWS_VERIFIED |
| TIME-001 | Hybrid exploration time | 30/60/120 FPS cho cùng game time quantum | NOT_STARTED |
| TIME-002 | Action time skip | Tu luyện/travel/craft cập nhật scheduler đúng thứ tự | NOT_STARTED |
| LIFE-001 | Effective lifespan | Realm/technique/pill/damage cập nhật remaining lifespan | SOURCE_ONLY |
| LIFE-002 | Trường thọ | Build hợp lệ sống tiếp mà không bị ép đoạt xá | NOT_STARTED |
| SOUL-001 | Soul/vessel separation | Công pháp/ký ức thuộc soul; body giữ căn cơ/tu vi | SOURCE_ONLY |
| SOUL-002 | Đoạt xá | Nhận identity/assets/relations mục tiêu; reset body cultivation | SOURCE_ONLY |
| SOUL-003 | Wandering Soul | Thất bại không kết thúc save | SOURCE_ONLY |
| ACT-001 | Shared activity pipeline | Registry Data Asset chọn cultivation/đột phá cùng Plan→Resolve→Commit→Present, không widget/subsystem riêng | WINDOWS_VERIFIED |
| ACT-002 | Replay safety | 1x/2x/skip/missing cue cùng result; load không reroll | NOT_STARTED |
| CULT-001 | Tu luyện/đột phá | Tu vi/time/lifespan thay đổi deterministic | WINDOWS_VERIFIED |
| ALC-001 | Luyện đan auto | Recipe/facility/strategy tạo pill số lượng/phẩm chất/tạp chất canonical, không minigame | WINDOWS_VERIFIED |
| FORM-001 | Bố trận auto | Plan/material/site tạo formation state + replay | WINDOWS_VERIFIED |
| CONFLICT-001 | Đấu pháp auto | Realm/build/counter/strategy quyết định, RNG nhỏ | SOURCE_ONLY |
| CONFLICT-002 | Tránh đấu | Có negotiate/pay/flee/sect-assist route | NOT_STARTED |
| NPC-001 | Canonical schedule | Spawn đúng anchor/time; offscreen không cần Actor | NOT_STARTED |
| NPC-002 | Long simulation | Soak 100 năm không duplicate/deadlock/ownership lỗi | NOT_STARTED |
| EVENT-001 | Authored seeded events | Condition/role/cooldown/seed ổn định qua load | NOT_STARTED |
| HOME-001 | Động phủ | Build/facility thay simulation và visual patch | NOT_STARTED |
| WORLD-001 | Four-zone travel | Một zone loaded; travel cinematic + time cost | NOT_STARTED |
| SAVE-001 | Save integrity | Atomic/backup/checksum/migration; pending replay phục hồi | SOURCE_ONLY |
| VIS-001 | Visual target | Corridor đạt art bible và 1080p budget | NOT_STARTED |
| PERF-001 | RTX3060 High | 60 FPS avg, 1% low ≥45, VRAM/RAM <8 GB | NOT_STARTED |
| RELEASE-001 | Endless demo | 3–5 giờ, hai đường sống, tiếp tục sau authored content | NOT_STARTED |

Mỗi task cập nhật đúng một hoặc vài row và gắn evidence. Không đổi sang PASS chỉ vì có tài liệu hoặc header C++.
