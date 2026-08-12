# 19 — Ngân sách hiệu năng và scalability

## Hardware target

Target chính của vertical slice và bản Windows V1:

- CPU: Intel Core i5-12400F.
- GPU: NVIDIA RTX 3060 12 GB.
- RAM hệ thống: 16 GB tối thiểu, 32 GB khuyến nghị cho editor.
- Độ phân giải: 1920×1080.
- High: trung bình 60 FPS, 1% low >= 45 FPS trong route đại diện.
- Medium: trung bình 60 FPS, 1% low >= 50 FPS; giữ gameplay/route/NPC quan trọng đầy đủ.
- 30 FPS không là quality target; chỉ là fallback nếu máy người chơi dưới spec.

Mục tiêu áp dụng cho packaged Development/Shipping Windows, sau shader warm-up. PIE/editor không dùng làm kết luận cuối.

## Frame budget

Ở 60 FPS, 16.67 ms/frame. Mục tiêu route vertical slice:

| Hạng mục | High target | Medium target | Ghi chú |
|---|---:|---:|---|
| Game thread | <= 8.0 ms | <= 7.5 ms | simulation không tick nặng mỗi frame |
| Render thread | <= 7.0 ms | <= 6.5 ms | draw/scene submission |
| GPU total | <= 15.5 ms | <= 14.5 ms | giữ margin cho spike |
| Lumen GI + reflections | <= 4.0 ms | <= 2.7 ms | profile riêng |
| VSM/shadows | <= 3.0 ms | <= 2.0 ms | sunlight + locals |
| Base pass/material | <= 3.0 ms | <= 2.5 ms | opaque/masked |
| Translucency/Niagara | <= 1.5 ms | <= 0.9 ms | combat burst profile |
| Post/TSR | <= 1.8 ms | <= 1.5 ms | không tăng blur |
| Streaming hitch | < 50 ms one-off | < 50 ms one-off | không trong combat input |
| 1% low | >= 45 FPS | >= 50 FPS | route 10–15 phút |

Nếu tổng GPU > budget, không tự giảm render scale để coi là pass. Phải xác định top GPU event bằng Unreal Insights/stat gpu và sửa nguyên nhân trước.

## Memory budget

| Pool | High limit | Medium limit |
|---|---:|---:|
| Process working set game | <= 8 GB | <= 6.5 GB |
| VRAM used peak | <= 9 GB | <= 7 GB |
| Texture streaming pool | 4.5 GB | 3.0 GB |
| Static mesh/Nanite streaming | 1.8 GB | 1.3 GB |
| Skeletal mesh/animation | 700 MB | 500 MB |
| Niagara / transient render | 450 MB | 250 MB |
| Audio + UI + gameplay state | 650 MB | 550 MB |
| Headroom | >= 15% | >= 15% |

Nếu RTX 3060 là bản 8 GB trong test, High có thể dùng texture pool 3.5 GB và phải ghi rõ profile. Không target VRAM sát 100%.

## Scene budgets: vertical slice

| Metric | High | Medium | Ghi chú |
|---|---:|---:|---|
| Hero/Combatant skeletal characters visible | <= 7 | <= 5 | player tính một |
| Service NPC full mesh visible | <= 12 | <= 8 | update rate giảm khi xa |
| Crowd visible | <= 40 | <= 25 | Mass/proxy, no full collision |
| Shadowed movable local lights | <= 6 | <= 3 | ngoài event burst |
| Non-shadow local lights | <= 12 | <= 8 | practical lights |
| Visible deferred decals | <= 32 | <= 18 | distance fade bắt buộc |
| Standard Niagara systems visible | <= 12 | <= 8 | micro aura có pool riêng |
| Hero Niagara systems visible | <= 4 | <= 2 | không overlap dài |
| Foliage instances in view | profile per biome | profile per biome | HLOD/cull start required |
| Translucent full-screen layers | <= 1 | <= 1 | tránh stacking |
| Texture resolution hero/standard | 2K / 1–2K | 2K / 1K | 4K exception reviewed |

Draw call count không có một number universal dưới Nanite; review bằng GPU capture/Insights. Dù vậy, nếu RenderThread > budget, reduce material variety, skeletal mesh, translucency, decals hoặc non-Nanite props trước khi giảm visual toàn cục.

## Scalability tiers

### High

- Lumen GI/Reflections High profile, software RT default.
- VSM High resolution profile.
- Nanite enabled.
- Foliage density 100%.
- Volumetric fog High but constrained local volume.
- Crowd 40 max.
- Character cloth/Groom only player + close hero.
- Niagara all tiers under cap.

### Medium

