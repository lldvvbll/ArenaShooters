// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Item/ASItem.h"
#include "Common/ASEnums.h"
#include "ASWeapon.generated.h"

class UASWeaponDataAsset;

UCLASS()
class ARENASHOOTERS_API UASWeapon : public UASItem
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetDataAsset(UASItemDataAsset* NewDataAsset) override;
	const UASWeaponDataAsset* GetWeaponDataAsset() const;

	const EWeaponType GetWeaponType() const;

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	UASWeaponDataAsset* WeaponDataAsset;
};
