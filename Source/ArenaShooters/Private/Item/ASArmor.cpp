// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASArmor.h"
#include "DataAssets/ItemDataAssets/ASArmorDataAsset.h"

UASArmor* UASArmor::CreateFromDataAsset(UObject* Owner, UASArmorDataAsset* DataAsset)
{
	if (DataAsset == nullptr)
		return nullptr;

	UASArmor* NewItem = ::NewObject<UASArmor>(Owner, DataAsset->ItemClass);
	NewItem->SetDataAsset(DataAsset);

	return NewItem;
}

const EArmorType UASArmor::GetArmorType() const
{
	auto ArmorDA = Cast<UASArmorDataAsset>(DataAsset);
	check(ArmorDA);

	return ArmorDA->ArmorType;
}
