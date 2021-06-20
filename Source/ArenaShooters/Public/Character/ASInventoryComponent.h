// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Components/ActorComponent.h"
#include "Common/ASEnums.h"
#include "ASInventoryComponent.generated.h"

class UASItem;
class UASWeapon;
class UASArmor;
class UASWeaponDataAsset;
class UASArmorDataAsset;
class AASWeaponActor;
class AASArmorActor;

using ItemBoolPair = TPair<UASItem*, bool>;
using ConstItemPtrBoolPair = TPair<TWeakObjectPtr<const UASItem>, bool>;

UCLASS()
class ARENASHOOTERS_API UASInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UASInventoryComponent();

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const EWeaponType GetSelectedWeaponType() const;

	bool InsertWeapon(EWeaponSlotType SlotType, UASWeapon* NewWeapon, UASItem*& Out_OldItem);
	bool InsertArmor(EArmorSlotType SlotType, UASArmor* NewArmor, UASItem*& Out_OldItem);

	void SelectWeapon(EWeaponSlotType SlotType);

	ConstItemPtrBoolPair FindItemFromWeaponSlot(EWeaponSlotType SlotType) const;
	ConstItemPtrBoolPair SetItemToWeaponSlot(EWeaponSlotType SlotType, UASItem* NewItem);
	ItemBoolPair RemoveItemFromWeaponSlot(EWeaponSlotType SlotType);

	ConstItemPtrBoolPair GetItemFromArmorSlot(EArmorSlotType SlotType) const;
	ConstItemPtrBoolPair SetItemToArmorSlot(EArmorSlotType SlotType, UASItem* NewItem);
	ItemBoolPair RemoveItemFromArmorSlot(EArmorSlotType SlotType);

private:
	ItemBoolPair GetItemFromWeaponSlot(EWeaponSlotType SlotType);
	ItemBoolPair GetItemFromArmorSlot(EArmorSlotType SlotType);

	void OnWeaponInserted(EWeaponSlotType SlotType, UASWeapon* InsertedWeapon);
	void OnArmorInserted(EArmorSlotType SlotType, UASArmor* InsertedArmor);
	void OnWeaponRemoved(EWeaponSlotType SlotType, UASWeapon* RemovedWeapon);
	void OnArmorRemoved(UASArmor* RemovedArmor);
	void OnSelectedWeaponChanged(UASWeapon* OldWeapon, UASWeapon* NewWeapon);

	void SpawnWeaponActor(UASWeapon& Weapon, const FName& AttachSocket);
	void SpawnArmorActor(UASArmor& Armor, const FName& AttachSocket);

	EWeaponSlotType GetWeaponSlotTypeFromWeapon(UASWeapon* InWeapon);

public:
	DECLARE_EVENT_TwoParams(UASInventoryComponent, FOnEquipWeaponEvent, EWeaponSlotType, UASWeapon*);
	FOnEquipWeaponEvent OnEquipWeapon;

	DECLARE_EVENT_TwoParams(UASInventoryComponent, FOnEquipArmorEvent, EArmorSlotType, UASArmor*);
	FOnEquipArmorEvent OnEquipArmor;

	static const FName UsingWeaponSocketName;
	static const FName BackSocketName;
	static const FName SideSocketName;
	static const FName HelmetSocketName;
	static const FName JacketSocketName;

private:
	UPROPERTY(Replicated)
	TArray<UASItem*> WeaponSlots;

	UPROPERTY(Replicated)
	TArray<UASItem*> ArmorSlots;

	UPROPERTY(Replicated)
	TWeakObjectPtr<UASWeapon> SelectedWeapon;
};
