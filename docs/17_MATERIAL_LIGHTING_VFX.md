# 17 — Material, lighting và VFX

## Mục tiêu

Ánh sáng và vật liệu phải bán được chất lượng UE5 ngay từ vertical slice: địa hình có chiều sâu, nội thất có không khí, linh khí có quy luật. Chúng không được che đường đi, làm UI khó đọc hay phá 60 FPS trên RTX 3060.

Lumen, Nanite, Virtual Shadow Maps và Niagara là presentation systems. Không dùng chúng để encode cơ chế gameplay hoặc outcome parity.

## Renderer baseline

- Deferred renderer.
- Lumen Global Illumination và Lumen Reflections, hardware ray tracing không bắt buộc.
- Virtual Shadow Maps.
- TSR ở 1080p; High chất lượng ưu tiên rõ hình hơn sharpness giả.
- Nanite cho static architecture, rock, ruin, hero props phù hợp.
- Forward/Compatibility renderer không là supported visual target.
- Exposure dùng Manual hoặc Auto Exposure có EV100 clamp profile theo zone; không để adaptation mạnh làm gameplay thay đổi khó đọc.

## Master material family

Không tạo material graph độc lập cho mỗi asset. Bắt đầu với family sau:

| Master | Dùng cho | Feature switch chính |
|---|---|---|
| M_TS_Surface | đá, gỗ, gạch, kim loại, đạo cụ | tint, roughness, macro/micro normal, dirt, wetness, vertex blend |
| M_TS_Terrain | terrain/landscape | layer blend, RVT, distance detail, foliage mask |
| M_TS_Foliage | cỏ/cây/lá | two-sided foliage, wind tier, subsurface, seasonal tint |
| M_TS_Cloth | trang phục/cờ/vải | weave normal, tint, dirt, optional cloth masking |
| M_TS_Spirit | linh khí/trận pháp/đạo cụ ma thuật | emissive clamp, dissolve, panner, depth fade, element profile |
| M_TS_Water | nước | shallow/deep color, normal, foam, low-cost refraction |
| M_TS_Decal | vết bẩn, rune, impact | DBuffer, fade distance, priority |
| M_TS_UIWorld | billboard/world marker | distance scaling, occlusion, accessibility mode |

Mọi switch compile-time cần budget shader permutation. Không thêm switch mới nếu chưa chứng minh cần cho từ hai asset family trở lên.

## Material rules

- Standard opaque surface: 1–2 texture set 1K/2K, packed ORM, normal, optional macro mask. Không dùng nhiều 4K map.
- Terrain: tối đa 4 paint layers visible trong one component; dùng Runtime Virtual Texture cho road/foliage blend khi profiling chứng minh cần.
- Transparent: tránh large screen-space translucent sheet. Dùng masked/dithered material cho foliage/cue khi có thể.
- Emissive: clamp trong M_TS_Spirit; strength controlled by FPresentationCueProfile, không hard-code extreme HDR.
- Water: một water profile per zone, maximum two overlapping water material types in view.
- Decal: max 32 visible deferred decals in vertical-slice scene; fade by distance/lifetime.
- Material instance param naming: BaseTint, RoughnessBias, DirtAmount, WetnessAmount, ElementPrimary, ElementSecondary, EmissiveStrength.

## Lighting profiles

FZoneLightingProfile là data asset consumed by UZonePresentationSubsystem:

~~~text
DirectionalLightAngle
DirectionalIntensityRange
SkyAtmosphereProfile
SkyLightIntensity
VolumetricFogProfile
CloudProfile
ExposureMinEV100
ExposureMaxEV100
ColorGradeLUT
PracticalLightSet
WeatherStateTag
AudioAmbienceId
~~~

Lighting is authored per zone/time/event but activation comes from simulation date/event state. A light actor không tự advance ngày/đêm.

### Zone lighting intention

| Zone | Key light | Fill | Fog | Đọc gameplay |
|---|---|---|---|---|
| Town | warm sun/lantern mix | neutral sky | very light | NPC/service/route rõ |
| Sect | clean mountain sun | cool skylight | thin mist | hierarchy/landmark rõ |
| Wilderness | directional sun/cloud | cool bounce | layered local fog | đường mòn/harvest vẫn rõ |
| Cave/alchemy | practical warm furnace + cool leak | controlled | minimal/local | station và face đọc được |
| Secret realm | one strong elemental identity | restrained | profile-specific | không che target/camera |
| Tribulation | storm directional + timed flash | dark blue ambient | sparse | reduced-flash fallback bắt buộc |

