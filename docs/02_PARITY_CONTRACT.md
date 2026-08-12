# Hợp đồng parity chức năng

## Ý nghĩa của “parity” trong dự án này

Mục tiêu là tái tạo **năng lực chơi, loại quyết định, quy tắc quan sát được và độ sâu liên kết hệ thống** của benchmark `觅长生`, trong một game có fiction, data, trình bày 3D và content do Trường Sinh UE5 tự author.

Parity **không** nghĩa là sao chép văn bản, UI pixel, tài sản, tên, nhân vật, câu chuyện, bản đồ, source code, định dạng save, bảng giá trị hoặc asset của benchmark. Mọi artifact benchmark chỉ được giữ riêng để audit nội bộ và không commit.

## Đơn vị parity: capability

Mỗi dòng `CAP-*` trong [05_PARITY_MATRIX.md](05_PARITY_MATRIX.md) mô tả một khả năng người chơi có thể quan sát. Một capability phải có năm bằng chứng:

1. **Question:** hành động/điều kiện cần audit là gì?
2. **Observed behavior:** ghi nhận trung tính, không sao chép nội dung sáng tạo.
3. **Own requirement:** behavior tương đương trong fiction và data gốc.
4. **Oracle:** test tự động hoặc kịch bản thủ công trả lời pass/fail.
5. **Evidence:** ID capture riêng, log test, result hash, screenshot/gameplay của bản dự án.

Không có evidence thì trạng thái không thể cao hơn `P1-SPECIFIED`.

## Thang trạng thái bắt buộc

| Trạng thái | Ý nghĩa | Điều kiện chuyển trạng thái |
| --- | --- | --- |
| `P0-NOT_AUDITED` | Chưa biết hành vi benchmark | Chưa được phép tự tạo công thức |
| `P1-SPECIFIED` | Đã có mô tả behavior và phạm vi | Có audit question + evidence ID + own requirement |
| `P2-TESTED` | Lõi luật đã có test/oracle | Unit/automation test pass, không phụ thuộc UI/level |
| `P3-PLAYABLE` | Người chơi thực hiện được trong build | PIE/package scenario pass và save/load đúng |
| `P4-COMPLETE` | Parity release-ready | P3 + coverage content cần thiết + performance/UX + regression pass |
| `BLOCKED` | Thiếu bằng chứng hoặc phụ thuộc | Ghi blocker và owner; không tự thay thông số |

Một capability không được chuyển trạng thái bằng cảm nhận “trông giống”.

## Quy tắc đánh giá

### Behavior tương đương

- Cùng loại input phải có cùng lớp outcome: hợp lệ/không hợp lệ, chi phí thời gian/tài nguyên, reward/risk, thay đổi world/NPC và save state.
- Các hằng số benchmark chưa audit phải để `REF_PENDING` trong data; code không được hard-code số phỏng đoán.
- Khi benchmark có biến ngẫu nhiên, hệ thống dự án dùng PRNG seeded. Oracle so outcome class, phân bố đã audit và replay, không dựa vào một lần quan sát đơn.
- Khi bản 3D kéo dài/đổi presentation, gameplay result không được thay đổi vì animation, camera, FPS, streaming hay skip cinematic.

### Content tương đương

- Số lượng và loại content phát hành chỉ được khóa sau audit có catalog. Cho đến lúc đó, `content-scale` là `UNKNOWN`, không được tuyên bố parity.
- Mỗi entry của dự án có ID/tên/lore/asset gốc. Không nhập hoặc dịch lại entry benchmark.
- Nếu benchmark có một hệ thống nhưng content gốc không đủ để người chơi thực hiện đầy đủ, capability chưa `P4`.

### UI và presentation

- Không cần pixel-perfect UI, nhưng mọi action, thông tin quyết định, trạng thái fail và kết quả phải dễ tìm và có thể thao tác bằng chuột/bàn phím.
- Mỗi action có preview/confirmation khi nó tiêu tốn thời gian, tuổi thọ, tài nguyên hiếm hoặc làm thay đổi quan hệ.
- 3D presentation là adapter: không giữ authority đối với combat, inventory, timeline hay quest.

## Audit an toàn và riêng tư

- Chỉ dùng bản hợp pháp qua gameplay bình thường để quan sát hành vi.
- Được ghi ghi chú, result table tự lập, capture màn hình/clip phục vụ audit nội bộ trong `ReferenceVault/` không commit.
- Không decompile, bypass DRM, extract asset, scrape/đóng gói lại data, hoặc đưa asset/text/save của benchmark vào repo.
- Commit chỉ giữ `REF-...` artifact ID, hash, mô tả trung tính và kết luận kỹ thuật.

## Change control

Khi phát hiện audit mới mâu thuẫn với code:

1. Hạ row về `P1-SPECIFIED` hoặc `BLOCKED`.
2. Ghi discrepancy ID trong `26_DECISION_LOG.md`.
3. Viết test tái hiện khác biệt trước khi sửa code.
4. Sửa simulation trước, presentation sau.
5. Chạy lại tất cả oracle liên quan và save migration test.

Không sửa số liệu bí mật trong Blueprint/Widget để “khớp nhanh”.

## Ma trận hoàn thành phát hành

| Gate | Điều kiện |
| --- | --- |
| `G0` | Reference version freeze và 100% CAP có question/owner |
| `G1` | CORE, SAVE, COMBAT foundations đạt P2 |
| `G2` | Vertical slice có route P3 từ khám phá → action → combat → save/load → breakthrough |
| `G3` | Tất cả nhóm system đạt P3, content-scale đã audit |
| `G4` | Mọi row bắt buộc P4, full run New Game → phi thăng, Windows package QA pass |
