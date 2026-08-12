# 10 — Đặc tả UI/UX

## Mục đích

UI phải giữ được chiều sâu menu, dữ liệu và lựa chọn theo audit hành vi, đồng thời để người chơi cảm thấy đang sống trong thế giới tu tiên 3D. Mọi hành động thay đổi simulation chỉ được gửi qua command có kiểu rõ ràng. Widget không được tự sửa tiền, tuổi thọ, inventory, quest hoặc trạng thái combat.

Toàn bộ text nhìn thấy bởi người chơi dùng tiếng Việt đơn giản từ String Table. Không chép nguyên văn, ảnh, icon, layout hay nội dung độc quyền của game tham chiếu; audit chỉ ghi hành vi, luồng thao tác và dữ kiện cần kiểm chứng parity.

## Nguyên tắc trải nghiệm

1. Khám phá là góc nhìn thứ ba. Tương tác trực tiếp với NPC, cổng zone, động phủ, lò đan và điểm cơ duyên.
2. Quyết định sâu là UI có ngữ cảnh. Ví dụ nói chuyện với trưởng lão mở hồ sơ tông môn; đứng cạnh lò đan mở panel luyện đan.
3. Không có action mơ hồ. Mỗi nút phải nêu chi phí thời gian, linh thạch, vật phẩm, điều kiện cảnh giới và hậu quả trước khi xác nhận.
4. Chỉ action hợp lệ mới hiển thị là khả dụng. Action chưa hợp lệ vẫn có thể xem lý do khóa.
5. Một action mô phỏng phải có một kết quả rõ: thành công, thất bại, hủy trước khi commit, hoặc bị từ chối với reason code.
6. Combat, cutscene, VFX và UI không được thay đổi kết quả calculation đã chốt.

## Kiến trúc screen stack

CommonUI là chủ quản layer, modal và focus. Không tự tạo nhiều AddToViewport độc lập.

~~~text
GameViewport
├── UTSRootLayout
│   ├── WorldHUDLayer               luôn có khi khám phá
│   ├── ContextPanelLayer           dialogue, interaction, build station
│   ├── PrimaryScreenLayer          map, character, sect, inventory
│   ├── CombatScreenLayer           chỉ trong combat
│   ├── ModalLayer                  xác nhận, chọn target, tooltip pin
│   ├── SystemLayer                 pause, settings, load/save, error
│   └── DebugLayer                  Development/Test, không Shipping
└── UCommonActivatableWidgetStack
~~~

Mọi screen kế thừa UTSActivatableScreen và khai báo:

- InputMode: Explore, UIOnly, Combat, Cinematic hoặc Paused.
- ClosePolicy: BackClosable, ExplicitConfirm hoặc LockedUntilResult.
- RequiredViewModel: snapshot đọc từ subsystem.
- RestoreFocusKey: stable key của control cần focus khi quay lại.

## Input contexts

Enhanced Input tách context, không hard-code phím trong widget:

| Context | Khi dùng | Input chính |
|---|---|---|
| IMC_Explore | Di chuyển trong zone | WASD, chuột, E tương tác, M bản đồ, C nhân vật, I túi |
| IMC_Dialogue | Hội thoại | chọn lựa, tiếp tục, back |
| IMC_Menu | Chức năng quản lý | chuột, phím tắt tab, confirm/cancel |
| IMC_Combat | Chọn hành động turn-based | chọn kỹ năng, target, xác nhận, tăng tốc |
| IMC_Build | Động phủ và linh điền | chọn lô, đặt công trình, xoay, xác nhận |
| IMC_Cinematic | Cutscene/combat replay | skip khi được phép, pause |

UTSInputRouterSubsystem push context cũ vào stack và phục hồi đúng sau modal/transition. Pause không được thay đổi simulation state.

## Luồng UI chính

~~~text
Main Menu
  → New Game → tạo nhân vật/tư chất → intro zone → Explore HUD
  → Continue → load snapshot → Explore HUD hoặc Combat Resume

Explore HUD
  ├── Interact NPC → Dialogue → service/quest/relationship
  ├── Interact station → Alchemy / Refining / Cultivation / Build panel
  ├── Interact gate → Travel confirmation → simulation advance → destination zone
  ├── Open Map → route / travel command
  ├── Open Character → realm / techniques / equipment / dao
  ├── Open Inventory → item / equip / use / split / discard
  ├── Open Sect → contribution / duty / exchange / rank
  └── Encounter → Combat screen → result → Explore HUD
~~~

## Explore HUD

HUD cố định tối thiểu:

- Góc trái trên: tên nhân vật, cảnh giới, tuổi hiện tại / thọ nguyên, trạng thái bất lợi quan trọng.
- Góc phải trên: ngày/tháng/năm game, linh thạch, tên zone, trạng thái tông môn/tán tu.
- Dưới trái: mục tiêu quest đang theo dõi, không quá ba dòng.
- Giữa dưới: interaction prompt gồm động từ, đối tượng, action cost nếu có.
- Dưới phải: hotkey mở Map, Character, Inventory, Sect/DongPhu và pause.
- Không hiển thị minimap bắt buộc. Zone map chỉ mở qua phím/map object nếu parity audit không chứng minh cần minimap.

HUD nhận FWorldHUDViewModel immutable từ presentation subsystem mỗi khi WorldStateRevision thay đổi. Không polling toàn bộ simulation mỗi Tick.

