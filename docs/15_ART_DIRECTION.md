# 15 — Art direction và visual bible

## Mục tiêu hình ảnh

Trường Sinh là tu tiên 3D bán hiện thực điện ảnh: thiên nhiên có khối lượng, kiến trúc Á Đông có tuổi đời, còn linh khí và công pháp tạo cảm giác siêu nhiên có quy luật. Hình ảnh phải đẹp ngay từ vertical slice nhưng vẫn chạy ở 1080p High trên i5-12400F + RTX 3060.

Đây là art direction gốc. Không sao chép map, asset, biểu tượng, nhân vật, giao diện, screenshot hoặc ngôn ngữ hình ảnh nhận diện của game tham chiếu. Audit reference chỉ dùng để ghi gameplay cần parity; visual production dùng moodboard và asset manifest riêng.

## Ba trụ cột

1. Tĩnh để thấy đạo: khoảng lặng, sương, tiếng gió, texture đá/gỗ/cũ, spatial hierarchy rõ.
2. Linh để thấy lực: linh khí nhìn như dòng/chất liệu có hướng, không là bụi lấp lánh phủ toàn màn hình.
3. Người để thấy hành trình: nhân vật và NPC có silhouette dễ đọc, trang phục/tông phái/cảnh giới thể hiện qua lớp vật liệu, phụ kiện và pose.

## Phong cách và tỷ lệ

- Bán hiện thực, không photoreal cực nặng và không anime phẳng.
- Khuôn mặt/đồ vải đơn giản hóa có chủ ý; material giữ roughness variation và micro detail vừa đủ.
- Kiến trúc có tỷ lệ con người: cửa cao 240–300 cm, hành lang 300–450 cm, bậc 15–18 cm cao / 28–32 cm sâu.
- Landmark lớn có thể phi thực: kiếm bia, thác linh khí, đỉnh núi, cổ thụ; nhưng phải có silhouette đọc từ xa.
- Camera third-person FOV mặc định 80°; chủ thể chiếm 18–28% chiều cao màn hình khi explore, không để môi trường che nhân vật liên tục.

## Màu sắc

### Bảng nền

| Nhóm | Vai trò | Hướng màu |
|---|---|---|
| Đá/sương | không khí, núi, bí cảnh | slate blue, xanh xám lạnh |
| Gỗ/đất | nhà cửa, trấn, động phủ | nâu trà, nâu đất, xanh rêu |
| Kim loại/ngọc | pháp khí, UI vật chất | bạc xỉn, đồng cũ, ngọc trắng/xanh |
| Ánh sáng đời thường | an toàn, sinh hoạt | vàng ấm nhẹ, không cam gắt |
| Bóng đêm | nguy hiểm, chiều sâu | xanh than/tím xám, vẫn còn visibility |

### Ngũ hành

| Hành | Màu chính | Màu phụ | Quy tắc |
|---|---|---|---|
| Kim | trắng bạc | vàng nhạt | sắc, đường thẳng, hạt kim loại |
| Mộc | xanh ngọc | xanh lá trầm | dây, lá, sinh trưởng |
| Thủy | cyan/xanh lam | trắng sương | dòng, giọt, refraction hạn chế |
| Hỏa | đỏ cam | hổ phách | heat distortion có giới hạn |
| Thổ | vàng đất | nâu vàng | bụi, đá, khối nặng |

Không dùng một màu duy nhất làm thông tin. Element luôn có icon, shader pattern và text trên UI.

## Visual hierarchy

Mỗi khung hình explore có ba tầng:

1. Foreground: nhân vật, prompt hoặc prop tương tác, contrast đủ cao.
2. Midground: đường đi, NPC/service, mục tiêu cục bộ.
3. Background: landmark/skyline/sương tạo định hướng.

Mỗi zone phải có một landmark orientation nhìn thấy từ ít nhất hai tuyến chính. Quest marker không được bù cho level thiếu định hướng.

## Visual kits theo zone

### Thành thị

- Đá lát, mái ngói, bảng hiệu giả tưởng, vải, quầy, cây, nước nông.
- Ánh sáng ấm, người đông vừa phải, âm thanh sinh hoạt.
- Không biến thành chợ fantasy lấp lánh; dịch vụ phải đọc được bằng props và silhouette.

### Tông môn

