// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RLPlugin : ModuleRules
{
    public RLPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bUsePrecompiled = true;
        PublicIncludePaths.AddRange(
            new string[] {
                // ... add public include paths required here ...
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths required here ...
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
                "Projects",
                "InputCore",
                "UnrealEd",
                "LevelEditor",
                "CoreUObject",
                "RenderCore",
                "Engine",
                "Slate",
                "SlateCore",
                "ContentBrowser",
                "DesktopPlatform",
                "MaterialEditor",
                "ImageWrapper",
                "EditorStyle",
                "RawMesh",
                "ClothingSystemEditorInterface",
                "SkeletalMeshEditor",
                "Json",
                "JsonUtilities",
            }
            );
        if( Target.Version.MajorVersion == 4 && Target.Version.MinorVersion <= 23 )
        {
            PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "ClothingSystemRuntime"
            } );
        }
        if( Target.Version.MajorVersion == 4 && Target.Version.MinorVersion >= 24 )
        {
            PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "ClothingSystemEditor",
                "ClothingSystemRuntimeCommon",
                "SkeletalMeshUtilitiesCommon",
                "ClothingSystemRuntimeInterface",
                "ClothingSystemRuntimeNv"
            } );
        }

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                // ... add any modules that your module loads dynamically here ...
            }
            );
    }
}
