// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor/ASBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/ASCharacter.h"

AASBullet::AASBullet()
{
	PrimaryActorTick.bCanEverTick = false;
	SetCanBeDamaged(false);
	bReplicates = true;
	Damage = 0.0f;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("Bullet"));
	Collision->CanCharacterStepUpOn = ECB_No;

	Projectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile"));
	Projectile->InitialSpeed = 12000.0f;
	Projectile->MaxSpeed = 15000.0f;

	TraceParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TraceParticle"));
	DefaultSparkParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("SparkParticle"));
	
	RootComponent = Collision;
	TraceParticle->SetupAttachment(RootComponent);
}

void AASBullet::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (GetLocalRole() == ROLE_Authority)
	{
		SetLifeSpan(1.0f);
	}
	else
	{
		if (TraceParticle != nullptr)
		{
			TraceParticle->Deactivate();
		}

		if (Other == nullptr || !Other->IsA(AASCharacter::StaticClass()))
		{
			// юс╫ц
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DefaultSparkParticle, Hit.ImpactPoint);
		}		
	}
}

float AASBullet::GetDamage() const
{
	return Damage;
}

void AASBullet::SetDamage(float NewDamage)
{
	Damage = NewDamage;
}
