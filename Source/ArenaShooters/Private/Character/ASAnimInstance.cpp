// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ASAnimInstance.h"
#include "Character/ASCharacter.h"

UASAnimInstance::UASAnimInstance()
{
	MaxWalkSpeedCrouched = 300.0f;
}

void UASAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!::IsValid(ASChar))
		return;
	
	bInAir = ASChar->GetCharacterMovement()->IsFalling();

	FVector CharVelocity = ASChar->GetVelocity();
	FRotator CharRotation = ASChar->GetActorRotation();

	CurrentSpeed = CharVelocity.Size();
	Direction = CalculateDirection(CharVelocity, CharRotation);
	bCrouched = ASChar->bIsCrouched;
	bSprinted = ASChar->IsSprinted();
	TurnValue = ASChar->GetTotalTurnValue();
}

void UASAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ASChar = Cast<AASCharacter>(TryGetPawnOwner());
	if (!::IsValid(ASChar))
		return;

	if (UCharacterMovementComponent* MoveComp = ASChar->GetCharacterMovement())
	{
		MaxWalkSpeedCrouched = MoveComp->MaxWalkSpeedCrouched;
	}
}
