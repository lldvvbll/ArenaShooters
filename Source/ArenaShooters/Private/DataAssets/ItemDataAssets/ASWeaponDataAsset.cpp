// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"
#include "ASAssetManager.h"
#include "Common/ASEnums.h"

UASWeaponDataAsset::UASWeaponDataAsset()
{
	AssetType = UASAssetManager::WeaponAssetType;
	ItemType = EItemType::Weapon;
}

EEquipmentSlotType UASWeaponDataAsset::GetEquipmentSlotType() const
{
	switch (WeaponType)
	{
	case EWeaponType::Pistol:
		return EEquipmentSlotType::SubWeapon;
	case EWeaponType::AssaultRifle:
		return EEquipmentSlotType::MainWeapon;
	default:
		AS_LOG_S(Warning);
		break;
	}

	return EEquipmentSlotType::SlotNum;
}
