// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ASInventoryComponent.h"
#include "Item/ASItem.h"
#include "Item/ASWeapon.h"
#include "Item/ASArmor.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Character/ASCharacter.h"
#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"
#include "DataAssets/ItemDataAssets/ASArmorDataAsset.h"
#include "ItemActor/ASWeaponActor.h"
#include "ItemActor/ASArmorActor.h"

const FName UASInventoryComponent::UsingWeaponSocketName = TEXT("weapon_rhand_socket");
const FName UASInventoryComponent::UsingWeaponPistolSocketName = TEXT("weapon_rhand_socket_pistol");
const FName UASInventoryComponent::BackSocketName = TEXT("weapon_back_socket");
const FName UASInventoryComponent::SideSocketName = TEXT("weapon_side_socekt");
const FName UASInventoryComponent::HelmetSocketName = TEXT("helmet_socket");
const FName UASInventoryComponent::JacketSocketName = TEXT("jacket_socket");

UASInventoryComponent::UASInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	SelectedWeaponSlotType = EWeaponSlotType::SlotNum;
	WeaponSlots.SetNumZeroed(static_cast<int32>(EWeaponSlotType::SlotNum));
	ArmorSlots.SetNumZeroed(static_cast<int32>(EArmorSlotType::SlotNum));
}

void UASInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UASInventoryComponent, WeaponSlots);
	DOREPLIFETIME(UASInventoryComponent, ArmorSlots);
	DOREPLIFETIME(UASInventoryComponent, SelectedWeapon);
	DOREPLIFETIME(UASInventoryComponent, SelectedWeaponSlotType);	
}

TWeakObjectPtr<UASWeapon> UASInventoryComponent::GetSelectedWeapon() const
{
	return MakeWeakObjectPtr(SelectedWeapon);
}

TWeakObjectPtr<AASWeaponActor> UASInventoryComponent::GetSelectedWeaponActor()
{
	if (SelectedWeapon == nullptr)
		return TWeakObjectPtr<AASWeaponActor>();

	return SelectedWeapon->GetActor();	
}

const EWeaponType UASInventoryComponent::GetSelectedWeaponType() const
{
	return (SelectedWeapon != nullptr) ? SelectedWeapon->GetWeaponType() : EWeaponType::None;
}

const EWeaponSlotType UASInventoryComponent::GetSelectedWeaponSlotType() const
{
	return SelectedWeaponSlotType;
}

bool UASInventoryComponent::IsSuitableWeaponSlot(EWeaponSlotType SlotType, UASWeapon* Weapon)
{
	if (Weapon == nullptr)
	{
		AS_LOG_S(Error);
		return false;
	}

	if (Weapon->GetItemType() != EItemType::Weapon)
	{
		AS_LOG_S(Error);
		return false;
	}

	switch (Weapon->GetWeaponType())
	{
	case EWeaponType::Pistol:
		return (SlotType == EWeaponSlotType::Sub);
	case EWeaponType::AssaultRifle:
		return (SlotType == EWeaponSlotType::Main);
	default:
		checkNoEntry();
		return false;
	}

	return false;
}

bool UASInventoryComponent::IsSuitableArmorSlot(EArmorSlotType SlotType, UASArmor* Armor)
{
	if (Armor == nullptr)
	{
		AS_LOG_S(Error);
		return false;
	}

	if (Armor->GetItemType() != EItemType::Armor)
	{
		AS_LOG_S(Error);
		return false;
	}

	switch (Armor->GetArmorType())
	{
	case EArmorType::Helmet:
		return (SlotType == EArmorSlotType::Helmet);
	case EArmorType::Jacket:
		return (SlotType == EArmorSlotType::Jacket);
	default:
		checkNoEntry();
		return false;
	}

	return false;
}

