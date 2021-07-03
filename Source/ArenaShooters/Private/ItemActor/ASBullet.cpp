// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor/ASBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"

AASBullet::AASBullet()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));

	Projectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile"));

	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	Sphere->SetCollisionProfileName(TEXT("NoCollision"));

	RootComponent = Collision;
	Sphere->SetupAttachment(RootComponent);

	Projectile->InitialSpeed = 3000.0f;
	Projectile->MaxSpeed = 3000.0f;
}
