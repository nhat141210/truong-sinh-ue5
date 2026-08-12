# Nhật ký quyết định

Mọi thay đổi tác động kiến trúc, parity, save, pipeline, performance hoặc phạm vi phải được ghi trước hoặc cùng commit. Không sửa lịch sử để làm như quyết định chưa từng thay đổi.

| ID | Ngày | Trạng thái | Quyết định | Lý do | Tác động / điều kiện xem lại |
|---|---|---|---|---|---|
| DEC-001 | 2026-08-12 | LOCKED | Một project UE5.8 C++ duy nhất, Windows x64 | Tránh bản 2D tạm phải viết lại | Chỉ đổi engine qua ADR mới và build gate |
| DEC-002 | 2026-08-12 | LOCKED | V1 dùng third-person 3D, open-zone và visual quality ngay từ đầu | Presentation 3D là mục tiêu phát hành | Core simulation vẫn headless-testable |
| DEC-003 | 2026-08-12 | LOCKED | Target là parity hành vi của bản Steam stable 觅长生; presentation không pixel-perfect | Bảo toàn depth/loop, tạo visual identity riêng | Version/build tham chiếu khóa tại M0 |
| DEC-004 | 2026-08-12 | LOCKED | Combat theo lượt/ngũ hành; arena 3D phát replay deterministic | Không làm action combat phá parity | Chỉ xét action branch sau M10 |
| DEC-005 | 2026-08-12 | LOCKED | Đi bộ trong zone không tăng thời gian game | Time cost chỉ theo action canonical | CORE-002 là regression bắt buộc |
| DEC-006 | 2026-08-12 | LOCKED | Simulation C++ + data canonical tách presentation UI/actor/VFX | Determinism, save, test không bị FPS/animation chi phối | Blueprint gameplay exception cần ADR |
| DEC-007 | 2026-08-12 | LOCKED | Open-zone, không seamless world trong V1 | Phù hợp RTX 3060 và production scope | Xem lại sau M10 |
| DEC-008 | 2026-08-12 | LOCKED | Data dùng stable ID, Primary Asset/DataTable, Gameplay Tags, String Tables | Content lớn và localization cần validator | Không literal UI/game rule trong C++/Blueprint |
| DEC-009 | 2026-08-12 | LOCKED | ReferenceVault ngoài Git; không copy proprietary assets/text/data | Audit riêng, project sạch | Metadata/oracle độc lập được commit |
| DEC-010 | 2026-08-12 | LOCKED | Donor clone trong _external, qua build/test gate trước integration | Không fork/reskin donor thành game | Commit donor khóa trong 21_DONOR_REUSE_MAP.md |
| DEC-011 | 2026-08-12 | LOCKED | Unreal MCP native local-only là bridge đầu; Monolith là fallback | Editor automation không thành dependency bắt buộc | Không mở port public/LAN |
| DEC-012 | 2026-08-12 | LOCKED | VPS không tham gia dự án UE; Windows code/build/art/QA/package | VPS chỉ giữ Ca Đêm web, không có GPU QA phù hợp | Không clone UE donor vào VPS |
| DEC-013 | 2026-08-12 | LOCKED | Không multiplayer, EOS/Steam, runtime online service hay action combat trước M10 | Bảo vệ parity và focus | Scope change chính thức mới mở |
| DEC-014 | 2026-08-12 | LOCKED | M1 dùng typed command/result, combat command/replay và revision guard trước presentation | Ngăn UI/animation/level actor ghi state trực tiếp | Chỉ simulation được resolve/advance revision; audit xác minh payload sau |
| DEC-015 | 2026-08-12 | LOCKED | UE5.8 native MCP chỉ là Editor local tool | Tránh MCP/server xuất hiện trong game package | `ModelContextProtocol` chỉ target Editor; config local bị ignore |
| DEC-016 | 2026-08-12 | LOCKED | M2 tách thành M2A golden loop canonical rồi M2B visual target | Tránh dựng quest/combat/art final trên simulation giả và phải viết lại | Chỉ mass-import asset sau cả gameplay và visual target gate |
| DEC-017 | 2026-08-12 | LOCKED | RNG v1 dùng SplitMix64, stream derive bằng FNV-1a UTF-8 + master seed, bounded draw dùng rejection sampling | Cần thuật toán nhỏ, portable, có save state và golden vector rõ | Đổi thuật toán/derive rule cần save migration + golden tests mới |
| DEC-018 | 2026-08-12 | LOCKED | Windows renderer baseline pin DX12/SM6, Lumen + Mesh Distance Fields + VSM + TSR; hardware RT tắt | Đúng yêu cầu UE5.8 cho Lumen/Nanite, phù hợp RTX 3060 và không phụ thuộc hardware RT | Chỉ mở hardware RT sau performance/visual ADR |
| DEC-019 | 2026-08-12 | LOCKED | Runtime modules khai báo explicit; TruongSinhUE5 là composition root; typed command dùng reflected `FInstancedStruct` payload | Module loading, Blueprint adapter và command schema phải thống nhất trước nhiều-agent implementation | Build.cs/UHT/UBT Windows là gate cuối; không dùng JSON/string map canonical |
| DEC-020 | 2026-08-12 | LOCKED | Canonical state/save payload hash dùng BLAKE3-256 qua UE Core; version prefix `blake3-v1` | Mạnh hơn hash 64-bit, API UE5.8 chính thức và cùng output cho deterministic fixture | Đổi canonical serialization/hash cần save migration và regression fixture |

## Mẫu entry mới

### DEC-### — Tên ngắn

- Ngày:
- Trạng thái: PROPOSED | ACCEPTED | LOCKED | SUPERSEDED
- Bối cảnh:
- Quyết định:
- Lựa chọn đã loại:
- Tác động tới module/data/save/test:
- Migration hoặc rollback:
- Bằng chứng/gate:
- Quyết định thay thế nếu có:
