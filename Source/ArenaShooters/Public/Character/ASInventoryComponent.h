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

	ItemBoolPair GetItemFromEquipmentSlot(EEquipmentSlotType SlotType);
	ConstItemBoolPair GetItemFromEquipmentSlot(EEquipmentSlotType SlotType) const;
	ItemBoolPair SetItemToEquipmentSlot(EEquipmentSlotType SlotType, UASItem* NewItem);
	ItemBoolPair RemoveItemFromEquipmentSlot(EEquipmentSlotType SlotType);

private:
	int32 ConvertToIndex(EEquipmentSlotType SlotType) const;

private:
	UPROPERTY(Replicated)
	TArray<UASItem*> EquipmentSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Test, Meta = (AllowPrivateAccess = true))
	FPrimaryAssetId TestWeaponAssetId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Test, Meta = (AllowPrivateAccess = true))
	FPrimaryAssetId TestArmorAssetId;
};
