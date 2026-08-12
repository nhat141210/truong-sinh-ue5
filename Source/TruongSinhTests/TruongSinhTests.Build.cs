using UnrealBuildTool;

public class TruongSinhTests : ModuleRules
{
    public TruongSinhTests(ReadOnlyTargetRules Target) : base(Target)
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
            "TruongSinhResolution",
            "TruongSinhSave",
            "TruongSinhWorld"
        });
    }
}
