// Copyright 2018 wevet works All Rights Reserved.

using UnrealBuildTool;

public class LocomotionSystem : ModuleRules
{
	public LocomotionSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Public/LocomotionSystem.h";

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("LocomotionSystem/Private");

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
			}
		);
	}
}

