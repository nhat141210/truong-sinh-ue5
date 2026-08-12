using UnrealBuildTool;

public class TruongSinhSave : ModuleRules
{
    public TruongSinhSave(ReadOnlyTargetRules Target) : base(Target)
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
            "TruongSinhCombat",
            "TruongSinhQuest"
        });
        PrivateDependencyModuleNames.AddRange(new string[] { "Json", "JsonUtilities" });
    }
}
