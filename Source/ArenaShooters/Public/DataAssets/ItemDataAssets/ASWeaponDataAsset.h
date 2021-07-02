// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "DataAssets/ItemDataAssets/ASItemDataAsset.h"
#include "Common/ASEnums.h"
#include "ASWeaponDataAsset.generated.h"

class AASWeaponActor;
class AASBullet;

UCLASS()
class ARENASHOOTERS_API UASWeaponDataAsset : public UASItemDataAsset
{
	GENERATED_BODY()
	
public:
	UASWeaponDataAsset();

	bool IsEnableToEquip(EWeaponSlotType SlotType) const;

public:
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AASWeaponActor> ASWeaponActorClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AASBullet> ASBulletClass;
};
