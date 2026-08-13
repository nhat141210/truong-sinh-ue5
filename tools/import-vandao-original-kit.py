"""Import original Blender Vấn Đạo Tông kit assets through UE Interchange."""

import os
import unreal


ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SPECS = (
    ("VanDaoGate/Derived/SM_VDT_MountainGate.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Architecture", "SM_VDT_MountainGate"),
    ("VanDaoHall/VDT_MainHallFacade.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Architecture", "SM_VDT_MainHallFacade"),
    ("VanDaoHall/VDT_RoofEave_Module.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Architecture", "SM_VDT_RoofEave_Module"),
    ("VanDaoHall/VDT_Dougong_Cluster.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Architecture", "SM_VDT_Dougong_Cluster"),
    ("VanDaoHall/VDT_MoonWindowPanel.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Architecture", "SM_VDT_MoonWindowPanel"),
    ("VanDaoProps/Normalized/SM_VDT_StoneStele.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Props", "SM_VDT_StoneStele"),
    ("VanDaoProps/Normalized/SM_VDT_StoneLantern.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Props", "SM_VDT_StoneLantern"),
    ("VanDaoProps/Normalized/SM_VDT_RitualBrazier.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Props", "SM_VDT_RitualBrazier"),
    ("VanDaoProps/Normalized/SM_VDT_BridgeBaluster.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Props", "SM_VDT_BridgeBaluster"),
    ("VanDaoProps/Normalized/SM_VDT_BridgeRailing_400.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Props", "SM_VDT_BridgeRailing_400"),
    ("VanDaoWater/Normalized/SM_VDT_PondShore_Curve_Quarter.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Water", "SM_VDT_PondShore_Curve_Quarter"),
    ("VanDaoWater/Normalized/SM_VDT_ArchedFootbridge_600.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Water", "SM_VDT_ArchedFootbridge_600"),
    ("VanDaoWater/Normalized/SM_VDT_LotusPadCluster_A.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Water", "SM_VDT_LotusPadCluster_A"),
    ("VanDaoWater/Normalized/SM_VDT_SteppingStoneCluster_A.fbx", "/Game/TruongSinh/Environment/VanDaoTong/Water", "SM_VDT_SteppingStoneCluster_A"),
)


for relative, destination, name in SPECS:
    source = os.path.join(ROOT, "SourceArt", "Architecture", relative)
    if not os.path.isfile(source):
        raise RuntimeError(f"Missing original Vấn Đạo Tông source: {source}")
    unreal.EditorAssetLibrary.make_directory(destination)
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", source)
    task.set_editor_property("destination_path", destination)
    task.set_editor_property("destination_name", name)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("save", True)
    options = unreal.FbxImportUI()
    options.set_editor_property("import_mesh", True)
    options.set_editor_property("import_as_skeletal", False)
    options.static_mesh_import_data.set_editor_property("combine_meshes", True)
    options.static_mesh_import_data.set_editor_property("import_uniform_scale", 1.0)
    options.static_mesh_import_data.set_editor_property("generate_lightmap_u_vs", True)
    options.static_mesh_import_data.set_editor_property("auto_generate_collision", True)
    task.set_editor_property("options", options)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    if not task.get_editor_property("imported_object_paths"):
        raise RuntimeError(f"Imported no assets: {relative}")
    unreal.log(f"Imported VDT kit: {relative}")

unreal.log(f"VANDAO original kit import PASS: assets={len(SPECS)}")
