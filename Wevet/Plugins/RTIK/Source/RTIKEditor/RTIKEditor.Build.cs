
using UnrealBuildTool;

public class RTIKEditor : ModuleRules
{
	public RTIKEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"RTIK",
				"Core",
				"CoreUObject",
				"Engine",
				"BlueprintGraph",
				"AnimGraph",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",
				"GraphEditor",
				"PropertyEditor",
				"AnimGraphRuntime",
				"EditorStyle",
			}
		);

	}
}
