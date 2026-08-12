using UnrealBuildTool;

public class TruongSinhData : ModuleRules
{
    public TruongSinhData(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "AssetRegistry",
            "GameplayTags",
            "TruongSinhCore"
        });
        PrivateDependencyModuleNames.AddRange(new string[] { "Json", "JsonUtilities" });
    }
}
