# Visual review — VIS-###

## Capture metadata

| Trường | Giá trị |
|---|---|
| Build/commit | |
| Engine | UE 5.8 |
| Map/zone | |
| Camera transform/FOV | |
| Time of day/weather/seed | |
| Preset/resolution | High / Medium / Low, 1920×1080 |
| Hardware/driver | i5-12400F + RTX 3060 hoặc test machine |
| Screenshot/video path | Build artifact hoặc external evidence ID |

## Mục tiêu review

- Art direction principle đang duyệt:
- Khu vực/feature:
- Câu hỏi cần trả lời:

## Checklist

| Hạng mục | Pass/Fail | Ghi chú / issue |
|---|---|---|
| Ánh sáng đọc rõ gameplay, không quá tối | | |
| Lumen GI/reflection ổn định | | |
| Material/texture scale nhất quán | | |
| Nanite/LOD/foliage không pop gây chú ý | | |
| VFX ngũ hành rõ nhưng không che combat UI | | |
| NPC/animation không vượt animation budget | | |
| UI dễ đọc, Vietnamese string không tràn | | |
| World state khớp simulation snapshot | | |
| Không dùng asset/reference proprietary | | |

## Performance evidence

| Metric | High | Medium | Low | Mục tiêu |
|---|---:|---:|---:|---|
| Average FPS | | | | 60 High |
| 1% low FPS | | | | ≥45 High |
| Game thread ms | | | | |
| GPU ms | | | | |
| VRAM/RAM | | | | |

Đính kèm/cite stat unit, stat gpu, Unreal Insights hoặc memory report bằng artifact ID. Không kết luận performance từ Editor viewport duy nhất.

## Quyết định

- PASS: có thể dùng làm baseline/nhân rộng.
- REWORK: liệt kê issue có owner và test lại.
- BLOCKED: cần asset, tech decision hoặc profiling bổ sung.

## Follow-up

-
