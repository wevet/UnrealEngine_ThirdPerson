// Copyright 2018 wevet works All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class WevetTarget : TargetRules
{
	public WevetTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "Wevet" } );
	}
}
