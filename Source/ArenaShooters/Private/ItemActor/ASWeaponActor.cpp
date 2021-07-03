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

	RootComponent = WeaponMesh;
	ScopeCamera->SetupAttachment(RootComponent);
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
		OutLocation = GetActorLocation();
		OutRotation = GetActorRotation();
		AS_LOG_S(Error);
	}	
}
