// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ArenaShooters.h"
#include "GameFramework/Character.h"
#include "ASCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UASActionComponent;

UCLASS()
class ARENASHOOTERS_API AASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AASCharacter();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void Jump() override;
	virtual void Falling() override;
	virtual bool CanCrouch() const override;

	bool IsSprinted() const;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	void Sprint();
	void SprintEnd();
	void ToggleCrouch();

	UFUNCTION(Server, Reliable)
	void ServerSprint();
	void ServerSprint_Implementation();

	UFUNCTION(Server, Reliable)
	void ServerSpintEnd();
	void ServerSpintEnd_Implementation();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = true))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = true))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Action, Meta = (AllowPrivateAccess = true))
	UASActionComponent* ASAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = true))
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = true))
	float BaseLookUpRate;

	UPROPERTY(Replicated)
	bool bSprinted;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement, Meta = (AllowPrivateAccess = true))
	float SprintSpeedRate;
};

