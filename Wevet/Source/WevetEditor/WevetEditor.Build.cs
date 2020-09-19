// Copyright 2018 wevet works All Rights Reserved.

using UnrealBuildTool;

public class WevetEditor : ModuleRules
{
	public WevetEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		//bLegacyPublicIncludePaths = false;
		//ShadowVariableWarningLevel = WarningLevel.Warning;
		//DefaultBuildSettings = BuildSettingsVersion.V2;
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;


		PublicDependencyModuleNames.AddRange(new string[]
			{
				"Wevet",
				"Core",
				"CoreUObject",
				"Engine",
				"BlueprintGraph",
				"AnimGraph",
			}
		);

		PrivateDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd",
				"GraphEditor",
				"PropertyEditor",
				"EditorStyle",
			}
		);

		//CircularlyReferencedDependentModules.AddRange(
		//    new string[]
		//    {
		//        "UnrealEd",
		//        "GraphEditor",
		//    }
		//);
	}
}
