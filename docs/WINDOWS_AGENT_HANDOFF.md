# Handoff duy nhất cho agent Windows — Trường Sinh UE5

> Đọc **toàn bộ file này trước khi mở Unreal Editor hoặc sửa bất kỳ file nào**. Sau đó thực hiện lần lượt các tài liệu bắt buộc trong `AGENTS.md`. File này là điểm vào thực tế cho ChatGPT/Codex trên máy Windows; nó không thay thế các tài liệu thiết kế chi tiết.

## 1. Kết quả cần tạo

Tạo game tu tiên Windows bằng **Unreal Engine 5.8**, C++ là nguồn sự thật và Blueprint là lớp trình bày/adapter:

- Khám phá ở góc nhìn thứ ba trong các open-zone 3D bán hiện thực.
- Combat theo lượt, linh khí/ngũ hành/công pháp; kết quả tính độc lập rồi mới phát animation, camera và Niagara.
- Vòng đời tu sĩ: thời gian, tuổi thọ, cảnh giới, tu luyện, đột phá, tông môn, động phủ, kinh tế, nhiệm vụ và phi thăng.
- Mục tiêu chất lượng hình ảnh: Lumen, Nanite, Virtual Shadow Maps và Niagara; i5-12400F + RTX 3060 ở 1080p High hướng tới 60 FPS.
- Toàn bộ chữ người chơi nhìn thấy dùng tiếng Việt đơn giản và đặt trong String Table/data localization.

`觅长生` chỉ là benchmark hành vi nội bộ. Không đưa asset, text, hình, video, save, dữ liệu trích xuất hay nội dung proprietary của game tham chiếu vào repository, `Content/` hoặc build. Không tuyên bố parity cho tới khi `docs/05_PARITY_MATRIX.md` có audit và test oracle tương ứng.

## 2. Trạng thái thật khi nhận repo

Đây là **M0/M1 foundation**, không phải game chơi được và không phải vertical slice hoàn chỉnh.

Đã có:

- Project UE5.8 C++ chia module Core, Data, Simulation, Combat, Quest, World, Presentation, UI, Save và Tests.
- Cấu hình Windows DX12/SM6, Lumen, Virtual Shadow Maps; một third-person C++ pawn/Camera shell tạm.
- Facade simulation độc lập với level/UI: command có revision, chống submit lặp, event có thứ tự, seed/RNG deterministic và hash trạng thái BLAKE3.
- Action nội bộ `core.advance_time` để chứng minh một gameplay action chỉ đổi state qua simulation. Đây **không phải** công thức lịch/thời gian đã parity.
- Save JSON v1 có schema, hash integrity và round-trip codec; chưa có file atomic, backup hay migration runtime.
- Sáu automation test nguồn: Stable ID, golden RNG, RNG isolation, command idempotency, deterministic replay và save round-trip.
- Scripts để kiểm môi trường, build và chạy automation; docs đầy đủ về design, art, parity, asset provenance và workflow.

Chưa có:

- Bằng chứng UHT/UBT build, Automation report, PIE, Standalone hoặc package Windows.
- Map game, input Enhanced Input, UI, Data Asset gameplay, combat thật, quest/NPC/world simulation, asset hay VFX.
- Reference audit/version lock, donor clone, save file I/O an toàn, benchmark RTX 3060 hoặc MCP cấu hình local.

Không biến mục “đã có source” thành “đã chạy”. Chỉ được cập nhật PASS trong `docs/27_IMPLEMENTATION_STATUS.md` sau khi có log/report/evidence đúng mốc.

## 3. Điều không được phá

