// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArenaShootersGameMode.h"
#include "ArenaShootersCharacter.h"
#include "UObject/ConstructorHelpers.h"

AArenaShootersGameMode::AArenaShootersGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
