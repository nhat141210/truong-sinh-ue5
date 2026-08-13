# Vấn Đạo Tông — Master-estate visual reference

Reference reviewed locally: `D:\GameDev\snaptik_7656635100439907605_v3.mp4` (65.876 s,
1024x576, 30 fps). Eight frames were extracted outside the repository for visual
study only; no video frames or copyrighted assets are shipped in the project.

The composition target is:

- green plateau occupies most of the frame, with the sect gate/hall as the
  central vertical landmark;
- a broad, readable open buildable area surrounds the sect instead of a small
  gameplay island;
- layered rocky mountain masses sit behind the buildings and fade into aerial
  perspective rather than ending as a hard horizon cutout;
- water gardens and tree clusters break the grass field at mid distance;
- a bright, continuous cloud sea wraps the outer rim and sits below the main
  camera line, making the plateau read as an immortal realm above the clouds.

Implementation contract now in the visual map:

- `SM_VDT_MasterEstate`: original Blender-authored oval plateau, approximately
  299 x 206 x 18 m, centred and min-Z normalized;
- eight named non-colliding expansion parcels around the central route;
- perimeter pine groves, mossy rock masses, and four overlapping expansion ponds;
- one UE VolumetricCloud layer plus SkyAtmosphere/height fog for the outer cloud
  sea; three distant vista islands remain non-gameplay background layers;
- native activity actors and stable simulation coordinates are unchanged.

This is a scale/layout foundation. Final photoreal foliage, authored terrain
height variation, and cinematic cloud art remain separate production gates.
