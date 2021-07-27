// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Item/ASItem.h"
#include "Common/ASEnums.h"
#include "ASWeapon.generated.h"

class UASWeaponDataAsset;
class AASWeaponActor;
class AASBullet;
class AASCharacter;

UCLASS()
class ARENASHOOTERS_API UASWeapon : public UASItem
{
	GENERATED_BODY()
	
public:
	static UASWeapon* CreateFromDataAsset(UWorld* World, AActor* NewOwner, UASWeaponDataAsset* DataAsset);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetDataAsset(UASItemDataAsset* NewDataAsset) override;

	const EWeaponType GetWeaponType() const;

	TWeakObjectPtr<AASWeaponActor>& GetActor();
	const TWeakObjectPtr<AASWeaponActor>& GetActor() const;

	AASBullet* Fire(EShootingStanceType ShootingStance, const FVector& MuzzleLocation, const FRotator& MuzzleRotation);

	EFireMode GetFireMode() const;
	void ChangeToNextFireMode();

	int64 GetFireInterval() const;
	bool CanFire() const;
	void SetLastFireTick();

	UFUNCTION()
	void OnRep_CurrentFireMode();

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	TWeakObjectPtr<AASWeaponActor> ASWeaponActor;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentFireMode, EditDefaultsOnly)
	EFireMode CurrentFireMode;

	UPROPERTY(Replicated)
	int64 LastFireTick;
};
