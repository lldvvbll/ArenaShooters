// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "GameFramework/PlayerController.h"
#include "ASPlayerController.generated.h"

class UUserWidget;
class UASWeapon;

UCLASS()
class ARENASHOOTERS_API AASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AASPlayerController();

	virtual void SetPawn(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;

	void OnScope(const TWeakObjectPtr<UASWeapon>& UsingWeapon);
	void OnUnscope();

	void ShowCrossHair(bool bShow);

private:
	UPROPERTY(EditDefaultsOnly, Category = UI, Meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> SimpleCrossHairClass;

	UPROPERTY()
	UUserWidget* SimpleCrossHair;
};
