// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PartyCrash : ModuleRules
{
	public PartyCrash(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", 
			"EnhancedInput", "SteamCoreShared", "Steamworks", "OnlineSubsystemUtils" });
	}
}
