// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ArenaShooters.h"
#include "GameFramework/Character.h"
#include "Common/ASEnums.h"
#include "ASCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UASActionComponent;
class UASInventoryComponent;

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
	float GetTotalTurnValue() const;

	EWeaponType GetCurrentWeaponType() const;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
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

	UFUNCTION()
	void OnRep_bSprinted();

	void SetMaxWalkSpeedRate(float Rate);

	UFUNCTION(Server, Reliable)
	void ServerSetTurnValue(float NewTurnValue);
	void ServerSetTurnValue_Implementation(float NewTurnValue);

	UFUNCTION(Server, Reliable)
	void ServerSetTurnRateValue(float NewTurnRateValue);
	void ServerSetTurnRateValue_Implementation(float NewTurnRateValue);

private:
	UPROPERTY(VisibleAnywhere, Category = Camera, Meta = (AllowPrivateAccess = true))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera, Meta = (AllowPrivateAccess = true))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = Action, Meta = (AllowPrivateAccess = true))
	UASActionComponent* ASAction;

	UPROPERTY(EditDefaultsOnly, Category = Inventory, Meta = (AllowPrivateAccess = true))
	UASInventoryComponent* ASInventory;

	UPROPERTY(VisibleAnywhere, Category = Camera, Meta = (AllowPrivateAccess = true))
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, Category = Camera, Meta = (AllowPrivateAccess = true))
	float BaseLookUpRate;

	UPROPERTY(ReplicatedUsing = OnRep_bSprinted)
	bool bSprinted;

	UPROPERTY(VisibleAnywhere, Category = Movement, Meta = (AllowPrivateAccess = true))
	float SprintSpeedRate;

	UPROPERTY(Replicated)
	float TurnValue;

	UPROPERTY(Replicated)
	float TurnRateValue;
};

