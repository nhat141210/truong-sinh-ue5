# Hợp đồng sandbox không hồi kết

## Bất biến

- Save đại diện cho world + nguyên thần, không chỉ current body.
- Không có cờ `GameCompleted`, canonical ending hoặc credits bắt buộc.
- Cái chết thân xác không xóa save. Nếu đoạt xá thất bại và không còn vessel, `WanderingSoul` cung cấp đường tiếp tục deterministic.
- Phi thăng tương lai là chuyển `WorldLayerId`, không kết thúc simulation.
- Simulation là authority duy nhất; presentation không được mutate result.
- Activity sau xác nhận không yêu cầu input phản xạ.
- Không progression khi game đóng.

## Quy tắc phạm vi

- Bản đầu chỉ bốn zone, một zone loaded tại một thời điểm.
- Không action combat, seamless open world, free climbing, bơi lặn sâu, ngự kiếm điều khiển tự do, multiplayer hoặc runtime LLM.
- Activity mới phải dùng `FActivityPlan → FAutoResolutionResult → FPresentationReplayPlan`.
- Không thêm profession/framework riêng nếu resolver + data hiện có biểu diễn được.
- Content mở rộng theo một family/zone/cảnh giới mỗi gate, không mass-import.

## Chất lượng thay vì quy mô

Tham khảo game khác để học bố cục, nhịp khám phá, hình ảnh và cảm giác sống; không dùng chúng làm hợp đồng parity, không sao chép asset, text, map, UI, story hoặc data. Mỗi capability của dự án có test/acceptance riêng trong `05_EXPERIENCE_CAPABILITY_MATRIX.md`.

## Definition of release đầu

- Người chơi hoàn thành được demo 3–5 giờ và tiếp tục sandbox sau content authored.
- Cả con đường trường thọ và đoạt xá đều hợp lệ.
- Auto activity phản ánh build/chuẩn bị và không reroll qua save/load.
- Bốn zone đạt visual/performance gate trên RTX 3060.
- Windows package chạy offline trên máy sạch không cài Epic Launcher.
