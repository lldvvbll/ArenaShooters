// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "DataAssets/ItemDataAssets/ASItemDataAsset.h"
#include "Common/ASEnums.h"
#include "ASArmorDataAsset.generated.h"

class AASArmorActor;

UCLASS()
class ARENASHOOTERS_API UASArmorDataAsset : public UASItemDataAsset
{
	GENERATED_BODY()
	
public:
	UASArmorDataAsset();

public:
	UPROPERTY(EditAnywhere)
	EArmorType ArmorType;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AASArmorActor> ASArmorActorClass;
};
