// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ASAnimInstance.h"
#include "Character/ASCharacter.h"

UASAnimInstance::UASAnimInstance()
{
}

void UASAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	AASCharacter* ASChar = Cast<AASCharacter>(TryGetPawnOwner());
	if (!::IsValid(ASChar))
		return;
	
	bInAir = ASChar->GetCharacterMovement()->IsFalling();
	CurrentSpeed = ASChar->GetVelocity().Size();
}
