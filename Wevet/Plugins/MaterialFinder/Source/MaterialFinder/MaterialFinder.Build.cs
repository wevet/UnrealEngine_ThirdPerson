// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MaterialFinder : ModuleRules
{
	public MaterialFinder(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Public/MaterialFinder.h";

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("MaterialFinder/Private");

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
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"MaterialEditor",
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
