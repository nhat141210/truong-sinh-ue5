# Content catalog — dữ liệu gốc, data-driven

## 1. Quy tắc catalog

Catalog là danh mục source-of-truth cho content gameplay. Dự án author toàn bộ entry gốc; benchmark chỉ giúp audit **loại capability và quan hệ hệ thống**, không phải nguồn để chép tên, mô tả, dialogue, map, NPC, quest, asset hoặc bảng data.

Không thêm content vào `Content/` bằng tay rồi hy vọng nó được registry phát hiện. Mỗi entry phải có stable ID, owner, localization, gameplay tags, dependency và test/validation path.

## 2. Namespace và ID

Prefix chính: `ts.`. ID lowercase ASCII, dot-separated, bất biến sau khi release:

```text
ts.realm.qi_refining
ts.technique.river_mirror
ts.ability.river_mirror.wave_cut
ts.item.herb.mist_reed
ts.sect.thanh_luu
ts.region.yun_khe
ts.quest.yun_khe.first_errand
ts.npc.nguyen_lam
```

Display name/description dùng String Table key, ví dụ `UI.Item.MistReed.Name`; không đưa tiếng Việt người chơi thấy vào ID hoặc C++ code. Mọi ID cần có migration alias nếu rename trước release data freeze.

## 3. Loại content bắt buộc

| Family | Data tối thiểu | Validation trọng yếu |
| --- | --- | --- |
| Realm/Dao/Root | requirement, modifier, unlock, localization | progression không có cycle / missing requirement |
| Technique/Ability | tags, source, cost, effect, cue, AI hints | target/cost/effect/cue references hợp lệ |
| Item/Herb/Pill/Artifact | category, stack, ownership, modifier/recipe refs | item không orphan, stack/quality contract |
| Recipe/Facility | inputs, tool/facility, time cost, outcomes | outputs and costs resolve được |
| NPC/Relation | state template, faction, location, goal hooks | location/quest/faction IDs hợp lệ |
| Sect | ranks, permissions, contribution rule, obligations | membership state machine complete |
| Region/Location/Link | zone, map graph, travel rules, encounter table | graph reachability, no broken link |
| Quest/Event/Encounter | precondition, choice, deadline, consequences | no unreachable node/dead-end ngoài explicit failure |
| Economy/Auction/Trade | market entries, stock/price rule, refresh | currency/item references/invariants |
| Presentation Cue | mesh/animation/VFX/SFX/camera key | missing asset fallback and no mechanics refs |

## 4. Authoring pipeline

```text
Original design brief
→ structured JSON/CSV draft
→ schema validation
→ importer creates/updates PrimaryDataAsset or DataTable row
→ ContentRegistry validation in Editor/CI
→ automation fixture + PIE scenario
→ approved content manifest
```

- JSON/CSV is review-friendly authoring input; generated `.uasset` is not edited to alter authoritative rules without updating source data.
- A content import must be deterministic and report additions/changes/removals by ID.
- Content deletion after a save-compatible milestone uses deprecation + migration, not silent removal.
- New art asset is linked by `PresentationCue`/asset reference only after logic content passes validation.

## 5. Metadata required per entry

```yaml
id: ts.item.herb.mist_reed
family: Herb
status: draft | playable | release
owner: content
localization_key: UI.Item.MistReed.Name
source: ORIGINAL
parity_capabilities: [CAP-PROD-001, CAP-ECON-001]
required_tags: [Item.Herb, Element.Water]
dependencies: [ts.region.yun_khe]
test_fixture: Automation.Content.HerbMistReed
presentation: ts.cue.item.mist_reed
notes: "Nội dung gốc; không chứa material/text benchmark."
```

`source` chỉ dùng `ORIGINAL`, `ORIGINAL_INSPIRED_BY_CAPABILITY`, `DONOR_CODE_ADAPTED` hoặc `PLACEHOLDER_INTERNAL`. Không có giá trị `REFERENCE_COPY`.

## 6. Content seed gốc cho vertical slice

Đây là bộ seed nhỏ để implementer có thể dựng slice mà không cần chờ catalog full. Nó không phải statement parity content scale.

| Family | IDs/tối thiểu | Mục đích |
| --- | ---: | --- |
| Region/zone | 3 zone + 1 arena + 1 home | thành thị, sect, wilds, combat, động phủ |
| Location links | 4 links | travel preview + local/no-time contrast |
| Player origin | 2 lựa chọn | chứng minh root/build selection |
| Realm | 2 state đầu + 1 breakthrough | progression/breakthrough slice |
| Technique | 3 | passive, resource, combat modifier |
| Ability | 5 | resource, attack, defense/status, utility, conditional |
| Item/herb/material | 12 | loot, market, recipe, inventory |
| Pill/artifact | 3 | build and crafting result |
| Recipe | 2 | one success/failure path each |
| NPC | 8 | merchant, sect contact, mentor, rival, local actors |
| Sect | 1 | membership/contribution sample |
| Quest | 3 | accept/branch/deadline/reward paths |
| Event/encounter | 4 | scheduled, travel, combat, breakthrough |

Entry names/lore are authored in Vietnamese in String Tables when implemented; this document intentionally lists only IDs and functional roles.

## 7. Catalog coverage matrix khởi tạo

| Family | Audit type known? | Schema ready? | Seed authored? | Full scale audited? | Release complete? |
| --- | --- | --- | --- | --- | --- |
| Realm/Dao/Root | `NO` | `NO` | `NO` | `NO` | `NO` |
| Technique/Ability | `NO` | `NO` | `NO` | `NO` | `NO` |
| Item/Recipe/Facility | `NO` | `NO` | `NO` | `NO` | `NO` |
| NPC/Sect/Relation | `NO` | `NO` | `NO` | `NO` | `NO` |
| Quest/Event/Encounter | `NO` | `NO` | `NO` | `NO` | `NO` |
| Region/Travel/Economy | `NO` | `NO` | `NO` | `NO` | `NO` |
| Presentation cue/asset | `NO` | `NO` | `NO` | `NO` | `NO` |

## 8. Quality gates

### Draft → Playable

- Schema validates; all stable IDs and tags resolve.
- Localization key exists in Vietnamese string table.
- At least one unit/automation fixture proves rule behavior.
- Placeholder presentation allowed only if it has fallback and no asset license/runtime blocker.

### Playable → Release

- Parity capability rows tied to entry are at least P3; full-scale target has been audit-derived.
- Entry participates in at least one reachable full-run path or is intentionally optional with documented discovery.
- NPC/quest/economy entry does not create dangling state after save/load/time advance.
- Art/audio/presentation has provenance in `20_ASSET_AND_REFERENCE_INDEX.md`.
- Build/package validator reports no missing asset, invalid tag, absent String Table key or obsolete deprecated ID.

## 9. Agent checklist khi thêm content

1. Xác định `CAP-*` và spec đã đạt P1 tối thiểu.
2. Tạo/duyệt schema, ID và String Table key trước asset/UI.
3. Add data entry qua importer hoặc source format đã chuẩn hóa.
4. Chạy registry validation và fixture có ID cụ thể.
5. Kiểm tra save/load + world simulation nếu entry có state/tick.
6. Cập nhật coverage matrix và `27_IMPLEMENTATION_STATUS.md` bằng trạng thái thật.

Không thêm hàng trăm item, NPC hay quest trước khi pipeline, validator và một vertical-slice entry của family đó đã pass.
