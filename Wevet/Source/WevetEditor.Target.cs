// Copyright 2018 wevet works All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class WevetEditorTarget : TargetRules
{
	public WevetEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "Wevet" } );
	}
}
