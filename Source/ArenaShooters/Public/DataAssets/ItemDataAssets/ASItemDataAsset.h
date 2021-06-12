// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Engine/DataAsset.h"
#include "Common/ASEnums.h"
#include "ASItemDataAsset.generated.h"

UCLASS(Abstract, BlueprintType)
class ARENASHOOTERS_API UASItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UASItemDataAsset();

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FPrimaryAssetType AssetType;

	UPROPERTY()
	EItemType ItemType;
};
