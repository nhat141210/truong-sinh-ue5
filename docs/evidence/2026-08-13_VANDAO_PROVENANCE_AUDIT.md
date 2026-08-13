# Vấn Đạo Tông Asset Provenance Audit

Date: 2026-08-13  
Gate: read-only Blender FBX re-import; no Unreal Editor writer was used.

Command:

```powershell
.\tools\run-vandao-provenance-audit.ps1
```

Result: `VDT_PROVENANCE_AUDIT_PASS assets=25 hashes=31 blender=5.2.0 LTS`.

The gate verifies the SHA-256 entries for each source `.blend`, production
FBX, normalized FBX, and donor archive in the VanDaoGate, VanDaoHall,
VanDaoIsland, VanDaoProps, VanDaoWater and VVayToyek pavilion manifests. It
re-imports the production and normalized FBXs into a factory-reset Blender
scene, checks that every asset is one static mesh, and validates dimensions and
material-slot counts. Normalized prop/water dimensions are exactly the source
kit dimensions multiplied by 0.01, with Unreal import scale remaining 1.0.

The mountain gate manifest records the measured exported bounds
`18.6068 × 5.1938 × 9.6887 m`; the extra width/depth is the authored eave
overhang, not a transform error. The audit also confirms the derived pavilion
archive and normalized FBX hashes.