- Không tự bịa công thức, cost thời gian, item, kỹ năng, quest, NPC hoặc phần thưởng trước audit. Ghi `UNKNOWN` và tạo experiment trong matrix.
- Không để animation, Widget, Actor, level Blueprint hoặc Tick quyết định damage, RNG, chi phí hay tiến trình.
- Không dùng random không seed; không dùng `FMath::Rand`, `FRand` hoặc thời gian thực/frame rate trong simulation.
- Đi bộ trong level 3D không được tự làm trôi lịch game. Chỉ canonical simulation action mới được tiến thời gian.
- Không clone/import donor ở bước boot đầu. Không commit `_external/`, `ReferenceVault/`, `Binaries/`, `DerivedDataCache/`, `Intermediate/`, `Saved/`, build package, asset chưa rõ quyền hay secret.
- Không dùng multiplayer, EOS, Steam Online, cloud save, analytics hoặc runtime API/service nếu chưa có quyết định mới trong `docs/26_DECISION_LOG.md`.
- Không dùng Blueprint làm data source chính. Data có stable ID, validation và test; C++ subsystem/simulation là authority.

## 4. Chuẩn bị máy Windows

Yêu cầu: Windows 10/11 x64, SSD còn ít nhất 250 GB, Git + Git LFS, PowerShell 7, Visual Studio cùng Windows SDK theo exact UE5.8/UBT hỗ trợ, Epic Games Launcher và Unreal Engine **5.8**.

Clone theo cách an toàn:

```powershell
New-Item -ItemType Directory -Force D:\GameDev | Out-Null
git clone git@github.com:nhat141210/truong-sinh-ue5.git D:\GameDev\truong-sinh-ue5
Set-Location D:\GameDev\truong-sinh-ue5
git lfs install
git status
git log -1 --oneline
```

Tạo Reference Vault **ngoài repo** nếu có quyền truy cập bản tham chiếu:

```powershell
New-Item -ItemType Directory -Force D:\GameDev\ReferenceVault\ImmortalWayOfLife | Out-Null
```

Tại thời điểm bootstrap, không tải hàng loạt Fab/Quixel/Marketplace asset và không clone donor. Native project phải build trước.

## 5. Thứ tự làm việc bắt buộc trong ngày đầu

### Bước A — đọc và xác minh

1. Đọc theo thứ tự trong `AGENTS.md`; đặc biệt đọc `docs/28_UE58_FOUNDATION_CHECKLIST.md` và `docs/27_IMPLEMENTATION_STATUS.md` cuối cùng.
2. Xác nhận `git status` sạch và đã có baseline commit. Nếu không sạch, dừng và hiểu thay đổi trước.
3. Chạy:

```powershell
.\tools\verify-windows-environment.ps1
.\tools\build-windows.ps1
.\tools\run-tests.ps1
```

4. Ghi exact UE `Build.version`, Visual Studio/SDK, kết quả UHT/UBT và đường dẫn report vào `docs/27_IMPLEMENTATION_STATUS.md`.
5. Nếu build/test lỗi: ưu tiên sửa lỗi C++/UHT nhỏ nhất, chạy lại; không tạo asset hoặc Blueprint để che lỗi native.

### Bước B — tạo playable smoke shell

Chỉ sau khi Bước A pass:

1. Mở đúng `TruongSinhUE5.uproject` bằng UE 5.8; không auto-upgrade engine.
2. Trong Editor tạo và lưu `/Game/Maps/Dev/L_Dev_Smoke` gồm floor collision, PlayerStart, Directional Light/SkyLight tạm và một điểm tương tác.
3. Chạy PIE rồi Standalone. Chỉ khi map load được mới đổi startup map trong `Config/DefaultEngine.ini`.
4. Tạo Enhanced Input assets: `IA_Move`, `IA_Look`, `IA_Jump`, `IMC_Default`; thay legacy input bootstrap sau khi binding mới hoạt động.
5. Commit riêng: `feat: add validated UE5 smoke map and enhanced input` — chỉ sau build, PIE và diff asset đều rõ.

### Bước C — M2A golden loop, không làm art trước

Mục tiêu chơi được tối thiểu:

```text
Người chơi đi trong graybox
→ tương tác actor 3D
→ adapter gửi canonical command
→ simulation trả event/result/state hash
→ UI tiếng Việt hiển thị kết quả
→ save/continue tái tạo cùng state
```

