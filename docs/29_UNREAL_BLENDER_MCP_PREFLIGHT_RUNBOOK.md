# Unreal, Blender, and MCP Preflight Runbook

## Purpose

This is the required production path for visual work in UE 5.8.1. It makes an
asset pass a small, repeatable contract before it can be placed in a production
level. It prevents scale, material, lighting, and editor-state failures from
being discovered after a large scene edit.

This runbook does not change canonical simulation, save data, or the native
smoke map. Visual presentation remains downstream of the committed result.

## Tool Ownership

| Tool | Use it for | Do not use it for |
| --- | --- | --- |
| Blender 5.2 headless | Original-model generation, FBX conversion, source re-import audit | Editing a `.umap` or producing unverifiable binary assets |
| `UnrealEditor-Cmd` + tracked Python | Deterministic import, sandbox validation, map construction, asset validation | Parallel writer processes or interactive camera review |
| Unreal MCP | Editor readiness, load/inspect scene, deterministic viewport capture | One-off map mutations or bulk import |
| `build-windows.ps1`, `run-tests.ps1`, `package-windows.ps1` | Native/build/package gates after the editor is closed | Running while Editor or Live Coding owns the DLLs |

Only one Unreal writer process may run at a time. Start a build, cook, package,
or commandlet only after `UnrealEditor` and `UnrealEditor-Cmd` have exited.

## Source and Unit Contract

1. Store the immutable source under `SourceArt/` with a provenance manifest and
   SHA-256 for every file that is imported.
2. Blender authored geometry uses metres: one Blender unit is one metre.
3. UE imports FBX at `ImportUniformScale = 1.0`; UE centimetres are produced by
   FBX unit conversion, never by an actor-scale workaround.
4. A mesh must have named material slots, collision policy, and intended bounds
   documented before it enters `/Game/TruongSinh/Environment/`.
5. Any legacy centimetre-number FBX is kept immutable and converted into a
   separately hashed `Normalized/` derivative using
   `tools/normalize-fbx-metre-contract.py`.

The proven example is `SM_VDT_StoneStele`: source normalised by 0.01, UE import
at scale 1.0, expected bounds 100 x 60 x 280 cm. UE5.8 actual bounds were
100 x 54 x 252 cm (ratios 1.0, 0.9, 0.9), inside the 30% authored tolerance,
with three material slots. This is recorded in `Saved/Logs/TruongSinhUE5.log`.

## Asset Gate: Run Before Map Authoring

### A. Blender source audit

```powershell
Set-Location D:\GameDev\truong-sinh-ue5
& 'C:\Program Files\Blender Foundation\Blender 5.2\blender.exe' --background --python tools\create-vandao-props.py
```

Run the generator's re-import/audit step and calculate the manifest hashes. A
failure means the asset is not eligible for import.

### B. Normalise an old source only when the source contract says it is needed

```powershell
& 'C:\Program Files\Blender Foundation\Blender 5.2\blender.exe' --background --python tools\normalize-fbx-metre-contract.py -- `
  --input SourceArt\Architecture\VanDaoProps\FBX\SM_VDT_StoneStele.fbx `
  --output SourceArt\Architecture\VanDaoProps\Normalized\SM_VDT_StoneStele.fbx
```

Never overwrite the provenance original. Record the derivative hash.

### C. UE isolated import contract

```powershell
$ue = 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
& $ue D:\GameDev\truong-sinh-ue5\TruongSinhUE5.uproject `
  -run=pythonscript `
  -script=D:\GameDev\truong-sinh-ue5\tools\preflight-vandao-import.py `
  -unattended -nop4 -nosplash
```

The preflight imports only to `/Game/External/VDTPreflight`. It must assert:

- expected source path and SHA-256;
- mesh existence;
- uniform import scale 1.0;
- bounds within the declared tolerance;
- authored material-slot count;
- asset saved successfully.

No asset may be referenced from `L_VanDaoTong` until this command exits zero and
the `VDT IMPORT PREFLIGHT PASS` line is present in the log.

### D. Map authoring and validation

The complete Vấn Đạo Tông rebuild is one tracked entry point:

```powershell
.\tools\rebuild-vandao-tong.ps1
```

The wrapper runs each preflight/import/authoring action in its own commandlet.
For the foundation pass it sets the process-local
`TRUONGSINH_VISUAL_MAP_PACKAGE` to `/Game/Maps/VisualTarget/L_VanDaoTong`;
`create-m2b-corridor.py` rejects any target outside `/Game/Maps/VisualTarget`.
This avoids UE5.8's stale Asset Registry behavior after deleting/duplicating a
map package and leaves the native M2B source map untouched.

Then run the structural validator, Asset Validation, and ResavePackages. Map
scripts must use named labels/prefixes so they can replace only generated
actors, and must preserve native gameplay anchors and the smoke map.

## MCP: Connection and Safe Operating Pattern

The project enables Unreal's official Model Context Protocol plugin. The local
editor user settings start its localhost server at `http://127.0.0.1:8000/mcp`.
Those user settings and any Codex local MCP config are machine-local and must
not be committed.

