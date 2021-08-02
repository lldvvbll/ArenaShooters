// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "DataAssets/ItemDataAssets/ASItemDataAsset.h"
#include "Common/ASEnums.h"
#include "ASAmmoDataAsset.generated.h"

UCLASS()
class ARENASHOOTERS_API UASAmmoDataAsset : public UASItemDataAsset
{
	GENERATED_BODY()

public:
	UASAmmoDataAsset();

public:
	UPROPERTY(EditDefaultsOnly)
	EAmmoType AmmoType;
};
