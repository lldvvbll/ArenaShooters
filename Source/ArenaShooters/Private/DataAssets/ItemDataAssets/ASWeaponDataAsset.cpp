// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"
#include "ASAssetManager.h"

UASWeaponDataAsset::UASWeaponDataAsset()
{
	AssetType = UASAssetManager::WeaponAssetType;
	ItemType = EItemType::Weapon;
}

bool UASWeaponDataAsset::IsEnableToEquip(EWeaponSlotType SlotType) const
{
	switch (WeaponType)
	{
	case EWeaponType::Pistol:
		return (SlotType == EWeaponSlotType::Sub);
	case EWeaponType::AssaultRifle:
		return (SlotType == EWeaponSlotType::Main);
	default:
		break;
	}

	return false;
}