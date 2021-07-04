// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor/ASBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"

AASBullet::AASBullet()
{
	PrimaryActorTick.bCanEverTick = true;
	SetCanBeDamaged(false);
	bReplicates = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("Bullet"));
	Collision->CanCharacterStepUpOn = ECB_No;

	Projectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile"));
	Projectile->InitialSpeed = 12000.0f;
	Projectile->MaxSpeed = 15000.0f;

	Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));

	RootComponent = Collision;
	Particle->SetupAttachment(RootComponent);
}

void AASBullet::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	AS_LOG_S(Warning);
	AS_LOG_SCREEN(1.0f, FColor::Yellow, TEXT("AASBullet::NotifyHit()"));
}
