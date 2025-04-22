/**
* Copyright (C) 2017-2022 eelDev AB
*
* Official Steamworks Documentation: https://partner.steamgames.com/doc/api/ISteamAppList
*/

#pragma once

#include "CoreMinimal.h"
#include "SteamCorePro/SteamCoreProModule.h"
#include "SteamAppListTypes.h"
#include "SteamProAppList.generated.h"

UCLASS()
class STEAMCOREPRO_API USteamProAppList : public USteamCoreInterface
{
	GENERATED_BODY()
public:
	USteamProAppList();
	virtual ~USteamProAppList() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SteamCore")
	static USteamProAppList* GetSteamAppList();
public:
	UPROPERTY(BlueprintAssignable, Category = "SteamCore|AppList|Delegates")
	FOnSteamAppInstalled SteamAppInstalled;
	UPROPERTY(BlueprintAssignable, Category = "SteamCore|AppList|Delegates")
	FOnSteamAppUninstalled SteamAppUninstalled;

public:
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	//		Steam API Functions
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

	/**
	* Get Number of Installed Apps
	*/
	UFUNCTION(BlueprintCallable, Category = "SteamCore|AppList")
	int32 GetNumInstalledApps();

	/**
	* Get Installed Apps
	*/
	UFUNCTION(BlueprintCallable, Category = "SteamCore|AppList")
	int32 GetInstalledApps(TArray<int32>& AppIDs, int32 MaxAppIDs);

	/**
	* Get App Name
	*/
	UFUNCTION(BlueprintCallable, Category = "SteamCore|AppList")
	int32 GetAppName(int32 AppID, FString& Name);

	/**
	* Get app install dir
	*/
	UFUNCTION(BlueprintCallable, Category = "SteamCore|AppList")
	int32 GetAppInstallDir(int32 AppID, FString& Directory);

	/**
	* Return the buildid of this app, may change at any time based on backend updates to the game
	*
	* @param
	*/
	UFUNCTION(BlueprintCallable, Category = "SteamCore|AppList")
	int32 GetAppBuildId(int32 AppID);

private:
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	//		Steam API Callbacks
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

#if WITH_STEAMCORE
	STEAM_CALLBACK_MANUAL(USteamProAppList, OnSteamAppInstalled, SteamAppInstalled_t, OnSteamAppInstalledCallback);
	STEAM_CALLBACK_MANUAL(USteamProAppList, OnSteamAppUninstalled, SteamAppUninstalled_t, OnSteamAppUninstalledCallback);
#endif
};
