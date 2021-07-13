// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Engine/GameInstance.h"
#include "ASGameInstance.generated.h"

UCLASS()
class ARENASHOOTERS_API UASGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	float GetDamageRateByBone(const USkinnedMeshComponent* MeshComp, const FName& BoneName) const;

private:
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	TMap<FName, float> DamageRateByBoneMap;
};
