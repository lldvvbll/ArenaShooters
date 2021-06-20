// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Item/ASItem.h"
#include "Common/ASEnums.h"
#include "ASArmor.generated.h"

class UASArmorDataAsset;
class AASArmorActor;

UCLASS()
class ARENASHOOTERS_API UASArmor : public UASItem
{
	GENERATED_BODY()

public:
	static UASArmor* CreateFromDataAsset(UObject* Owner, UASArmorDataAsset* DataAsset);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const EArmorType GetArmorType() const;
	bool IsEnableToEquip(EArmorSlotType SlotType) const;

	TWeakObjectPtr<AASArmorActor>& GetActor();
	const TWeakObjectPtr<AASArmorActor>& GetActor() const;

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	TWeakObjectPtr<AASArmorActor> ASArmorActor;
};
