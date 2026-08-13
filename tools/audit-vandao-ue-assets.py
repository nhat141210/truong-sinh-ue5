"""Print imported Vấn Đạo Tông asset bounds for scale validation."""

import unreal

PATHS = (
    "/Game/TruongSinh/Environment/VanDaoTong/Architecture/SM_VDT_MountainGate.SM_VDT_MountainGate",
    "/Game/TruongSinh/Environment/VanDaoTong/Architecture/SM_VDT_MainHallFacade.SM_VDT_MainHallFacade",
    "/Game/TruongSinh/Environment/VanDaoTong/Architecture/SM_VDT_Pavilion_Hero.SM_VDT_Pavilion_Hero",
)
for path in PATHS:
    asset = unreal.load_asset(path)
    if not asset:
        raise RuntimeError(f"Missing asset: {path}")
    bounds = asset.get_bounding_box()
    size = bounds.max - bounds.min
    slots = [str(item.get_editor_property("material_slot_name")) for item in asset.get_editor_property("static_materials")]
    unreal.log(f"VDT BOUNDS {path}: min={bounds.min}; max={bounds.max}; size={size}; slots={slots}")
