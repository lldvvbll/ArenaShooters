// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASWeapon.h"
#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "ItemActor/ASBullet.h"
#include "ItemActor/ASWeaponActor.h"
#include "Character/ASCharacter.h"

UASWeapon* UASWeapon::CreateFromDataAsset(UObject* Owner, UASWeaponDataAsset* DataAsset)
{
	if (DataAsset == nullptr)
		return nullptr;

	UASWeapon* NewItem = ::NewObject<UASWeapon>(Owner, DataAsset->ItemClass);
	NewItem->SetDataAsset(DataAsset);

	return NewItem;
}

void UASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UASWeapon, ASWeaponActor);
	DOREPLIFETIME(UASWeapon, CurrentFireMode);
	DOREPLIFETIME(UASWeapon, LastFireTick);
}

void UASWeapon::SetDataAsset(UASItemDataAsset* NewDataAsset)
{
	Super::SetDataAsset(NewDataAsset);

	LastFireTick = 0;
}

const EWeaponType UASWeapon::GetWeaponType() const
{
	auto WeaponDA = Cast<UASWeaponDataAsset>(GetDataAsset());
	check(WeaponDA);

	return WeaponDA->WeaponType;
}

bool UASWeapon::IsEnableToEquip(EWeaponSlotType SlotType) const
{
	auto WeaponDA = Cast<UASWeaponDataAsset>(GetDataAsset());
	check(WeaponDA);

	return WeaponDA->IsEnableToEquip(SlotType);
}

TWeakObjectPtr<AASWeaponActor>& UASWeapon::GetActor()
{
	return ASWeaponActor;
}

const TWeakObjectPtr<AASWeaponActor>& UASWeapon::GetActor() const
{
	return ASWeaponActor;
}

AASBullet* UASWeapon::Fire(AASCharacter* Owner, EShootingStanceType ShootingStance, const FVector& MuzzleLocation, const FRotator& MuzzleRotation)
{
	auto WeaponDA = Cast<UASWeaponDataAsset>(GetDataAsset());
	if (WeaponDA == nullptr)
		return nullptr;

	if (!CanFire())
		return nullptr;

	FActorSpawnParameters Param;
	Param.Owner = Owner;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	auto Bullet = GetWorld()->SpawnActor<AASBullet>(WeaponDA->ASBulletClass, MuzzleLocation, MuzzleRotation, Param);
	if (Bullet != nullptr)
	{
		Bullet->SetDamage(WeaponDA->Damage);

		LastFireTick = FDateTime::Now().GetTicks();
	}

	return Bullet;
}

EFireMode UASWeapon::GetFireMode() const
{
	return CurrentFireMode;
}

void UASWeapon::ChangeToNextFireMode()
{
	auto WeaponDA = Cast<UASWeaponDataAsset>(GetDataAsset());
	if (WeaponDA == nullptr || WeaponDA->FireModes.Num() == 0)
	{
		AS_LOG_S(Error);
		return;
	}

	int32 ModeNum = WeaponDA->FireModes.Num();
	for (int32 Idx = 0; Idx < ModeNum; ++Idx)
	{
		if (CurrentFireMode == WeaponDA->FireModes[Idx])
		{
			int32 NextIdx = Idx + 1;
			if (NextIdx == ModeNum)
			{
				NextIdx = 0;
			}

			CurrentFireMode = WeaponDA->FireModes[NextIdx];
			break;
		}
	}
}

int64 UASWeapon::GetFireInterval() const
{
	auto WeaponDA = Cast<UASWeaponDataAsset>(GetDataAsset());
	if (WeaponDA == nullptr)
	{
		AS_LOG_S(Error);
		return -1;
	}

	return WeaponDA->FireInterval;
}

bool UASWeapon::CanFire() const
{
	int64 FireInterval = GetFireInterval();
	if (FireInterval < 0)
		return false;

	int64 DeltaTick = FDateTime::Now().GetTicks() - LastFireTick;
	if (DeltaTick < (FireInterval * ETimespan::TicksPerMillisecond))
		return false;

	return true;
}

void UASWeapon::SetLastFireTick()
{
	LastFireTick = FDateTime::Now().GetTicks();
}

void UASWeapon::OnRep_CurrentFireMode()
{
	AS_LOG_SCREEN(1.0f, FColor::Yellow, TEXT("Current Fire Mode: %s"), (CurrentFireMode == EFireMode::SemiAuto ? TEXT("SemiAuto") : TEXT("FullAuto")));
}
