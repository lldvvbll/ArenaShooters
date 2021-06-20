// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Item/ASItem.h"
#include "Common/ASEnums.h"
#include "ASWeapon.generated.h"

class UASWeaponDataAsset;
class AASWeaponActor;

UCLASS()
class ARENASHOOTERS_API UASWeapon : public UASItem
{
	GENERATED_BODY()
	
public:
	static UASWeapon* CreateFromDataAsset(UObject* Owner, UASWeaponDataAsset* DataAsset);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const EWeaponType GetWeaponType() const;
	bool IsEnableToEquip(EWeaponSlotType SlotType) const;

	TWeakObjectPtr<AASWeaponActor>& GetActor();
	const TWeakObjectPtr<AASWeaponActor>& GetActor() const;

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	TWeakObjectPtr<AASWeaponActor> ASWeaponActor;
};
