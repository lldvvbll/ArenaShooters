// Fill out your copyright notice in the Description page of Project Settings.


#include "ASPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Character/ASCharacter.h"
#include "Item/ASWeapon.h"
#include "ItemActor/ASWeaponActor.h"

AASPlayerController::AASPlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> SimpleCrossHairClassFinder(
		TEXT("/Game/ArenaShooters/Blueprints/GUI/SimpleCrossHair.SimpleCrossHair_C"));
	if (SimpleCrossHairClassFinder.Succeeded())
	{
		SimpleCrossHairClass = SimpleCrossHairClassFinder.Class;
	}
}

void AASPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (auto ASChar = Cast<AASCharacter>(InPawn))
	{
		ASChar->OnScopeEvent.AddUObject(this, &AASPlayerController::OnScope);
		ASChar->OnUnscopeEvent.AddUObject(this, &AASPlayerController::OnUnscope);
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

void AASPlayerController::OnScope(const TWeakObjectPtr<UASWeapon>& UsingWeapon)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (UsingWeapon.IsValid())
		{
			if (AASWeaponActor* WeaponActor = UsingWeapon->GetActor().Get())
			{
				SetViewTargetWithBlend(WeaponActor, 0.3f, EViewTargetBlendFunction::VTBlend_EaseOut);
			}
		}
	}	

	if (IsLocalPlayerController())
	{
		ShowCrossHair(false);
	}
}

void AASPlayerController::OnUnscope()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		SetViewTargetWithBlend(GetCharacter());
	}

	if (IsLocalPlayerController())
	{
		ShowCrossHair(true);
	}
}

void AASPlayerController::ShowCrossHair(bool bShow)
{
	if (SimpleCrossHair != nullptr)
	{
		SimpleCrossHair->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
