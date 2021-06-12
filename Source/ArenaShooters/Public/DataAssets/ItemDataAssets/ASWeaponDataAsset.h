// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "DataAssets/ItemDataAssets/ASItemDataAsset.h"
#include "ASWeaponDataAsset.generated.h"

UCLASS()
class ARENASHOOTERS_API UASWeaponDataAsset : public UASItemDataAsset
{
	GENERATED_BODY()
	
public:
	UASWeaponDataAsset();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponType WeaponType;
};
