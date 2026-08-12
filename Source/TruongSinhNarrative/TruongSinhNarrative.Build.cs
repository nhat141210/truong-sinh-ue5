using UnrealBuildTool;

public class TruongSinhNarrative : ModuleRules
{
    public TruongSinhNarrative(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core"
        });
    }
}
