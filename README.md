# Trường Sinh Tiên giới

Sandbox-life simulation tu tiên Windows dùng Unreal Engine 5.8. Người chơi trực tiếp khám phá một thế giới phương Đông 3D, chuẩn bị công pháp/tài nguyên và sống qua hàng trăm năm; tu luyện, đấu pháp, luyện đan và bố trận được simulation tự giải quyết rồi UE5 trình diễn bằng animation, Niagara, camera và âm thanh.

Không có action combat, multiplayer, runtime LLM hoặc ending bắt buộc. Một save theo một nguyên thần qua nhiều thân xác; tu sĩ đủ mạnh có thể liên tục tăng tuổi thọ, còn đoạt xá là lựa chọn dự phòng.

## Trạng thái

Repo đang ở product reset M0/M1. Có C++ foundation deterministic và Windows scripts; chưa có bằng chứng UHT/UBT, map, PIE, VFX hoặc benchmark vì VPS không cài Unreal Engine.

## Bắt đầu trên Windows

```powershell
git clone git@github.com:nhat141210/truong-sinh-ue5.git D:\GameDev\truong-sinh-ue5
Set-Location D:\GameDev\truong-sinh-ue5
git lfs install
.\tools\verify-windows-environment.ps1
.\tools\build-windows.ps1
.\tools\run-tests.ps1
```

Trước khi sửa, đọc toàn bộ `docs/WINDOWS_AGENT_HANDOFF.md`, rồi đọc đúng thứ tự trong `AGENTS.md`.

## Trụ cột

- **Sống trong thế giới:** ít zone nhưng cảnh quan, thời tiết, NPC và động phủ có phản hồi rõ.
- **Chuẩn bị thay cho thao tác:** mọi activity dùng `Plan → Resolve → Commit → Present`.
- **Thời gian có ý nghĩa:** cảnh giới, công pháp, đan dược và tài nguyên thay đổi tuổi thọ.
- **Một nguyên thần, nhiều đời:** đoạt xá giữ tri thức nhưng đổi thân xác và thân phận.
- **Đẹp trong budget:** 1080p High hướng tới 60 FPS trên i5-12400F + RTX 3060; không Hardware Ray Tracing.

## Cấu trúc

- `docs/`: design, capability, kiến trúc, art, performance và status.
- `Source/`: canonical C++ simulation/resolution/save; presentation là consumer.
- `Content/`: asset gốc của dự án, bắt buộc có provenance.
- `_external/`: donor local, bị ignore và không thuộc đường boot đầu.
- `ReferenceVault/`: nghiên cứu local, không commit.
