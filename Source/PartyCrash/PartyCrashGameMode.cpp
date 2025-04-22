// Copyright Epic Games, Inc. All Rights Reserved.

#include "PartyCrashGameMode.h"
#include "PartyCrashCharacter.h"
#include "UObject/ConstructorHelpers.h"

APartyCrashGameMode::APartyCrashGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
