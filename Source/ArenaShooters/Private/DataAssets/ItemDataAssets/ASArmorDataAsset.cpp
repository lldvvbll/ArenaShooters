// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/ItemDataAssets/ASArmorDataAsset.h"
#include "ASAssetManager.h"

UASArmorDataAsset::UASArmorDataAsset()
{
	AssetType = UASAssetManager::ArmorAssetType;
	ItemType = EItemType::Armor;
}

bool UASArmorDataAsset::IsEnableToEquip(EArmorSlotType SlotType) const
{
	switch (ArmorType)
	{
	case EArmorType::Helmet:
		return (SlotType == EArmorSlotType::Helmet);
	case EArmorType::Jacket:
		return (SlotType == EArmorSlotType::Jacket);
	default:
		break;
	}

	return false;
}