## Màn hình dữ liệu bắt buộc

### Character

Tab: Tổng quan, Cảnh giới, Linh căn, Công pháp, Thần thông, Trang bị, Trạng thái, Quan hệ. Mỗi tab phải cho biết:

- Giá trị hiện tại, giá trị gốc và modifier.
- Điều kiện mở khóa / cảnh giới yêu cầu.
- Nguồn của modifier.
- Hành động có thể làm tiếp theo.

### World Map và Travel

Map hiển thị location node, connection, điều kiện vào, thời gian di chuyển, nguy hiểm và trạng thái đã biết. Travel luôn gọi request:

~~~text
FTravelCommand {
  FromLocationId,
  ToLocationId,
  RouteId,
  ExpectedWorldRevision
}
~~~

Màn hình hiển thị preview trước khi xác nhận. Khi command được `Committed`, UI
khóa nút, gửi progress state sang transition, và chỉ mở destination sau
`FTruongSinhActionResult` cùng travel domain events từ simulation.

### Inventory, trang bị và kho

Item card phải có StableItemInstanceId, definition ID, stack, quality, bind state và provenance hiển thị debug-only. Các thao tác dùng command:

- MoveItem
- SplitStack
- EquipItem
- UnequipItem
- ConsumeItem
- SellItem
- DepositToStorage
- WithdrawFromStorage

Widget không sửa array item trực tiếp. Nếu revision mismatch, UI reload snapshot và thông báo ngắn: Trạng thái đã thay đổi, hãy thử lại.

### Tu luyện, bế quan, đột phá

Panel tách ba bước:

1. Xem điều kiện và dự báo: tài nguyên, thời gian, tỷ lệ hoặc rule đã audit.
2. Chọn input: công pháp, đan dược, thời lượng, hỗ trợ.
3. Xác nhận action mô phỏng.

Kết quả trả về `FTruongSinhActionResult`; TimeDelta, ResourceDeltas và WorldEvents
là typed `FTruongSinhDomainEvent`. Presentation chiếu VFX/camera từ event nhưng
không tính lại.

### Luyện đan / luyện khí

Recipe UI có điều kiện cảnh giới, station, kỹ năng; slot input rõ số lượng; preview output hợp lệ theo rule audit; chi phí thời gian/resource; batch count nếu reference cho phép. Không có mini-game phản xạ nếu parity audit không xác nhận. Animation lò đan chỉ là presentation của command simulation đã giải quyết.

### Tông môn, quest và động phủ

Screen Tông môn gồm Duty, Contribution Exchange, Rank, Relation, Mission Log. Quest card hiển thị deadline lịch game, location, phần thưởng, điều kiện thất bại và state. Động phủ mở từ actor động phủ, hiển thị sơ đồ lô/room, công trình, linh điền, lò, kho và trận pháp. Preview actor chỉ transient; FBuildCommand là con đường duy nhất ghi state.

## Combat UI

Combat HUD không thay simulation:

- Turn owner, phase, số lượt và speed control.
- Pool linh khí/ngũ hành và modifier.
- Hand/deck/skill list theo mô hình combat đã audit.
- Target selector, target validation và action preview.
- Combat log có event ID và replay index để debug parity.
- Auto / Fast Forward chỉ gửi policy cho CombatSimulation; không tự chọn action ở UI.

Trình bày chi tiết tại 18_3D_COMBAT_PRESENTATION.md.

## ViewModel và event contract

Các ViewModel USTRUCT chỉ đọc:

~~~text
FCharacterViewModel
FInventoryViewModel
FWorldMapViewModel
FSectViewModel
FQuestLogViewModel
FCombatViewModel
FActionPreviewViewModel
~~~

Các event công khai:

~~~text
FOnWorldStateRevisionChanged(int64 Revision)
FOnActionCommitted(const FTruongSinhActionResult& Result)
FOnCombatReplayEvent(const FTruongSinhCombatReplayEvent& Event)
FOnQuestStateChanged(FPrimaryAssetId QuestId, EQuestState State)
FOnPresentationStateChanged(ETwitterPresentationState State)
~~~

Chỉ composition facade/presentation adapter phát event. Widget bind/unbind khi
Activated/Deactivated; không giữ raw pointer qua level transition.

## Accessibility, localization và readability

- UI scale: 80%, 100%, 125%, 150%.
- Font phải hỗ trợ đầy đủ dấu tiếng Việt; fallback được test bằng tất cả glyph dùng trong String Table.
- Không truyền trạng thái chỉ bằng màu: ngũ hành có icon, tên và pattern.
- High contrast, giảm flash, giảm camera shake, tắt blur, subtitle size và background opacity.
- Toàn bộ action có keyboard/mouse; controller navigation thêm sau vertical slice nhưng kiến trúc focus phải sẵn.
- Tooltip có thể pin; tooltip quan trọng không biến mất khi rê chuột qua link.
- Mục tiêu: mọi hành động đầu game hiểu được trong 90 giây, không cần đọc wiki.

## Acceptance UI

- Chuyển Explore → UI → Explore phục hồi input/focus không kẹt.
- Mỗi command có preview, pending và result/error state.
- Không widget nào sửa simulation state trực tiếp.
- 1080p, 1440p, UI scale 80–150% không che nút xác nhận/hủy.
- Combat speed/skip cho cùng combat result và replay hash.
- Mọi string player-facing đến từ String Table, không literal C++/Blueprint.
