// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Components/ActorComponent.h"
#include "Common/ASEnums.h"
#include "ASInventoryComponent.generated.h"

class UASItem;

UCLASS()
class ARENASHOOTERS_API UASInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	using ItemBoolPair = TPair<UASItem*, bool>;
	using ConstItemBoolPair = TPair<const UASItem*, bool>;

public:	
	UASInventoryComponent();

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void CreateTestItem();
	const EWeaponType GetWeaponType() const;

	ItemBoolPair GetItemFromWeaponSlot(EWeaponSlotType SlotType);
	ConstItemBoolPair GetItemFromWeaponSlot(EWeaponSlotType SlotType) const;
	ItemBoolPair SetItemToWeaponSlot(EWeaponSlotType SlotType, UASItem* NewItem);
	ItemBoolPair RemoveItemFromWeaponSlot(EWeaponSlotType SlotType);

	ItemBoolPair GetItemFromArmorSlot(EArmorSlotType SlotType);
	ConstItemBoolPair GetItemFromArmorSlot(EArmorSlotType SlotType) const;
	ItemBoolPair SetItemToArmorSlot(EArmorSlotType SlotType, UASItem* NewItem);
	ItemBoolPair RemoveItemFromArmorSlot(EArmorSlotType SlotType);

private:
	UPROPERTY(Replicated)
	TArray<UASItem*> WeaponSlots;

	UPROPERTY(Replicated)
	TArray<UASItem*> ArmorSlots;

	UPROPERTY(Replicated)
	EWeaponSlotType SelectedWeaponSlotType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Test, Meta = (AllowPrivateAccess = true))
	FPrimaryAssetId TestWeaponAssetId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Test, Meta = (AllowPrivateAccess = true))
	FPrimaryAssetId TestArmorAssetId;
};
