// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor/ASWeaponActor.h"
#include "Net/UnrealNetwork.h"

const FName AASWeaponActor::MuzzleSocketName = TEXT("MuzzleFlash");

AASWeaponActor::AASWeaponActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetCanBeDamaged(false);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

	ScopeCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ScopeCamera"));

	MuzzleFlashParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MuzzleFlashParticle"));
	MuzzleFlashParticle->SetAutoActivate(false);

	RootComponent = WeaponMesh;
	ScopeCamera->SetupAttachment(RootComponent);
	MuzzleFlashParticle->SetupAttachment(RootComponent, MuzzleSocketName);
}

FVector AASWeaponActor::GetMuzzleLocation() const
{
	if (WeaponMesh != nullptr)
	{
		return WeaponMesh->GetSocketLocation(MuzzleSocketName);
	}
	else
	{
		AS_LOG_S(Error);
		return GetActorLocation();
	}	
}

void AASWeaponActor::GetMuzzleLocationAndRotation(FVector& OutLocation, FRotator& OutRotation) const
{
	if (WeaponMesh != nullptr)
	{	
		OutLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);
		OutRotation = WeaponMesh->GetSocketRotation(MuzzleSocketName);
	}
	else
	{
		AS_LOG_S(Error);
		OutLocation = GetActorLocation();
		OutRotation = GetActorRotation();
	}	
}

void AASWeaponActor::MulticastPlayFireEffect_Implementation()
{
	if (!IsNetMode(NM_DedicatedServer))
	{
		if (MuzzleFlashParticle != nullptr)
		{
			if (MuzzleFlashParticle->IsActive())
			{
				MuzzleFlashParticle->ForceReset();
			}
			else
			{
				MuzzleFlashParticle->Activate();
			}
		}
	}
}
