// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Item/ASItem.h"
#include "Common/ASEnums.h"
#include "ASAmmo.generated.h"

UCLASS()
class ARENASHOOTERS_API UASAmmo : public UASItem
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetDataAsset(UASItemDataAsset* NewDataAsset) override;
	virtual void SetCount(int32 NewCount) override;

	EAmmoType GetAmmoType() const;
};
