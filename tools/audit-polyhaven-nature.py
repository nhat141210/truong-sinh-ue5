"""Audit imported Poly Haven nature assets in Unreal Engine 5.8.

Run headless after import:
  UnrealEditor-Cmd.exe TruongSinhUE5.uproject \
    -ExecutePythonScript=tools/audit-polyhaven-nature.py -unattended -NoSplash

The audit is read-only. It reports exact object paths, mesh bounds, LOD/geometry
counts, Nanite settings, and imported normal-texture convention evidence.
"""

import unreal


ROOT = "/Game/VisualTarget/Nature"


def property_or(obj, name, fallback="UNAVAILABLE"):
    try:
        return obj.get_editor_property(name)
    except Exception:
        return fallback


asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
assets = asset_registry.get_assets_by_path(ROOT, recursive=True)
mesh_assets = []
normal_assets = []

for asset_data in assets:
    asset = asset_data.get_asset()
    if isinstance(asset, unreal.StaticMesh):
        mesh_assets.append((str(asset_data.package_name), asset))
    elif isinstance(asset, unreal.Texture2D) and "nor_gl" in str(asset_data.asset_name).lower():
        normal_assets.append((str(asset_data.package_name), asset))

if not mesh_assets:
    raise RuntimeError(f"No StaticMesh assets found under {ROOT}")

mesh_subsystem = unreal.get_editor_subsystem(unreal.StaticMeshEditorSubsystem)
unreal.log(f"POLYHAVEN_NATURE_AUDIT meshes={len(mesh_assets)} normals={len(normal_assets)}")

for package_name, mesh in sorted(mesh_assets, key=lambda item: item[0]):
    bounds = mesh.get_bounding_box()
    size = bounds.max - bounds.min
    lod_count = mesh.get_num_lods()
    lod_metrics = []
    for lod_index in range(lod_count):
        mesh_description = None
        try:
            mesh_description = mesh.get_mesh_description(lod_index)
        except Exception:
            pass
        try:
            vertices = mesh_description.get_vertex_count() if mesh_description else mesh_subsystem.get_number_verts(mesh, lod_index)
        except Exception:
            vertices = "UNAVAILABLE"
        try:
            triangles = mesh_description.get_triangle_count() if mesh_description else "UNAVAILABLE"
        except Exception:
            triangles = "UNAVAILABLE"
        lod_metrics.append(f"LOD{lod_index}:v={vertices},t={triangles}")

    nanite = property_or(mesh, "nanite_settings")
    nanite_enabled = property_or(nanite, "enabled") if nanite != "UNAVAILABLE" else "UNAVAILABLE"
    unreal.log(
        "MESH path={path}.{name} size_cm=({x:.2f},{y:.2f},{z:.2f}) "
        "lods={lods} nanite={nanite} geometry=[{geometry}]".format(
            path=package_name,
            name=mesh.get_name(),
            x=size.x,
            y=size.y,
            z=size.z,
            lods=lod_count,
            nanite=nanite_enabled,
            geometry=";".join(lod_metrics),
        )
    )

if not normal_assets:
    raise RuntimeError("No imported *_nor_gl Texture2D assets found; normal convention cannot be audited")

for package_name, texture in sorted(normal_assets, key=lambda item: item[0]):
    compression = property_or(texture, "compression_settings")
    srgb = property_or(texture, "srgb")
    flip_green = property_or(texture, "flip_green_channel")
    if compression != unreal.TextureCompressionSettings.TC_NORMALMAP:
        raise RuntimeError(f"Normal texture does not use TC_NORMALMAP: {package_name}")
    if srgb is not False:
        raise RuntimeError(f"Normal texture has sRGB enabled: {package_name}")
    if flip_green is not True:
        raise RuntimeError(
            f"OpenGL normal was not converted for Unreal's DirectX convention: {package_name}"
        )
    unreal.log(
        "NORMAL path={path}.{name} source_convention=OpenGL "
        "compression={compression} srgb={srgb} flip_green={flip}".format(
            path=package_name,
            name=texture.get_name(),
            compression=compression,
            srgb=srgb,
            flip=flip_green,
        )
    )

unreal.log("Poly Haven nature quality audit PASS")
