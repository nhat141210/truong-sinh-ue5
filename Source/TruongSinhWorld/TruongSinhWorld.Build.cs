using UnrealBuildTool;

public class TruongSinhWorld : ModuleRules
{
    public TruongSinhWorld(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "TruongSinhCore",
            "TruongSinhData",
            "TruongSinhSimulation",
            "TruongSinhQuest"
        });
    }
}
