// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASWeapon.h"
#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"
#include "Net/UnrealNetwork.h"

void UASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UASWeapon, WeaponDataAsset);
}

void UASWeapon::SetDataAsset(UASItemDataAsset* NewDataAsset)
{
	Super::SetDataAsset(NewDataAsset);

	auto NewWeaponDataAsset = Cast<UASWeaponDataAsset>(NewDataAsset);
	check(NewWeaponDataAsset);

	WeaponDataAsset = NewWeaponDataAsset;
}

const UASWeaponDataAsset* UASWeapon::GetWeaponDataAsset() const
{
	return WeaponDataAsset;
}

const EWeaponType UASWeapon::GetWeaponType() const
{
	if (WeaponDataAsset == nullptr)
	{
		AS_LOG(Warning, TEXT("WeaponDataAsset == nullptr"));
	}

	return (WeaponDataAsset != nullptr) ? WeaponDataAsset->WeaponType : EWeaponType::None;
}
