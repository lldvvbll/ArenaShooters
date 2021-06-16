// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Item/ASItem.h"
#include "Common/ASEnums.h"
#include "ASArmor.generated.h"

class UASArmorDataAsset;

UCLASS()
class ARENASHOOTERS_API UASArmor : public UASItem
{
	GENERATED_BODY()

public:
	static UASArmor* CreateFromDataAsset(UObject* Owner, UASArmorDataAsset* DataAsset);

	const EArmorType GetArmorType() const;
};