# Performance budget — i5-12400F / RTX 3060 / 32 GB

## Target

| Preset | Output | Mục tiêu |
|---|---:|---:|
| High | 1920×1080 | 60 FPS avg, 1% low ≥45 |
| Medium | 1920×1080 | 60 FPS ổn định, giảm presentation |
| Low | 1920×1080 | gameplay đầy đủ, Lumen fallback |

Frame budget 16,67 ms; cảnh chuẩn nên giữ GPU ≤14,5 ms để còn headroom activity burst.

## High

- Software Lumen GI High, SSR mặc định, VSM, TSR Quality/Balanced.
- Không Hardware RT.
- ≤10 full skeletal NPC trong frame; khuyến nghị 6–8.
- ≤4 local shadow lights visible.
- Một volumetric fog layer.
- Niagara có per-cue particle/light/translucency cap.
- VRAM <8 GB; package RAM <8 GB; demo package <25 GB.

## Medium/Low

- Medium: Lumen Medium/SSR, giảm shadow distance/fog/foliage/NPC animation rate/VFX.
- Low: tắt Lumen, fallback GI/SSR, giảm crowd và VFX; không xóa NPC/activity/thông tin gameplay.

## Zone/content

- Một zone loaded mỗi lần.
- Town 250m, sect 300m, wild ≤600m, home 100m.
- PCG là tùy chọn ở M2B, không phải dependency nền; nếu dùng thì bake trong Editor, không runtime generation nặng.
- Workspace/cache/source giữ dưới khoảng 180 GB.

## Profiling gate

`stat unit`, `stat gpu`, `ProfileGPU`, Unreal Insights, `memreport` trên Windows. Tối ưu theo thứ tự: NPC animation → shadow lights → translucency/Niagara → reflections → fog/foliage → material → TSR internal resolution.

Không hy sinh toàn cảnh để giữ một cue quá nặng. Cue vượt budget phải có scale variant.
