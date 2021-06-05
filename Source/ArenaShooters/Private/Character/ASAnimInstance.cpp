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
	CurrentSpeed = CharVelocity.Size();
	Direction = CalculateDirection(CharVelocity, ASChar->GetActorRotation());
	
	bCrouched = ASChar->bIsCrouched;
	bSprinted = ASChar->IsSprinted();
}

void UASAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ASChar = Cast<AASCharacter>(TryGetPawnOwner());

	if (UCharacterMovementComponent* MoveComp = ASChar->GetCharacterMovement())
	{
		MaxWalkSpeedCrouched = MoveComp->MaxWalkSpeedCrouched;
	}
}
