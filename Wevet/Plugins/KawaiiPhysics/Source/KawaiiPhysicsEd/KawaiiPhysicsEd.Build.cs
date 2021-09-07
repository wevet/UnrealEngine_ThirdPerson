using UnrealBuildTool;

public class KawaiiPhysicsEd : ModuleRules
{
	public KawaiiPhysicsEd(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new string[] 
			{ 
				"Core",
				"CoreUObject",
				"Engine", 
				"InputCore",
				"KawaiiPhysics"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] 
			{ 
				"AnimGraph",
				"BlueprintGraph", 
				"Persona", 
				"UnrealEd",
				"AnimGraphRuntime", 
				"SlateCore", 
				"EditorFramework"
			}
		);


	}
}

