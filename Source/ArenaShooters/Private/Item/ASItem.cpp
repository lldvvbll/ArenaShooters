// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASItem.h"
#include "DataAssets/ItemDataAssets/ASItemDataAsset.h"
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

const UASItemDataAsset* UASItem::GetDataAsset() const
{
	return DataAsset;
}

EItemType UASItem::GetItemType() const
{
	return (DataAsset != nullptr) ? DataAsset->ItemType : EItemType::None;
}

UTexture2D* UASItem::GetEquipmentSlotImage() const
{
	return (DataAsset != nullptr) ? DataAsset->EquipmentSlotImage : nullptr;
}
