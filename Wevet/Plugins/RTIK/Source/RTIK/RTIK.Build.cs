using UnrealBuildTool;

public class RTIK : ModuleRules
{
	public RTIK(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Public/RTIK.h";

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("RTIK/Private");

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
				"AnimGraphRuntime",
			}
		);
	}
}
