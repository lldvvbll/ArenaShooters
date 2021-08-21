// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Components/ActorComponent.h"
#include "ASDamageComponent.generated.h"

class UASStatusComponent;
class AASBullet;

UCLASS()
class ARENASHOOTERS_API UASDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UASDamageComponent();

	virtual void BeginPlay() override;

	void TakeBulletDamage(AASBullet* InBullet, const FHitResult& InHit);

private:
	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

private:
	UPROPERTY()
	AASCharacter* ASChar;

	UPROPERTY()
	UASStatusComponent* ASStatusComp;
};
