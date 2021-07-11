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
	CurrentWeaponType = ASChar->GetUsingWeaponType();
	ShootingStance = ASChar->GetShootingStance();
	if (ShootingStance != EShootingStanceType::None)
	{
		FRotator AimRot = ASChar->GetAimOffsetRotator();
		AimYaw = FMath::ClampAngle(AimRot.Yaw, -90.0f, 90.0f);
		AimPitch = FMath::ClampAngle(AimRot.Pitch, -90.0f, 90.0f);
	}
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

	bLocallyControlled = ASChar->IsLocallyControlled();
}

bool UASAnimInstance::IsActualSprinted() const
{
	return  bSprinted && !bInAir && (CurrentSpeed > 800.0f);
}

void UASAnimInstance::PlayShootMontage(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EWeaponType::Pistol:
		{
			Montage_Play(PistolShootMontage);
		}
		break;
	case EWeaponType::AssaultRifle:
		{
			Montage_Play(ARShootMontage);
		}
		break;
	default:
		AS_LOG_SCREEN(1.0f, FColor::Red, TEXT("UASAnimInstance::PlayShootMontage() - Invalid WeaponType"));
		break;
	}
}
