// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor/ASWeaponActor.h"
#include "Net/UnrealNetwork.h"

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
