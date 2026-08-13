"""Migrate selected packages from an opened Epic sample project.

This script runs in the *source* project's UnrealEditor-Cmd and lets Unreal's
AssetTools resolve the dependency graph. It deliberately refuses a blind whole
Content copy. Edit APPROVED_PACKAGES only after inspecting the downloaded pack.
"""

import os
import unreal


TARGET_CONTENT = os.environ.get("TRUONG_SINH_TARGET_CONTENT")
if not TARGET_CONTENT:
    raise RuntimeError("Set TRUONG_SINH_TARGET_CONTENT to the destination Content directory")
if not os.path.isdir(TARGET_CONTENT):
    raise RuntimeError(f"Destination Content directory does not exist: {TARGET_CONTENT}")

# Filled with exact package paths after the official pack is present locally.
# Keeping this empty prevents accidentally migrating a sample's entire project,
# plugins, maps, game modes or editor-only dependencies.
APPROVED_PACKAGES = tuple(
    item.strip()
    for item in os.environ.get("TRUONG_SINH_APPROVED_PACKAGES", "").split(";")
    if item.strip()
)
if not APPROVED_PACKAGES:
    raise RuntimeError(
        "No packages approved. Set TRUONG_SINH_APPROVED_PACKAGES to semicolon-separated /Game/... paths"
    )

for package in APPROVED_PACKAGES:
    if not package.startswith("/Game/"):
        raise RuntimeError(f"Only /Game packages can be migrated: {package}")

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
result = asset_tools.migrate_packages(list(APPROVED_PACKAGES), TARGET_CONTENT)
if result is False:
    raise RuntimeError("Unreal AssetTools migration failed")

unreal.log(
    f"Environment migration PASS: roots={len(APPROVED_PACKAGES)}, destination={TARGET_CONTENT}"
)
