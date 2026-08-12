# Game design document — Trường Sinh UE5

## 1. Trải nghiệm cốt lõi

Người chơi vào vai một tu sĩ vô danh tại **Vạn Cảnh Giới**. Họ không được hứa hẹn chiến thắng nhanh; lựa chọn mỗi ngày quyết định tuổi thọ, quan hệ, cơ duyên và con đường đến phi thăng. Thế giới 3D phải làm cho những lựa chọn hệ thống có hình hài: động phủ có thật, NPC có mặt ở nơi hợp lý, thiên kiếp thay đổi bầu trời, còn đấu pháp là một nghi thức căng thẳng trong arena.

Mọi lore, nhân vật và quest là nội dung gốc. Các hệ thống được benchmark theo loại quyết định/độ sâu quan sát được của game tham chiếu, theo hợp đồng parity.

## 2. Pillar thiết kế

| Pillar | Quy tắc triển khai |
| --- | --- |
| Một đời tu sĩ | Lịch, tuổi thọ và thời gian khiến mọi action có giá trị; không grind thời gian thực để tăng chỉ số. |
| Build có hệ quả | Linh căn, đạo, công pháp, pháp bảo và đan dược có synergies/trade-off đọc được. |
| Thế giới phản hồi | Quest, NPC, market, sect và encounter nhận event từ cùng world simulation. |
| 3D phục vụ luật | Khám phá/tương tác tạo action; camera/VFX không tự quyết damage, quest hay time cost. |
| Tự do có đường | Tông môn và tán tu đều hoàn chỉnh, có rủi ro/lợi thế khác nhau; không có route giả. |

## 3. Vòng lặp và nhịp chơi

### Vòng chiến lược dài hạn

```text
Mục tiêu cảnh giới / mục tiêu cá nhân
→ chọn route: tông môn hoặc tán tu
→ kiếm resource, học công pháp và lập build
→ tu luyện / chế tạo / động phủ / relation
→ đối phó deadline, market, rival, encounter
→ đột phá hoặc thay đổi chiến lược
→ mở region, facility, quest và lớp cảnh giới mới
```

### Vòng session 10–20 phút

```text
Load zone 3D
→ nhận thông tin / quan sát opportunity
→ chọn action có cost preview
→ combat, crafting, dialogue hoặc travel resolve
→ world simulation advance
→ đọc hậu quả, save checkpoint và chọn mục tiêu tiếp
```

### Quy tắc thời gian

- `LocalExplore`, camera, nói chuyện không-confirm và mở menu không tự advance ngày.
- `TravelLink`, `Cultivate`, `Retreat`, `Craft`, `Rest`, `QuestCommit`, `TradeRoute` và event có thể resolve là action có `TimeCost` audit được.
- Một transaction chỉ advance calendar một lần. Nếu user hủy trước commit, state/time không đổi.
- Sau `AdvanceTime`, scheduler cập nhật NPC, production, deadline, market, encounter và age theo thứ tự canonical.

## 4. Loop khám phá 3D

### Open-zone

Mỗi zone là một không gian có thể đi bộ, tương tác và quay lại; zone không phải mô phỏng toàn thế giới. Bản đồ thế giới giữ graph các `LocationLink` và quyết định time/risk cost khi chuyển vùng.

| Loại zone | Vai trò gameplay | Yêu cầu presentation |
| --- | --- | --- |
| Thành thị | market, NPC, quest, auction, social | đông vừa phải, readable POI, interior trọng yếu |
| Tông môn | membership, contribution, master, training | hierarchy rõ, bảng nhiệm vụ, facility chuyên môn |
| Hoang dã | resource, encounter, travel risk | đường chính + landmark + spawn có seed |
| Động phủ | build/home, retreat, farming/crafting | persistent layout phản ánh save |
| Bí cảnh | challenge có lifecycle | state instance, biến mất/khôi phục theo simulation |
| Arena | combat presentation | camera, turn UI, VFX nhưng state đọc từ snapshot |
| Thiên kiếp | progression milestone | trời/âm thanh/VFX theo event log, không đổi result |

### Tương tác

Actor 3D chỉ expose `FTruongSinhInteractionOffer`: candidate ID, String Table key,
trạng thái, ưu tiên và range. Khi người chơi chọn,
`ITruongSinhInteractionProvider` tạo `FTruongSinhActionCommand`; composition root
gửi command sang simulation canonical. Action bị từ chối trả `ReasonId` để UI tra
String Table. Actor không tự trừ tài nguyên, tiến thời gian hoặc đổi quest state.