Start a normal Editor, wait for port 8000, then query toolsets through the
tracked bridge:

```powershell
Set-Location D:\GameDev\truong-sinh-ue5
.\tools\invoke-unreal-mcp.ps1 -ToolName list_toolsets
```

`invoke-unreal-mcp.ps1` performs the required JSON-RPC `initialize` handshake
with protocol `2025-11-25`, captures `Mcp-Session-Id`, sends
`notifications/initialized`, calls a tool, then closes the session. For a
toolset command, invoke the official wrapper with its exact registered
toolset/name pair:

```powershell
.\tools\invoke-unreal-mcp.ps1 `
  -ToolsetName 'editor_toolset.toolsets.scene.SceneTools' `
  -ToolName 'load_level' `
  -ArgumentsJson '{"level_path":"/Game/Maps/VisualTarget/L_VanDaoTong"}'
```

Tool names are case-sensitive and toolset-local. Discover them first with
`list_toolsets` and `describe_toolset`; do not guess argument casing from
third-party MCP examples. After `load_level`, call `get_current_level` and
verify `L_VanDaoTong` before any capture.

MCP is restricted to these review operations for this project:

- load a known level;
- query official toolset schemas;
- capture a named, fixed camera transform;
- read actor/asset state for audit.

All source-of-truth edits remain tracked UE Python scripts. This leaves each
change reviewable and avoids an unreproducible editor session.

### Persistent Editor iteration (default)

Do not restart Unreal for routine scene, camera, lighting, or material work.
Start or reuse one project Editor and leave it open:

```powershell
.\tools\ensure-unreal-session.ps1
```

The machine-local `Saved/Config/WindowsEditor/Engine.ini` enables Epic's
Python Remote Execution on loopback only. Run a tracked authoring or audit
script inside that existing Editor without another startup:

```powershell
python .\tools\invoke-unreal-python-remote.py `
  --script .\tools\audit-vandao-map-actors.py
```

The bridge fails if it cannot identify exactly one matching project node; it
never chooses arbitrarily between multiple Editors. Continue to use MCP for
level loading, state queries, fixed-camera review, and viewport capture.

Close the persistent Editor only at an explicit exclusive gate: native C++
build/test, clean commandlet import/resave that cannot safely run in-process,
or cook/package. Reopen once after the gate and resume the same session. The
full `rebuild-vandao-tong.ps1` commandlet pipeline remains the deterministic
clean-rebuild gate, not the normal per-edit loop.

### Primary references

- Epic's [Unreal MCP guide](https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor?application_version=5.8): server defaults, tool-search mode, serial game-thread calls, and its experimental status.
- Epic's [FBX import options reference](https://dev.epicgames.com/documentation/en-us/unreal-engine/fbx-import-options-reference-in-unreal-engine): unit conversion and `Import Uniform Scale`.
- Epic's [static mesh FBX pipeline](https://dev.epicgames.com/documentation/unreal-engine/fbx-static-mesh-pipeline-in-unreal-engine): material-slot, UV, collision, and FBX-version checks.

## Capture and Quality Gate

For every accepted visual change, capture the same five named transforms at
the same scalability preset and resolution. Review the actual D3D image, not a
NullRHI commandlet result. Check before accepting:

- no BasicShape trees/cone mountains in the chosen shots;
- no geometry occupies the camera or clips the route;
- controlled exposure and readable gate silhouette;
- foliage uses masked/two-sided material and has LOD/Nanite policy;
- rock/building material has a verified UV channel before PBR assignment;
- at most four visible shadow-casting local lights;
- no Lumen surface-cache warnings, shader errors, or missing material errors.

Only after this gate passes do we profile packaged Development on the target
RTX 3060. Only after native tests, map validation, D3D capture, and package
smoke all pass may a milestone be presented as complete.

## Failure Handling

Do not repair a failed asset by resizing actors in the map. Fix the source or
make a separately hashed normalised derivative, rerun the isolated preflight,
and only then regenerate the map. Keep the failed evidence and its reason in
the relevant manifest/evidence note. This makes the next run deterministic
instead of repeating the same trial-and-error.