Hoàn thiện file save atomic + backup trước khi gọi nó là Continue. Viết automation test trước cho mọi state mới. Không thêm combat/art/zone mở rộng trước khi loop trên chạy được trong PIE và Standalone.

## 6. Ranh giới kỹ thuật cần giữ

```text
Input / Actor 3D / Widget
        ↓  chỉ tạo command hợp lệ
UTruongSinhGameSimulationFacade
        ↓
FTruongSinhGameSimulation (pure deterministic state transition)
        ↓
Event + state hash + save snapshot
        ↓
Presentation (UI / animation / Niagara / camera)
```

- `FTruongSinhGameSimulation` không đọc `UWorld`, actor, widget, Tick hay DeltaSeconds.
- Mỗi command có `CommandId`, `ExpectedWorldRevision`, stable ActionId và payload typed.
- Retry command sau load không được tính phí hai lần.
- Combat sau này phải nhận snapshot + seed, trả result/replay log; presentation lỗi không được đổi result.
- Lumen/Nanite/VFX là presentation, không được nằm trong gameplay authority.

Các file cần hiểu trước khi thay M1:

| Khu vực | File |
|---|---|
| Kiểu command/event/RNG | `Source/TruongSinhCore/Public/Core/TruongSinhTypes.h` |
| RNG deterministic | `Source/TruongSinhCore/Public/Core/TruongSinhDeterministicRng.h` |
| Facade + time proof | `Source/TruongSinhSimulation/Public/Simulation/TruongSinhGameSimulation.h` |
| Save JSON v1 | `Source/TruongSinhSave/Public/Save/TruongSinhSaveGameV1.h` |
| Test hiện có | `Source/TruongSinhTests/Private/TruongSinhBootstrapSpec.cpp` |
| Trạng thái/gate | `docs/27_IMPLEMENTATION_STATUS.md` |

## 7. MCP và Python Editor

Chỉ thiết lập sau native build pass:

1. Bật Editor-only `ModelContextProtocol`, `AllToolsets` và `PythonScriptPlugin` nếu thật sự dùng Python Editor.
2. Chỉ bind `127.0.0.1`; không mở firewall/LAN/Internet.
3. Trong console Editor, chạy `ModelContextProtocol.GenerateClientConfig Codex`; config local ở `.codex/` bị Git ignore.
4. MCP bắt đầu read-only. Trước write tạo git checkpoint; sau write compile, save asset, Data Validation, PIE/test rồi xem diff.

MCP/Python chỉ dựng hoặc kiểm tra content editor. Không để chúng là nguồn gameplay state hay blocker cho C++ workflow.

## 8. Khi nào mới làm hình ảnh “đẹp ngay từ đầu”

Sau M2A golden loop pass, dựng **một corridor/zone nhỏ final-quality**, không dựng cả thế giới:

- Một góc thành thị/tông môn, một đoạn hoang dã, interior luyện đan và combat arena mẫu.
- Chỉ import asset hợp lệ có record trước trong `docs/20_ASSET_AND_REFERENCE_INDEX.md`.
- Dùng Lumen, Nanite, VSM, Niagara, material theo `docs/15_ART_DIRECTION.md` đến `docs/19_PERFORMANCE_BUDGET.md`.
- Đo `stat unit`, `stat gpu`, Unreal Insights và memory trên RTX 3060, 1080p High/Medium trước khi mở rộng asset/zone.

Không dùng asset/reference game tham chiếu. Nếu asset miễn phí chưa có provenance rõ, giữ graybox/procedural primitive.

## 9. Mỗi task phải kết thúc thế nào

1. Build/test/PIE đúng mức rủi ro.
2. Ghi kết quả thực, command đã chạy và blocker vào `docs/27_IMPLEMENTATION_STATUS.md`.
3. Cập nhật parity matrix nếu task chạm hành vi benchmark.
4. Kiểm tra `git diff --check`, `git status` và không có secret.
5. Commit nhỏ, một mục đích, không nói quá mức hoàn thành.

Thứ tự ưu tiên luôn là: **native build → smoke map/input → golden loop/save an toàn → visual target corridor → combat parity → mở rộng content**.
