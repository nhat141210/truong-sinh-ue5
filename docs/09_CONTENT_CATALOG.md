# Content catalog

## Nguyên tắc

Mỗi entry có stable ID, localization key, resolver/cue references, version và validation. Content do dự án author; nguồn tham khảo chỉ giúp nghiên cứu cảm giác, không cung cấp text/data/asset.

## Family bản đầu

| Family | Demo target | Bắt buộc |
|---|---:|---|
| Realm | 3 | lifespan bonus, breakthrough requirement |
| Technique | 4 | 2 cultivation, 1 longevity, 1 possession |
| Activity | 5 | cultivation, breakthrough, alchemy, formation, conflict |
| Pill/recipe | 4 | material, facility, result/cue |
| Formation | 1 | spirit gathering |
| Zone | 4 | map, travel cost, presentation profile |
| Canonical NPC | 12–20 | schedule, lifespan, identity, relation hooks |
| Sect | 1 | membership, facilities, local tasks |
| Authored event chain | ≥3 | conditions, roles, choices, cooldown |
| Home facility | 4 | meditation, furnace, formation, storage |
| Presentation cue module | shared library | element/delivery/impact/scale/symbol |

## Gate entry

- ID canonical lowercase và không duplicate.
- Player text có String Table key.
- Mọi reference tồn tại.
- Activity chỉ trỏ resolver được đăng ký.
- Cue thiếu có fallback; không làm gameplay fail.
- Stateful entry có save/load test.
- Asset ngoài dự án có provenance trước import.

Không mass-author hàng trăm entry trước khi một entry của family chạy qua build, validator, save/load và presentation fallback.
