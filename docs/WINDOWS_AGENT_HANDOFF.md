# Handoff cho agent Windows — Trường Sinh UE5

> Đọc toàn bộ file này trước khi mở Editor hoặc sửa source/asset. Sau đó đọc đúng thứ tự trong `AGENTS.md`.

## Sản phẩm đã khóa

Game sandbox-life simulation tu tiên UE5.8, góc nhìn thứ ba, bốn open-zone. Người chơi trực tiếp khám phá và chuẩn bị; tu luyện, đột phá, luyện đan, bố trận và đấu pháp được simulation tự giải quyết rồi UE5 trình diễn. Không action combat, minigame phản xạ, multiplayer, runtime LLM, offline progress, main quest hoặc ending.

Một save theo một nguyên thần qua nhiều thân xác. Realm/technique/pill/resource tăng tuổi thọ nên tu sĩ có thể sống rất lâu; possession là lựa chọn dự phòng. Phi thăng chưa thuộc bản đầu.

## Trạng thái thật

Source đã có deterministic foundation, auto-resolver proof, lifespan/soul/vessel/possession rules, save JSON v2 và automation tests. Chưa có bất kỳ bằng chứng UE build, map, Blueprint, VFX, PIE, FPS hoặc package nào.

## Ngày đầu

```powershell
git clone git@github.com:nhat141210/truong-sinh-ue5.git D:\GameDev\truong-sinh-ue5
Set-Location D:\GameDev\truong-sinh-ue5
git lfs install
git status
git log -1 --oneline
.\tools\verify-windows-environment.ps1
.\tools\build-windows.ps1
.\tools\run-tests.ps1
```

Nếu build/test lỗi, sửa native source/UHT nhỏ nhất và chạy lại. Không tạo asset/Blueprint để che lỗi. Ghi exact versions/log/report vào status.

## Sau native pass

1. Tạo `L_Dev_Smoke`, floor, PlayerStart, light, interaction.
2. Tạo Enhanced Input move/look/jump/interact.
3. Kết nối một cultivation ActivityPlan → preview → canonical commit → summary.
4. Hoàn thiện atomic save/backup; Continue không reroll.
5. Chỉ sau golden loop mới làm visual corridor, một NPC và một Niagara cue.

## Ranh giới

- C++ simulation/resolution/save authority; Blueprint/Python/MCP presentation/editor only.
- `TruongSinhResolution` thay old Combat; `TruongSinhNarrative` thay old Quest.
- Activity mới dùng framework chung, không subsystem/widget/minigame riêng.
- Một zone loaded; NPC offscreen là state, không Actor/NavMesh.
- MCP loopback/read-only trước; checkpoint trước write.
- Không donor/mass asset/reference vault trong boot đầu.

## Machine/visual

i5-12400F, RTX3060, RAM32GB, SSD250GB+. High: 1080p avg60/1%45, Software Lumen, SSR, VSM, TSR, no hardware RT; ≤10 full NPC; VRAM/RAM package <8GB.

## Thứ tự ưu tiên

`native build → smoke/input → cultivation golden loop/save → visual corridor → shared activities → living world → lifespan/possession integration → four-zone demo`.
