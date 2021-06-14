// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "UObject/NoExportTypes.h"
#include "ASItem.generated.h"

class UASItemDataAsset;
class AASItemActor;

UCLASS(Abstract)
class ARENASHOOTERS_API UASItem : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;

	void SetDataAsset(UASItemDataAsset* NewDataAsset);
	const UASItemDataAsset* GetDataAsset() const;

	template <typename T>
	const T* GetDataAsset() const
	{
		return Cast<T>(DataAsset);
	}

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	UASItemDataAsset* DataAsset;
};
