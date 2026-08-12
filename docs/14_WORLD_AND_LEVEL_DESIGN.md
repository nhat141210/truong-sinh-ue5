# World và level design

## Bốn open-zone

| Zone | Giới hạn | Kỹ thuật |
|---|---:|---|
| Thị trấn | 250×250m | map thường, Level Instances |
| Tông môn | 300×300m | map thường, Data Layers |
| Hoang dã | ≤600×600m | cân nhắc World Partition sau profiling |
| Động phủ | 100×100m | map riêng, slot/facility patches |

Mỗi lần chỉ một zone loaded. World map submit travel command, commit time, phát cinematic ngự kiếm/loading rồi load target. Không free-flight hoặc seamless transition bản đầu.

## Landmark/density

Mỗi zone có một silhouette chính, 2–4 secondary landmarks và đường nhìn dẫn mục tiêu tiếp theo. Ưu tiên mật độ encounter/âm thanh/chuyển động thay vì diện tích rỗng.

## NPC presentation

- Spawn tại authored schedule anchors theo canonical time/state.
- 6–10 full skeletal NPC visible; crowd trang trí 15–20 tối đa.
- NPC xa giảm tick/animation hoặc proxy; dùng Animation Budget Allocator từ visual slice.
- Offscreen NPC không Actor/NavMesh; simulation state batch theo ngày/tháng.

## Environment state

Ngày/đêm, weather và story/world patches dùng preset + Data Layers. Không mô phỏng khí tượng vật lý sâu. PCG chưa bật trong nền tối thiểu; chỉ bật ở M2B khi thật sự giúp dựng cảnh, sau đó bake kết quả. Gameplay-critical resource/spawn luôn có stable anchor.

## Traversal

Đi/chạy/nhảy và mantle ở ledge đánh dấu. Không free climbing, bơi lặn sâu hoặc parkour hệ thống bản đầu. Collision/nav/accessibility route phải pass ở High/Medium/Low.
