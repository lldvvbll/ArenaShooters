// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASWeapon.h"
#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"
#include "Net/UnrealNetwork.h"

UASWeapon* UASWeapon::CreateFromDataAsset(UObject* Owner, UASWeaponDataAsset* DataAsset)
{
	if (DataAsset == nullptr)
		return nullptr;

	UASWeapon* NewItem = ::NewObject<UASWeapon>(Owner, DataAsset->ItemClass);
	NewItem->SetDataAsset(DataAsset);

	return NewItem;
}

void UASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

EEquipmentSlotType UASWeapon::GetEquipmentSlotType() const
{
	auto WeaponDA = Cast<UASWeaponDataAsset>(GetDataAsset());
	if (WeaponDA == nullptr)
	{
		AS_LOG_S(Warning);
	}

	return (WeaponDA != nullptr) ? WeaponDA->GetEquipmentSlotType() : EEquipmentSlotType::SlotNum;
}

const EWeaponType UASWeapon::GetWeaponType() const
{
	auto WeaponDA = Cast<UASWeaponDataAsset>(GetDataAsset());
	if (WeaponDA == nullptr)
	{
		AS_LOG_S(Warning);
	}

	return (WeaponDA != nullptr) ? WeaponDA->WeaponType : EWeaponType::None;
}
