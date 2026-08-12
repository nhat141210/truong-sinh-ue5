# Bản đồ tái sử dụng donor

## Nguyên tắc

Donor là tài liệu kỹ thuật hoặc nguồn để đánh giá và chọn lọc module, không phải một game được reskin. Project UE5.8 sạch là nguồn sự thật. Chỉ nhập phần đã qua build/test gate, có integration record và có owner. Không để repo donor thành submodule, không commit nguyên cây _external, không phụ thuộc runtime vào đường dẫn donor.

Các commit bên dưới là khóa đầu vào dự kiến. Script bootstrap phải kiểm tra commit tồn tại sau clone; nếu khác, dừng và ghi mismatch vào decision log, không tự đổi commit.

| Donor | URL | Commit khóa | Giá trị chính | Cách dùng | Không dùng làm |
|---|---|---|---|---|---|
| ALIS | fallintodusk/alis | 02eb63bb32e0d3585e1b0e8f1cda8c320da30906 | Inventory, interaction, save/settings, dialogue, JSON/data pipeline | Đọc kiến trúc, port chọn lọc sang module dự án hoặc thử plugin trên branch riêng | Project gốc, source data tu tiên, asset final |
| Cultivation World Simulator | 4thfever/cultivation-world-simulator | b87a8c81f2d399374157033ae9de69b72fc9253e | Realm, NPC, sect, tuổi thọ, vật phẩm, world simulation | Chỉ quan sát pattern/mô hình ở gate hiện tại; tự viết definition/test từ requirement độc lập | Code/data donor hoặc backend UE; commit khóa hiện có license phi thương mại |
| SimpleQuest | TheGeebus/SimpleQuest | 882be0eaca9251e787707c03ce53669ab54a81d6 | Quest graph, điều kiện, progression | Compile thử UE5.8 rồi import/adapt có chọn lọc | Nơi lưu toàn bộ simulation/content canonical |
| SoulGame | X4V13R95/SoulGame | aed0f39cad83123d73e9b70f97aa4bd259f28151 | Ví dụ GAS, CommonUI, animation/traversal | Tham khảo integration 3D sau core contract | Base repo V1 hoặc multiplayer stack |
| Monolith | tumourlove/monolith | e67544caa9e11569e87c1a4f616568544822d8b6 | MCP editor mở rộng | Fallback sau MCP native UE5.8 | Dependency bắt buộc khi build/chạy |

## Vị trí clone Windows

D:\GameDev\truong-sinh-ue5\_external\
  alis\
  cultivation-world-simulator\
  simplequest\
  soulgame\                 chỉ khi bật IncludeFuture3D
  monolith\                  chỉ khi native MCP thiếu chức năng

External phải bị Git ignore. CWS dùng sparse checkout: src/classes, src/config, src/sim, src/systems, tests, docs/specs. Không clone/migrate frontend nếu chưa có task riêng.

## Cổng nhập donor

1. Ghi DONOR-### vào decision log: module, commit, tác động, rollback.
2. Ghi license/SPDX, file/path áp dụng và compatibility decision. Không import code,
   data hoặc asset khi quyền chưa tương thích mục tiêu Windows thương mại.
3. Tạo branch thử nghiệm; không sửa main trực tiếp.
4. Kiểm tra UE5.8 build, PIE, package Development và automation test.
5. Chuyển API/content được phép sang schema canonical; không giữ path relative donor.
6. Thêm ít nhất một automation test và một manual smoke test.
7. Chỉ merge khi chứng minh module không làm simulation phụ thuộc widget, animation, actor level hay FPS.

Nếu cổng fail, giữ donor ngoài project, ghi nguyên nhân và viết implementation native. Không để plugin compile lỗi chặn M1.

## Thứ tự đánh giá

| Thứ tự | Việc | Quyết định đầu ra |
|---:|---|---|
| 1 | Native CommonUI, SaveGame, Gameplay Tags, GAS, Enhanced Input | Nền bắt buộc, không donor dependency |
| 2 | ALIS: data import, save/settings, inventory | Port/adapt từng hệ thống nếu vượt native baseline |
| 3 | CWS: lifecycle, realm, NPC/world event model | Viết simulator C++ deterministic riêng |
| 4 | SimpleQuest | Dùng plugin hoặc viết adapter tối thiểu |
| 5 | Game Animation Sample/SoulGame | Chỉ nhập presentation sau core contract |
| 6 | Monolith | Bật nếu Unreal MCP native thiếu thao tác editor cần thiết |

## MCP: native trước, donor sau

UE5.8 Editor MCP là bridge chính: agent kết nối qua stdio/localhost tới Editor trên Windows. Chỉ bind loopback, không mở port Internet/LAN. Bắt đầu read-only: project state, log, scene/asset inspection. Trước mỗi MCP write phải có checkpoint Git sạch.

Monolith chỉ là fallback. Nếu dùng, ghi port, version, toolsets và cách tắt trong decision log; firewall Windows phải chặn mọi kết nối không phải loopback. MCP không phải source of truth: C++, Blueprint và Data Asset sau commit mới là nguồn sự thật.

## Integration record tối thiểu

DONOR-###
Nguồn/commit:
Phần được chuyển:
Module dự án nhận:
Dependency mới:
API công khai:
Automation test:
Manual smoke test:
Rollback:
Trạng thái:
