// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASArmor.h"
#include "DataAssets/ItemDataAssets/ASArmorDataAsset.h"
#include "Net/UnrealNetwork.h"

void UASArmor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UASArmor, ASArmorActor);
	DOREPLIFETIME_CONDITION(UASArmor, CurrentDurability, COND_AutonomousOnly);
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

int32 UASArmor::GetCurrentDurability() const
{
	return CurrentDurability;
}

void UASArmor::SetCurrentDurability(int32 NewDurability)
{
	auto ArmorDA = Cast<UASArmorDataAsset>(DataAsset);
	check(ArmorDA);

	CurrentDurability = FMath::Clamp<float>(NewDurability, 0.0f, (ArmorDA != nullptr ? ArmorDA->MaxDurability : 100.0f));

	if (CurrentDurability < KINDA_SMALL_NUMBER)
	{
		AS_LOG(Warning, TEXT("Armor is broken!"));
	}
}

void UASArmor::ModifyDurability(int32 Value)
{
	SetCurrentDurability(CurrentDurability + Value);
}
