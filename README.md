# Trường Sinh Tiên giới

Foundation cho game tu tiên Windows dùng Unreal Engine 5.8: trình bày open-zone 3D góc nhìn thứ ba, simulation/data-driven và chiến đấu theo lượt được biểu diễn trong đấu trường 3D.



## Trạng thái

Hiện tại là M0/M1 bootstrap: tài liệu handoff, scripts Windows, manifest donor và C++ skeleton có thể mở rộng. Chưa có project UE được build trên VPS; Unreal Engine phải được cài và kiểm tra trên Windows.

## Bắt đầu trên Windows

```powershell
git clone <REMOTE-CUA-BAN> D:\GameDev\truong-sinh-ue5
cd D:\GameDev\truong-sinh-ue5
.\tools\verify-windows-environment.ps1
.\tools\build-windows.ps1
```

Trước hết, đưa agent Windows đọc toàn bộ
[WINDOWS_AGENT_HANDOFF.md](docs/WINDOWS_AGENT_HANDOFF.md), sau đó đọc theo thứ tự
trong `AGENTS.md`. Chỉ clone donor cần thiết sau native build M1; donor không thuộc
đường boot đầu.

Repo bootstrap phải có commit/remote thật trước khi chuyển máy. Nếu `git log -1`
không có kết quả, dừng: chưa có baseline để Windows clone hoặc MCP checkpoint.

## Cấu trúc

- `docs/`: nguồn sự thật cho design, parity, kiến trúc, workflow và trạng thái.
- `Source/`: C++ core/simulation; Blueprint chỉ dùng như adapter hoặc presentation.
- `Content/`: asset và Data Asset gốc của dự án, không chứa material từ reference audit.
- `_external/`: donor clone cục bộ, do script tạo và bị Git ignore.
- `ReferenceVault/`: audit cục bộ của bản Steam tham chiếu, bị Git ignore.

## Nguyên tắc quyết định

- Parity là kết quả hành vi được đo, không phải sao chép asset hoặc text.
- Một codebase duy nhất: simulation được giữ nguyên khi presentation 3D nâng cấp.
- Golden loop canonical phải chạy trước; sau đó khóa visual target corridor trước
  khi mở rộng hàng loạt zone/content.
