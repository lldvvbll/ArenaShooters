// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "UObject/NoExportTypes.h"
#include "Common/ASEnums.h"
#include "ASItem.generated.h"

class UASItemDataAsset;
class AASItemActor;

UCLASS(Abstract)
class ARENASHOOTERS_API UASItem : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;

	virtual void SetDataAsset(UASItemDataAsset* NewDataAsset);
	const UASItemDataAsset* GetDataAsset() const;

	EItemType GetItemType() const;
	UTexture2D* GetEquipmentSlotImage() const;

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	UASItemDataAsset* DataAsset;
};

using ItemBoolPair = TPair<UASItem*, bool>;
using ConstItemBoolPair = TPair<const UASItem*, bool>;