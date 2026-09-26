using UnrealBuildTool;

public class BDFR_PhotoMode : ModuleRules
{
    public BDFR_PhotoMode(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "DeveloperSettings"
        });
    }
}
