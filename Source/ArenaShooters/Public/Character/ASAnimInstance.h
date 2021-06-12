// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Animation/AnimInstance.h"
#include "Common/ASEnums.h"
#include "ASAnimInstance.generated.h"

class AASCharacter;

UCLASS()
class ARENASHOOTERS_API UASAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UASAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;

private:
	UPROPERTY()
	AASCharacter* ASChar;

	UPROPERTY(VisibleAnywhere, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* RifleFireMontage;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	bool bInAir;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	float CurrentSpeed;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	float Direction;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	bool bCrouched;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	bool bSprinted;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	float MaxWalkSpeedCrouched;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	float CurrentRotationSpeed;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	float TurnValue;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = true))
	EWeaponType CurrentWeaponType;
};
