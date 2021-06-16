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

protected:
	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* WeaponMesh;
};
