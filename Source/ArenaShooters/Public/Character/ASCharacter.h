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
class UASWeapon;
class UASArmor;
class AASWeaponActor;
class AASArmorActor;

UCLASS()
class ARENASHOOTERS_API AASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AASCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void Jump() override;
	virtual void Falling() override;
	virtual bool CanCrouch() const override;

	bool IsSprinted() const;
	float GetTotalTurnValue() const;
	EWeaponType GetUsingWeaponType() const;
	bool IsAiming() const;
	FRotator GetAimOffsetRotator() const;
	bool CanAim() const;
	bool IsScoping() const;

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
	void PressedAimButton();
	void ReleasedAmiButton();
	void SelectMainWeapon();
	void SelectSubWeapon();

	void ResetAimKeyState();

	UFUNCTION(Server, Reliable)
	void ServerSprint();
	void ServerSprint_Implementation();

	UFUNCTION(Server, Reliable)
	void ServerSprintEnd();
	void ServerSprintEnd_Implementation();

	UFUNCTION()
	void OnRep_bSprinted();

	void SetMaxWalkSpeedRate(float Rate);

	UFUNCTION(Server, Reliable)
	void ServerSetTurnValue(float NewTurnValue);
	void ServerSetTurnValue_Implementation(float NewTurnValue);

	UFUNCTION(Server, Reliable)
	void ServerSetTurnRateValue(float NewTurnRateValue);
	void ServerSetTurnRateValue_Implementation(float NewTurnRateValue);

	UFUNCTION(Server, Reliable)
	void ServerSelectWeapon(EWeaponSlotType WeaponSlotType);
	void ServerSelectWeapon_Implementation(EWeaponSlotType WeaponSlotType);

	UFUNCTION(Server, Reliable)
	void ServerAiming(bool bIsAiming);
	void ServerAiming_Implementation(bool bIsAiming);

	UFUNCTION()
	void OnRep_bAiming();

	UFUNCTION(Server, Reliable)
	void ServerScope(bool bIsScoping);
	void ServerScope_Implementation(bool bIsScoping);

	UFUNCTION()
	void OnRep_bScoping();

public:
	DECLARE_EVENT_OneParam(AASCharacter, FOnScopeEvent, const TWeakObjectPtr<UASWeapon>&)
	FOnScopeEvent OnScopeEvent;

	DECLARE_EVENT(AASCharacter, FOnUnscopeEvent)
	FOnUnscopeEvent OnUnscopeEvent;

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

	UPROPERTY(EditDefaultsOnly, Category = Camera, Meta = (AllowPrivateAccess = true))
	FVector NormalCamOffset;

	UPROPERTY(EditDefaultsOnly, Category = Camera, Meta = (AllowPrivateAccess = true))
	FVector AimingCamOffset;

	UPROPERTY(EditDefaultsOnly, Category = Camera, Meta = (AllowPrivateAccess = true))
	float NormalCamArmLength;

	UPROPERTY(EditDefaultsOnly, Category = Camera, Meta = (AllowPrivateAccess = true))
	float AimingCamArmLength;

	UPROPERTY(ReplicatedUsing = OnRep_bSprinted)
	bool bSprinted;

	UPROPERTY(VisibleAnywhere, Category = Movement, Meta = (AllowPrivateAccess = true))
	float SprintSpeedRate;

	UPROPERTY(Replicated)
	float TurnValue;

	UPROPERTY(Replicated)
	float TurnRateValue;

	bool bPressedAimButton;
	float AimKeyHoldTime;

	UPROPERTY(ReplicatedUsing = OnRep_bAiming)
	bool bAiming;

	UPROPERTY(EditDefaultsOnly, Category = Aiming, Meta = (AllowPrivateAccess = true))
	float MaxAimKeyHoldTime;

	UPROPERTY(Replicated)
	FRotator AimOffsetRotator;

	UPROPERTY(EditDefaultsOnly, Category = Aiming, Meta = (AllowPrivateAccess = true))
	float AimingSpeedRate;

	UPROPERTY(ReplicatedUsing = OnRep_bScoping)
	bool bScoping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Test, Meta = (AllowPrivateAccess = true))
	FPrimaryAssetId TestARAssetId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Test, Meta = (AllowPrivateAccess = true))
	FPrimaryAssetId TestPistolAssetId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Test, Meta = (AllowPrivateAccess = true))
	FPrimaryAssetId TestArmorAssetId;
};

