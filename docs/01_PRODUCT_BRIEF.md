# Product brief — Trường Sinh UE5

## Tuyên ngôn sản phẩm

**Trường Sinh UE5** là RPG tu tiên 3D góc nhìn thứ ba, nơi người chơi sống một đời tu sĩ dài hạn: chọn tông môn hoặc tán tu, tích lũy tài nguyên, tu luyện, chiến đấu theo lượt, xây động phủ, đối mặt tuổi thọ và cuối cùng thử phi thăng.

Điểm khác biệt của dự án không nằm ở việc thêm action combat vội, mà ở việc cho một mô phỏng tu tiên sâu xuất hiện thành **địa điểm, nhân vật, thiên tượng và đấu pháp 3D có thể nhìn thấy**. Mọi hậu quả hệ thống vẫn do simulation deterministic quyết định.

## Khán giả và nền tảng

| Mục | Quyết định |
| --- | --- |
| Nền tảng V1 | Windows x64, build độc lập |
| Engine | Unreal Engine 5.8, C++ là lõi |
| Điều khiển | Chuột/bàn phím trước; controller sau vertical slice |
| Ngôn ngữ đầu | Tiếng Việt đơn giản qua String Tables |
| Góc nhìn khám phá | Third-person camera |
| Đấu pháp | Theo lượt, thông tin rõ, có trình diễn arena 3D |
| Thế giới | Open-zone; bản đồ thế giới nối zone và tính travel cost |
| Hiệu năng chuẩn | i5-12400F + RTX 3060, 1080p High 60 FPS mục tiêu |

## Lời hứa trải nghiệm

Người chơi luôn thấy được bốn câu hỏi có ý nghĩa:

1. **Ta dùng thời gian còn lại vào đâu?** Tu luyện, kiếm tài nguyên, tông môn, quan hệ và nguy hiểm đều cạnh tranh với tuổi thọ.
2. **Ta xây build nào?** Linh căn, công pháp, thần thông, pháp bảo, đan dược và con đường đạo phải tạo quyết định dài hạn.
3. **Ta thuộc về đâu?** Tông môn đổi an toàn lấy nghĩa vụ; tán tu đổi tự do lấy rủi ro và cơ duyên.
4. **Ta đã thay đổi thế giới ra sao?** NPC, kinh tế, quan hệ, quyền kiểm soát vùng và nội dung động phủ phản hồi lại hành động của người chơi.

## Vòng lặp chính

```text
Chọn mục tiêu
→ khám phá zone / chuyển vùng
→ tương tác, nhận nhiệm vụ hoặc tìm cơ duyên
→ giải quyết đấu pháp / giao dịch / chế tạo
→ Simulation tiêu tốn thời gian và cập nhật thế giới
→ trở về tu luyện, động phủ hoặc tông môn
→ chuẩn bị đột phá / thiên kiếp
→ mở lớp mục tiêu mới
```

Đi bộ, quan sát và nói chuyện trong một zone là presentation thời gian thực. Các action như travel liên vùng, bế quan, luyện đan, luyện khí, nhiệm vụ, giao dịch hoặc nghỉ ngơi là transaction gameplay có `ActionTimeCost` rõ ràng.

## Scope phát hành

### Bắt buộc trước bản 1.0

- Toàn bộ nhóm capability trong `05_PARITY_MATRIX.md` đạt `P4-COMPLETE`.
- Hành trình hoàn chỉnh từ tạo nhân vật đến phi thăng.
- Combat, tu luyện, chế tạo, động phủ, tông môn/tán tu, NPC, quest, kinh tế và sự kiện thế giới tương tác cùng một save.
- Một bộ open-zone 3D hoàn chỉnh cho mỗi loại trải nghiệm chính; không dùng menu 2D để thay thế vĩnh viễn trải nghiệm không gian.
- High/Medium/Low, save migration, pause, alt-tab, clean-machine package test.

### Không làm trước parity

- Multiplayer, PvP, MMO/backend, cloud save, analytics hoặc marketplace.
- Seamless map toàn bộ thế giới, VR, mobile hoặc console.
- Action combat thay cho combat theo lượt.
- Quy trình tạo content vô hạn/procedural để che thiếu content authored.
- Quét/chép lại asset, đối thoại, map, NPC hoặc nội dung của game tham chiếu.

## Vertical slice phải chứng minh điều gì

Vertical slice có nội dung **gốc** và phải hoàn thành được trong 25–35 phút:

1. Tạo nhân vật với một lựa chọn linh căn/căn cơ.
2. Đi qua một thị trấn nhỏ, một đường hoang dã và một tiền sảnh tông môn 3D.
3. Nhận một nhiệm vụ hoặc lựa chọn làm tán tu.
4. Nhặt/giao dịch nguyên liệu, học một công pháp, tu luyện một lần.
5. Chơi một đấu pháp ngũ hành theo lượt có replay 3D.
6. Luyện một vật phẩm cấp thấp **hoặc** xử lý một thất bại chế tạo.
7. Tiêu tốn thời gian, thấy một NPC/world event cập nhật, save/load trở lại đúng state.
8. Thực hiện một đột phá cấp đầu với kết quả deterministic theo input/seed.

Slice chỉ pass khi nó vừa đạt logic, vừa đạt visual target, vừa đạt budget FPS. Không mở rộng thêm zone trước khi cả ba đều pass.

## Nguyên tắc nội dung gốc

- Bối cảnh làm việc: **Vạn Cảnh Giới**, một quần giới do linh mạch suy kiệt và cổ trận phong ấn chia cắt. Đây là fiction gốc, dùng để author content riêng.
- Nhân vật, tông môn, địa danh, cốt truyện, icon, thuật ngữ hiển thị, nhiệm vụ và lời thoại phải do dự án tạo mới.
- Game tham chiếu chỉ cung cấp benchmark riêng tư cho loại quyết định, nhịp tiến trình và độ sâu liên kết hệ thống.
- Không dùng “parity” để biện minh cho việc sao chép biểu đạt sáng tạo.

## Chỉ số thành công

| Chỉ số | Gate |
| --- | --- |
| Functional parity | 100% row bắt buộc `P4-COMPLETE` |
| Full run | New Game đến phi thăng không có blocker |
| Determinism | Cùng snapshot + seed + command log cho cùng result hash |
| Save integrity | Save/load không đổi world snapshot hợp lệ |
| Visual | 1080p High trên RTX 3060: mục tiêu 60 FPS; 1% low ≥45 |
| Content integrity | Không có dangling ID, missing localization hay quest dead-end |
| UX | Action có time cost và hậu quả hiển thị rõ trước/hoặc ngay sau khi xác nhận |
