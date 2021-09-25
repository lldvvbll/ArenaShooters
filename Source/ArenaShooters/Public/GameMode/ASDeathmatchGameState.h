// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "GameFramework/GameState.h"
#include "ASDeathmatchGameState.generated.h"

class UASItemFactoryComponent;

UCLASS()
class ARENASHOOTERS_API AASDeathmatchGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AASDeathmatchGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UASItemFactoryComponent* GetItemFactory();

protected:
	UPROPERTY(Replicated, EditDefaultsOnly)
	UASItemFactoryComponent* ItemFactory;
};
