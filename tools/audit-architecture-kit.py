"""Log exact UE 5.8 LevelPrototyping mesh bounds for authored scaling."""

import unreal


for path in (
    "/Game/LevelPrototyping/Meshes/SM_ChamferCube",
    "/Game/LevelPrototyping/Meshes/SM_Cylinder",
    "/Game/LevelPrototyping/Meshes/SM_QuarterCylinder",
    "/Game/LevelPrototyping/Meshes/SM_QuarterCylinderOuter",
    "/Game/LevelPrototyping/Meshes/SM_Plane",
):
    mesh = unreal.load_asset(path)
    if not isinstance(mesh, unreal.StaticMesh):
        raise RuntimeError(f"Missing architecture mesh {path}")
    bounds = mesh.get_bounding_box()
    size = bounds.max - bounds.min
    unreal.log(f"ARCH_BOUNDS path={path} size_cm=({size.x:.2f},{size.y:.2f},{size.z:.2f})")

unreal.log("Architecture kit bounds audit PASS")
