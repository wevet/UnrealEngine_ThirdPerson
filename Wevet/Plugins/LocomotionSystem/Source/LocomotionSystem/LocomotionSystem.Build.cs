
namespace UnrealBuildTool.Rules
{
	public class LocomotionSystem : ModuleRules
	{
		public LocomotionSystem(ReadOnlyTargetRules Target) : base(Target)
		{

			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicIncludePaths.AddRange(
				new string[] 
				{
					"LocomotionSystem/Public"
					// ... add public include paths required here ...
				}
			);

			PrivateIncludePaths.AddRange(
				new string[] 
				{
					"LocomotionSystem/Private",
					// ... add other private include paths required here ...
				}
			);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					// ... add other public dependencies that you statically link with here ...
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"CoreUObject",
					"Engine",
					"InputCore",
					"Slate",
					"SlateCore",
					// ... add private dependencies that you statically link with here ...	
				}
			);

		}
	}


}
