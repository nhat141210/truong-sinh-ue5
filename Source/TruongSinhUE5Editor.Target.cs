using UnrealBuildTool;
using System.Collections.Generic;

public class TruongSinhUE5EditorTarget : TargetRules
{
    public TruongSinhUE5EditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.AddRange(new string[] { "TruongSinhUE5", "TruongSinhTests" });
    }
}