bool UASInventoryComponent::InsertWeapon(EWeaponSlotType SlotType, UASWeapon* NewWeapon, UASItem*& Out_OldItem)
{
	if (!IsSuitableWeaponSlot(SlotType, NewWeapon))
		return false;

	ItemBoolPair RemoveResultPair = RemoveItemFromWeaponSlot(SlotType);
	if (!RemoveResultPair.Value)
	{
		AS_LOG_S(Error);
		return false;
	}

	ConstItemPtrBoolPair SetResultPair = SetItemToWeaponSlot(SlotType, NewWeapon);
	if (!SetResultPair.Value)
	{
		WeaponSlots[static_cast<int32>(SlotType)] = RemoveResultPair.Key;

		AS_LOG_S(Error);
		return false;
	}

	Out_OldItem = RemoveResultPair.Key;	
	OnInsertWeapon.Broadcast(SlotType, Cast<UASWeapon>(RemoveResultPair.Key));

	return true;
}

bool UASInventoryComponent::InsertArmor(EArmorSlotType SlotType, UASArmor* NewArmor, UASItem*& Out_OldItem)
{
	if (!IsSuitableArmorSlot(SlotType, NewArmor))
		return false;
		
	ItemBoolPair RemoveResultPair = RemoveItemFromArmorSlot(SlotType);
	if (!RemoveResultPair.Value)
	{
		AS_LOG_S(Error);
		return false;
	}

	ConstItemPtrBoolPair SetResultPair = SetItemToArmorSlot(SlotType, NewArmor);
	if (!SetResultPair.Value)
	{
		ArmorSlots[static_cast<int32>(SlotType)] = RemoveResultPair.Key;

		AS_LOG_S(Error);
		return false;
	}

	Out_OldItem = RemoveResultPair.Key;
	OnInsertArmor.Broadcast(SlotType, Cast<UASArmor>(RemoveResultPair.Key));

	return true;
}

void UASInventoryComponent::SelectWeapon(EWeaponSlotType SlotType)
{
	ItemBoolPair ResultPair = GetItemFromWeaponSlot(SlotType);
	if (!ResultPair.Value)
	{
		AS_LOG_S(Error);
		return;
	}

	// 선택하려는 슬롯에 무기가 없다면 실패 처리
	auto NewWeapon = Cast<UASWeapon>(ResultPair.Key);
	if (NewWeapon == nullptr)
		return;

	// 이미 선택된 슬롯을 또 선택하면 아무일도 없다.
	UASWeapon* OldWeapon = SelectedWeapon;
	if (OldWeapon == NewWeapon)
		return;

	SelectedWeapon = NewWeapon;
	SelectedWeaponSlotType = SlotType;
	OnSelectedWeaponChanged(OldWeapon, NewWeapon);
}

