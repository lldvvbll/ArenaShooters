// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASItem.h"
#include "Net/UnrealNetwork.h"

void UASItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UASItem, DataAsset);
}

bool UASItem::IsSupportedForNetworking() const
{
	return true;
}

void UASItem::SetDataAsset(UASItemDataAsset* NewDataAsset)
{
	DataAsset = NewDataAsset;
}

EEquipmentSlotType UASItem::GetEquipmentSlotType() const
{
	return EEquipmentSlotType::SlotNum;
}

const UASItemDataAsset* UASItem::GetDataAsset() const
{
	return DataAsset;
}
