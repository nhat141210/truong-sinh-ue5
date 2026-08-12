# Bắt đầu ở đây — Trường Sinh UE5

> Tài liệu vận hành đầu tiên cho mọi agent trên Windows. Đọc xong file này rồi đọc theo đúng thứ tự ở dưới trước khi tạo, sửa hoặc import bất kỳ thứ gì.

## Mục tiêu hiện tại

`Trường Sinh UE5` là một game tu tiên Windows bằng Unreal Engine 5.8. Bản đầu dùng trình bày 3D góc nhìn thứ ba, nhưng lõi luật chơi phải tách khỏi level, animation và UI. Chuẩn tham chiếu chức năng là **觅长生 / Immortal Way of Life**: người chơi phải có hành trình đầy đủ từ khởi đầu đến phi thăng, với thời gian, tuổi thọ, tu luyện, tông môn/tán tu, đấu pháp, chế tạo, kinh tế và thế giới NPC vận hành.

Đây là chuẩn **hành vi và chiều sâu hệ thống**, không phải yêu cầu sao chép tài sản sáng tạo. Không chép truyện, hội thoại, tên riêng, bản đồ, asset, UI, nhạc, save format, bảng dữ liệu hay mã nguồn của game tham chiếu.

## Thứ tự đọc bắt buộc

1. [01_PRODUCT_BRIEF.md](01_PRODUCT_BRIEF.md)
2. [02_PARITY_CONTRACT.md](02_PARITY_CONTRACT.md)
3. [03_REFERENCE_VERSION.md](03_REFERENCE_VERSION.md)
4. [04_REFERENCE_AUDIT_GUIDE.md](04_REFERENCE_AUDIT_GUIDE.md)
5. [05_PARITY_MATRIX.md](05_PARITY_MATRIX.md)
6. [06_GAME_DESIGN_DOCUMENT.md](06_GAME_DESIGN_DOCUMENT.md)
7. [07_SYSTEM_RULEBOOK.md](07_SYSTEM_RULEBOOK.md)
8. [08_COMBAT_SPEC.md](08_COMBAT_SPEC.md)
9. [09_CONTENT_CATALOG.md](09_CONTENT_CATALOG.md)
10. `11_TECHNICAL_ARCHITECTURE.md`, `14_WORLD_AND_LEVEL_DESIGN.md`,
    `15_ART_DIRECTION.md`, `22_PRODUCTION_ROADMAP.md`,
    `24_WINDOWS_WORKFLOW.md`, `25_AI_AGENT_PLAYBOOK.md`,
    `28_UE58_FOUNDATION_CHECKLIST.md` và `27_IMPLEMENTATION_STATUS.md`.

Nếu một file ghi `TBD_REF`, việc cần làm là audit có kiểm soát; không được tự đoán rồi gọi là parity.

## Cài đặt Windows tối thiểu

- Windows 10/11 x64, bản Visual Studio C++ được exact UE5.8/UBT hỗ trợ, workload
  **Game development with C++**, MSVC x64, Windows SDK và Unreal Engine tools.
  Không khóa cứng VS2022: tài liệu UE5.8 hiện hành có thể yêu cầu toolchain mới hơn.
- Unreal Engine **5.8.x** từ Epic Games Launcher; dùng đúng patch đã ghi trong `03_REFERENCE_VERSION.md` khi khóa môi trường.
- Git, Git LFS, PowerShell 7; ít nhất 250 GB trống trước khi tải engine/sample/asset.
- Máy đích ban đầu: i5-12400F + RTX 3060. Duyệt ở 1920×1080; High mục tiêu 60 FPS, 1% low tối thiểu 45 FPS.

Sau khi clone repo, không chạy script sửa dự án trước khi kiểm tra trạng thái:

```powershell
git status --short --branch
git lfs install
Get-ChildItem . -Force
```

Project C++ skeleton `TruongSinhUE5.uproject` đã có trong repo, nhưng chưa có bằng chứng build UE5.8. Chu kỳ xác minh tối thiểu trên Windows là: tạo solution → build `Development Editor Win64` → mở Editor → PIE → chạy automation test liên quan → package Development Win64. Không coi việc Editor mở được là hoàn thành tính năng.

## Luật làm việc không được phá

- C++/data là nguồn sự thật; Blueprint chỉ là adapter presentation, UI hoặc level composition. Không giấu luật gameplay trong widget, animation notify, Level Blueprint hay actor cụ thể.
- Simulation không được đọc `DeltaSeconds`, FPS, thời gian thật hoặc trạng thái camera. Mọi kết quả ngẫu nhiên đi qua RNG có seed và được lưu save.
- Đi bộ trong zone 3D không đổi lịch game. Chỉ `GameplayAction` hợp lệ mới tiêu tốn ngày/tháng/năm.
- Không bật multiplayer, EOS, Steam Online Subsystem hoặc backend trong giai đoạn parity.
- Không import một plugin/donor thẳng vào `main`. Clone vào `_external/`, build ở branch thử nghiệm, ghi quyết định rồi mới đưa phần đã chọn vào dự án.
- Không commit `ReferenceVault/`, file game tham chiếu, recording, screenshot tham chiếu, save tham chiếu, extracted data, API key, DerivedDataCache, Intermediate hay Binaries.
- Mọi chuỗi người chơi nhìn thấy dùng String Table; nội dung gốc viết bằng tiếng Việt đơn giản.
- Một tính năng chỉ được ghi `DONE` khi có: mục parity liên quan, test/oracle, save/load test nếu có state và bằng chứng chạy trong `27_IMPLEMENTATION_STATUS.md`.

## Vòng làm việc cho một agent

1. Đọc status, parity row và rulebook của subsystem cần sửa.
2. Chọn **một** capability có tiêu chí pass rõ; không mở rộng content song song.
3. Viết hoặc cập nhật test trước/đồng thời với logic deterministic.
4. Chạy build, test và PIE smoke test; giữ log lỗi cần thiết.
5. Cập nhật parity row, status và decision log bằng sự thật có thể kiểm tra.
6. Commit nhỏ, mô tả capability + test. Không gộp refactor lớn với thay đổi luật.

## Việc đầu tiên sau khi repo được clone lên Windows

1. Chạy `tools/verify-windows-environment.ps1` ở chế độ read-only.
2. Điền chứng cứ môi trường và bản tham chiếu vào `03_REFERENCE_VERSION.md`; chưa khóa version thì không bắt đầu audit công thức.
3. Mở project C++ UE5.8 skeleton có sẵn; xác nhận các module build được trước khi
   import asset. Các plugin baseline trong `.uproject` phải tồn tại trong exact
   engine build; thiếu plugin thì ghi blocker, không âm thầm xóa để project mở.
4. Làm theo `28_UE58_FOUNDATION_CHECKLIST.md`: tạo `L_Dev_Smoke` bằng Editor,
   Enhanced Input, command facade và save/load snapshot giống hệt.
5. Mở một parity row ở nhóm `CORE-*`, không dựng nhiều level hay tải asset hàng loạt.

## Khi bị thiếu thông tin

- Thiếu luật tham chiếu: ghi `UNKNOWN` cùng câu hỏi cụ thể trong matrix, rồi chạy audit theo `04_REFERENCE_AUDIT_GUIDE.md`.
- Thiếu asset: dùng primitive/material tạm có ID rõ ràng; không để visual blocker thay đổi logic.
- Thiếu plugin: giữ interface nội bộ, không để plugin trở thành điều kiện build của core simulation.
- Thiếu hiệu năng GPU: hạ presentation scalability, không thay đổi kết quả simulation.
