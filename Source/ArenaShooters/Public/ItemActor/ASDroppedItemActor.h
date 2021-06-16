// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "GameFramework/Actor.h"
#include "ASDroppedItemActor.generated.h"

class UASItem;

UCLASS(Abstract)
class ARENASHOOTERS_API AASDroppedItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AASDroppedItemActor();

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	TArray<UASItem*> ASItems;
};