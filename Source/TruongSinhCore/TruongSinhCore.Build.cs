using UnrealBuildTool;

public class TruongSinhCore : ModuleRules
{
    public TruongSinhCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject"
        });
    }
}
