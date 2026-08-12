# System rulebook — endless cultivation simulation

## Authority

Simulation nhận snapshot + command + seed và trả immutable result/events. Nó không đọc Actor, Widget, input, DeltaSeconds, animation, camera, Lumen hoặc Niagara.

```text
Validate → Resolve deterministic → Commit once → Hash/save → Present
```

## Thời gian

- Canonical unit: phút game; 1 ngày = 1.440 phút.
- Exploration adapter gom real time và gửi quantum 10 phút game.
- Action lớn gửi duration đã được resolver xác nhận.
- Pause/menu/game đóng không tiến thời gian.
- Scheduler order: time → lifespan → facility/home → NPC → faction/social → economy → authored event → presentation notification.
- Thứ tự và seed phải giống nhau khi batch nhiều ngày hoặc chạy từng quantum.

## Tuổi thọ

`Effective = Base + Realm + Technique + PillResource - PermanentDamage`.

- Mọi contribution là integer days và có source ID/transaction ID khi triển khai catalog.
- Đột phá tăng RealmBonus; dưỡng sinh tăng TechniqueBonus; đan dược/tài nguyên tăng PillResourceBonus.
- Dùng lặp áp diminishing-return policy từ data; không hard-code trong UI.
- Body expired queue một soul-transition event; không Game Over.

## Activity

- Preview không tiêu RNG canonical.
- Confirm tạo CommandId; resolver dùng stream dẫn xuất từ seed + CommandId.
- Result commit trước replay; retry CommandId trả result cũ/reject duplicate.
- Bounded variation mặc định ±300 basis-point units; không đảo power gap lớn.
- Activity mới đăng ký resolver ID, data và cue; không tạo subsystem/minigame riêng.

## Soul/vessel

- Soul: learned techniques, memories, integrity, karma/scars, vessel history.
- Vessel: body roots, realm/cultivation, lifespan, identity, sect, relations, property.
- Possession success nhận target vessel identity/property/relations, reset realm về mortal và cultivation về 0.
- Technique knowledge còn trong soul nhưng eligibility phụ thuộc vessel.
- Failure giảm integrity; body mất thì emergency vessel bảo đảm save tiếp tục.

## NPC/event

- Canonical NPC tồn tại dạng state; Actor chỉ là presentation khi zone loaded.
- In-zone schedule theo anchor/time; offscreen batch không chạy NavMesh.
- Event authored có preconditions, role slots, cooldown, priority, seed stream và consequences.
- Không runtime LLM hoặc chuỗi sinh ngẫu nhiên không kiểm soát.

## Không được có

- Action combat/hitbox/turn input.
- Frame-rate dependent gameplay.
- Offline progress/system clock.
- Direct mutable state từ Blueprint/UI.
- Canonical state lưu hard reference đến level Actor.
