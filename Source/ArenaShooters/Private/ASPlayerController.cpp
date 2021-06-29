// Fill out your copyright notice in the Description page of Project Settings.


#include "ASPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Character/ASCharacter.h"

AASPlayerController::AASPlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> SimpleCrossHairClassFinder(
		TEXT("/Game/ArenaShooters/Blueprints/GUI/SimpleCrossHair.SimpleCrossHair_C"));
	if (SimpleCrossHairClassFinder.Succeeded())
	{
		SimpleCrossHairClass = SimpleCrossHairClassFinder.Class;
	}
}

void AASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		SimpleCrossHair = CreateWidget<UUserWidget>(this, SimpleCrossHairClass);
		SimpleCrossHair->AddToViewport();
	}
}
