# Kiến trúc kỹ thuật UE5.8

## Module

| Module | Authority | Không được làm |
|---|---|---|
| TruongSinhCore | IDs, command/event, RNG primitives | biết world/UI |
| TruongSinhData | Data Assets, registry, validators | mutable save state |
| TruongSinhSimulation | world/time/life/soul/NPC state và commit | đọc Actor/frame time |
| TruongSinhResolution | preview/resolver/result/beat | commit world hoặc phát VFX |
| TruongSinhNarrative | authored events/local tasks/rumors | main quest/ending authority |
| TruongSinhSave | schema/checksum/atomic/backup/migration | reroll result |
| TruongSinhWorld | zone loading, interaction adapter, NPC actor presentation | canonical NPC AI |
| TruongSinhPresentation | animation/camera/Niagara/audio/replay | quyết định outcome |
| TruongSinhUI | CommonUI/view model/input | mutate state trực tiếp |
| TruongSinhUE5 | composition root, GameMode/player shell | domain formula |

Dependency direction:

```text
Core ← Data
Core/Data ← Resolution
Core ← Simulation
Core/Data/Simulation ← Narrative
Simulation/Resolution/Narrative ← Save
Simulation/Narrative ← World
Resolution/Simulation ← Presentation/UI
all runtime modules ← composition root
```

## Public flow

```text
Actor/UI creates typed request
→ Simulation validates state/revision/resources
→ Resolution produces deterministic draft + beats
→ Simulation commits deltas and state hash
→ Save records pending replay
→ Presentation consumes beats
→ replay completion clears pending replay
```

Blueprint chỉ gọi preview hoặc submit command vào facade. C++ resolver không trả mutable references. Presentation cue ID là stable data, không hard object reference trong save.

## World/NPC

- Mỗi zone là map riêng; một zone loaded.
- Canonical NPC state không phải Actor.
- World spawner materialize 6–10 NPC actors theo time/schedule anchors.
- Offscreen simulation batch theo scheduler; không NavMesh/Behavior Tree.

## Editor automation

Python/MCP chỉ bind loopback và dùng sau native build. Source/data là nguồn thật; Editor write phải compile/save/validate và commit checkpoint.
