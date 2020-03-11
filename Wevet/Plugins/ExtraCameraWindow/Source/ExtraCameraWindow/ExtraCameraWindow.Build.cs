// Copyright 2018 wevet works All Rights Reserved.

using UnrealBuildTool;

public class ExtraCameraWindow : ModuleRules
{
	public ExtraCameraWindow(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Public/ExtraCameraWindow.h";

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("ExtraCameraWindow/Private");

		
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
				"InputCore",
				"Slate",
				"SlateCore",
				"UMG",
			}
		);

	}
}
