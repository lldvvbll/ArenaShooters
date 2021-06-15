// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "DataAssets/ItemDataAssets/ASItemDataAsset.h"
#include "Common/ASEnums.h"
#include "ASWeaponDataAsset.generated.h"

class AASWeaponActor;

UCLASS()
class ARENASHOOTERS_API UASWeaponDataAsset : public UASItemDataAsset
{
	GENERATED_BODY()
	
public:
	UASWeaponDataAsset();

	EEquipmentSlotType GetEquipmentSlotType() const;

public:
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AASWeaponActor> ASWeaponActorClass;
};
