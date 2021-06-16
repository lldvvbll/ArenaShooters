// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ASInventoryComponent.h"
#include "Item/ASWeapon.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "ASAssetManager.h"
#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"
#include "DataAssets/ItemDataAssets/ASArmorDataAsset.h"
#include "Character/ASCharacter.h"
#include "Item/ASItem.h"
#include "Item/ASWeapon.h"
#include "Item/ASArmor.h"
#include "ItemActor/ASWeaponActor.h"
#include "ItemActor/ASArmorActor.h"

UASInventoryComponent::UASInventoryComponent()
{
	//PrimaryComponentTick.bCanEverTick = true;

	EquipmentSlots.SetNumZeroed(ConvertToIndex(EEquipmentSlotType::SlotNum));

	SetIsReplicatedByDefault(true);
}

bool UASInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	for (auto& SlotItem : EquipmentSlots)
	{
		if (SlotItem != nullptr && !SlotItem->IsPendingKill())
		{
			WroteSomething |= Channel->ReplicateSubobject(SlotItem, *Bunch, *RepFlags);
		}
	}
		
	return WroteSomething;
}

void UASInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UASInventoryComponent, EquipmentSlots);
}

void UASInventoryComponent::CreateTestItem()
{
	if (GetOwner()->HasAuthority())
	{
		if (auto WeaponDataAsset = UASAssetManager::Get().GetDataAsset<UASWeaponDataAsset>(TestWeaponAssetId))
		{
			EEquipmentSlotType SlotType = EEquipmentSlotType::MainWeapon;
			if (SetItemToEquipmentSlot(SlotType, UASWeapon::CreateFromDataAsset(this, WeaponDataAsset)).Value)
			{
				if (auto WeaponActor = GetWorld()->SpawnActor<AASWeaponActor>(WeaponDataAsset->ASWeaponActorClass))
				{
					if (auto Owner = Cast<AASCharacter>(GetOwner()))
					{
						Owner->SetWeaponActor(WeaponActor);
						WeaponActor->SetOwner(Owner);
						WeaponActor->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName(TEXT("hand_R_socket")));
					}
					else
					{
						AS_LOG(Warning, TEXT("Owner == nullptr"));
					}
				}
				else
				{
					AS_LOG(Warning, TEXT("WeaponActor == nullptr"));
				}
			}
		}

		if (auto ArmorDataAsset = UASAssetManager::Get().GetDataAsset<UASArmorDataAsset>(TestArmorAssetId))
		{
			EEquipmentSlotType SlotType = EEquipmentSlotType::Helmet;
			if (SetItemToEquipmentSlot(SlotType, UASArmor::CreateFromDataAsset(this, ArmorDataAsset)).Value)
			{
				if (auto ArmorActor = GetWorld()->SpawnActor<AASArmorActor>(ArmorDataAsset->ASArmorActorClass))
				{
					if (auto Owner = Cast<AASCharacter>(GetOwner()))
					{
						Owner->SetHelmetMesh(ArmorActor);
						ArmorActor->SetOwner(Owner);
						ArmorActor->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName(TEXT("head_Helmet_socket")));
					}
					else
					{
						AS_LOG(Warning, TEXT("Owner == nullptr"));
					}
				}
				else
				{
					AS_LOG(Warning, TEXT("ArmorActor == nullptr"));
				}
			}
		}
	}
}

const EWeaponType UASInventoryComponent::GetWeaponType() const
{
	auto ItemPair = GetItemFromEquipmentSlot(EEquipmentSlotType::MainWeapon);
	if (!ItemPair.Value)
	{
		AS_LOG_S(Error);
		return EWeaponType::None;
	}

	auto Item = Cast<UASWeapon>(ItemPair.Key);
	return (Item != nullptr) ? Item->GetWeaponType() : EWeaponType::None;
}

TPair<UASItem*, bool> UASInventoryComponent::GetItemFromEquipmentSlot(EEquipmentSlotType SlotType)
{
	if (SlotType == EEquipmentSlotType::SlotNum)
	{
		AS_LOG_S(Error);
		return ItemBoolPair(nullptr, false);
	}

	return ItemBoolPair(EquipmentSlots[ConvertToIndex(SlotType)], true);
}

TPair<const UASItem*, bool> UASInventoryComponent::GetItemFromEquipmentSlot(EEquipmentSlotType SlotType) const
{
	if (SlotType == EEquipmentSlotType::SlotNum)
	{
		AS_LOG_S(Error);
		return ConstItemBoolPair(nullptr, false);
	}

	return ConstItemBoolPair(EquipmentSlots[ConvertToIndex(SlotType)], true);
}

TPair<UASItem*, bool> UASInventoryComponent::SetItemToEquipmentSlot(EEquipmentSlotType SlotType, UASItem* NewItem)
{
	if (SlotType == EEquipmentSlotType::SlotNum || NewItem == nullptr)
	{
		AS_LOG_S(Error);
		return ItemBoolPair(nullptr, false);
	}

	int32 Idx = ConvertToIndex(SlotType);
	if (EquipmentSlots[Idx] != nullptr)
		return ItemBoolPair(EquipmentSlots[Idx], false);

	EquipmentSlots[Idx] = NewItem;
	return ItemBoolPair(EquipmentSlots[Idx], true);
}

TPair<UASItem*, bool> UASInventoryComponent::RemoveItemFromEquipmentSlot(EEquipmentSlotType SlotType)
{
	auto ItemPair = GetItemFromEquipmentSlot(SlotType);
	if (!ItemPair.Value)
	{
		AS_LOG_S(Error);
		return ItemBoolPair(nullptr, false);
	}

	EquipmentSlots[ConvertToIndex(SlotType)] = nullptr;
	return ItemBoolPair(ItemPair.Key, true);
}

int32 UASInventoryComponent::ConvertToIndex(EEquipmentSlotType SlotType) const
{
	return static_cast<int32>(SlotType);
}
