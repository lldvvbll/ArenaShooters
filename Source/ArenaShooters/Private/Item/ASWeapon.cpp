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

	DOREPLIFETIME(UASWeapon, ASWeaponActor);
}

const EWeaponType UASWeapon::GetWeaponType() const
{
	auto WeaponDA = Cast<UASWeaponDataAsset>(GetDataAsset());
	check(WeaponDA);

	return WeaponDA->WeaponType;
}

bool UASWeapon::IsEnableToEquip(EWeaponSlotType SlotType) const
{
	auto WeaponDA = Cast<UASWeaponDataAsset>(GetDataAsset());
	check(WeaponDA);

	return WeaponDA->IsEnableToEquip(SlotType);
}

TWeakObjectPtr<AASWeaponActor>& UASWeapon::GetActor()
{
	return ASWeaponActor;
}

const TWeakObjectPtr<AASWeaponActor>& UASWeapon::GetActor() const
{
	return ASWeaponActor;
}