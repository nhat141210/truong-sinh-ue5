using UnrealBuildTool;
using System.Collections.Generic;

public class TruongSinhUE5Target : TargetRules
{
    public TruongSinhUE5Target(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.AddRange(new string[] { "TruongSinhUE5" });
    }
}
