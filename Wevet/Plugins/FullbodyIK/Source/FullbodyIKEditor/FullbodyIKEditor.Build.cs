// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class FullbodyIKEditor : ModuleRules
	{
		public FullbodyIKEditor(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicIncludePaths.AddRange(
				new string[] {
					"FullbodyIKEditor/Public"
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"FullbodyIKEditor/Private",
				}
				);
			
			PublicDependencyModuleNames.AddRange(
				new string[] {
					"Core",
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"CoreUObject",
					"Engine",
					"Slate",
					"SlateCore",
					"UnrealEd",
					"AnimGraph",
					"BlueprintGraph",
					"FullbodyIK",
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[] {
				}
				);
		}
	}
}
