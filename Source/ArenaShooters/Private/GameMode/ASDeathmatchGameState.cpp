// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ASDeathmatchGameState.h"
#include "GameMode/ASItemFactoryComponent.h"
#include "Net/UnrealNetwork.h"

AASDeathmatchGameState::AASDeathmatchGameState()
{
	ItemFactory = CreateDefaultSubobject<UASItemFactoryComponent>(TEXT("ItemFactory"));
}

void AASDeathmatchGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AASDeathmatchGameState, ItemFactory);
}

UASItemFactoryComponent* AASDeathmatchGameState::GetItemFactory()
{
	return ItemFactory;
}
