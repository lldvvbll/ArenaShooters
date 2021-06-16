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
	static UASWeapon* CreateFromDataAsset(UObject* Owner, UASWeaponDataAsset* DataAsset);

	const EWeaponType GetWeaponType() const;
};
