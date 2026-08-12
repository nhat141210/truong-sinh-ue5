using UnrealBuildTool;

public class TruongSinhResolution : ModuleRules
{
    public TruongSinhResolution(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "TruongSinhCore"
        });
    }
}
