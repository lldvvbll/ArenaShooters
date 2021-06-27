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

	UFUNCTION(BlueprintPure, Meta = (BlueprintThreadSafe))
	bool IsActualSprinted() const;

private:
	UPROPERTY()
	AASCharacter* ASChar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Montage, Meta = (AllowPrivateAccess = true))
	UAnimMontage* RifleFireMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float CurrentSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bCrouched;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bSprinted;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float MaxWalkSpeedCrouched;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float CurrentRotationSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float TurnValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	EWeaponType CurrentWeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	bool bAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float AimYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	float AimPitch;
};
