using UnrealBuildTool;

public class TruongSinhUI : ModuleRules
{
    public TruongSinhUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UMG",
            "CommonUI",
            "EnhancedInput",
            "TruongSinhCore",
            "TruongSinhData",
            "TruongSinhSimulation",
            "TruongSinhResolution",
            "TruongSinhNarrative"
        });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
    }
}