ConstItemPtrBoolPair UASInventoryComponent::FindItemFromWeaponSlot(EWeaponSlotType SlotType) const
{
	ConstItemPtrBoolPair ResultPair(nullptr, false);

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

ConstItemPtrBoolPair UASInventoryComponent::SetItemToWeaponSlot(EWeaponSlotType SlotType, UASItem* NewItem)
{
	ConstItemPtrBoolPair ResultPair(nullptr, false);

	if (SlotType != EWeaponSlotType::SlotNum)
	{
		if (NewItem != nullptr && NewItem->GetItemType() == EItemType::Weapon)
		{
			int32 Idx = static_cast<int32>(SlotType);
			if (WeaponSlots[Idx] == nullptr)
			{
				if (NewItem != nullptr)
				{
					NewItem->SetOwner(GetOwner());
				}
				WeaponSlots[Idx] = NewItem;

				ResultPair.Key = WeaponSlots[Idx];
				ResultPair.Value = true;

				OnWeaponInserted(SlotType, Cast<UASWeapon>(WeaponSlots[Idx]));
			}
			else
			{
				ResultPair.Key = WeaponSlots[Idx];
				ResultPair.Value = false;

				AS_LOG_S(Error);
			}
		}
		else
		{
			AS_LOG_S(Error);
		}
	}
	else
	{
		checkNoEntry();
	}

	return ResultPair;
}

ItemBoolPair UASInventoryComponent::RemoveItemFromWeaponSlot(EWeaponSlotType SlotType)
{
	ItemBoolPair ResultPair(nullptr, false);

	if (SlotType != EWeaponSlotType::SlotNum)
	{
		int32 Idx = static_cast<int32>(SlotType);

		ResultPair.Key = WeaponSlots[Idx];
		ResultPair.Value = true;

		if (ResultPair.Key != nullptr)
		{
			ResultPair.Key->SetOwner(nullptr);
		}

		WeaponSlots[Idx] = nullptr;

		OnWeaponRemoved(SlotType, Cast<UASWeapon>(ResultPair.Key));
	}
	else
	{
		checkNoEntry();
	}

	return ResultPair;
}

ConstItemPtrBoolPair UASInventoryComponent::FindItemFromArmorSlot(EArmorSlotType SlotType) const
{
	ConstItemPtrBoolPair ResultPair(nullptr, false);

	if (SlotType != EArmorSlotType::SlotNum)
	{
		ResultPair.Key = ArmorSlots[static_cast<int32>(SlotType)];
		ResultPair.Value = true;
	}
	else
	{
		checkNoEntry();
	}

	return ResultPair;
}

ConstItemPtrBoolPair UASInventoryComponent::SetItemToArmorSlot(EArmorSlotType SlotType, UASItem* NewItem)
{
	ConstItemPtrBoolPair ResultPair(nullptr, false);

	if (SlotType != EArmorSlotType::SlotNum)
	{
		if (NewItem != nullptr && NewItem->GetItemType() == EItemType::Armor)
		{
			int32 Idx = static_cast<int32>(SlotType);
			if (ArmorSlots[Idx] == nullptr)
			{
				if (NewItem != nullptr)
				{
					NewItem->SetOwner(GetOwner());
				}
				ArmorSlots[Idx] = NewItem;

				ResultPair.Key = ArmorSlots[Idx];
				ResultPair.Value = true;

				OnArmorInserted(SlotType, Cast<UASArmor>(ArmorSlots[Idx]));
			}
			else
			{
				ResultPair.Key = ArmorSlots[Idx];
				ResultPair.Value = false;

				AS_LOG_S(Error);
			}
		}
		else
		{
			AS_LOG_S(Error);
		}
	}
	else
	{
		checkNoEntry();
	}

	return ResultPair;
}

ItemBoolPair UASInventoryComponent::RemoveItemFromArmorSlot(EArmorSlotType SlotType)
{
	ItemBoolPair ResultPair(nullptr, false);

	if (SlotType != EArmorSlotType::SlotNum)
	{
		int32 Idx = static_cast<int32>(SlotType);

		ResultPair.Key = ArmorSlots[Idx];
		ResultPair.Value = true;

		if (ResultPair.Key != nullptr)
		{
			ResultPair.Key->SetOwner(nullptr);
		}

		ArmorSlots[Idx] = nullptr;

		OnArmorRemoved(Cast<UASArmor>(ResultPair.Key));
	}
	else
	{
		checkNoEntry();
	}

	return ResultPair;
}

ItemBoolPair UASInventoryComponent::GetItemFromWeaponSlot(EWeaponSlotType SlotType)
{
	ItemBoolPair ResultPair(nullptr, false);

	if (SlotType != EWeaponSlotType::SlotNum)
	{
		ResultPair.Key = WeaponSlots[static_cast<int32>(SlotType)];
		ResultPair.Value = true;
	}
	else
	{
		checkNoEntry();
	}

	return ResultPair;
}

ItemBoolPair UASInventoryComponent::GetItemFromArmorSlot(EArmorSlotType SlotType)
{
	ItemBoolPair ResultPair(nullptr, false);

	if (SlotType != EArmorSlotType::SlotNum)
	{
		ResultPair.Key = ArmorSlots[static_cast<int32>(SlotType)];
		ResultPair.Value = true;
	}
	else
	{
		checkNoEntry();
	}

	return ResultPair;
}

void UASInventoryComponent::OnWeaponInserted(EWeaponSlotType SlotType, UASWeapon* InsertedWeapon)
{
	if (InsertedWeapon == nullptr)
		return;

	if (SlotType == EWeaponSlotType::SlotNum)
	{
		checkNoEntry();
		return;
	}

	if (SelectedWeapon != nullptr)
	{
		// 이미 들고 있는 무기가 있다.
		SpawnWeaponActor(*InsertedWeapon, GetProperWeaponSocketName(InsertedWeapon->GetWeaponType(), false));
	}
	else
	{
		SpawnWeaponActor(*InsertedWeapon, GetProperWeaponSocketName(InsertedWeapon->GetWeaponType(), true));
		SelectedWeapon = InsertedWeapon;
		SelectedWeaponSlotType = SlotType;
	}
}

void UASInventoryComponent::OnArmorInserted(EArmorSlotType SlotType, UASArmor* InsertedArmor)
{
	if (InsertedArmor == nullptr)
		return;

	switch (InsertedArmor->GetArmorType())
	{
	case EArmorType::Helmet:
		SpawnArmorActor(*InsertedArmor, HelmetSocketName);
		break;
	case EArmorType::Jacket:
		SpawnArmorActor(*InsertedArmor, JacketSocketName);
		break;
	default:
		checkNoEntry();
		break;
	}
}

void UASInventoryComponent::OnWeaponRemoved(EWeaponSlotType SlotType, UASWeapon* RemovedWeapon)
{
	if (SelectedWeaponSlotType == SlotType)
	{
		SelectedWeaponSlotType = EWeaponSlotType::SlotNum;
	}

	if (RemovedWeapon != nullptr)
	{	
		if (SelectedWeapon == RemovedWeapon)
		{
			SelectedWeapon = nullptr;
		}

		TWeakObjectPtr<AASWeaponActor>& WeaponActor = RemovedWeapon->GetActor();
		if (WeaponActor.IsValid())
		{
			WeaponActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			WeaponActor->SetActorHiddenInGame(true);
			WeaponActor->Destroy();
		}

		WeaponActor.Reset();
	}
}

void UASInventoryComponent::OnArmorRemoved(UASArmor* RemovedArmor)
{
	if (RemovedArmor != nullptr)
	{
		TWeakObjectPtr<AASArmorActor>& ArmorActor = RemovedArmor->GetActor();
		if (ArmorActor.IsValid())
		{
			ArmorActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			ArmorActor->SetActorHiddenInGame(true);
			ArmorActor->Destroy();
		}

		ArmorActor.Reset();
	}	
}

void UASInventoryComponent::OnSelectedWeaponChanged(UASWeapon* OldWeapon, UASWeapon* NewWeapon)
{
	if (OldWeapon != nullptr)
	{
		TWeakObjectPtr<AASWeaponActor>& OldWeaponActor = OldWeapon->GetActor();
		if (OldWeaponActor.IsValid())
		{
			OldWeaponActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

			if (auto ASChar = Cast<AASCharacter>(GetOwner()))
			{
				OldWeaponActor->AttachToComponent(ASChar->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform,
					GetProperWeaponSocketName(OldWeapon->GetWeaponType(), false));
			}
		}
	}

	if (NewWeapon != nullptr)
	{
		TWeakObjectPtr<AASWeaponActor>& NewWeaponActor = NewWeapon->GetActor();
		if (NewWeaponActor.IsValid())
		{
			NewWeaponActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			
			if (auto ASChar = Cast<AASCharacter>(GetOwner()))
			{
				NewWeaponActor->AttachToComponent(ASChar->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform,
					GetProperWeaponSocketName(NewWeapon->GetWeaponType(), true));
			}
		}
	}
}

void UASInventoryComponent::SpawnWeaponActor(UASWeapon& Weapon, const FName& AttachSocket)
{
	auto WeaponDataAsset = Cast<UASWeaponDataAsset>(Weapon.GetDataAsset());
	if (WeaponDataAsset == nullptr)
	{
		AS_LOG_S(Error);
		return;
	}

	auto NewWeaponActor = GetWorld()->SpawnActor<AASWeaponActor>(WeaponDataAsset->ASWeaponActorClass);
	if (NewWeaponActor == nullptr)
	{
		AS_LOG_S(Error);
		return;
	}

	auto ASChar = Cast<AASCharacter>(GetOwner());
	if (ASChar == nullptr)
	{
		AS_LOG_S(Error);

		NewWeaponActor->SetActorHiddenInGame(true);
		NewWeaponActor->Destroy();
		return;
	}

	NewWeaponActor->SetOwner(ASChar);
	NewWeaponActor->AttachToComponent(ASChar->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachSocket);

	Weapon.GetActor() = NewWeaponActor;
}

void UASInventoryComponent::SpawnArmorActor(UASArmor& Armor, const FName& AttachSocket)
{
	auto ArmorDataAsset = Cast<UASArmorDataAsset>(Armor.GetDataAsset());
	if (ArmorDataAsset == nullptr)
	{
		AS_LOG_S(Error);
		return;
	}

	auto NewArmorActor = GetWorld()->SpawnActor<AASArmorActor>(ArmorDataAsset->ASArmorActorClass);
	if (NewArmorActor == nullptr)
	{
		AS_LOG_S(Error);
		return;
	}

	auto ASChar = Cast<AASCharacter>(GetOwner());
	if (ASChar == nullptr)
	{
		AS_LOG_S(Error);

		NewArmorActor->SetActorHiddenInGame(true);
		NewArmorActor->Destroy();
		return;
	}

	NewArmorActor->SetOwner(ASChar);
	NewArmorActor->AttachToComponent(ASChar->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachSocket);

	Armor.GetActor() = NewArmorActor;
}

EWeaponSlotType UASInventoryComponent::GetWeaponSlotTypeFromWeapon(UASWeapon* InWeapon)
{
	if (InWeapon != nullptr)
	{
		int32 SlotNum = static_cast<int32>(EWeaponSlotType::SlotNum);
		for (int32 Idx = 0; Idx < SlotNum; ++Idx)
		{
			if (WeaponSlots[Idx] == InWeapon)
				return static_cast<EWeaponSlotType>(Idx);
		}
	}

	return EWeaponSlotType::SlotNum;
}

const FName& UASInventoryComponent::GetProperWeaponSocketName(EWeaponType WeaponType, bool bUsing) const
{
	if (bUsing)
	{
		switch (WeaponType)
		{
		case EWeaponType::Pistol:
			return UsingWeaponPistolSocketName;
		case EWeaponType::AssaultRifle:
			return UsingWeaponSocketName;
		default:
			checkNoEntry();
			break;
		}

		return UsingWeaponSocketName;
	}
	else
	{
		switch (WeaponType)
		{
		case EWeaponType::Pistol:
			return SideSocketName;
		case EWeaponType::AssaultRifle:
			return BackSocketName;
		default:
			checkNoEntry();
			break;
		}

		return BackSocketName;
	}
}

void UASInventoryComponent::OnRep_WeaponSlots(TArray<UASItem*>& OldWeaponSlots)
{
	for (int32 Idx = 0; Idx < WeaponSlots.Num(); ++Idx)
	{
		if (WeaponSlots[Idx] != OldWeaponSlots[Idx])
		{
			OnInsertWeapon.Broadcast(static_cast<EWeaponSlotType>(Idx), Cast<UASWeapon>(OldWeaponSlots[Idx]));
		}
	}
}

void UASInventoryComponent::OnRep_ArmorSlots(TArray<UASItem*>& OldArmorSlots)
{
	for (int32 Idx = 0; Idx < ArmorSlots.Num(); ++Idx)
	{
		if (ArmorSlots[Idx] != OldArmorSlots[Idx])
		{
			OnInsertArmor.Broadcast(static_cast<EArmorSlotType>(Idx), Cast<UASArmor>(OldArmorSlots[Idx]));
		}
	}
}

void UASInventoryComponent::OnRep_SelectedWeapon(UASWeapon* OldWeapon)
{
}
