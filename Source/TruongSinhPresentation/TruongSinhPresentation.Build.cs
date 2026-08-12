using UnrealBuildTool;

public class TruongSinhPresentation : ModuleRules
{
    public TruongSinhPresentation(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "TruongSinhCore",
            "TruongSinhCombat"
        });
    }
}
