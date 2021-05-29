// Copyright Epic Games, Inc. All Rights Reserved.

#include "ASGameMode.h"
#include "Character/ASCharacter.h"
#include "UObject/ConstructorHelpers.h"

AASGameMode::AASGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/BP_Character"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