- Lumen GI Medium, reflection quality reduced.
- Shadowed local lights max 3, lower VSM page budget.
- Foliage density 65%, cull distances shorter.
- Volumetric fog lower grid/temporal cost; keep route readability.
- Crowd 25 max; hero NPC/quest NPC never culled incorrectly.
- Cloth/Groom off except player close-up when needed.
- Hero VFX use medium variant; particle light mostly off.
- Texture pool 3.0 GB, streaming bias only after clear visual test.

### Low (functional fallback)

- Lumen low/software profile or project-approved alternative only after visual review.
- Reduced shadow distance/resolution, no volumetric fog.
- Foliage 35%, crowd 12 max.
- No Groom/cloth, low Niagara.
- Gameplay, interaction anchors, NPC/services and combat cues remain available.
- Low is not a visual approval target but must not cause logic/UI failures.

## CPU rules

- Simulation advances only on accepted commands/calendar batches, never all NPC full update per frame.
- Crowd uses MassEntity/proxy or update-rate optimization; no 40 full Behavior Trees ticking every frame.
- Hero AI/StateTree update at 5–10 Hz when not in immediate interaction/combat.
- No UMG binding polling every frame; ViewModel events update widgets on revision.
- No synchronous asset loading during travel/combat action; prefetch cue/arena assets through soft references.
- Tick disabled by default for actor/component; enabled only with measured need.
- Use object pools for repeated combat VFX/projectiles and interaction highlights.
- Avoid per-frame line traces beyond player interaction/camera needs; cap interaction traces at 1–2 per frame.

## GPU rules

- Nanite for dense static geometry; regular LOD for skeletal, foliage when appropriate, translucency and objects with incompatible deformation.
- Material instruction count/overdraw reviewed using Shader Complexity and Quad Overdraw views.
- Masked foliage density and shadowing are budgeted; no dense two-sided masked canopy without cull/HLOD.
- VFX must use fixed bounds, scalability emitters and no unbounded GPU particle burst.
- Screen-space effects are bounded by screen coverage; no more than one high-cost full-screen distortion at once.
- Dynamic lights from VFX disabled by default and use hard lifetime/count cap.
- Lumen scene quality, VSM and fog are tuned per zone profile, not blindly raised project-wide.

## Profiling protocol

Profile the canonical 10–15 minute vertical-slice route at:

1. Fresh boot, after shader warm-up.
2. Town crowd and service interaction.
3. Sect courtyard wide vista.
4. Wilderness traversal with foliage/fog.
5. Alchemy interior.
6. Combat standard action loop.
7. Hero ability / tribulation burst.
8. Save/load and zone transition.

For each capture record:

- Build commit and content manifest hash.
- Hardware/driver, resolution, preset, DLSS/TSR setting.
- Average FPS, 1% low, Game/Render/GPU ms.
- stat gpu top 10.
- Unreal Insights trace and screenshot/camera ID.
- Memory report / texture pool.
- Known hitch classification and owner.

Do not compare Editor FPS with packaged Shipping FPS. Every performance bug must have reproduction route and threshold.

## Required commands and tools

Windows profiling commands:

~~~text
stat unit
stat unitgraph
stat gpu
stat scenerendering
stat rhi
stat niagara
stat streaming
profilegpu
MemReport -full
r.Streaming.PoolSize
~~~

Use Unreal Insights for CPU/render/asset-load traces. Use RenderDoc only if GPU capture is stable with the packaged Development build. Shader compilation/warm-up is measured separately from runtime hitch.

## Performance gates

### M2B visual target gate

- High route meets 60 FPS average / 45 FPS 1% low.
- Medium route meets 60 FPS average / 50 FPS 1% low.
- No repeatable hitch > 100 ms on route after warm-up.
- Working set and VRAM remain within budget.
- All interaction/NPC/combat readability preserved on Medium.

### Before a new zone/content batch

- Add it to a representative route capture.
- Demonstrate it does not regress top GPU/CPU budget more than remaining margin.
- If it does, optimize or reduce scope before adding another visual kit.

### Release gate

- 30-minute soak at High and Medium without memory growth beyond 10% after stabilization.
- 20 sequential combats and five zone transitions without orphan Niagara/actor/widget.
- Clean-machine packaged build applies correct scalability on first launch.
- No crash/OOM on target machine at 1080p High route.

## Optimization order

When over budget, use this order:

1. Identify actual CPU/GPU/memory limiter with capture.
2. Remove or simplify unseen/overlapping geometry, lights, translucency and particle work.
3. Fix streaming, HLOD, texture resolution and material permutation.
4. Reduce crowd/animation update rate and actor Tick.
5. Tune Lumen/VSM/fog per zone.
6. Adjust scalability profile.
7. Only then consider lower resolution/render scale as user option.

Never “fix” performance by changing simulation determinism, removing canonical interaction, or reducing required gameplay content.
