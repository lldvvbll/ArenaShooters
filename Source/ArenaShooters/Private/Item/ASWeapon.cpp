// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASWeapon.h"
#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"

UASWeapon* UASWeapon::CreateFromDataAsset(UObject* Owner, UASWeaponDataAsset* DataAsset)
{
	if (DataAsset == nullptr)
		return nullptr;

	UASWeapon* NewItem = ::NewObject<UASWeapon>(Owner, DataAsset->ItemClass);
	NewItem->SetDataAsset(DataAsset);

	return NewItem;
}

const EWeaponType UASWeapon::GetWeaponType() const
{
	auto WeaponDA = Cast<UASWeaponDataAsset>(GetDataAsset());
	check(WeaponDA);

	return WeaponDA->WeaponType;
}
