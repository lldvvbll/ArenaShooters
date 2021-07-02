// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "GameFramework/Actor.h"
#include "ASBullet.generated.h"

class UProjectileMovementComponent;

UCLASS()
class ARENASHOOTERS_API AASBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	AASBullet();

protected:
	UPROPERTY(EditDefaultsOnly)
	USphereComponent* Collision;

	UPROPERTY(EditDefaultsOnly)
	UProjectileMovementComponent* Projectile;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Sphere;
};
