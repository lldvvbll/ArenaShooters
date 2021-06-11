// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "UObject/NoExportTypes.h"
#include "Common/ASEnums.h"
#include "ASItem.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ARENASHOOTERS_API UASItem : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;

	void ChangeType();
	EWeaponType GetWeaponType() const;

protected:
	EWeaponType WeaponType;
};
