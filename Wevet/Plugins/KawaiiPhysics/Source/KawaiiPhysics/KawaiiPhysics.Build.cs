using UnrealBuildTool;

public class KawaiiPhysics : ModuleRules
{
	public KawaiiPhysics(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Public/KawaiiPhysics.h";

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("KawaiiPhysics/Private");

		PublicIncludePaths.AddRange(
			new string[] 
			{
			}
		);


		PrivateIncludePaths.AddRange(
			new string[] 
			{
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                 "AnimGraphRuntime",
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
            }
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}

