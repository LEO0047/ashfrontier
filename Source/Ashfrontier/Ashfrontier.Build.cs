using UnrealBuildTool;

public class Ashfrontier : ModuleRules
{
    public Ashfrontier(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "UMG",
            "AIModule",
            "NavigationSystem",
            "GameplayTasks"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {});
    }
}
