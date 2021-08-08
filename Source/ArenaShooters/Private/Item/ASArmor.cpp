// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASArmor.h"
#include "DataAssets/ItemDataAssets/ASArmorDataAsset.h"
#include "Net/UnrealNetwork.h"

void UASArmor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UASArmor, ASArmorActor);
	DOREPLIFETIME(UASArmor, CurrentDurability);
}

void UASArmor::SetDataAsset(UASItemDataAsset* NewDataAsset)
{
	Super::SetDataAsset(NewDataAsset);

	auto ArmorDA = Cast<UASArmorDataAsset>(NewDataAsset);
	check(ArmorDA);

	SetCurrentDurability(ArmorDA->MaxDurability);
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

float UASArmor::GetCurrentDurability() const
{
	return CurrentDurability;
}

void UASArmor::SetCurrentDurability(float NewDurability)
{
	auto ArmorDA = Cast<UASArmorDataAsset>(DataAsset);
	check(ArmorDA);

	CurrentDurability = FMath::Clamp<float>(NewDurability, 0.0f, (ArmorDA != nullptr ? ArmorDA->MaxDurability : 100.0f));

	if (CurrentDurability < KINDA_SMALL_NUMBER)
	{
		AS_LOG(Warning, TEXT("Armor is broken!"));
	}
}

void UASArmor::ModifyDurability(float Value)
{
	SetCurrentDurability(CurrentDurability + Value);
}
