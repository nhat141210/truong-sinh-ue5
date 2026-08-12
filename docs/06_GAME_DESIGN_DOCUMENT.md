# Game Design Document — Trường Sinh UE5

## Trải nghiệm

Người chơi là một nguyên thần đang sống trong một thân xác tu sĩ. Họ trực tiếp khám phá thế giới 3D, chọn nơi sống, công pháp, quan hệ và cách chuẩn bị. Hệ thống tự tính các hoạt động phức tạp; UE5 biến kết quả thành cảnh quan, animation và VFX thuyết phục.

## Động từ trực tiếp

- Đi, chạy, nhảy, mantle tại ledge đánh dấu.
- Quan sát, nói chuyện, thu thập và tương tác.
- Mở bản đồ, chọn travel/cinematic ngự kiếm.
- Bố trí facility/đồ vật trong động phủ theo slot/preview.
- Chọn activity, tài nguyên, facility, thời gian và strategy.

Không có input combat, combo, dodge, parry, căn lửa, nối trận văn hoặc minigame phản xạ.

## Activity loop

```text
Khám phá → biết cơ hội/rủi ro → lập kế hoạch
→ xem preview → xác nhận → simulation commit
→ cinematic/replay → hậu quả world/NPC/time
```

## Thời gian

- Khám phá mặc định: 24 phút thực = một ngày game.
- Presentation adapter gửi quantum 10 phút game; simulation không đọc DeltaSeconds.
- Pause/menu toàn màn hình/game đóng: không tiến thời gian.
- Travel, bế quan, luyện đan, bố trận và đột phá: nhảy thời gian theo activity.

## Tuổi thọ

`EffectiveLifespan = Base + Realm + Technique + Pill/Resource - PermanentDamage`.

Đột phá tăng tuổi thọ; dưỡng sinh, động phủ và linh mạch hỗ trợ; đan dược lặp bị diminishing return/kháng dược. Tu sĩ chuẩn bị tốt có thể sống hàng trăm/nghìn năm. Đoạt xá là lựa chọn, không phải timer buộc chết.

## Soul/vessel

- Soul giữ Soul ID, integrity, công pháp đã học, ký ức/bí mật, nghiệp và lịch sử các đời.
- Vessel giữ căn cơ, linh căn, tuổi, thọ nguyên, thân thể, tu vi và identity/social/property.
- Đoạt xá nhận toàn bộ identity/assets/relations của NPC mục tiêu, giữ tri thức soul nhưng tái lập kinh mạch nên body cultivation về tầng thấp.
- Nếu không còn vessel, Wandering Soul deterministic cung cấp thân phàm nhân yếu; không Game Over.

## Bốn zone demo

| Zone | Kích thước tối đa | Nội dung |
|---|---:|---|
| Thị trấn | 250×250m | market, NPC, social, tin đồn |
| Tông môn | 300×300m | công pháp, nhiệm vụ địa phương, facility |
| Hoang dã | 600×600m | landmark, tài nguyên, kỳ ngộ |
| Động phủ | 100×100m | tu luyện, đan, trận, xây dựng |

Một zone loaded mỗi lần. Demo có 12–20 NPC canonical, 6–10 full-quality visible.

## Demo 3–5 giờ

1. Khám phá thị trấn và chọn tông môn/tán tu.
2. Nhận động phủ nhỏ; học hai công pháp và một phương pháp dưỡng sinh.
3. Tu luyện, luyện một mẻ đan, dựng trận tụ linh.
4. Xây quan hệ với ít nhất tám NPC; gặp ba chuỗi event authored.
5. Giải quyết một xung đột bằng tránh đấu hoặc đấu pháp auto.
6. Đủ chuẩn bị thì đột phá và tăng thọ; nếu chọn đổi đường thì chuẩn bị đoạt xá.
7. QA phải chơi được cả route trường thọ và route đoạt xá.
8. Hết content authored, scheduler/event pool vẫn cho tiếp tục sandbox; không credits.
