# Playbook cho ChatGPT/Codex agent trên Windows

## Vai trò

Agent là implementer có kỷ luật: chuyển một task nhỏ thành thay đổi kiểm chứng được trong project UE5.8. Agent không tự thay luật gameplay/công thức/reference để làm task dễ hơn. Khi parity chưa audit, ghi UNKNOWN và tạo thí nghiệm; không bịa kết quả.

## Bắt đầu mỗi phiên

1. Đọc AGENTS.md theo thứ tự bắt buộc.
2. Đọc `28_UE58_FOUNDATION_CHECKLIST.md`, `27_IMPLEMENTATION_STATUS.md`, task đang
   active và acceptance criteria trong `22_PRODUCTION_ROADMAP.md`.
3. Chạy git status --short --branch; tôn trọng thay đổi của người khác.
4. Mở tối thiểu source/data/test liên quan trước khi đề xuất sửa.
5. Nếu task cần audit reference, dùng ReferenceVault ngoài Git và template thích hợp; không copy content proprietary vào project.

## Vòng lặp implement chuẩn

Phân tích một task
→ khóa interface/data contract
→ thêm hoặc sửa test trước/đồng thời
→ implement simulation C++ deterministic qua command facade
→ nối presentation/UI bằng adapter
→ build + validation + automation
→ PIE/Standalone nếu cần
→ cập nhật matrix/status/decision log
→ commit nhỏ

Mỗi commit nêu rõ thay đổi hành vi, test pass và mốc/task được cập nhật. Không refactor kiến trúc, import asset và thêm content rộng trong cùng một commit.

## Source of truth

| Loại | Nơi authoritative |
|---|---|
| Rule, chronology, combat result, RNG, save | C++ simulation + data canonical |
| Content | Primary Data Asset/DataTable/JSON importer đã version |
| Chuỗi hiển thị | String Table/localization data |
| UI/animation/VFX/actor | Presentation layer; không quyết định gameplay |
| Level | Bố trí, trigger, presentation; không giữ progression canonical |

Blueprint dùng để ráp presentation, Widget, VFX cue, map actor và authoring data. Không giấu combat formula, action-time cost, inventory mutation, quest state hoặc save logic quan trọng trong graph Blueprint.

## Làm việc với Unreal MCP

- Chỉ dùng MCP khi Editor Windows đang chạy và server local-only.
- Mở đầu bằng inspect Output Log/project/asset state; tạo checkpoint Git trước write.
- Một agent/một writer MCP cho mỗi Editor session. Không cho nhiều agent ghi cùng Blueprint/level.
- Sau write, compile asset, save có chủ đích, đọc log, chạy test và kiểm tra diff.
- Nếu MCP không hoàn thành thao tác, đổi sang C++/Blueprint editor thủ công; không cài bridge/server lạ để vượt qua bằng mọi giá.

## Điều không được làm

- Không đưa asset, text, screenshot/video, save, dữ liệu trích xuất hoặc source proprietary của game tham chiếu vào Git/project.
- Không commit key, token, file ENV, ReferenceVault, donor raw tree, build output hay cache.
- Không biến đi bộ trong zone thành action tăng date; không để FPS/animation ảnh hưởng simulation.
- Không thêm multiplayer, EOS/Steam, seamless world hay action combat trước M10.
- Không báo DONE khi chỉ có UI mock, Blueprint chưa compile, asset chưa save hoặc parity matrix thiếu oracle/test.
- Không coi Editor mở, exit code 0 với zero tests hoặc `/Engine/Maps/Entry` là
  bằng chứng playable foundation.
- Không force push, reset hard hay xóa thay đổi không sở hữu.

## Prompt mẫu: task simulation

Đọc AGENTS.md, 05_PARITY_MATRIX.md, 11_TECHNICAL_ARCHITECTURE.md, 12_DATA_SCHEMAS.md, 23_TEST_PLAN.md và 27_IMPLEMENTATION_STATUS.md.
Triển khai duy nhất TASK-ID. Giữ simulation deterministic và tách presentation.
Trước khi sửa, nêu interface, data migration và test. Sau khi sửa, build UE5.8, chạy automation liên quan, cập nhật parity/status và báo chính xác file thay đổi.
Không sửa donor/reference vault, không thêm feature ngoài TASK-ID.

## Prompt mẫu: task UE/MCP presentation

Đọc AGENTS.md, 14_WORLD_AND_LEVEL_DESIGN.md, 15_ART_DIRECTION.md, 18_3D_COMBAT_PRESENTATION.md, 19_PERFORMANCE_BUDGET.md và status.
Thực hiện TASK-ID trong UE5.8. Dùng MCP local read-only trước; tạo checkpoint trước write.
Không đặt gameplay rule trong level/widget. Sau thao tác, compile, PIE/Standalone smoke, capture perf evidence và cập nhật status.

## Prompt mẫu: task audit

Chỉ audit FEATURE-ID bằng bản tham chiếu cục bộ. Dùng template dưới docs/templates, lưu media ở ReferenceVault ngoài Git. Ghi hành vi quan sát, input, result, confidence và test oracle độc lập. Không copy text, asset, save hay code sang repo; không implement feature trong task audit.

## Khi cần dừng và hỏi

Dừng, ghi blocker và yêu cầu quyết định khi:

- Reference có hành vi khác nhau theo version/mod/setting chưa khóa.
- Donor cần engine/plugin không build được UE5.8.
- Thay đổi phá save schema hoặc cần migration chưa có fixture.
- Task cần asset/voice/reference không có metadata dự án.
- Performance gate không đạt sau capture rõ ràng.

Không dừng chỉ vì task lớn; chia thành task nhỏ có API và test rõ.

## Scripts agent phải duy trì dần

- verify-windows-environment.ps1: kiểm tra toolchain/path/disk, không cài ngầm.
- build-windows.ps1: chạy UHT/UBT Editor/Game, fail ngay khi source/module lỗi.
- bootstrap-donors.ps1: chỉ sau native M1 pass, clone/fetch donor commit khóa vào
  `_external`, kiểm tra HEAD.
- inventory-reference-game.ps1: inventory/hash cục bộ sang vault, không copy content sang repo.
- run-tests.ps1: đã có bootstrap; chạy automation filter vào `Artifacts/Automation`, sau M1 bổ sung Data Validation và full suite.
- package-windows.ps1: đã có bootstrap; package repeatable ra ngoài source tree, chỉ dùng `-Clean` cho archive path đã kiểm tra.
