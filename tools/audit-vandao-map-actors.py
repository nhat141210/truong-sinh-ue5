"""Audit Vấn Đạo Tông generated actor world bounds."""

import unreal

MAP = "/Game/Maps/VisualTarget/L_VanDaoTong"
level = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if not level.load_level(MAP):
    raise RuntimeError(f"Could not load {MAP}")
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()
for actor in actors:
    label = actor.get_actor_label()
    if label.startswith("VDT_"):
        origin, extent = actor.get_actor_bounds(False, False)
        unreal.log(f"VDT ACTOR {label}: location={actor.get_actor_location()}; extent={extent}")
