// Copyright 2018 wevet works All Rights Reserved.

using UnrealBuildTool;

public class Wevet : ModuleRules
{
	public Wevet(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Public/Wevet.h";

		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		bUseUnity = true;

		PrivateIncludePaths.Add("Wevet/Private");

		PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"CoreUObject",
				"Engine"
			}
		);

		PrivateDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"AIModule",
				"GameplayTags",
				"GameplayTasks",
				"UMG",
				"Slate",
				"SlateCore",
				"NavigationSystem",
				//"ExtraCameraWindow",
				//"ShaderCore",
				"RenderCore",
				"RHI",
				"LevelSequence",
				"MoviePlayer",
				"GameplayAbilities",
				"LocomotionSystem",
				"LoadingScreen",
				"RTIK",
			}
		);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
