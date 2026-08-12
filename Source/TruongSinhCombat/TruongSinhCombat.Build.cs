using UnrealBuildTool;

public class TruongSinhCombat : ModuleRules
{
    public TruongSinhCombat(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "TruongSinhCore",
            "TruongSinhData"
        });
    }
}
