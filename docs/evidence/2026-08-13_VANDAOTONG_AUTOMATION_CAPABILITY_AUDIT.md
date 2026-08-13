# Vấn Đạo Tông automation capability audit

Date: 2026-08-13
Project: `D:/GameDev/truong-sinh-ue5`
Engine: Unreal Engine 5.8.1, CL 56057345

## Result

The Windows agent can operate the project autonomously through the native
Unreal MCP bridge and the repository's deterministic commandlet/build scripts.
The persistent editor session was started with `tools/ensure-unreal-session.ps1`
and kept alive on `127.0.0.1:8000`.

Verified native MCP operations:

- `list_toolsets` returned the Editor, Scene, Actor, Asset, StaticMesh, Material,
  PCG, Niagara, Sequencer, UMG, Slate and Automation toolsets.
- `SceneTools.load_level` and `SceneTools.get_current_level` loaded and verified
  `/Game/Maps/VisualTarget/L_VanDaoTong`.
- `EditorAppToolset.StartPIE`, `CaptureViewport` and `StopPIE` completed on the
  Vấn Đạo Tông map. A post-rebuild PIE capture is stored outside tracked source
  at `Artifacts/VisualTarget/pie-fixed.png`.

The Blender/FBX pipeline is also self-contained: original Vấn Đạo Tông source
assets have reproducible Blender 5.2 generators, manifests, hashes and UE
commandlet import/provenance audits.

## Limitation

UE Python Remote Execution was tested separately and returned `No Unreal Python
node found`. This is not a blocker for the current workflow because native MCP
provides the required map/editor controls and commandlets handle deterministic
bulk generation. It does mean editor-only Python calls should not be treated as
available; scripts must use native MCP or a closed-editor commandlet gate.

## Current gate

The master estate placement fix was rebuilt and verified with a fresh PIE
capture: the central gate, estate structures, green plateau and surrounding
rock masses render without the previous black occlusion.

Final gate evidence for this revision:

- `tools/build-windows.ps1`: Editor and Game Development builds succeeded.
- `tools/run-tests.ps1`: all registered automation tests succeeded.
- `tools/package-windows.ps1`: UAT BuildCookRun succeeded (pak + IoStore +
  prerequisites) at `D:/GameDev/TruongSinhBuilds/VDT-MasterEstate-20260813-3`.
- Packaged executable stayed alive for 20 seconds under `-NullRHI` and loaded
  `L_VanDaoTong`; smoke log contains zero fatal/ensure/assertion/error tokens.
