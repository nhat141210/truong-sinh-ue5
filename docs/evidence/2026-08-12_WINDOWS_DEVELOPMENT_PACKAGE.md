# Windows Development package evidence — 2026-08-12

## Gate

- Engine: Unreal Engine 5.8.1, changelist `56057345`.
- Command:

  ```powershell
  pwsh -NoProfile -File .\tools\package-windows.ps1 `
    -ArchiveDirectory 'D:\GameDev\TruongSinhBuilds\Development-20260812-2' `
    -Configuration Development `
    -Clean
  ```

- UAT result: `BUILD SUCCESSFUL`, exit code `0`.
- Cook result: `Success - 0 error(s), 0 warning(s)`.
- BuildCookRun stages completed: build, cook, stage, package and archive.

## Portable archive

- Launcher: `D:\GameDev\TruongSinhBuilds\Development-20260812-2\Windows\TruongSinhUE5.exe`
- Archive size: 1015.01 MiB across 50 files.
- Content containers: one `.pak`, two `.utoc` and two `.ucas` files.
- No `ue.projectstore` is present in the archive.
- VC++ x64 and ARM64 redistributables are included under `Engine\Extras\Redist\en-us`.

SHA-256:

- Launcher: `0F81FC5FDC446771DDC1038E8274DD16A7B8EA228CCFA000533344DAEBB77E67`
- Game binary: `C989C274C5028174A9DD1E5F5C0C9F4A33466F77CA9F23D2099695E32D9D2951`
- Main UTOC: `502D2537104622AB250373D46013481113F2CDCB11DDB49D0B064FB170924AEB`
- Main UCAS: `32F9302214B645D4AAE1460D8DF9A82B8C423830C1555A724C7CA17CBDB96239`

## Packaged runtime smoke

The inner packaged binary was launched for 15 seconds with:

```powershell
TruongSinhUE5.exe -NullRHI -NoSound -Unattended -log
```

It remained alive until the smoke harness stopped it. The packaged log proves:

- `Truong Sinh bootstrap initialized.`
- `Game Engine Initialized.`
- Browse and load of `/Game/Maps/Dev/L_Dev_Smoke`.
- `Bringing World ... up for play`.
- Enhanced Input initialized.
- Mouse capture changed to `CapturePermanently_IncludingInitialMouseDown` and lock to `LockAlways`.
- A new deterministic sandbox initialized.
- `Load map complete /Game/Maps/Dev/L_Dev_Smoke`.
- Zero occurrences of `Error:`, `Fatal error`, `Ensure condition failed` or `Assertion failed` in the smoke log.

Runtime log path (not committed):

`D:\GameDev\TruongSinhBuilds\Development-20260812-2\Windows\TruongSinhUE5\Saved\Logs\TruongSinhUE5.log`

This evidence proves a standalone Windows Development package on the build machine. It does not yet prove Shipping configuration, clean-machine launch, manual interaction UX or RTX 3060 performance.
