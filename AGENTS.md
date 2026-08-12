# Quy ước cho agent — Trường Sinh UE5

Đọc toàn bộ các tài liệu sau theo đúng thứ tự trước khi sửa source, Blueprint, map, Data Asset hoặc content:

1. `docs/00_START_HERE.md`
2. `docs/02_PARITY_CONTRACT.md`
3. `docs/03_REFERENCE_VERSION.md`
4. `docs/05_PARITY_MATRIX.md`
5. `docs/06_GAME_DESIGN_DOCUMENT.md`
6. `docs/11_TECHNICAL_ARCHITECTURE.md`
7. `docs/14_WORLD_AND_LEVEL_DESIGN.md`
8. `docs/15_ART_DIRECTION.md`
9. `docs/22_PRODUCTION_ROADMAP.md`
10. `docs/24_WINDOWS_WORKFLOW.md`
11. `docs/25_AI_AGENT_PLAYBOOK.md`
12. `docs/28_UE58_FOUNDATION_CHECKLIST.md`
13. `docs/27_IMPLEMENTATION_STATUS.md`

## Mục tiêu làm việc

- Đây là game Windows UE 5.8 C++/Blueprint, góc nhìn thứ ba, open-zone, trình bày 3D bán hiện thực.
- `觅长生` là benchmark hành vi cho parity; gameplay simulation phải độc lập với animation, widget và level actor.
- V1 dùng chiến đấu theo lượt, linh khí/ngũ hành và combat replay 3D. Không tự chuyển thành action combat.
- Tất cả chữ hiện cho người chơi là tiếng Việt đơn giản; chuỗi phải nằm trong String Table hoặc data localization, không chôn trong logic.

## Quy tắc không được phá

- Không tự bịa công thức, dữ liệu, phần thưởng, điều kiện hoặc chi phí thời gian khi reference audit chưa xác minh. Ghi `UNKNOWN` vào parity matrix và tạo thí nghiệm.
- Không thay đổi lịch game vì người chơi đi bộ trong level 3D. Chỉ `Chronology` được quyền tiến thời gian.
- Không đưa asset, đoạn thoại, cốt truyện, dữ liệu trích xuất hoặc media từ game tham chiếu vào Git, `Content/` hoặc bản build. `ReferenceVault/` là thư mục local, audit-only và bị ignore.
- Không dùng Blueprint làm nguồn dữ liệu hoặc nơi quyết định kết quả simulation. C++ subsystem + data là nguồn đúng; Blueprint chỉ presentation/adapter.
- Không thêm content ngoài phạm vi vertical slice trước khi visual gate và core parity gate của slice đạt pass.
- Không thêm multiplayer, EOS, Steam Online, cloud save, workshop, analytics hoặc runtime service nếu chưa có quyết định mới trong `docs/26_DECISION_LOG.md`.
- Không dùng random không seed trong gameplay. Mọi RNG phải có seed, state và replay/save coverage.
- Không force-push, reset hard, ghi đè thay đổi của người khác hoặc chạy codegen/formatter làm thay đổi nhiều file khi chưa xem diff.

## Quy trình thay đổi

1. Đọc state hiện tại và matrix/case liên quan.
2. Viết hoặc cập nhật test oracle trước khi sửa logic parity.
3. Thay đổi nhỏ, tách core simulation khỏi presentation.
4. Build Editor/Development, chạy test phù hợp và PIE smoke test.
5. Cập nhật `docs/27_IMPLEMENTATION_STATUS.md` và parity matrix.
6. Commit nhỏ, mô tả đúng trạng thái; không gọi incomplete là complete.

## Definition of done một feature

- Có stable ID và content/data validation.
- Có hành vi audit được hoặc được đánh dấu rõ `UNKNOWN`.
- Có automated test hoặc manual test card.
- Save/load và deterministic replay được xét nếu feature thay đổi state.
- Presentation 3D không thay đổi simulation result.
- UI tiếng Việt, accessibility cơ bản và lỗi trạng thái được xử lý.
