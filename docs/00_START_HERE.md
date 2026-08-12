# Bắt đầu tại đây — Trường Sinh UE5

## Tuyên ngôn

Trường Sinh UE5 là game sandbox tu tiên 3D không có hồi kết cố định. Người chơi đi lại, khám phá, kết giao, xây động phủ và chọn cách chuẩn bị. Simulation tự giải quyết tu luyện, đấu pháp, luyện đan, bố trận và đột phá; UE5 chỉ trình diễn kết quả đã commit.

Mục tiêu trải nghiệm là thế giới phương Đông sống động, điện ảnh và giàu hoạt động như các open-world hiện đại, nhưng phạm vi được khóa cho dự án AI/solo: bốn open-zone, 12–20 NPC canonical, không action combat, multiplayer, seamless world hoặc runtime AI-chat.

## Đọc tiếp

1. `02_ENDLESS_SANDBOX_CONTRACT.md` — điều không được đổi.
2. `05_EXPERIENCE_CAPABILITY_MATRIX.md` — trạng thái capability thật.
3. `06_GAME_DESIGN_DOCUMENT.md` — vòng chơi và demo 3–5 giờ.
4. `08_AUTO_RESOLUTION_SPEC.md` — framework chung mọi activity.
5. `11_TECHNICAL_ARCHITECTURE.md` — module và authority.
6. `13_SAVE_AND_DETERMINISM.md` — save, RNG và replay.
7. `14_WORLD_AND_LEVEL_DESIGN.md` — bốn open-zone.
8. `15_ART_DIRECTION.md` đến `19_PERFORMANCE_BUDGET.md` — visual trong budget RTX 3060.
9. `22_PRODUCTION_ROADMAP.md` và `27_IMPLEMENTATION_STATUS.md` — việc phải làm tiếp và bằng chứng hiện có.

## Luật sản phẩm

- Không main quest hoặc ending bắt buộc.
- Phi thăng không thuộc bản đầu; khi thêm sẽ mở thượng giới, không chạy credits.
- Tu sĩ có thể trường thọ nhờ cảnh giới, công pháp, đan dược và tài nguyên; đoạt xá không bị ép.
- Activity không có input phản xạ sau khi xác nhận.
- Đi bộ/khám phá có thời gian lai; action lớn nhảy ngày/tháng/năm; game đóng thì thế giới dừng.
- NPC canonical dùng cùng lịch, tuổi thọ và progression với người chơi ở mức simulation.
- Toàn bộ chữ người chơi thấy là tiếng Việt đơn giản từ String Table/data.

## Ngày đầu trên Windows

1. Xác nhận working tree sạch và exact UE5.8.
2. Chạy `verify-windows-environment.ps1`, `build-windows.ps1`, `run-tests.ps1`.
3. Ghi evidence vào status.
4. Chỉ khi native gate pass mới tạo smoke map và Enhanced Input.
5. Chỉ khi golden loop pass mới import asset cho visual target corridor.

Không dùng VPS làm bằng chứng cho UE build, hình ảnh, FPS hoặc Windows package.
