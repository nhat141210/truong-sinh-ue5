# Quy ước cho agent — Trường Sinh UE5

Đọc toàn bộ các tài liệu sau theo đúng thứ tự trước khi sửa source, Blueprint, map, Data Asset hoặc content:

1. `docs/WINDOWS_AGENT_HANDOFF.md`
2. `docs/00_START_HERE.md`
3. `docs/02_ENDLESS_SANDBOX_CONTRACT.md`
4. `docs/05_EXPERIENCE_CAPABILITY_MATRIX.md`
5. `docs/06_GAME_DESIGN_DOCUMENT.md`
6. `docs/08_AUTO_RESOLUTION_SPEC.md`
7. `docs/11_TECHNICAL_ARCHITECTURE.md`
8. `docs/13_SAVE_AND_DETERMINISM.md`
9. `docs/14_WORLD_AND_LEVEL_DESIGN.md`
10. `docs/15_ART_DIRECTION.md`
11. `docs/19_PERFORMANCE_BUDGET.md`
12. `docs/22_PRODUCTION_ROADMAP.md`
13. `docs/25_AI_AGENT_PLAYBOOK.md`
14. `docs/27_IMPLEMENTATION_STATUS.md`

## Mục tiêu đã khóa

- Đây là sandbox-life simulation tu tiên Windows bằng Unreal Engine 5.8, C++/Blueprint, góc nhìn thứ ba và open-zone.
- Mục tiêu là cảm giác nhìn, đi và sống trong thế giới tu tiên bán hiện thực; không sao chép quy mô, content, asset hoặc action combat của game tham khảo.
- Không có main quest, ending hoặc credits bắt buộc. Một save theo một nguyên thần qua nhiều thân xác và tiếp tục không giới hạn bởi một kết cục authored.
- Người chơi chuẩn bị và lựa chọn; các hoạt động lớn dùng chung pipeline `Plan → Resolve → Commit → Present`.
- Đấu pháp, tu luyện, đột phá, luyện đan và bố trận tự giải quyết deterministic. Không thêm thao tác phản xạ/minigame riêng.
- Phi thăng chưa thuộc bản đầu; sau này chỉ mở world layer mới, không kết thúc save.

## Quy tắc không được phá

- Simulation C++ + data là authority. Actor, Widget, animation, Niagara, camera và level Blueprint không được quyết định kết quả.
- Không dùng `FMath::Rand`, random không seed, đồng hồ hệ điều hành hoặc FPS làm entropy gameplay.
- Không runtime LLM, AI-chat/voice, backend, multiplayer, EOS, PvP, cloud save hoặc tiến triển ngoại tuyến.
- Không biến dự án thành seamless open world. Bản đầu chỉ bốn zone tải riêng; mỗi lần một zone.
- Không thêm action combat, hitbox, combo, dodge, parry, boss mechanics hoặc Behavior Tree chiến đấu.
- Không tạo minigame cho luyện đan, bố trận, chế tạo hay độ kiếp. Activity mới phải dùng resolver/presentation framework chung.
- Không import hàng loạt asset. Mọi asset ngoài dự án có provenance; `ReferenceVault/`, `_external/`, cache, build và secret không được commit.
- Không tuyên bố map, Blueprint, VFX, build, PIE, FPS hoặc package đã PASS nếu chưa có evidence Windows UE5.8.
- Không force-push, reset hard hoặc ghi đè thay đổi chưa hiểu.

## Quy trình thay đổi

1. Đọc status và capability liên quan.
2. Nêu canonical state, command/result và test trước khi sửa.
3. Thay đổi nhỏ; ưu tiên một resolver/cue framework dùng lại.
4. Build, automation, PIE/Standalone đúng mức rủi ro trên Windows.
5. Cập nhật `docs/05_EXPERIENCE_CAPABILITY_MATRIX.md`, `docs/26_DECISION_LOG.md` và `docs/27_IMPLEMENTATION_STATUS.md` bằng sự thật có evidence.
6. Commit nhỏ, không gọi source-only là runtime complete.

## Definition of done một capability

- Có stable ID, schema và validator.
- Kết quả deterministic, save/load không reroll hoặc commit hai lần.
- Presentation có thể lỗi/skip mà state vẫn đúng.
- Có automated test hoặc manual test card.
- UI tiếng Việt đơn giản; không hard-code player text trong C++/Blueprint logic.
- Nếu có 3D/UI/VFX: có PIE/Standalone evidence và performance capture phù hợp.
