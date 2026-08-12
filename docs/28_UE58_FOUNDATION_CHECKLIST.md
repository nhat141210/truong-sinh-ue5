# Checklist UE5.8 foundation

## Native gate

- [ ] Exact UE5.8 Build.version được ghi.
- [ ] VS C++/Windows SDK/Git LFS/PowerShell 7 pass.
- [ ] UHT/UBT Editor + Development pass.
- [ ] Tất cả automation `TruongSinh` có JSON report pass.
- [ ] Module `TruongSinhResolution` và `TruongSinhNarrative` load; không còn old module descriptor.

## Smoke gameplay

- [ ] `/Game/Maps/Dev/L_Dev_Smoke` có floor/PlayerStart/light/interaction.
- [ ] Enhanced Input move/look/jump/interact hoạt động.
- [ ] Activity preview không mutate state.
- [ ] Cultivation sample resolve/commit/replay summary hoạt động.
- [ ] Save v2 Continue không reroll/duplicate.

## Visual gate

- [ ] Software Lumen, SSR mặc định, no hardware RT.
- [ ] Một corridor, một NPC, một activity cue.
- [ ] `stat unit/gpu`, ProfileGPU, Insights, memreport lưu evidence.
- [ ] RTX3060 1080p High avg60/1%45; Medium fallback.
- [ ] Asset có provenance; không mass import.

## Không được bật

- Action combat/multiplayer/runtime LLM/offline progress.
- Seamless four-zone world/free flight/free climbing.
- Gameplay outcome trong Blueprint/Niagara/animation.
