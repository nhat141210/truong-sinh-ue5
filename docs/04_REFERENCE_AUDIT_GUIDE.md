# Hướng dẫn audit benchmark chức năng

## Mục đích

Audit biến quan sát gameplay hợp pháp thành yêu cầu kỹ thuật kiểm chứng được. Nó không phải quy trình sao chép content. Đầu ra duy nhất được commit là: câu hỏi, behavior đã khái quát, confidence, artifact ID riêng, requirement gốc và oracle của dự án.

## Chu trình cho một capability

1. Chọn một row `CAP-*` đang `P0-NOT_AUDITED` trong parity matrix.
2. Viết **một câu hỏi đo được**, ví dụ: “Action travel tiêu tốn bao nhiêu loại tài nguyên và những scheduler nào được chạy?”
3. Đặt baseline và chỉ thay đổi một biến mỗi trial.
4. Chạy ít nhất ba trial nếu kết quả có yếu tố ngẫu nhiên hoặc điều kiện ẩn.
5. Ghi observation trung tính: input, pre-state, post-state, ngày/thời gian, UI state, result class. Không chép text/lore/tên/content gốc.
6. Lập giả thuyết hành vi với confidence `high`, `medium` hoặc `low`.
7. Tạo own requirement và test oracle; cả hai dùng ID/nội dung gốc của dự án.
8. Chỉ nâng row lên `P1-SPECIFIED` khi evidence và requirement đều có mặt.

## Quy ước artifact

```text
REF-<SYSTEM>-<QUESTION>-<RUN>-<YYYYMMDD>

Ví dụ:
REF-CHRONO-TRAVEL-COST-001-20260812
REF-COMBAT-TURN-ORDER-003-20260812
REF-ALCHEMY-FAILURE-002-20260812
```

Artifact vật lý ở `ReferenceVault/`; docs Git chỉ ghi ID và SHA-256 của file/capture nếu cần truy xuất nội bộ. Không dùng tên file/capture chứa text hay asset benchmark.

## Protocol chung

| Bước | Bắt buộc |
| --- | --- |
| Baseline | Chụp pre-state: date, tuổi thọ, resource, build, quest/world flags |
| Isolation | Thay một input hoặc một condition ở mỗi run |
| Repetition | 1 run deterministic rõ ràng; tối thiểu 3 run nếu có RNG/AI/hidden condition |
| Recording | Ghi action order, post-state, result, error/failure message class |
| Comparison | Đối chiếu baseline, không suy luận từ animation/màu/UI đơn thuần |
| Confidence | High = lặp và giải thích được; Medium = quan sát lặp nhưng còn biến; Low = chỉ manh mối |
| Conversion | Viết test/project requirement rồi mới code |

## Checklist audit theo hệ thống

### Chronology, tuổi thọ và world tick

- Các action nào tiêu tốn thời gian; action nào chỉ là điều hướng UI/presentation?
- Chi phí đi liên vùng, tu luyện, crafting, nghỉ, quest và transaction được báo cho người chơi ra sao?
- Khi lịch nhảy, subsystem nào đổi: tuổi thọ, NPC, market, quest deadline, event, resource production?
- Điều gì xảy ra ở mốc ngày/tháng/năm và khi tuổi thọ cạn?

### Tu luyện và đột phá

- Điều kiện vào/ra mỗi state (cảnh giới, progress, resource, location, trạng thái) là gì?
- Hành động nào tăng progress; có failure/success branches nào?
- Đột phá tiêu tốn gì, có preview/confirmation gì, cập nhật state nào?
- Thứ tự xử lý thiên kiếp/hậu quả/cứu vãn là gì?

### Combat

- Snapshot đầu combat gồm những resource/status/equipment nào?
- Turn order, resource gain/spend, action validity, draw/refresh, duration/status ticks xử lý theo thứ tự nào?
- Player/AI có lựa chọn gì; outcome áp trở lại world ở thời điểm nào?
- Animation có thay đổi kết quả không? (Dự án phải trả lời là không.)

### Chế tạo, động phủ và tài nguyên

- Ingredient/material, quality, tool/location, time cost và outcome class.
- Failure behavior: mất gì, giữ gì, có thể retry không?
- Động phủ/linh điền tác động vào production hoặc cultivation ở thời điểm tick nào?

### NPC, tông môn, quest, economy

- Relation/deadline/status nào mở/khóa action?
- Tông môn và tán tu dùng resource/obligation khác nhau thế nào?
- Giá, stock, auction/trade được cập nhật theo action/tick nào?
- Quest có precondition, branch, timeout, reward, world patch gì?

## Mẫu audit record ngắn

```markdown
### REF-<ID>
- Capability: `CAP-...`
- Question: ...
- Baseline: ...
- Changed variable: ...
- Trials: ...
- Observation: ...
- Confidence: High / Medium / Low
- Project rule: ...
- Oracle: `Automation.<Suite>.<Case>` hoặc manual scenario ID
- Open question: ...
```

## Khi kết quả chưa rõ

- Ghi `UNKNOWN`, không chuyển nó thành một hằng số trong code.
- Lập experiment kế tiếp với biến kiểm soát tốt hơn hoặc trạng thái khác.
- Nếu chưa thể audit, thiết kế interface/supporting data nhưng khóa action bằng feature flag `RefPending`.
- Nếu agent có ý tưởng mới, đưa vào backlog hậu parity; không trộn vào own requirement hiện tại.

## Kiểm tra chất lượng audit

Một audit bị từ chối nếu:

- Không xác định được pre-state và post-state.
- Chỉ có một run cho một behavior rõ ràng có RNG.
- Copy nguyên văn text, lore hoặc danh mục content tham chiếu vào note.
- Dùng capture/asset/reference file trong Git.
- Không liên kết tới parity row và test oracle.
