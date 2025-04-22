// Copyright (C) 2017-2022 eelDev AB

using UnrealBuildTool;

public class SteamCorePro : ModuleRules
{
	public SteamCorePro(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateIncludePaths.Add("SteamCorePro/Public/SteamCorePro");
		PrivateIncludePaths.Add("SteamCorePro/Private/SteamCorePro");
		PrivatePCHHeaderFile = "Private/SteamCorePro/SteamCoreProPluginPrivatePCH.h";
		PrivateDefinitions.Add("ONLINESUBSYSTEMSTEAMCORE_PACKAGE=1");

		PublicDependencyModuleNames.AddRange
		(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
				"Networking",
				"Sockets",
				"Projects",
				"SteamLibrary",
				"SteamCoreShared",
				"AudioExtensions"
			}
		);

		PrivateDependencyModuleNames.AddRange
		(
			new[]
			{
				"HTTP", 
				"OnlineSubsystemSteamCore",
				"AudioExtensions"
			}
		);
		
		if (Target.Version.MajorVersion == 5 && Target.Version.MinorVersion >= 2)
		{
			PrivateDependencyModuleNames.AddRange
			(
				new[]
				{
					"AudioExtensions"
				}
			);
		}

		PublicDependencyModuleNames.Add("DeveloperSettings");
	}
}