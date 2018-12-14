// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class FullbodyIK : ModuleRules
	{
		public FullbodyIK(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicIncludePaths.AddRange(
				new string[] {
					"FullbodyIK/Public"
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"FullbodyIK/Private"
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
					"AnimGraphRuntime",
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[] {
				}
				);
		}
	}
}
