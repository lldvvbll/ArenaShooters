// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ASInventoryComponent.h"
#include "Item/ASWeapon.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "ASAssetManager.h"
#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"

UASInventoryComponent::UASInventoryComponent()
{
	//PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

bool UASInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	if (TestItem != nullptr && !TestItem->IsPendingKill())
	{
		WroteSomething |= Channel->ReplicateSubobject(TestItem, *Bunch, *RepFlags);
	}
	
	return WroteSomething;
}

void UASInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UASInventoryComponent, TestItem);
}

void UASInventoryComponent::CreateTestItem()
{
	if (GetOwner()->HasAuthority())
	{
		if (TestItem == nullptr)
		{
			if (auto DataAsset = UASAssetManager::Get().GetDataAsset<UASWeaponDataAsset>(TestItemAssetId))
			{
				TestItem = ::NewObject<UASWeapon>(this, DataAsset->ItemClass);
				TestItem->SetDataAsset(DataAsset);
			}
		}
	}
}

const EWeaponType UASInventoryComponent::GetWeaponType() const
{
	return (TestItem != nullptr) ? TestItem->GetWeaponType() : EWeaponType::None;
}
