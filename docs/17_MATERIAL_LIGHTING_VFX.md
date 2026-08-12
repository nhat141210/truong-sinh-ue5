# Material, lighting và VFX

## Rendering baseline

- Deferred, DX12/SM6.
- Software Lumen GI; không Hardware Ray Tracing.
- VSM, TSR và selective Nanite.
- Lumen Reflection chỉ hero scene nếu budget; mặc định SSR.
- Một unbound Post Process Volume/zone; bounded volume hiếm.

## Material

- Shared master materials, material instances và packed masks.
- Nanite cho đá/kiến trúc/occluder/high-poly static; không bật bừa cho sky/effect mesh.
- Không 8K texture, POM nặng hoặc nhiều translucent layer.
- Vật static không đổi transform liên tục để tránh invalidation distance field.

## Lighting/weather

- Một Directional Light + SkyLight nền.
- Tối đa bốn movable local shadow lights visible ngoài cinematic.
- Weather dùng authored presets/Data Layers: clear, rain/mist, storm/tribulation.
- Một volumetric fog chính; không chồng nhiều local fog.

## VFX module

Niagara library ghép `ngũ hành × delivery × impact × scale × symbol`. Cue có particle/light/translucency budget và reduced-flash fallback. Niagara không tính hit, quality, success hoặc reward.

Missing cue trả text/result summary và không chặn gameplay. 1x/2x/skip không thay canonical result.
