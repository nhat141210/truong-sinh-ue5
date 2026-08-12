# Quy trình làm việc trên Windows

## Phạm vi máy

Toàn bộ UE5.8, asset, design research, profiling, package và QA thực hiện trên máy Windows. VPS chỉ chuẩn bị source/docs/static checks; không cài Unreal, tạo asset/video, benchmark hay package game UE.

Workstation hiện tại cần tối thiểu: i5-12400F, RTX 3060, Windows 10/11 x64 và ít nhất 250 GB trống. Lưu project trên SSD NVMe nếu có thể.

## Chuẩn bị một lần

1. Cài Git, Git LFS, Epic Games Launcher, Unreal Engine 5.8 và bản Visual Studio
   được exact UE5.8/UBT hỗ trợ, workload Game development with C++ (MSVC x64,
   Windows SDK, Unreal tools). Tài liệu UE5.8 hiện hành là nguồn version; UBT build
   là kiểm chứng cuối, không khóa cứng VS2022 theo hướng dẫn cũ.
2. Qua Epic Launcher tải Game Animation Sample và package Fab/Quixel cần cho vertical slice. Không import mass asset ở bước này.
3. Chỉ tạo ReferenceVault ngoài Git khi một research task có ID cụ thể; không cần vault trong đường boot đầu.
4. Clone repo:

    New-Item -ItemType Directory -Force D:\GameDev | Out-Null
    git clone <REMOTE_URL> D:\GameDev\truong-sinh-ue5
    Set-Location D:\GameDev\truong-sinh-ue5
    git lfs install

5. Đọc theo thứ tự trong AGENTS.md, tạo/xác nhận baseline commit, sau đó chạy:

    .\tools\verify-windows-environment.ps1
    .\tools\build-windows.ps1
    .\tools\run-tests.ps1

Mỗi script phải fail rõ khi prerequisite thuộc phạm vi của nó thiếu: exact UE5.8,
Visual Studio/SDK, Git LFS, dung lượng, path, test report hoặc donor commit khóa.
Chúng không được tải asset proprietary vào repo.

Không chạy `bootstrap-donors.ps1` hoặc inventory reference trong đường boot đầu.
Chỉ clone đúng donor sau native M1 pass và khi task có acceptance rõ.

## Mở và build project

Project `TruongSinhUE5.uproject` đã tồn tại nhưng chưa được build bằng UE5.8. Trên Windows:

1. Chạy `tools/build-windows.ps1`; đây là UHT/UBT gate trước Editor.
2. Mở project bằng exact `UnrealEditor.exe`, không chấp nhận auto-upgrade engine.
3. Tạo `/Game/Maps/Dev/L_Dev_Smoke` bằng Editor: floor collision, PlayerStart,
   light và interaction actor; save/commit asset.
4. Chỉ sau khi map load được mới đổi startup map trong `DefaultEngine.ini`.
5. Tạo Enhanced Input assets theo checklist 28; legacy input chỉ là bootstrap.
6. Chạy PIE rồi Standalone. Sau data/Blueprint change, compile/save asset, Data
   Validation và automation liên quan.

## Unreal MCP local workflow

MCP giúp agent đọc/chỉnh Unreal Editor đang chạy, nhưng không thay source control.

1. Mở project bằng UE5.8 trên Windows.
2. Xác nhận **Unreal MCP** (`ModelContextProtocol`) và `AllToolsets` đã bật
   Editor-only; Toolset Registry được dependency tự bật. Xác nhận
   `PythonScriptPlugin` nếu task dùng Python Editor.
3. Bật server theo nhu cầu trong Editor Preferences hoặc console, chỉ bind loopback `127.0.0.1`. Không mở port LAN/Internet và không đưa token/secret vào repo, prompt, log hay history.
4. Trong Editor console, dùng `ModelContextProtocol.GenerateClientConfig Codex` để sinh cấu hình local trong `.codex/`; thư mục này bị Git ignore.
5. Agent bắt đầu read-only: project state, Output Log, scene/asset inspection.
6. Trước MCP write: git status, commit/checkpoint hoặc stash thay đổi đã hiểu.
7. Sau MCP write: compile Blueprint/C++, save asset có chủ đích, run test, kiểm tra diff và ghi status.

Nếu native MCP không đủ, đánh giá Monolith theo 21_DONOR_REUSE_MAP.md; không chạy đồng thời nhiều MCP writer vào cùng Editor. MCP chỉ phục vụ Editor, không khởi động trong package Windows.

## Daily loop

Pull và đọc status
→ chọn một task có acceptance criteria
→ tạo branch/checkpoint
→ implement C++/data/presentation tách biệt
→ build + automation
→ PIE/Standalone nếu có 3D/UI
→ update docs/status
→ commit nhỏ, mô tả hành vi và test

Không commit Binaries, DerivedDataCache, Intermediate, Saved, .vs, _external, ReferenceVault, build package hay API key. Dùng Git LFS cho asset do dự án chọn version, không cho asset vault.

## Build và package Windows

Trước package: build C++, Data Validation, automation test và standalone smoke. Dùng UE Project Launcher hoặc RunUAT BuildCookRun sau khi project có profile/script chuẩn. Output release đặt ngoài source tree, ví dụ D:\GameDev\Builds\TruongSinhUE5\<build-id>.

Release candidate luôn test trên Windows sạch không cài Unreal Engine/Epic
Launcher. Game package không cần kết nối Epic khi chạy; chỉ dịch vụ được chủ động
thêm sau này mới có network dependency. Trước release vẫn package Development định kỳ
để phát hiện cook/module/asset reference lỗi sớm.

## Khi lỗi

- C++ compile lỗi: lưu Output Log/build log, thu hẹp task xuống compile fix; không sửa Blueprint ngẫu nhiên để che lỗi.
- Blueprint compile lỗi: ghi asset, parent class, node lỗi và reproduce steps; kiểm tra C++ API trước.
- Shader hitch/FPS: capture stat unit, stat gpu, Unreal Insights; không tắt Lumen/Nanite toàn cục chỉ để kết luận pass.
- MCP mất kết nối: đóng bridge/Editor theo hướng dẫn plugin, mở lại và tiếp tục qua source nếu cần. MCP không được là blocker duy nhất.
