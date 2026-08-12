# Checklist UE5.8 foundation

## Native gate

- [x] Exact UE build: 5.8.1 changelist 56057345.
- [x] VS C++/Windows SDK/Git LFS/PowerShell 7 pass.
- [x] UHT/UBT Editor + Development pass.
- [x] Toàn bộ automation `TruongSinh` có JSON report pass: 12/12.
- [x] Module runtime và test load; test module dùng `DeveloperTool` trên UE 5.8.

## Smoke gameplay

- [x] `/Game/Maps/Dev/L_Dev_Smoke` có floor/PlayerStart/light/interaction.
- [x] Enhanced Input native cho move/look/jump/interact/pause.
- [x] Activity preview không mutate state.
- [x] Cultivation resolve → canonical commit → summary → autosave.
- [x] Automation chứng minh duplicate/reload không reroll hoặc cộng progress hai lần.
- [x] Map package validation 0 errors/0 warnings.
- [x] Standalone Editor đưa map/world lên play và khởi tạo sandbox.
- [x] Development package portable cook/stage/archive và executable độc lập load smoke map sạch log.
- [x] Manny visual foundation hiển thị third-person với idle/jog/fall trong packaged runtime.
- [ ] Manual PIE/Standalone input và Continue capture.

## Visual gate

- [x] Software Lumen GI, SSR (`r.ReflectionMethod=2`), VSM, TSR, no hardware RT.
- [ ] Một corridor, một NPC, một activity cue production-quality.
- [ ] `stat unit/gpu`, ProfileGPU, Insights, memreport evidence.
- [ ] RTX3060 1080p High avg60/1%45; Medium fallback.
- [ ] Asset có provenance; không mass import.

## Không được bật

- Action combat/multiplayer/runtime LLM/offline progress.
- Seamless four-zone world/free flight/free climbing.
- Gameplay outcome trong Blueprint/Niagara/animation.
