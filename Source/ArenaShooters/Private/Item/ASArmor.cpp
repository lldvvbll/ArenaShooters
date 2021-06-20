// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASArmor.h"
#include "DataAssets/ItemDataAssets/ASArmorDataAsset.h"
#include "Net/UnrealNetwork.h"

UASArmor* UASArmor::CreateFromDataAsset(UObject* Owner, UASArmorDataAsset* DataAsset)
{
	if (DataAsset == nullptr)
		return nullptr;

	UASArmor* NewItem = ::NewObject<UASArmor>(Owner, DataAsset->ItemClass);
	NewItem->SetDataAsset(DataAsset);

	return NewItem;
}

void UASArmor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UASArmor, ASArmorActor);
}

const EArmorType UASArmor::GetArmorType() const
{
	auto ArmorDA = Cast<UASArmorDataAsset>(DataAsset);
	check(ArmorDA);

	return ArmorDA->ArmorType;
}

bool UASArmor::IsEnableToEquip(EArmorSlotType SlotType) const
{
	auto ArmorDA = Cast<UASArmorDataAsset>(GetDataAsset());
	check(ArmorDA);

	return ArmorDA->IsEnableToEquip(SlotType);
}

TWeakObjectPtr<AASArmorActor>& UASArmor::GetActor()
{
	return ASArmorActor;
}

const TWeakObjectPtr<AASArmorActor>& UASArmor::GetActor() const
{
	return ASArmorActor;
}
