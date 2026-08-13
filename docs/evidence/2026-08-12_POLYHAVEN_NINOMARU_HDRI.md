# Poly Haven Ninomaru Teien HDRI — acquisition/import evidence

Date: 2026-08-12
Asset: `ninomaru_teien` by Greg Zaal
Source: <https://polyhaven.com/a/ninomaru_teien>
License: CC0 1.0 Universal

## Source evidence

- Official API file entry: `https://api.polyhaven.com/files/ninomaru_teien`.
- Selected variant: 4K Radiance HDR, 28,651,738 bytes.
- Upstream MD5: `3C20EA63DC6D65A844D9D1646AFEFBE5`.
- Local SHA-256: `87EE3C894C753655D8B3A84CBFC596123C1A0E4CFA9256099FF0A886E3B102B0`.
- Poly Haven metadata: sunrise, partly cloudy, low contrast, natural outdoor light, 5400 K white balance and 8 EV range.
- Acquisition command: `tools/download-polyhaven-hdri.ps1 -Resolution 4k`.

## UE import contract

- Source file: `SourceArt/Environment/PolyHaven/hdri/ninomaru_teien_4k.hdr`.
- UE asset: `/Game/VisualTarget/HDRI/TC_NinomaruTeien_4K`.
- Validated type/settings: `TextureCube`, UE 5.8 `TC_HDR_COMPRESSED`, sRGB disabled, imported `TEXTUREGROUP_WORLD`, max texture size 4096.
- Import: `tools/import-polyhaven-hdri.py`.
- Validation: `tools/validate-polyhaven-hdri.py`.

## Exact corridor integration proposal

Use the HDRI for image-based ambient lighting and reflections, not as a literal background. Keep the authored Sky Atmosphere, volumetric fog and directional sun so the corridor retains its own silhouette and controllable day/night state.

Configure the corridor `SkyLightComponent` as follows:

| Property | Value |
|---|---|
| Mobility | `Movable` |
| Source Type | `SLS_SPECIFIED_CUBEMAP` |
| Cubemap | `/Game/VisualTarget/HDRI/TC_NinomaruTeien_4K` |
| Real Time Capture | `false` |
| Cubemap Resolution | `1024` |
| Source Cubemap Angle | `205.0` degrees, then tune only after the locked hero camera comparison |
| Intensity Scale | `0.65` initial target |
| Lower Hemisphere Is Solid Color | `true` |
| Lower Hemisphere Color | linear RGB `(0.012, 0.016, 0.014)` |

Keep one directional sun at roughly 5400 K and start near 20,000 lux. Evaluate exposure and shadow direction in the same locked hero view before changing HDRI intensity. This avoids double-capturing Sky Atmosphere (`Real Time Capture=false`) and leaves the specified cubemap deterministic across packaged runs.

## Validation result

- Provenance validator: PASS, 41 source files / 41 SHA-256 entries.
- UE 5.8 import: PASS; `/Game/VisualTarget/HDRI/TC_NinomaruTeien_4K` created as a 26,053,516-byte `.uasset`.
- UE validation: PASS; `TextureCube`, `TC_HDR_COMPRESSED`, sRGB disabled, source import path matched the approved 4K HDR.

Status: `UE58_IMPORT_VALIDATED`; ready for corridor Skylight integration.
