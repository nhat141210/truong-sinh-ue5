# Kế hoạch kiểm thử

## Nguyên tắc

Simulation là authoritative; presentation chỉ biểu diễn kết quả. Mọi test gameplay phải chạy được không cần FPS, animation, asset đẹp hay actor level. Test parity mô tả hành vi quan sát được bằng data độc lập; không nhúng text, asset, save hoặc code của game tham chiếu vào fixture.

## Tầng test

| Tầng | Mục tiêu | Chạy ở đâu | Ví dụ |
|---|---|---|---|
| Unit | Rule thuần/determinism | Windows commandlet hoặc automation | RNG, ngũ hành, action cost, checksum |
| Simulation | Snapshot → kết quả | Headless/commandlet Windows | tuổi thọ, NPC tick, breakthrough, economy |
| Integration | Subsystem + save + data | Editor/PIE | travel, quest, save/load, registry |
| Presentation | Bridge state → 3D/UI | PIE/standalone | actor spawn, combat replay, fast-forward |
| Visual/performance | Art và frame time | Windows RTX 3060 | Lumen, VSM, Niagara, streaming |
| Release | Người chơi thật | Máy sạch Windows | packaged Shipping smoke |

VPS không chạy UE5, không là bằng chứng FPS/hình ảnh và không lưu ReferenceVault. Nó chỉ phục vụ website Ca Đêm theo phạm vi riêng.

## Test bắt buộc theo hệ thống

### Core và thời gian

- CORE-001: cùng seed + cùng action sequence cho cùng snapshot/hash.
- CORE-002: đi bộ/nhìn/camera trong open-zone không tăng game date.
- CORE-003: travel, tu luyện, crafting, quest, event trừ đúng action-time cost.
- CORE-004: tuổi thọ, deadline, calendar rollover và pause không phụ thuộc delta time/FPS.

Bootstrap automation source hiện có:

- `TruongSinh.Core.StableId`.
- `TruongSinh.Core.DeterministicRng.GoldenVectorV1`.
- `TruongSinh.Core.DeterministicRng.StreamIsolation`.
- `TruongSinh.Simulation.CanonicalCommand`.
- `TruongSinh.Simulation.DeterministicReplay`.
- `TruongSinh.Save.RoundTripV1`.

Đây là test đã viết nhưng chưa được gọi PASS cho đến khi UHT/UBT và Automation
Framework UE5.8 chạy trên Windows, report có test count và không có failure.

### Tu luyện và chế tạo

- CULT-001: điều kiện đột phá đủ/thiếu, success/failure và hậu quả.
- CULT-002: bế quan dừng/resume/save/load bảo toàn state.
- CRAFT-001: recipe hợp lệ, nguyên liệu thiếu, variation theo seed và inventory result.
- HOME-001: động phủ/linh điền áp modifier vào simulation, không chỉ thay visual.

### Combat

- CMB-001: snapshot/seed tạo battle log giống nhau qua nhiều lần.
- CMB-002: từng lượt áp đúng cost, element, effect, duration và damage/heal.
- CMB-003: AI chọn action hợp lệ, không dùng ability thiếu điều kiện.
- CMB-004: replay, skip, fast-forward không thay result hoặc save.
- CMB-005: asset/animation cue mất vẫn hoàn thành simulation với fallback an toàn.

### World, quest và save

- WORLD-001: NPC lifecycle, relationship, sect và event scheduler deterministic.
- QUEST-001: accept/progress/fail/deadline/reward và branch không tạo dead-end.
- ECO-001: shop/auction/trade có price/result đúng snapshot.
- SAVE-001: save v1, corruption fallback, backup, atomic write và migration fixture.
- SAVE-002: load trong zone/combat-safe checkpoint khôi phục world, actor presentation, UI state nhất quán.

### Content và localization

- DATA-001: stable ID duy nhất, reference hợp lệ, GameplayTag hợp lệ.
- DATA-002: content không mồ côi; required quest/location/item có route kiểm thử.
- LOC-001: không literal tiếng Việt trong C++/Blueprint gameplay; String Table key tồn tại.

## Golden oracles và audit

Mỗi hàng parity matrix có một hoặc nhiều oracle:

Oracle ID:
Audit ID / nguồn quan sát cục bộ:
Tiền điều kiện:
Input/action sequence:
Seed nếu có:
Kết quả quan sát mong đợi:
Kết quả dự án:
Automation test:
Confidence: OBSERVED | INFERRED | UNKNOWN

INFERRED chỉ dùng tiếp tục R&D; không được đóng parity. UNKNOWN là blocker M9 nếu áp dụng đường chơi chính.

## Quy trình chạy test Windows

1. Build C++ theo cấu hình task từ Visual Studio hoặc Unreal Build Tool.
2. Mở Unreal Editor, chạy automation test tập TruongSinh.*.
3. Chạy PIE/Standalone manual smoke cho thay đổi có presentation.
4. Với release candidate, chạy commandlet/headless test nếu project đã có launcher script và chạy packaged build.
5. Ghi commit, engine version, map, seed, hardware và kết quả vào 27_IMPLEMENTATION_STATUS.md.

`tools/run-tests.ps1` phải kiểm cả process exit code lẫn report export: có ít
nhất một test khớp filter và không có state fail/not-run. Zero-test run không được
tính pass. Không dùng test thao tác chuột làm test duy nhất.

## Visual và performance gate

Thiết bị chuẩn: i5-12400F, RTX 3060, Windows 10/11, 1920×1080. Đo sau shader warm-up ở build Development/Shipping.

| Profile | Mục tiêu | Kiểm tra |
|---|---|---|
| High | trung bình 60 FPS, 1% low ≥45 FPS | vertical slice, combat Niagara, 20 NPC high-quality |
| Medium | gameplay đủ, giảm shadow/fog/reflection/foliage hợp lý | cùng tuyến slice |
| Low | ổn định, UI/combat dễ đọc | cùng tuyến slice |

Capture stat unit, stat gpu, memory report và Unreal Insights khi có regression. Screenshot đẹp không phải proof gameplay.

## Exit criteria M10

- Toàn bộ automation bắt buộc pass trên commit release.
- Không crash/blocker trong smoke New Game → travel → quest → combat → save/load → return.
- Hai long-run save fixture và một clean-machine run pass.
- Không asset/reference file ngoài manifest trong packaged output.