Không có reward trực tiếp trong Blueprint interaction, pickup actor hay animation notify.

## 5. Con đường người chơi

### Tông môn

- Gia nhập yêu cầu data-driven condition và tạo `SectMembershipState`.
- Nhận nhiệm vụ, quyền truy cập facility/truyền thừa, contribution và nghĩa vụ.
- Rank mở content theo audited rules; rời/vi phạm tạo hậu quả bằng world event, không chỉ ẩn UI.

### Tán tu

- Không bị ép gia nhập tông môn để hoàn thành full run.
- Tìm resource, trade route, mentor, bí cảnh, temporary alliance và động phủ riêng.
- Rủi ro, price, encounter và access phải khác thật với sect route trong data/world simulation.

### Động phủ

- Là home zone persistent, không chỉ một menu.
- Facility và linh điền thay đổi production/cultivation modifier ở simulation tick.
- Placement decor 3D có thể có, nhưng slot/facility effect canonical nằm trong `HomeState`; không lấy transform làm điều kiện luật.

## 6. Tiến trình

```text
Khởi đầu
→ học cách lấy tài nguyên và tu luyện
→ chọn affiliation
→ xây build công pháp + pháp bảo + đan dược
→ mở động phủ / crafting / social network
→ vượt cảnh giới, zone và quest layer
→ đối mặt lifespan / rival / tribulation
→ hoàn thành điều kiện phi thăng
```

Cảnh giới, công pháp, reward và content scale cụ thể chỉ được nhập sau audit và catalog gốc. Không hard-code một cấp/công thức không có evidence.

## 7. Combat trong game flow

Combat là một `GameplayAction` tạo snapshot tại thời điểm encounter commit. Kết quả combat có thể mở quest, tiêu resource, thay relation, chết NPC hoặc advance event. Arena 3D phát replay log nhưng không có quyền thay combat state.

Người chơi cần hiểu trước mỗi command:

- Resource nào bị tiêu/tạo.
- Mục tiêu, điều kiện và outcome dự kiến.
- Status/cooldown/turn order nào bị ảnh hưởng.
- Tác động có thể có sau combat: reward, injury, escape, relation hoặc quest.

Chi tiết engine nằm ở [08_COMBAT_SPEC.md](08_COMBAT_SPEC.md).

## 8. Fiction gốc cho vertical slice

Các entry sau chỉ là internal seed content; không phải content benchmark:

| ID | Nội dung gốc | Vai trò |
| --- | --- | --- |
| `ts.region.yun_khe` | Thị trấn Vân Khê bên hồ mù | zone khởi đầu, market/quest |
| `ts.sect.thanh_luu` | Thanh Lưu Môn, tông môn giữ cổ thủy đạo | sect route đầu |
| `ts.wilds.lac_van` | Lạc Vân Lĩnh có mạch đá linh | exploration/resource |
| `ts.home.thach_tinh` | Động Thạch Tĩnh | home/retreat sample |
| `ts.npc.nguyen_lam` | Người dẫn đường độc lập | NPC tutorial/rogue hook |
| `ts.event.thien_vu` | Mưa sao linh lực bất thường | event/tribulation presentation sample |

Tên, mục tiêu quest, dialogue, visual và reward của các entry này được author riêng trong content catalog.

## 9. UI flow tối thiểu

```text
Main Menu → New Game / Continue
New Game → Character Setup → First Zone
HUD → Interaction Offer → Action Preview → Confirm/Cancel
Action Result → World/Event/Combat/Reward → HUD
HUD → Character / Techniques / Inventory / Home / Sect / Quest / Map / Calendar
Combat Enter → Turn UI → Replay → Result → World commit
Pause → Settings / Save / Exit
```

Mỗi màn hình là CommonUI layer, có back/cancel behavior thống nhất và không giữ state gameplay riêng. UI spec chi tiết sẽ nằm trong `10_UI_UX_SPEC.md`.

## 10. Không làm trong critical path

- Không thêm realtime action combat, gacha, PvP, social online hay daily login.
- Không dùng “đi bộ lâu” làm chi phí progression.
- Không dùng cinematic kéo dài để che loading/combat calculation.
- Không có NPC quan trọng chỉ tồn tại nhờ level actor; save phải tái tạo được họ từ simulation.
