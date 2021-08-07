// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Components/ActorComponent.h"
#include "Common/ASEnums.h"
#include "ASInventoryComponent.generated.h"

class UASItem;
class UASWeapon;
class UASArmor;
class UASAmmo;
class UASWeaponDataAsset;
class UASArmorDataAsset;
class AASWeaponActor;
class AASArmorActor;

using ItemBoolPair = TPair<UASItem*, bool>;
using ItemPtrBoolPair = TPair<TWeakObjectPtr<UASItem>, bool>;

UCLASS()
class ARENASHOOTERS_API UASInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UASInventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	TWeakObjectPtr<UASWeapon> GetSelectedWeapon() const;
	TWeakObjectPtr<AASWeaponActor> GetSelectedWeaponActor();
	const EWeaponType GetSelectedWeaponType() const;
	const EWeaponSlotType GetSelectedWeaponSlotType() const;

	static EWeaponSlotType GetSuitableWeaponSlotType(EWeaponType WeaponType);
	static EArmorSlotType GetSuitableArmorSlotType(EArmorType ArmorType);
	static bool IsSuitableWeaponSlot(EWeaponSlotType SlotType, const UASWeapon* Weapon);
	static bool IsSuitableArmorSlot(EArmorSlotType SlotType, const UASArmor* Armor);

	bool InsertWeapon(EWeaponSlotType SlotType, UASWeapon* NewWeapon, UASItem*& Out_OldItem);
	bool InsertArmor(EArmorSlotType SlotType, UASArmor* NewArmor, UASItem*& Out_OldItem);

	void SelectWeapon(EWeaponSlotType SlotType);

	ItemBoolPair RemoveItem(UASItem* InItem);

	ItemPtrBoolPair FindItemFromWeaponSlot(EWeaponSlotType SlotType) const;
	ItemPtrBoolPair SetItemToWeaponSlot(EWeaponSlotType SlotType, UASItem* NewItem);
	ItemBoolPair RemoveItemFromWeaponSlot(EWeaponSlotType SlotType);

	ItemPtrBoolPair FindItemFromArmorSlot(EArmorSlotType SlotType) const;
	ItemPtrBoolPair SetItemToArmorSlot(EArmorSlotType SlotType, UASItem* NewItem);
	ItemBoolPair RemoveItemFromArmorSlot(EArmorSlotType SlotType);

	bool IsEnableToAddItemToInventory(UASItem* NewItem) const;
	bool AddItemToInventory(UASItem* NewItem);
	TArray<TWeakObjectPtr<UASItem>> GetInventoryItems() const;
	bool Contains(UASItem* InItem) const;

	TArray<UASAmmo*> GetAmmos(EAmmoType AmmoType) const;
	void SetReloadingAmmo(UASAmmo* InAmmo);
	UASAmmo* GetReloadingAmmo() const;

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
	const FName& GetProperWeaponSocketName(EWeaponType WeaponType, bool bUsing) const;

	UFUNCTION()
	void OnRep_WeaponSlots(TArray<UASItem*>& OldWeaponSlots);

	UFUNCTION()
	void OnRep_ArmorSlots(TArray<UASItem*>& OldArmorSlots);

	UFUNCTION()
	void OnRep_SelectedWeapon(UASWeapon* OldWeapon);

	UASItem* FindItemFromInventory(UClass* InClass) const;

	UFUNCTION()
	void OnRep_InventoryItems(TArray<UASItem*>& OldInventoryItems);

public:
	DECLARE_EVENT_TwoParams(UASInventoryComponent, FOnInsertWeaponEvent, EWeaponSlotType, UASWeapon*);
	FOnInsertWeaponEvent OnInsertWeapon;

	DECLARE_EVENT_TwoParams(UASInventoryComponent, FOnInsertArmorEvent, EArmorSlotType, UASArmor*);
	FOnInsertArmorEvent OnInsertArmor;

	DECLARE_EVENT_OneParam(UASInventoryComponent, FOnAddInventoryItemEvent, const TWeakObjectPtr<UASItem>&);
	FOnAddInventoryItemEvent OnAddInventoryItem;

	DECLARE_EVENT_OneParam(UASInventoryComponent, FOnRemoveInventoryItemEvent, const TWeakObjectPtr<UASItem>&);
	FOnRemoveInventoryItemEvent OnRemoveInventoryItem;

	static const FName UsingWeaponSocketName;
	static const FName UsingWeaponPistolSocketName;
	static const FName BackSocketName;
	static const FName SideSocketName;
	static const FName HelmetSocketName;
	static const FName JacketSocketName;

private:
	UPROPERTY(ReplicatedUsing = OnRep_WeaponSlots)
	TArray<UASItem*> WeaponSlots;

	UPROPERTY(ReplicatedUsing = OnRep_ArmorSlots)
	TArray<UASItem*> ArmorSlots;

	UPROPERTY(ReplicatedUsing = OnRep_InventoryItems)
	TArray<UASItem*> InventoryItems;

	UPROPERTY(ReplicatedUsing = OnRep_SelectedWeapon)
	UASWeapon* SelectedWeapon;

	UPROPERTY(Replicated)
	EWeaponSlotType SelectedWeaponSlotType;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxInventoryItemCount;

	UPROPERTY()
	UASAmmo* ReloadingAmmo;
};
