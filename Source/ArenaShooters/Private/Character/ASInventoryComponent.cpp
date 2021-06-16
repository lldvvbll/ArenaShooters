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

	WeaponSlots.SetNumZeroed(static_cast<int32>(EWeaponSlotType::SlotNum));
	ArmorSlots.SetNumZeroed(static_cast<int32>(EArmorSlotType::SlotNum));

	SelectedWeaponSlotType = EWeaponSlotType::SlotNum;

	SetIsReplicatedByDefault(true);
}

bool UASInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	for (auto& SlotItem : WeaponSlots)
	{
		if (SlotItem != nullptr && !SlotItem->IsPendingKill())
		{
			WroteSomething |= Channel->ReplicateSubobject(SlotItem, *Bunch, *RepFlags);
		}
	}

	for (auto& SlotItem : ArmorSlots)
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

	DOREPLIFETIME(UASInventoryComponent, WeaponSlots);
	DOREPLIFETIME(UASInventoryComponent, ArmorSlots);
	DOREPLIFETIME(UASInventoryComponent, SelectedWeaponSlotType);
}

void UASInventoryComponent::CreateTestItem()
{
	if (GetOwner()->HasAuthority())
	{
		if (auto WeaponDataAsset = UASAssetManager::Get().GetDataAsset<UASWeaponDataAsset>(TestWeaponAssetId))
		{
			EWeaponSlotType SlotType = EWeaponSlotType::Main;
			if (SetItemToWeaponSlot(SlotType, UASWeapon::CreateFromDataAsset(this, WeaponDataAsset)).Value)
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
			EArmorSlotType SlotType = EArmorSlotType::Helmet;
			if (SetItemToArmorSlot(SlotType, UASArmor::CreateFromDataAsset(this, ArmorDataAsset)).Value)
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
	auto ItemPair = GetItemFromWeaponSlot(EWeaponSlotType::Main);
	if (!ItemPair.Value)
	{
		AS_LOG_S(Error);
		return EWeaponType::None;
	}

	auto Item = Cast<UASWeapon>(ItemPair.Key);
	return (Item != nullptr) ? Item->GetWeaponType() : EWeaponType::None;
}

TPair<UASItem*, bool> UASInventoryComponent::GetItemFromWeaponSlot(EWeaponSlotType SlotType)
{
	 ItemBoolPair ResultPair(nullptr, false);

	if (SlotType != EWeaponSlotType::SlotNum)
	{
		ResultPair.Key = WeaponSlots[static_cast<int32>(SlotType)];
		ResultPair.Value = true;
	}
	else
	{
		AS_LOG_S(Error);
	}

	return ResultPair;
}

TPair<const UASItem*, bool> UASInventoryComponent::GetItemFromWeaponSlot(EWeaponSlotType SlotType) const
{
	ConstItemBoolPair ResultPair(nullptr, false);

	if (SlotType != EWeaponSlotType::SlotNum)
	{
		ResultPair.Key = WeaponSlots[static_cast<int32>(SlotType)];
		ResultPair.Value = true;
	}
	else
	{
		AS_LOG_S(Error);
	}

	return ResultPair;
}

TPair<UASItem*, bool> UASInventoryComponent::SetItemToWeaponSlot(EWeaponSlotType SlotType, UASItem* NewItem)
{
	ItemBoolPair ResultPair(nullptr, false);

	if (SlotType != EWeaponSlotType::SlotNum)
	{
		if (NewItem != nullptr && NewItem->GetItemType() == EItemType::Weapon)
		{
			int32 Idx = static_cast<int32>(SlotType);
			if (WeaponSlots[Idx] == nullptr)
			{
				WeaponSlots[Idx] = NewItem;

				ResultPair.Key = WeaponSlots[Idx];
				ResultPair.Value = true;
			}
			else
			{
				ResultPair.Key = WeaponSlots[Idx];
				ResultPair.Value = false;
			}
		}
		else
		{
			AS_LOG_S(Error);
		}
	}
	else
	{
		AS_LOG_S(Error);
	}

	return ResultPair;
}

TPair<UASItem*, bool> UASInventoryComponent::RemoveItemFromWeaponSlot(EWeaponSlotType SlotType)
{
	ItemBoolPair ResultPair(nullptr, false);

	if (SlotType != EWeaponSlotType::SlotNum)
	{
		int32 Idx = static_cast<int32>(SlotType);

		ResultPair.Key = WeaponSlots[Idx];
		ResultPair.Value = true;

		WeaponSlots[Idx] = nullptr;
	}
	else
	{
		AS_LOG_S(Error);
	}

	return ResultPair;
}

TPair<UASItem*, bool> UASInventoryComponent::GetItemFromArmorSlot(EArmorSlotType SlotType)
{
	ItemBoolPair ResultPair(nullptr, false);

	if (SlotType != EArmorSlotType::SlotNum)
	{
		ResultPair.Key = ArmorSlots[static_cast<int32>(SlotType)];
		ResultPair.Value = true;
	}
	else
	{
		AS_LOG_S(Error);
	}

	return ResultPair;
}

TPair<const UASItem*, bool> UASInventoryComponent::GetItemFromArmorSlot(EArmorSlotType SlotType) const
{
	ConstItemBoolPair ResultPair(nullptr, false);

	if (SlotType != EArmorSlotType::SlotNum)
	{
		ResultPair.Key = ArmorSlots[static_cast<int32>(SlotType)];
		ResultPair.Value = true;
	}
	else
	{
		AS_LOG_S(Error);
	}

	return ResultPair;
}

TPair<UASItem*, bool> UASInventoryComponent::SetItemToArmorSlot(EArmorSlotType SlotType, UASItem* NewItem)
{
	ItemBoolPair ResultPair(nullptr, false);

	if (SlotType != EArmorSlotType::SlotNum)
	{
		if (NewItem != nullptr && NewItem->GetItemType() == EItemType::Armor)
		{
			int32 Idx = static_cast<int32>(SlotType);
			if (ArmorSlots[Idx] == nullptr)
			{
				ArmorSlots[Idx] = NewItem;

				ResultPair.Key = ArmorSlots[Idx];
				ResultPair.Value = true;
			}
			else
			{
				ResultPair.Key = ArmorSlots[Idx];
				ResultPair.Value = false;
			}
		}
		else
		{
			AS_LOG_S(Error);
		}
	}
	else
	{
		AS_LOG_S(Error);
	}

	return ResultPair;
}

TPair<UASItem*, bool> UASInventoryComponent::RemoveItemFromArmorSlot(EArmorSlotType SlotType)
{
	ItemBoolPair ResultPair(nullptr, false);

	if (SlotType != EArmorSlotType::SlotNum)
	{
		int32 Idx = static_cast<int32>(SlotType);

		ResultPair.Key = ArmorSlots[Idx];
		ResultPair.Value = true;

		ArmorSlots[Idx] = nullptr;
	}
	else
	{
		AS_LOG_S(Error);
	}

	return ResultPair;
}
