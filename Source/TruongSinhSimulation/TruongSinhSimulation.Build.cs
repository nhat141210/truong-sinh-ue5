using UnrealBuildTool;

public class TruongSinhSimulation : ModuleRules
{
    public TruongSinhSimulation(ReadOnlyTargetRules Target) : base(Target)
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
