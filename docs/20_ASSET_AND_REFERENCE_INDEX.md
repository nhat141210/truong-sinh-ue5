# Chỉ mục asset và reference

## Mục đích

Tài liệu này quản lý nguồn của mọi asset nhìn/nghe thấy trong bản game và mọi tư liệu dùng để audit game tham chiếu. Game cần đạt chiều sâu hệ thống của 觅长生, nhưng không được đưa texture, model, nhạc, thoại, ảnh chụp màn hình, video, text, data trích xuất hay save của game tham chiếu vào Git hoặc bản phát hành.

## Hai kho tách biệt

| Kho | Vị trí Windows | Được commit | Dùng cho |
|---|---|---:|---|
| Project | D:\GameDev\truong-sinh-ue5 | Có | Source, asset tự tạo/được phép dùng, metadata và test |
| ReferenceVault | D:\GameDev\ReferenceVault\ImmortalWayOfLife | Không | Quan sát, screenshot/video audit, save thử nghiệm, OCR cục bộ và hash |

ReferenceVault phải nằm ngoài repo hoặc trong thư mục bị Git ignore hoàn toàn. Không tạo symbolic link từ Content sang vault. Chỉ metadata đã tổng hợp, không chứa nội dung độc quyền, mới được ghi vào docs.

## Nguồn asset đích

Vertical slice dùng một bộ asset nhỏ, nhất quán và đủ để duyệt chất lượng: thành thị nhỏ, sân tông môn, đoạn hoang dã, interior luyện đan, arena, nhân vật chính, hai NPC và một yêu thú. Không tải asset hàng loạt trước khi visual target corridor được duyệt.

| Nhóm | Nguồn ưu tiên | Cách dùng |
|---|---|---|
| Đá, đất, cây, hang động | Quixel/Megascans và Fab | Tạo biome, material layer, PCG foliage; kiểm tra LOD/Nanite |
| Kiến trúc/prop Á Đông | Fab hoặc asset tự dựng | Dùng bộ có metadata rõ; material hóa theo art direction dự án |
| Nhân vật/animation | Game Animation Sample, MetaHuman hoặc asset skeleton tương thích | Dùng cho presentation; gameplay không phụ thuộc montage cụ thể |
| VFX/SFX | Niagara/asset tự tạo hoặc thư viện có metadata | Mỗi cue có fallback không VFX để simulation luôn chạy |
| UI/icon | Tự thiết kế hoặc asset có metadata | Không copy layout, icon hay text của game tham chiếu |

## Asset manifest bắt buộc

Trước khi import asset bên ngoài vào Content, thêm một entry vào bảng dưới. Khi số lượng tăng, agent phải tạo manifest machine-readable tương ứng; Markdown này vẫn là chỉ mục dễ đọc.

| ID | Asset/nhóm | Loại | Nguồn URL/package | Tác giả/nhà phát hành | Phiên bản/ngày tải | Vị trí project | Dùng ở đâu | Metadata quyền | SHA-256 file gốc | Chỉnh sửa | Trạng thái |
|---|---|---|---|---|---|---|---|---|---|---|---|
| ASSET-EXAMPLE-001 | Ví dụ, chưa import | Static mesh | https://... | ... | ... | Content/Art/... | Vertical slice | Ghi tại đây | ... | ... | PLANNED |

Trạng thái hợp lệ: PLANNED, DOWNLOADED, VERIFIED, IMPORTED, REPLACED, REMOVED. Không import asset có trạng thái khác VERIFIED, trừ primitive graybox do dự án tự tạo.

## Quy tắc reference

1. Audit quan sát hành vi, UI flow, timing và kết quả; mô tả lại bằng lời của dự án.
2. Mọi screenshot/video tham chiếu ở vault, kèm mã audit như REF-CMB-014; trong Git chỉ ghi mã, hash, ngày và kết luận.
3. Không đưa text cốt truyện, dialog, hình, model, âm thanh, texture, danh sách data nguyên văn hoặc save vào Content, docs hay issue.
4. Một công thức chỉ được đưa vào parity matrix sau khi có thí nghiệm lặp lại được. Nếu chưa rõ, dùng UNKNOWN.
5. Asset cuối phải có visual identity riêng: kiến trúc, palette, VFX, UI và nhân vật không được là bản sao presentation của reference.

## Quy trình audit cục bộ

Script dự kiến tools/inventory-reference-game.ps1 chỉ được nhận đường dẫn game tham chiếu và vault, ghi build/version nhìn thấy được, danh sách file, dung lượng và SHA-256 vào vault. Script không sửa file Steam, không giải nén/copy asset sang repo, không upload dữ liệu.

## Kiểm tra trước khi commit

- Git status --ignored không được cho thấy ReferenceVault hay Steam library nằm trong vùng theo dõi.
- Không có media lớn, SAV, PAK, dump hay file game tham chiếu trong repo.
- Mỗi asset ngoài primitive/asset tự tạo có manifest entry đủ trường.
- Build Packaging chỉ tham chiếu asset thuộc Content của project, không dùng đường dẫn tuyệt đối tới vault.
