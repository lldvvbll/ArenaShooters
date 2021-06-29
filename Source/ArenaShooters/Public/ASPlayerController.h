// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "GameFramework/PlayerController.h"
#include "ASPlayerController.generated.h"

class UUserWidget;

UCLASS()
class ARENASHOOTERS_API AASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AASPlayerController();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = UI, Meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> SimpleCrossHairClass;

	UPROPERTY()
	UUserWidget* SimpleCrossHair;
};
