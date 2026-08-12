# UI/UX spec

## Mục tiêu

UI giúp người chơi hiểu “ta chuẩn bị gì, mất bao lâu, rủi ro ở đâu và kết quả vì sao”, không yêu cầu phản xạ. Toàn bộ chữ là tiếng Việt đơn giản từ String Table.

## Layer

- Explore HUD: giờ/ngày, tuổi thọ còn lại, tu vi, interaction prompt.
- Character/Soul: body stats, lifespan contributions, soul knowledge, vessel history.
- Activity Planner: method, facility, resources, duration, strategy, preview.
- Result Replay: 1x, 2x, skip; factors và summary.
- World/Map: bốn zone, travel time, cinematic transition.
- Home/Social/Sect/Inventory/Settings.

## Activity Planner chung

Một widget dùng cho mọi resolver:

```text
Chọn activity/method
→ chọn resources/facility/duration/strategy
→ eligibility + cost + risk band
→ xác nhận
→ pending/committed
→ replay + result factors
```

Không hiển thị exact RNG roll trước confirm. UI không tự trừ item/time hoặc apply outcome.

## Conflict

Cho chọn loadout, support items và `ÁP ĐẢO / THẬN TRỌNG / KÉO DÀI / RÚT LUI`, đồng thời hiện route đàm phán/bồi thường/bỏ chạy/nhờ tông môn nếu đủ điều kiện. Sau confirm không có skill bar, health input, target cycling hoặc timing prompt.

## Accessibility

- Remap, sensitivity, invert Y, FOV, brightness.
- Subtitle, text scale, giảm shake/flash và tắt motion blur.
- Replay 2x/skip; missing cue dùng text summary.
- Không truyền thông tin chỉ bằng màu.
