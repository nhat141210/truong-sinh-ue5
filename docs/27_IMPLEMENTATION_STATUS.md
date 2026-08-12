# Trạng thái triển khai

## Cập nhật gần nhất

- Ngày: 2026-08-12
- Checkpoint: M0 repository bootstrap đã audit/chỉnh theo UE5.8; chưa có Git commit baseline và M0 Windows evidence. M1 skeleton đang chờ xác minh Windows.
- Trạng thái tổng thể: Có documentation bootstrap, explicit module descriptors,
  DX12/SM6/Lumen baseline, Editor-only MCP/AllToolsets/Python, third-person code
  shell, typed command/event, reflected combat DTO, Blueprintable interaction,
  pure simulation facade, internal time-action proof, deterministic hash/RNG v1,
  save JSON codec v1 và source-level automation tests. Chưa có bằng chứng UHT/UBT
  build, project map, PIE, atomic file save/recovery, reference audit hoặc package.
- Nguồn sự thật: file này và Git commit hiện tại; không suy ra hệ thống đã tồn tại chỉ vì nó có trong roadmap.

## Bảng mốc

| Mốc | Trạng thái | Bằng chứng hiện có | Việc mở khóa tiếp theo |
|---|---|---|---|
| M0 Docs/toolchain | PASS (static) / PENDING (Git + Windows) | Docs/handoff, donor lock, scripts, UE5.8 foundation checklist và static validation | Tạo baseline commit; agent Windows khóa exact UE build và chạy verify |
| M1 Architectural proof | IN PROGRESS | Config/module/plugin baseline; typed command/event; RNG v1; pure facade; internal time proof; deterministic state hash; JSON save v1 round-trip source; reflected combat DTO; Blueprintable interaction; third-person shell | UHT/UBT; automation report; atomic file save; smoke map; Enhanced Input; package |
| M2A Golden loop | NOT STARTED | Chưa có playable canonical loop | Graybox interaction → time action → combat → save/continue |
| M2B Visual target | NOT STARTED | Chưa có map/asset/perf capture | Dựng một corridor final-quality sau M2A và kiểm tra RTX 3060 |
| M3 Core parity | NOT STARTED | Chưa có simulation/test oracle | Chronology, RNG, save, data registry |
| M4 Combat parity | NOT STARTED | Chưa có combat snapshot/replay | Combat simulation + golden scenarios |
| M5 Production systems | NOT STARTED | Chưa có crafting/home/tribulation | Implement theo roadmap |
| M6 World/social/economy | NOT STARTED | Chưa có NPC/quest/economy runtime | Implement theo roadmap |
| M7 Content parity | NOT STARTED | Chưa có catalog nhập/validated | Content pipeline + validators |
| M8 Full run | NOT STARTED | Chưa có endgame route | Full progression smoke |
| M9 QA | NOT STARTED | Chưa có matrix report/perf baseline | Full test run |
| M10 Windows release | NOT STARTED | Chưa có package | Shipping clean-machine QA |

## Build và test gần nhất

| Hạng mục | Kết quả | Commit/build | Máy | Ghi chú |
|---|---|---|---|---|
| UE5.8 Editor build | NOT RUN | — | Windows | Skeleton đang chờ build đầu tiên |
| C++ build | NOT RUN | — | Windows | Module skeleton đang chờ build đầu tiên |
| Automation | NOT RUN | — | Windows | Chờ test launcher |
| PIE/Standalone | NOT RUN | — | Windows | Chờ map smoke |
| Package Shipping | NOT RUN | — | Windows sạch | Chưa tới M10 |
| Performance | NOT RUN | — | i5-12400F / RTX 3060 | Chờ M2B |

## Task tiếp theo theo thứ tự

1. Tạo commit baseline và remote trước mọi MCP/Editor write; hiện repo chưa có commit.
2. Trên Windows, đọc AGENTS.md, chạy environment check và ghi exact `Build.version`.
3. Build UHT/UBT; chạy 6 bootstrap automation tests và sửa compile/UHT có bằng chứng trước khi tạo asset.
4. Tạo/lưu `Content/Maps/L_Dev_Smoke` bằng Editor, thêm PlayerStart/floor/light;
   sau đó mới đổi startup map khỏi `/Engine/Maps/Entry`.
5. Tạo Enhanced Input assets, facade canonical và save v1 round-trip; chạy test.
6. Tạo ReferenceVault ngoài Git và khóa bản tham chiếu.
7. Chỉ sau native M1 pass mới clone donor cần thiết vào `_external`.

## Blocker hiện tại

| ID | Blocker | Ảnh hưởng | Cần làm |
|---|---|---|---|
| BLK-001 | Chưa có Windows UE5.8 toolchain evidence | Không thể tạo/QA/build project | Thiết lập workstation theo 24_WINDOWS_WORKFLOW.md |
| BLK-002 | Bản Steam/version tham chiếu chưa khóa/audit | Không thể xác nhận parity formula/content | Chạy M0 reference audit ngoài Git |
| BLK-003 | Chưa có UE5.8 build evidence và map smoke cho skeleton | Không thể chạy automation hay donor compile gate đáng tin cậy | Hoàn thành build/PIE M1 trên Windows |
| BLK-004 | Repository chưa có commit baseline/remote | Không có checkpoint an toàn cho Editor/MCP write | Tạo commit đầu tiên và xác nhận remote trước Windows handoff |
| BLK-005 | Save v1 mới là JSON codec, chưa có file I/O atomic/backup/migration | Continue chưa an toàn trước crash/corruption | Implement và test trên Windows filesystem trước M2A |
| BLK-006 | Facade/time action/RNG/save tests chưa chạy qua UHT/UBT | Source có thể còn lỗi reflection/API exact UE5.8 | Chạy build-windows + run-tests, không tự nâng trạng thái PASS |

## Quy tắc cập nhật

Mỗi agent kết thúc task cập nhật: ngày, mốc, commit, file/module, lệnh build/test, kết quả, evidence path (nếu ngoài Git chỉ ghi mã/hash), task tiếp theo và blocker. Đổi trạng thái mốc sang PASS chỉ khi gate trong 22_PRODUCTION_ROADMAP.md có evidence.
