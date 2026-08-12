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
            "TruongSinhCore",
            "TruongSinhSimulation"
        });
        PrivateDependencyModuleNames.AddRange(new string[] { "Json", "JsonUtilities" });
    }
}
