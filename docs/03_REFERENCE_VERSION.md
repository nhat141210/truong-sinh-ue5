# Khóa bản tham chiếu — 觅长生 / Immortal Way of Life

> File này là manifest audit, không phải nơi lưu nội dung của game tham chiếu. Điền trên máy Windows có Steam và giữ artifact gốc ở `ReferenceVault/` ngoài Git.

## Trạng thái khóa hiện tại

| Trường | Giá trị |
| --- | --- |
| Reference title | 觅长生 / Immortal Way of Life |
| Steam App ID | `1189490` |
| Audit status | `UNLOCKED — phải hoàn thành trước khi suy luận công thức` |
| Audit owner | `TBD` |
| Date frozen | `TBD` |
| Steam branch | `TBD` |
| Build ID | `TBD` |
| Game version hiển thị | `TBD` |
| OS / locale | `TBD` |
| DLC / mod trạng thái | `TBD — mặc định: không mod, DLC ghi rõ` |
| Resolution / display scale | `TBD — mặc định audit 1920×1080, 100%` |

Không được ghi `FROZEN` khi còn một trường `TBD` ngoài owner/date có lý do được ghi rõ.

## Quy trình freeze trên Windows

1. Cài bản chính thức qua Steam, chọn branch stable được dùng làm benchmark và tắt toàn bộ mod/Workshop.
2. Ghi build ID/version mà Steam hoặc game hiển thị; chụp một capture riêng `REF-META-BOOT-*`.
3. Ghi Windows build, GPU driver, locale game, độ phân giải, FPS cap, difficulty/rule preset và controller/keyboard layout.
4. Tạo thư mục cục bộ không commit:

```text
ReferenceVault/
├── screenshots/
├── recordings/
├── saves/
├── experiments/
├── ui-measurements/
├── hashes/
└── audit-notes/
```

5. Tạo một save mới, một save giữa tiến trình và các save thí nghiệm theo `04_REFERENCE_AUDIT_GUIDE.md`. Các save này chỉ là artifact riêng, không được parse/import vào game dự án.
6. Lập `REF-META-MANIFEST-001` trong `ReferenceVault/audit-notes/`: artifact IDs, thời điểm capture, file hash, điều kiện thí nghiệm. Commit vào repo chỉ mã artifact/hashes và kết luận, không commit file gốc.
7. Khi đã bắt đầu P2 test của bất cứ CAP nào, không đổi version. Nếu Steam cập nhật, tạo manifest version mới và đánh dấu các CAP bị ảnh hưởng là `REVALIDATE`.

## Bộ save/capture tối thiểu cần có

| ID | Mục đích | Nội dung riêng cần quan sát |
| --- | --- | --- |
| `REF-SAVE-NEW-001` | baseline | tạo nhân vật, UI đầu, trạng thái ban đầu |
| `REF-SAVE-EARLY-001` | core loop | action time, inventory, quest, combat cơ bản |
| `REF-SAVE-CRAFT-001` | production | luyện đan/luyện khí/động phủ khi đã mở |
| `REF-SAVE-SECT-001` | social | tông môn, nhiệm vụ, cống hiến, relation |
| `REF-SAVE-MID-001` | progression | cảnh giới giữa, economy/world event |
| `REF-SAVE-LATE-001` | endgame | thiên kiếp/phi thăng và điều kiện cuối |

Nếu không thể có một save hợp lệ, ghi `NOT_AVAILABLE` và audit qua playthrough bình thường; không tải save lạ hoặc bypass tiến trình.

## Metadata mỗi thí nghiệm

Mỗi experiment note phải có các trường sau:

```yaml
artifact_id: REF-COMBAT-RESOURCE-001
reference_manifest: REF-META-MANIFEST-001
system: combat
question: "Khi action A được dùng trong condition B, state nào thay đổi?"
controlled_variables: [build, item_state, action_order]
changed_variable: action_A
observations: ["mô tả trung tính, không chép text/lore"]
trials: 3
confidence: high | medium | low
captures: [private-relative-path-or-hash]
project_capability: CAP-COMBAT-...
conclusion: "behavior cần triển khai, không phải asset/data sao chép"
```

## Điều không đưa vào manifest/repo

- Ảnh/video/audio/raw asset của game tham chiếu.
- File cài đặt, asset bundle, executable, DLL, source giả định hoặc data extract.
- Lời thoại, lore, text quest, danh sách NPC/item/map hay bảng dữ liệu nguyên văn.
- Save reference hay định dạng/file structure của save reference.

## Chứng nhận hoàn thành freeze

| Điều kiện | Trạng thái |
| --- | --- |
| Bản stable và build ID đã ghi | `PENDING` |
| Mod/DLC và locale đã khóa | `PENDING` |
| Baseline capture có hash | `PENDING` |
| ReferenceVault bị ignore bởi Git | `PENDING` |
| Mỗi CAP bắt đầu audit có manifest ID | `PENDING` |