- Cổng, sân đá, bậc núi, pavilion, cờ hiệu/totem gốc, vườn/training yard.
- Trật tự, đối xứng vừa đủ; địa vị thể hiện qua độ cao, vật liệu, mật độ trang trí.
- Màu tông phái do profile data quyết định, không hard-code trong map.

### Hoang dã

- Biome kit: đá, cây, cỏ, nước, sương, thảo dược, dấu vết đường mòn.
- PCG tạo density không làm mất route readability.
- Linh thảo và encounter anchor nổi bật ở cự ly 10–25 m nhưng không dùng glow quá mạnh.

### Động phủ / luyện đan

- Không gian kín, gỗ/đá/gạch và dải ánh sáng hướng vào station.
- Lò đan là focal point; storage, bàn tu luyện, linh điền và trận pháp đọc được bằng hình/âm thanh.
- Trạng thái construction/world simulation thay set dressing qua Data Layer/instance, không thiết kế map cố định.

### Bí cảnh và thiên kiếp

- Bí cảnh dùng một ý niệm thiên nhiên phi thường cho mỗi kit: đá nổi, nước ngược dòng, rừng bị phong hóa, hang tinh thể.
- Thiên kiếp ưu tiên tương phản mây, wind, cột sét và ground decal; không che input/UI bằng flash toàn màn hình.
- Có reduced-flash cue thay thế.

## Asset production policy

- Mọi external asset có entry trong 20_ASSET_AND_REFERENCE_INDEX.md trước khi import.
- Asset source không quyết định layout/gameplay. Asset chỉ map vào visual kit/technical budget đã được duyệt.
- Tạo một master material family và modular kit trước khi tải/import hàng loạt.
- Không tạo một set asset mới cho mỗi quest; ưu tiên material variation, decal, Data Layer và Level Instance.
- Texture master: hero material 2K, standard props 1K–2K, background/foliage 1K. 4K chỉ dùng cho một số hero close-up sau profiling.
- Mọi mesh có naming TS_<Zone>_<Category>_<Name> và metadata visual kit, LOD/Nanite state, source manifest ID.
- Cảnh/asset thử nghiệm ở Content/Dev/ không được tham gia Shipping cook.

## Visual target corridor

Sau golden loop M2A và trước khi mở rộng content, khóa một corridor 60–90 giây ở M2B:

~~~text
Town entry → street turn → sect gate vista → forest path → alchemy interior
→ combat arena reveal → return at dusk
~~~

Corridor phải đại diện cho: outdoor daylight, indoor warm/cool lighting, fog, foliage, character close/medium/far, NPC density, VFX, UI overlay và streaming. Mọi guideline sau đó phải match mức hoàn thiện này.

## Presentation profiles

Mỗi location/ability/character definition trỏ tới profile data:

- FZoneLightingProfile: skylight, fog, cloud/weather, exposure bounds, LUT, ambient sound.
- FCharacterVisualProfile: mesh parts, palette, material parameter set, animation set, importance tier.
- FPresentationCueProfile: Niagara, decal, camera impulse, sound, accessibility fallback.
- FEnvironmentMaterialProfile: master material variant, texture set, wetness/damage/snow option.

Không lưu color, particle count hoặc hard object reference trực tiếp trong gameplay definition.

## Art review gates

Mỗi zone pass review khi:

- Player path và objective đọc được không cần marker liên tục.
- Ở High và Medium, person/NPC/interaction không mất silhouette vì fog, bloom, backlight hoặc VFX.
- Nền không quá chi tiết so với nhân vật/mục tiêu.
- Material có đúng roughness/scale, không thấy tiling rõ ở khoảng cách gameplay.
- Không asset placeholder trên critical path.
- Screenshot daytime, dusk, interior và combat được lưu review metadata.
- GPU/CPU budget tại 19_PERFORMANCE_BUDGET.md pass.

## Những điều không làm

- Không neon đỏ/phát sáng phủ cảnh để thay ánh sáng thật.
- Không photogrammetry 4K phủ tất cả vì mục tiêu RTX 3060.
- Không dùng VFX để che motion/animation lỗi.
- Không camera shake, chromatic aberration hoặc depth of field mạnh làm giảm đọc gameplay.
- Không thêm một phong cách visual mới chỉ vì asset miễn phí không đồng nhất.
