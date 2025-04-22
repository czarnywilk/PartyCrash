/**
* Copyright (C) 2017-2022 eelDev AB
*
* Official Steamworks Documentation: https://partner.steamgames.com/doc/api/ISteamAppList
*/

#include "SteamAppList/SteamProAppList.h"
#include "SteamCoreProPluginPrivatePCH.h"

USteamProAppList::USteamProAppList()
{
#if WITH_STEAMCORE
	OnSteamAppInstalledCallback.Register(this, &USteamProAppList::OnSteamAppInstalled);
	OnSteamAppUninstalledCallback.Register(this, &USteamProAppList::OnSteamAppUninstalled);

	if (IsRunningDedicatedServer())
	{
		OnSteamAppInstalledCallback.SetGameserverFlag();
		OnSteamAppUninstalledCallback.SetGameserverFlag();
	}
#endif
}

USteamProAppList::~USteamProAppList()
{
#if WITH_STEAMCORE
	OnSteamAppInstalledCallback.Unregister();
   	OnSteamAppUninstalledCallback.Unregister();
#endif
}

USteamProAppList* USteamProAppList::GetSteamAppList()
{
#if WITH_STEAMCORE
	return SteamAppList() ? ThisClass::StaticClass()->GetDefaultObject<USteamProAppList>() : nullptr;
#endif
	return nullptr;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		Steam API Functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

int32 USteamProAppList::GetNumInstalledApps()
{
	LogVeryVerbose("");

	int32 Result = 0;
#if WITH_STEAMCORE

	if (SteamAppList())
	{
		Result = SteamAppList()->GetNumInstalledApps();
	}

#endif
	return Result;
}

int32 USteamProAppList::GetInstalledApps(TArray<int32>& OutAppIds, int32 MaxAppIDs)
{
	LogVeryVerbose("");
	int32 Result = 0;
	
#if WITH_STEAMCORE
	OutAppIds.Empty();

	if (SteamAppList())
	{
		TArray<uint32> DataArray;
		DataArray.SetNum(MaxAppIDs);

		Result = SteamAppList()->GetInstalledApps(DataArray.GetData(), DataArray.Num());

		if (Result > 0)
		{
			for (int32 i = 0; i < MaxAppIDs; i++)
			{
				OutAppIds.Add(DataArray[i]);
			}
		}
	}

#endif
	return Result;
}

int32 USteamProAppList::GetAppName(int32 AppID, FString& OutName)
{
	LogVeryVerbose("");

	int32 Result = 0;
#if WITH_STEAMCORE
	OutName.Empty();

	if (SteamAppList())
	{
		TArray<char> DataArray;
		DataArray.SetNum(128);

		Result = SteamAppList()->GetAppName(AppID, DataArray.GetData(), DataArray.Num());

		OutName = UTF8_TO_TCHAR(DataArray.GetData());
	}
#endif
	return Result;
}

int32 USteamProAppList::GetAppInstallDir(int32 AppID, FString& OutDirectory)
{
	LogVeryVerbose("");

	int32 Result = 0;
#if WITH_STEAMCORE
	OutDirectory.Empty();

	if (SteamAppList())
	{
		TArray<char> DataArray;
		DataArray.SetNum(512);

		Result = SteamAppList()->GetAppInstallDir(AppID, DataArray.GetData(), DataArray.Num());

		OutDirectory = UTF8_TO_TCHAR(DataArray.GetData());
	}
#endif
	return Result;
}

int32 USteamProAppList::GetAppBuildId(int32 AppID)
{
	LogVeryVerbose("");

	int32 Result = 0;
#if WITH_STEAMCORE
	if (SteamAppList())
	{
		Result = SteamAppList()->GetAppBuildId(AppID);
	}
#endif
	return Result;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		Steam API Callbacks
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

#if WITH_STEAMCORE
void USteamProAppList::OnSteamAppInstalled(SteamAppInstalled_t* pParam)
{
	LogVerbose("");
	auto Data = *pParam;
	AsyncTask(ENamedThreads::GameThread, [this, Data]()
	{
		SteamAppInstalled.Broadcast(Data);
	});
}

void USteamProAppList::OnSteamAppUninstalled(SteamAppUninstalled_t* pParam)
{
	LogVerbose("");

	auto Data = *pParam;
	AsyncTask(ENamedThreads::GameThread, [this, Data]()
	{
		SteamAppUninstalled.Broadcast(Data);
	});
}
#endif
