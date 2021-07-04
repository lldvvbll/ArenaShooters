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

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved,
		FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	bool IsSprinted() const;
	float GetTotalTurnValue() const;
	EWeaponType GetUsingWeaponType() const;
	FRotator GetAimOffsetRotator() const;
	bool CanAim() const;
	EShootingStanceType GetShootingStance() const;

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
	void ReleasedAimButton();
	void SelectMainWeapon();
	void SelectSubWeapon();
	void Shoot();

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
	void ServerChangeShootingStance(EShootingStanceType NewShootingStance);
	void ServerChangeShootingStance_Implementation(EShootingStanceType NewShootingStance);

	UFUNCTION()
	void OnRep_ShootingStance(EShootingStanceType OldShootingStance);

	void Aim(bool bIsAiming);
	void Scope(bool bIsScoping);

	UFUNCTION(Server, Reliable)
	void ServerShoot(const FVector& MuzzleLocation, const FRotator& MuzzleRotation);
	void ServerShoot_Implementation(const FVector& MuzzleLocation, const FRotator& MuzzleRotation);

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

	UPROPERTY(EditDefaultsOnly, Category = Aiming, Meta = (AllowPrivateAccess = true))
	float MaxAimKeyHoldTime;

	UPROPERTY(Replicated)
	FRotator AimOffsetRotator;

	UPROPERTY(EditDefaultsOnly, Category = Aiming, Meta = (AllowPrivateAccess = true))
	float AimingSpeedRate;

	UPROPERTY(ReplicatedUsing = OnRep_ShootingStance)
	EShootingStanceType ShootingStance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Test, Meta = (AllowPrivateAccess = true))
	FPrimaryAssetId TestARAssetId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Test, Meta = (AllowPrivateAccess = true))
	FPrimaryAssetId TestPistolAssetId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Test, Meta = (AllowPrivateAccess = true))
	FPrimaryAssetId TestArmorAssetId;
};

