// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Components/ActorComponent.h"
#include "Common/ASEnums.h"
#include "ASInventoryComponent.generated.h"

class UASItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENASHOOTERS_API UASInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UASInventoryComponent();

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void CreateTestItem();
	EWeaponType GetWeaponType() const;

private:
	UPROPERTY(Replicated)
	UASItem* TestItem;
};
