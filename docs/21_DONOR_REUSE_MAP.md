# Donor reuse map

Donor là tài liệu/thử nghiệm local trong `_external/`, không phải runtime dependency mặc định. Chỉ clone sau native M1 build pass và chỉ khi một task có acceptance rõ.

| Donor | Có thể học/tái sử dụng sau review | Không mang vào |
|---|---|---|
| alis | inventory/save/settings/dialogue patterns | project architecture nguyên khối |
| cultivation-world-simulator | NPC lifecycle, realm/time simulation ideas | web runtime/content/công thức chưa duyệt |
| Game Animation Sample (Epic) | locomotion và mantle có đánh dấu | action combat hoặc free climbing |
| monolith | MCP fallback nếu native MCP thiếu | public server/multiple writers |

Chỉ giữ hai donor Git nền (`alis`, `cultivation-world-simulator`) và `monolith` tùy chọn cho editor tooling. Mọi import phải compile UE5.8 riêng, có license record, test và adapter hẹp. Không donor nào được phép kéo action combat, multiplayer, seamless world hoặc framework nghề nghiệp riêng vào project.
