# AI agent playbook

## Trước task

Đọc AGENTS, contract, capability matrix, architecture, roadmap và status. Nêu task ID, canonical state/API, files, test và điều không làm. Nếu cần Editor, native Windows build phải pass trước.

## Khi code

- C++ simulation/resolution/save là authority.
- Blueprint/Python/MCP chỉ adapter/presentation/authoring.
- Activity mới dùng framework chung; không minigame/subsystem riêng.
- RNG seeded, fixed-point/int, stable ordering.
- UI text qua localization data.
- Không import asset hoặc clone donor nếu task không yêu cầu/provenance chưa có.

## Khi dùng Editor/MCP

Bind loopback, bắt đầu read-only, checkpoint Git trước write. Sau write compile/save/Data Validation/PIE và kiểm diff. Không nhiều MCP writer cùng lúc.

## Kết thúc task

Build/test đúng mức; cập nhật capability/status/decision; ghi evidence path; secret scan; commit nhỏ. Không nói map/VFX/FPS hoàn thành khi chỉ có source hoặc docs.

## Dừng đúng lúc

Dừng và báo blocker khi thiếu exact UE build, asset license, Windows evidence, data/schema decision hoặc task yêu cầu mở rộng action combat/multiplayer/seamless world trái contract.
