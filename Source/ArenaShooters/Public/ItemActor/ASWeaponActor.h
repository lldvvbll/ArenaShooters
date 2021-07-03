// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "GameFramework/Actor.h"
#include "ASWeaponActor.generated.h"

UCLASS()
class ARENASHOOTERS_API AASWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AASWeaponActor();

	void GetMuzzleLocationAndRotation(FVector& OutLocation, FRotator& OutRotation) const;

protected:
	static const FName MuzzleSocketName;

protected:
	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly)
	UCameraComponent* ScopeCamera;
};
