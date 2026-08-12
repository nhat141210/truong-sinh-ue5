# Asset và design reference index

## Asset manifest bắt buộc

Mỗi asset ngoài dự án ghi: Asset ID, URL gốc, tác giả, license, ngày tải, SHA-256, file đã import, chỉnh sửa, commercial scope và người duyệt. Không import trước khi record tồn tại.

Cho phép asset miễn phí/CC0/commercial-use rõ ràng. Cấm rip, re-upload không nguồn, logo/thương hiệu thật và nội dung proprietary từ game tham khảo.

## Reference notes

Design reference chỉ lưu URL công khai, ngày truy cập, observation và quyết định tự viết. `ReferenceVault/` local-only nếu cần screenshot nghiên cứu; không link/symlink vào Content và không commit media.

## Budget import

- Hero 4K tối đa; environment 2K; prop 1K–2K.
- Một visual kit được duyệt trước khi tải thêm family tương tự.
- Không clone cả Fab/Megascans library.
- Target demo package <25 GB và workspace <180 GB.

## Manifest — UE 5.8.1 Characters visual foundation

- Asset ID: `UE58-CHARACTERS-MANNY-FOUNDATION`.
- Nguồn gốc: Epic Games Launcher, Unreal Engine 5.8.1, `Templates/TemplateResources/High/Characters/Content`.
- Tác giả/chủ sở hữu: Epic Games, Inc.
- License/scope: Unreal Engine EULA đã được người dùng chấp nhận khi cài UE; chỉ dùng trong sản phẩm Unreal Engine này, không phân phối asset độc lập. Cần legal/release review lại trước phát hành thương mại.
- Ngày import: 2026-08-12.
- Chỉnh sửa: không sửa binary; chỉ chọn 18 dependency cần cho Manny + idle/jog/fall. Script tái lập: `tools/import-ue58-mannequin.ps1`.
- Người duyệt: Windows agent theo yêu cầu visual foundation; trạng thái `TEMPORARY`, không phải character art final.

| File import | SHA-256 |
|---|---|
| `Content/Characters/Mannequins/Anims/Unarmed/Jog/MF_Unarmed_Jog_Fwd.uasset` | `0CCBE851F85A6D4CD7C6438C219CB6AF8E281784107B5BE8E9B052E4DF3069CB` |
| `Content/Characters/Mannequins/Anims/Unarmed/Jump/MM_Fall_Loop.uasset` | `408BE742C63ADFFE8C6C1D22442E99EA34A7618CD33C6E78E0EED3D7DB877433` |
| `Content/Characters/Mannequins/Anims/Unarmed/MM_Idle.uasset` | `11EE7FDEB84DB89C1A66785B43B9BFC1415782122B8AC229517F4C5125D9CB0D` |
| `Content/Characters/Mannequins/Materials/M_Mannequin.uasset` | `D7FFE412D906380BD5BD4462F464B2A1FE48FDB79B9077F29BD0F958A782035C` |
| `Content/Characters/Mannequins/Materials/Manny/MI_Manny_01_New.uasset` | `9F1E4ED382304D020B750C81505140FC1DE4D339B0909430ADB5380A6BB89B65` |
| `Content/Characters/Mannequins/Materials/Manny/MI_Manny_02_New.uasset` | `26389DB06F0B7928B487494BEF3320D6B0E6BCCD58193BCCEDD67C2C89132A46` |
| `Content/Characters/Mannequins/Meshes/SK_Mannequin.uasset` | `22029D47455F24471195B26A0E04A1E1389F27D6A924E88C9F46A0F600C9A530` |
| `Content/Characters/Mannequins/Meshes/SKM_Manny_Simple.uasset` | `BE9F011191EE1887B97CD13BCE09886A6066C2FD62C619991070EC142184E1BF` |
| `Content/Characters/Mannequins/Rigs/CR_Mannequin_Body.uasset` | `57F84458A160C35B77F0442D2AF41054E41E021EBBB496C2C76DEA76D1D89580` |
| `Content/Characters/Mannequins/Rigs/PA_Mannequin.uasset` | `C0CEAF003947088FF076555B81A46E5CCB11149C94025EB2A9E74263FBAA0D67` |
| `Content/Characters/Mannequins/Textures/Manny/T_Manny_01_BN.uasset` | `2F623209571ECBD8185B5B463786BE033C3284190B4FC7B249E8D1D2ED995FD8` |
| `Content/Characters/Mannequins/Textures/Manny/T_Manny_01_D.uasset` | `6AE0F4F9908C30ECC9AEDF1EBB442E4983E526A7832EC791542064C7853F9999` |
| `Content/Characters/Mannequins/Textures/Manny/T_Manny_01_MRA.uasset` | `3E42E888AED8AEFFF68DA7D7DDA4F177C5647B19473E4E913F61375C1319ABD8` |
| `Content/Characters/Mannequins/Textures/Manny/T_Manny_02_BN.uasset` | `C9575D21C9E2872B3358C0EFB9E202535CE2EFCE94C338B9D3BD04CB9831E353` |
| `Content/Characters/Mannequins/Textures/Manny/T_Manny_02_D.uasset` | `474728713232C6833F55C041548F555BC62C5C49E74082A5D16E68A5CE488CFE` |
| `Content/Characters/Mannequins/Textures/Manny/T_Manny_02_MRA.uasset` | `6697972EAF2DFE201923DBA6A302F024B0EF4C4E46B4ECF578178717098E8277` |
| `Content/Characters/Mannequins/Textures/Manny/T_Manny_02_N.uasset` | `61A9DB73F72FE2D7D8821B33A79F57F3F11DF9DA9455822392B84B199C78E8DF` |
| `Content/Characters/Mannequins/Textures/Shared/T_UE_Logo_M.uasset` | `34E6FA3414C04A86B7A5287E5569EA9D6E53C319D6E7DBFC8B09DAB5B70611A6` |

## Manifest — generated jade HUD frame

- Asset ID: `GEN-UI-JADE-FRAME-001`.
- Nguồn/tác giả: OpenAI built-in ImageGen, tạo trong Codex ngày 2026-08-12; không dùng ảnh tham chiếu ngoài.
- Intended scope: texture presentation cho Unreal HUD của dự án; cần release/legal review theo điều khoản OpenAI hiện hành trước phát hành thương mại.
- Source: `SourceArt/UI/Generated/T_UI_JadeFrame_Chroma.png`; chroma removal bằng skill helper thành `T_UI_JadeFrame.png` RGBA.
- SHA-256 output alpha: `B5762C106501A2FE3ADD1C97EE0E3303D07E612C8F11AD3DFA43BE7C918570C4`.
- Chỉnh sửa: chỉ xóa nền phẳng `#ff00ff`, soft matte và despill; không compositing hay reference-derived content.
- Prompt chính: khung HUD orthographic bằng ngọc tối và chỉ vàng cổ, họa tiết mây/núi tiết chế, tâm rỗng, không chữ/logo/watermark, nền chroma phẳng.
- Import script: `tools/import-ui-art.py`; trạng thái `M2B FOUNDATION`, chưa phải toàn bộ UI skin final.
