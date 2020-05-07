using UnrealBuildTool;

public class RTIK : ModuleRules
{
    public RTIK(ReadOnlyTargetRules Target) : base(Target)
    {     
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] 
            {
                "Core",
                "CoreUObject",
                "Engine",
                "AnimGraphRuntime",
                "AnimationCore"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] 
            {
            }
        );
	}
}
