# 28 — Checklist nền tảng Unreal Engine 5.8

> Đây là checklist thi hành trên Windows, dựa trên cách tổ chức project/module,
> config, rendering, automation và MCP trong tài liệu Epic UE5.8. Check một mục chỉ
> khi có bằng chứng build/log/asset thật; file tồn tại trong Git chưa phải bằng chứng.

## 1. Nguồn chính thức áp dụng

- [Install Unreal Engine](https://dev.epicgames.com/documentation/unreal-engine/install-unreal-engine)
- [Hardware and Software Specifications](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine)
- [Unreal Engine Modules](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-modules)
- [Game Mode and Game State](https://dev.epicgames.com/documentation/en-us/unreal-engine/game-mode-and-game-state-in-unreal-engine)
- [Enhanced Input](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine)
- [Programming with C++](https://dev.epicgames.com/documentation/en-us/unreal-engine/programming-with-cplusplus-in-unreal-engine)
- [Automation Test Framework](https://dev.epicgames.com/documentation/en-us/unreal-engine/automation-test-framework-in-unreal-engine)
- [Lumen Technical Details](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-technical-details-in-unreal-engine)
- [Nanite Virtualized Geometry](https://dev.epicgames.com/documentation/en-us/unreal-engine/nanite-virtualized-geometry-in-unreal-engine)
- [Unreal MCP](https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor?application_version=5.8)
- [Scripting the Editor using Python](https://dev.epicgames.com/documentation/en-us/unreal-engine/scripting-the-unreal-editor-using-python)
- [Packaging Unreal Engine Projects](https://dev.epicgames.com/documentation/en-us/unreal-engine/packaging-your-project)

Nếu URL/version tài liệu thay đổi, ưu tiên trang có `application_version=5.8` hoặc
API/docs đi kèm engine đã cài. Ghi thay đổi đáng kể vào decision log.

## 2. Exact engine và toolchain

- [ ] `EngineAssociation` là `5.8`, nhưng đã ghi thêm exact patch/changelist từ
      `Engine/Build/Build.version` vào evidence M0 trong
      `27_IMPLEMENTATION_STATUS.md` (không ghi vào manifest game tham chiếu).
- [ ] Visual Studio có MSVC x64, Windows SDK và workload game C++ mà UBT của exact
      engine chấp nhận.
- [ ] Git LFS hoạt động; repo nằm trên SSD; còn ít nhất 250 GB trước asset/sample.
- [ ] Chạy `tools/verify-windows-environment.ps1` bằng PowerShell 7.
- [ ] Chạy `tools/build-windows.ps1`; không dùng việc IntelliSense hiện code làm
      bằng chứng compile.

## 3. Module và build graph

- [ ] Runtime module và test module có descriptor đúng Type/LoadingPhase trong
      `.uproject`.
- [ ] Mỗi module có `Build.cs`, Public/Private và module implementation hợp lệ.
- [ ] Dependency graph không cycle; dependency chỉ dùng trong `.cpp` chuyển sang
      `PrivateDependencyModuleNames` khi có thể.
- [ ] `TruongSinhCore` không phụ thuộc Engine world/UI/asset.
- [ ] `TruongSinhUE5` là composition root; World/UI không ghi state trực tiếp.
- [ ] `TruongSinhTests` không được đưa vào Shipping dependency chain.

## 4. Boot, map và input

- [ ] `GameInstanceClass` và `GlobalDefaultGameMode` nằm trong
      `DefaultEngine.ini` phần `GameMapsSettings`.
- [ ] Tạo bằng Editor và lưu `/Game/Maps/Dev/L_Dev_Smoke` gồm floor collision,
      `PlayerStart`, ánh sáng tối thiểu và một interaction actor.
- [ ] Sau khi asset tồn tại và load được, đổi `EditorStartupMap` và
      `GameDefaultMap` sang `L_Dev_Smoke`; không trỏ config đến asset chưa commit.
- [ ] Tạo Enhanced Input assets: `IA_Move`, `IA_Look`, `IA_Jump`, `IA_Interact`,
      `IMC_Explore`; C++ add/remove mapping context theo local player.
- [ ] Xóa legacy mappings trong `DefaultInput.ini` chỉ sau khi Enhanced Input pass
      PIE và Standalone.
- [ ] PIE và Standalone spawn `ATruongSinhCharacter`, mouse/gamepad focus đúng,
      đi bộ không làm đổi game date.

## 5. Canonical gameplay spine

- [ ] Tất cả ghi state đi qua `UTruongSinhGameSimulationFacade::Execute`.
- [ ] Command có CommandId, stable IDs, typed payload, expected revision, sequence.
- [ ] Rejected command không đổi state/RNG/date; Committed command tạo ordered
      domain events, journal và state hash.
- [ ] Duplicate CommandId trả lại result cũ hoặc reject idempotent, không double cost.
- [ ] Stable sort dựa canonical ID string, không dựa TMap/TSet/FName internal index.
- [ ] SplitMix64 v1 golden vectors pass; stream state/draw count round-trip qua save.
- [ ] Save v1 dùng DTO, checksum, temp/backup/atomic replacement và migration hook.

## 6. C++ ↔ Blueprint boundary

- [ ] Interface Blueprint implement được dùng `UINTERFACE(..., Blueprintable)` và
      `BlueprintNativeEvent`/`BlueprintImplementableEvent` theo tài liệu Epic.
- [ ] Blueprint interaction chỉ tạo offer/command; simulation kiểm eligibility và
      commit cuối.
- [ ] DTO cần Blueprint/serialization là `USTRUCT` + `UPROPERTY` hoặc có reflected
      adapter riêng.
- [ ] Level Blueprint, Widget, AnimNotify và Niagara không chứa công thức, reward,
      time cost, quest mutation hay combat damage authority.
- [ ] Presentation cue thiếu có fallback; skip/fast-forward không re-resolve combat.

## 7. Rendering baseline RTX 3060

- [ ] Windows RHI là DX12, targeted shader format là SM6.
- [ ] Lumen GI/reflections, Mesh Distance Fields, VSM và TSR hoạt động sau restart.
- [ ] Hardware ray tracing không là requirement và tắt ở baseline.
- [ ] Kiểm `Lumen Overview`, `Surface Cache`, `Mesh Distance Fields`, Nanite
      visualization; không chỉ nhìn Lit viewport.
- [ ] High/Medium có scalability khác nhau; không hard-code một preset cho mọi GPU.
- [ ] `stat unit`, `stat gpu`, profile GPU và Unreal Insights được capture sau
      shader warm-up ở M2B.

## 8. MCP và Python Editor

- [ ] `ModelContextProtocol`, `AllToolsets` và `PythonScriptPlugin` tồn tại trong
      exact engine build và chỉ cần ở Editor.
- [ ] MCP bind loopback; config client sinh local bằng
      `ModelContextProtocol.GenerateClientConfig Codex`; `.codex/` không commit.
- [ ] Một MCP writer cho một Editor session; không gửi overlapping tool calls.
- [ ] Checkpoint Git trước write; sau write compile/save asset, đọc Output Log và
      kiểm source-control changes.
- [ ] Python chỉ dùng Editor automation/generation/validation, không là runtime
      gameplay dependency.

## 9. Test, cook và package gate

- [ ] Automation report có ít nhất một test `TruongSinh.*`, không chỉ exit code 0.
- [ ] Unit/simulation test chạy không cần level hoặc GPU.
- [ ] PIE + Standalone test interaction/input/map travel.
- [ ] Development Game build và Development package chạy được.
- [ ] Shipping package chạy trên máy sạch không có UE/Epic Launcher.
- [ ] Package không chứa test module, MCP server, ReferenceVault, donor tree,
      developer secrets hoặc editor-only asset.

## 10. Gate mở M2A

Chỉ mở golden loop khi toàn bộ mục sau có evidence trong
`27_IMPLEMENTATION_STATUS.md`: exact UE build, clean UHT/UBT Editor build,
`L_Dev_Smoke` PIE/Standalone, RNG golden test, command reject/commit/idempotency,
save round-trip hash, Development package và clean Git checkpoint.
