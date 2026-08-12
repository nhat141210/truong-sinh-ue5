using UnrealBuildTool;

public class TruongSinhQuest : ModuleRules
{
    public TruongSinhQuest(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "TruongSinhCore",
            "TruongSinhData",
            "TruongSinhSimulation"
        });
    }
}
