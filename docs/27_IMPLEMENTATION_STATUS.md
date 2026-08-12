# Trạng thái triển khai

## Cập nhật 2026-08-12 — Windows UE 5.8.1

M1 Windows Native Foundation đã đạt build, automation, map package và Standalone smoke. M2A Golden Loop tối thiểu đã có đường đi `walk → interact → plan → resolve → commit → summary → autosave/Continue`, nhưng world hiện vẫn là smoke composition; chưa phải visual target hoặc bản phát hành cuối.

## Đã có bằng chứng Windows

- UE 5.8.1 changelist `56057345`; VS 2022 17.14.37; MSVC 14.44.35207; Windows SDK 10.0.26100.0.
- UBT `TruongSinhUE5Editor Win64 Development` và `TruongSinhUE5 Win64 Development` pass.
- Automation report: 12/12 Success, 0 warning/fail/not-run; gồm mouse-capture defaults và deterministic interaction selection.
- Map thật `/Game/Maps/Dev/L_Dev_Smoke` có floor collision, PlayerStart, directional/sky/atmosphere/fog và cultivation interaction site.
- Enhanced Input native: WASD, mouse look, Space, E và Esc; PlayerController quản lý mouse capture/lock và pause cursor.
- Native HUD hiển thị canonical realm/cultivation/time/revision, contextual E prompt, result summary và pause overlay.
- Cultivation resolver xuất deterministic outcome/progress/replay ID; simulation commit time + cultivation trong một revision.
- Duplicate command không cộng thưởng hai lần; save v2 giữ pending replay; Continue hiển thị summary rồi clear marker mà không re-apply reward.
- Autosave ghi `.tmp`, đọc lại và xác minh hash, rotate `.bak`, rồi promote current.
- `ResavePackages -Verify` cho smoke map: 0 errors, 0 warnings.
- Standalone Editor `-game` load map, đưa world lên play và khởi tạo deterministic sandbox không crash.
- Windows Development portable package đã cook/stage/archive thành `.pak` + IoStore và chạy bằng executable độc lập; runtime log không có Error/Fatal/Ensure/Assertion.
- Manny chính thức từ UE 5.8.1 Characters template đã hiển thị third-person với idle/jog/fall; runtime D3D screenshot xác nhận mesh/camera.
- M2B có map riêng `/Game/Maps/VisualTarget/L_M2B_Corridor`: gate, corridor, water, ánh sáng/sương, caretaker tạm và cultivation site. Map qua structural validation, ResavePackages và được nạp bằng Development executable độc lập.
- HUD native đã dùng frame ngọc/vàng có provenance rõ ràng; `realm.mortal` hiển thị là `PHÀM NHÂN` thay vì stable ID kỹ thuật.
- M3 có lát cắt đột phá Trúc Cơ chạy qua chung pipeline activity: cần 800 tu vi, resolve deterministic, commit realm + thọ nguyên, autosave/pending replay và HUD result. Payload có precondition realm canonical nên không thể lặp lại để farm thọ nguyên.
- Activity registry Data Asset `/Game/Data/DA_ActivityRegistry` giờ là nguồn setup cho cultivation, breakthrough và alchemy. Bản Windows đã cook/load registry với 3 definitions; interaction không dùng fallback hard-code nếu registry lỗi/mất.
- Luyện đan Thanh Tâm có furnace tương tác trong hai map dev, resolve deterministic và lưu output pill với quantity, quality, impurity trong canonical ledger. Ledger được hash/save/load; retry command không thể tạo output lần hai.

## Chưa được gọi là hoàn thành cuối

- M2B corridor hiện vẫn dùng Engine primitives và material nền tảng; đây là bố cục/testable visual foundation, chưa phải corridor/NPC/cinematic/material/environment production.
- Manny chỉ là visual foundation có provenance, chưa phải nhân vật tu tiên production; audio/VFX production chưa có.
- PIE thao tác tay, performance capture RTX 3060, packaged Windows clean-machine và Shipping chưa qua gate.
- Dev save migration cũ chưa triển khai; fallback lỗi save cần UI tiếng Việt hoàn chỉnh hơn.

## Mốc

| Mốc | Trạng thái | Bằng chứng/gate còn lại |
|---|---|---|
| M0 Product reset | COMPLETE | endless sandbox source/docs |
| M1 Windows native | COMPLETE (smoke evidence) | packaged build không thuộc M1 |
| M2A Golden loop | IMPLEMENTED, RUNTIME SMOKE | cần manual interaction/Continue capture để đóng gate UX |
| M2B Visual target | FOUNDATION VERIFIED | corridor loadable + temporary NPC/cultivation site + HUD frame; còn production art, activity cue, RTX3060 profile |
| M3 Activity framework | SLICE PASS | registry Data Asset + cultivation/breakthrough/alchemy chạy chung pipeline; còn formation, conflict runtime |
| M4 Living world | NOT STARTED | canonical NPC/event/social/economy |
| M5 Lifespan/soul | SOURCE PROOF | gameplay content/UI/save migration |
| M6 Demo | NOT STARTED | four zones và authored content |

## Việc tiếp theo

1. Mở PIE, đi bộ/nhìn/nhảy/pause và thực hiện một cultivation interaction bằng E.
2. Đóng/mở Standalone, xác nhận Continue hiện summary và tu vi không cộng hai lần.
3. Thêm registry Data Asset và các resolver alchemy/formation/conflict vào pipeline chung trước khi mở rộng world/NPC; đồng thời thay primitive/Manny tạm bằng art production đã duyệt.
