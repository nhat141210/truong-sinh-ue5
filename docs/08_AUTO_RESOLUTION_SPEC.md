# Auto-resolution spec

## Hợp đồng chung

Mọi activity lớn dùng cùng giao diện:

```text
FTruongSinhActivityPlan
→ FTruongSinhAutoResolver::Resolve(snapshot, plan)
→ FTruongSinhAutoResolutionResult
→ simulation commit (M2A)
→ presentation replay plan (M2A)
```

Plan chứa stable IDs cho activity, actor/targets, method, facility, location, resources, duration, strategy và seed context. Result chứa status/reason, time/resource deltas, outcome values, injuries/effects và ordered beats. Replay chỉ chứa cue IDs/timing; không chứa formula hoặc mutable state.

## Resolver registry đích

Source proof hiện dùng một resolver thuần với `ETruongSinhActivityType`; registry Data Asset chưa triển khai. Bản đầu cần các type:

- Cultivation.
- Breakthrough.
- Alchemy.
- Formation.
- Conflict.

Artifact/Tribulation chỉ thêm sau khi framework và demo pass. Activity mới đăng ký resolver ID + data; không tạo subsystem/UI framework riêng.

## Preview

Preview chỉ trả eligibility, costs, duration, risk band và yếu tố chính. Nó không tiêu RNG canonical và không được tiết lộ roll chính xác. Confirm tạo CommandId/seed context; retry cùng command trả cùng result.

## Conflict auto

Input: realm/progress, cultivation units, technique mastery/counters, body/root, artifacts/pills, injuries, terrain và strategy `overwhelm/cautious/endure/retreat`.

- Power gap lớn cho kết quả ổn định; bounded RNG không đảo ngược chênh lệch lớn.
- Close match có thể đổi bởi counter, preparation và strategy.
- Trước conflict có route negotiate/pay/flee/sect-assist nếu điều kiện cho phép.
- Không turn input, hitbox, combo, dodge/parry hoặc combat Behavior Tree.

## Alchemy/formation/cultivation

- Alchemy: recipe, batch, materials, furnace, strategy, skill, environment → quantity/quality/impurity/failure.
- Formation: blueprint, purpose, anchors/site, materials, skill → integrity/effect/duration.
- Cultivation: technique, site, duration, supplements, risk → progress/insight/injury/event.
- Breakthrough: realm gate, foundation, preparation, location, risk → success/failure/injury/lifespan delta.

Không resolver nào đọc Actor, animation, camera, Niagara hoặc frame time.

## Presentation

Ordered beats dùng module `element × delivery × impact × scale × symbol`. Director hỗ trợ 1x, 2x, skip và missing-cue fallback. Result đã commit trước replay; thoát giữa cinematic không reroll/duplicate reward.
