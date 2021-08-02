// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASAmmo.h"
#include "DataAssets/ItemDataAssets/ASAmmoDataAsset.h"
#include "Net/UnrealNetwork.h"

void UASAmmo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UASAmmo, CurrentBundleCount);
}

void UASAmmo::SetDataAsset(UASItemDataAsset* NewDataAsset)
{
	Super::SetDataAsset(NewDataAsset);
}

int32 UASAmmo::GetBundleCount() const
{
	return CurrentBundleCount;
}

void UASAmmo::SetBundleCount(int32 NewCount)
{
	auto AmmoDA = Cast<UASAmmoDataAsset>(DataAsset);
	check(AmmoDA);

	CurrentBundleCount = FMath::Clamp<int32>(NewCount, 0, (AmmoDA != nullptr ? AmmoDA->MaxBundleCount : 100));

	if (CurrentBundleCount <= 0)
	{

	}
}

void UASAmmo::ModifyBundleCount(int32 Value)
{
	SetBundleCount(CurrentBundleCount + Value);
}