## Dynamic light budget

High vertical slice targets:

- 1 directional light, 1 skylight, 1 sky atmosphere, 1 volumetric cloud system per zone.
- Tối đa 6 shadow-casting movable local lights visible simultaneously outside cinematic/combat burst.
- Tối đa 12 non-shadow local lights visible.
- VFX light default off; chỉ top-tier cue bật trong 0.2–0.8 seconds, tối đa 2 visible.
- Hero alchemy/tribulation cinematic có thể burst hơn trong <2 seconds nhưng phải profile GPU spike.
- Light channels không dùng làm logic visibility/game rule.

Medium giảm 50% shadowed local lights, disables most VFX lights, lowers volumetric fog quality và Lumen reflection quality.

## Lumen/VSM workflow

1. Blockout scene with correct scale and lighting direction.
2. Set Nanite/HLOD/mesh material before final Lumen tuning.
3. Bake no lightmap dependency for primary dynamic mood; static lightmap chỉ dùng nếu an optional optimization không đổi look.
4. Profile Lumen Scene, reflections and VSM with stat gpu in representative camera route.
5. Clamp exposure and test white/black readability with UI/HUD.
6. Record profile screenshot and cvar list in visual review.
7. Re-test High and Medium after any new hero material/VFX.

Không solve noise/flicker bằng increasing every quality knob globally. Fix mesh thickness, surface cache visibility, screen coverage, reflection captures/profile or reduce material complexity first.

## Niagara architecture

### Cue ownership

UTSCharacterCueComponent / UCombatPresentationDirector request cue by FPrimaryAssetId. Niagara system never decides whether hit, dodge, status, crafting success or breakthrough succeeds.

~~~text
FPresentationCueProfile
  ├── NiagaraSystemSoftPath
  ├── AttachSocket / world anchor policy
  ├── ElementTag
  ├── IntensityTier
  ├── Lifetime
  ├── CameraImpulseProfile
  ├── AudioProfile
  └── AccessibilityVariant
~~~

### System tiers

| Tier | Dùng cho | Target visible |
|---|---|---|
| Micro | aura, footstep, interaction hint | <= 30 |
| Standard | ability cast, impact, herb pickup | <= 12 |
| Hero | breakthrough, strong technique | <= 4 |
| Event | tribulation/weather/boss scene | <= 1 |

CPU particles tránh cho ambient density lớn. Ưu tiên GPU sprites/mesh particles với fixed bounds, scalability emitter và LOD. Mỗi Niagara system phải có warm-up policy and pooled reuse path; no unbounded spawn.

### Effects readability

- Mỗi elemental cue dùng shape/motion riêng, không chỉ đổi màu.
- Spawn location không che target ring/UI.
- Projectile/beam có source-target readability at 10–30 m.
- Screen flash duration default <= 100 ms and max opacity <= 0.35; reduced-flash uses ground/rim cue thay thế.
- Camera shake is opt-in / scaleable; no repeated shake from DOT ticks.

## Post process

- One unbound base PPV per zone plus rare bounded combat/cinematic volume.
- Bloom restrained; lens flare off by default.
- Chromatic aberration off by default.
- Motion blur low or off; user setting.
- DOF only dialogue/cinematic, never during action selection.
- Color grading must retain UI/world marker contrast and element distinguishability.
- Vignette <= subtle threshold, no black edge for ambience.

## Shader and asset validation

Each material/VFX addition must provide:

- Master/material instance family.
- Texture resolution and memory estimate.
- Nanite/masked/translucent state.
- Shader complexity capture.
- LOD/scalability settings.
- Expected visible count.
- Asset manifest ID and visual kit assignment.
- Medium fallback result.

Data validation fails if a PresentationCue lacks an accessibility variant when it has flash/camera impulse, or if a material references missing texture/profile.

## Acceptance

- Town, sect, wilderness, cave, arena and tribulation test scene have no persistent black crush, white clip or unreadable interaction prompt.
- High and Medium retain route, NPC and target readability.
- No major Lumen/VSM flicker in 60-second camera traversal after shader warm-up.
- Niagara particle counts, translucency and dynamic-light spikes meet 19_PERFORMANCE_BUDGET.md.
- Material shader complexity does not show widespread red/white hot areas on critical path.
- A failed/missing cue degrades gracefully to UI/combat-log text, not gameplay failure.
