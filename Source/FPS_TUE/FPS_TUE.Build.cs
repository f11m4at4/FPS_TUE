// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPS_TUE : ModuleRules
{
	public FPS_TUE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
