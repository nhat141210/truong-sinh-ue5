using UnrealBuildTool;

public class TruongSinhUE5 : ModuleRules
{
    public TruongSinhUE5(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "TruongSinhCore",
            "TruongSinhData",
            "TruongSinhSimulation",
            "TruongSinhCombat",
            "TruongSinhQuest",
            "TruongSinhWorld",
            "TruongSinhPresentation",
            "TruongSinhUI",
            "TruongSinhSave"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Projects"
        });
    }
}
