// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASWeapon.h"
#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"
#include "Net/UnrealNetwork.h"

void UASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

const EWeaponType UASWeapon::GetWeaponType() const
{
	const UASWeaponDataAsset* WeaponDataAsset = GetDataAsset<UASWeaponDataAsset>();
	if (WeaponDataAsset == nullptr)
	{
		AS_LOG(Warning, TEXT("WeaponDataAsset == nullptr"));
	}

	return (WeaponDataAsset != nullptr) ? WeaponDataAsset->WeaponType : EWeaponType::None;
}
