// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASArmor.h"
#include "DataAssets/ItemDataAssets/ASArmorDataAsset.h"
#include "Net/UnrealNetwork.h"

UASArmor* UASArmor::CreateFromDataAsset(UWorld* World, AActor* NewOwner, UASArmorDataAsset* DataAsset)
{
	if (DataAsset == nullptr)
	{
		AS_LOG_S(Error);
		return nullptr;
	}
	if (World == nullptr)
	{
		AS_LOG_S(Error);
		return nullptr;
	}

	UASArmor* NewItem = ::NewObject<UASArmor>(World->GetCurrentLevel(), DataAsset->ItemClass);
	if (NewItem == nullptr)
	{
		AS_LOG_S(Error);
		return nullptr;
	}

	NewItem->SetOwner(NewOwner);
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

TWeakObjectPtr<AASArmorActor>& UASArmor::GetActor()
{
	return ASArmorActor;
}

const TWeakObjectPtr<AASArmorActor>& UASArmor::GetActor() const
{
	return ASArmorActor;
}
