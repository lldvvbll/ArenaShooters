// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/ASCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/ASActionComponent.h"
#include "Character/ASInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "ASAssetManager.h"
#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"
#include "DataAssets/ItemDataAssets/ASArmorDataAsset.h"
#include "Item/ASWeapon.h"
#include "Item/ASArmor.h"
#include "ItemActor/ASWeaponActor.h"
#include "ItemActor/ASArmorActor.h"
#include "GameFramework/PlayerInput.h"

AASCharacter::AASCharacter()
{
	bReplicates = true;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	NormalCamOffset = FVector(0.0f, 30.0f, 100.0f);
	NormalCamArmLength = 200.0f;
	AimingCamOffset = FVector(0.0f, 45.0f, 75.0f);
	AimingCamArmLength = 80.0f;
	AimingSpeedRate = 0.5f;
	SprintSpeedRate = 1.6f;
	MaxAimKeyHoldTime = 0.3f;
	ShootingStance = EShootingStanceType::None;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = NormalCamArmLength;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = NormalCamOffset;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	ASAction = CreateDefaultSubobject<UASActionComponent>(TEXT("ASAction"));
	ASInventory = CreateDefaultSubobject<UASInventoryComponent>(TEXT("ASInventory"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	UCharacterMovementComponent* CharMoveComp = GetCharacterMovement();
	CharMoveComp->JumpZVelocity = 600.f;
	CharMoveComp->AirControl = 0.0f;
	CharMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	CharMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	//CharMoveComp->bUseControllerDesiredRotation = true;
	//CharMoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
}

void AASCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bPressedAimButton && CanAim() && IsLocallyControlled())
	{
		AimKeyHoldTime += DeltaSeconds;
		if (AimKeyHoldTime >= MaxAimKeyHoldTime)
		{
			ResetAimKeyState();
			ServerChangeShootingStance(EShootingStanceType::Aiming);
		}
	}

	if (ShootingStance != EShootingStanceType::None)
	{
		if (IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
		{
			AimOffsetRotator = (GetControlRotation() - GetActorRotation()).GetNormalized();
		}
	}
}

void AASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AASCharacter, bSprinted);
	DOREPLIFETIME_CONDITION(AASCharacter, TurnValue, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AASCharacter, TurnRateValue, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AASCharacter, AimOffsetRotator, COND_SimulatedOnly); 
	DOREPLIFETIME(AASCharacter, ShootingStance);
}

void AASCharacter::Jump()
{
	if (IsLocallyControlled())
	{
		if (bIsCrouched)
		{
			UnCrouch(true);
			return;
		}
	}

	Super::Jump();
}

void AASCharacter::Falling()
{
	if (IsLocallyControlled())
	{
		if (bIsCrouched)
		{
			UnCrouch(true);
		}

		if (ShootingStance != EShootingStanceType::None)
		{
			ServerChangeShootingStance(EShootingStanceType::None);
		}
	}
}

bool AASCharacter::CanCrouch() const
{
	if (GetCharacterMovement()->IsFalling())
		return false;

	return Super::CanCrouch();
}

bool AASCharacter::IsSprinted() const
{
	return bSprinted;
}

float AASCharacter::GetTotalTurnValue() const
{
	return TurnValue + TurnRateValue;
}

EWeaponType AASCharacter::GetUsingWeaponType() const
{
	return (ASInventory != nullptr) ? ASInventory->GetSelectedWeaponType() : EWeaponType::None;
}

FRotator AASCharacter::GetAimOffsetRotator() const
{
	return AimOffsetRotator;
}

bool AASCharacter::CanAim() const
{
	return (ShootingStance == EShootingStanceType::None) && (GetUsingWeaponType() != EWeaponType::None) && !GetCharacterMovement()->IsFalling();
}

EShootingStanceType AASCharacter::GetShootingStance() const
{
	return ShootingStance;
}

void AASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AASCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AASCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AASCharacter::SprintEnd);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AASCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AASCharacter::PressedAimButton);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AASCharacter::ReleasedAimButton);
	PlayerInputComponent->BindAction("SelectMainWeapon", IE_Pressed, this, &AASCharacter::SelectMainWeapon);
	PlayerInputComponent->BindAction("SelectSubWeapon", IE_Pressed, this, &AASCharacter::SelectSubWeapon);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AASCharacter::Shoot);

	PlayerInputComponent->BindAxis("MoveForward", this, &AASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AASCharacter::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &AASCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AASCharacter::LookUpAtRate);
}

void AASCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (ASAction == nullptr)
		return;

	EMovementMode CurMode = GetCharacterMovement()->MovementMode.GetValue();
	switch (CurMode)
	{
	case EMovementMode::MOVE_Walking:		// fallthough
	case EMovementMode::MOVE_NavWalking:
		{
			ASAction->SetMovementState(EMovementState::Grounded);
		}		
		break;
	case EMovementMode::MOVE_Falling:
		{
			ASAction->SetMovementState(EMovementState::InAir);
		}		
		break;
	default:
		break;
	}
}

void AASCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		if (ShootingStance != EShootingStanceType::None && Value > 0.0f)
		{
			Value /= 2.0f;
		}

		AddMovementInput(Direction, Value);
	}
}

void AASCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
	}
}

void AASCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);

	TurnValue = Value;
	ServerSetTurnValue(Value);
}

void AASCharacter::TurnAtRate(float Rate)
{
	float Value = Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds();
	AddControllerYawInput(Value);

	TurnRateValue = Value;
	ServerSetTurnRateValue(Value);
}

void AASCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AASCharacter::Sprint()
{
	if (bSprinted || (ShootingStance != EShootingStanceType::None))
		return;

	ServerSprint();
}

void AASCharacter::SprintEnd()
{
	if (!bSprinted)
		return;

	ServerSprintEnd();
}

void AASCharacter::ToggleCrouch()
{
	if (IsLocallyControlled())
	{
		if (bIsCrouched)
		{
			UnCrouch(true);
		}
		else
		{
			Crouch(true);
		}
	}
}

void AASCharacter::PressedAimButton()
{
	bPressedAimButton = true;
	AimKeyHoldTime = 0.0f;
}

void AASCharacter::ReleasedAimButton()
{
	if (ShootingStance != EShootingStanceType::Aiming && bPressedAimButton)
	{
		ServerChangeShootingStance((ShootingStance == EShootingStanceType::None) ? EShootingStanceType::Scoping : EShootingStanceType::None);
	}
	else
	{
		ServerChangeShootingStance(EShootingStanceType::None);
	}

	ResetAimKeyState();
}

void AASCharacter::SelectMainWeapon()
{
	if (ASInventory == nullptr)
		return;
	if (ASInventory->GetSelectedWeaponSlotType() == EWeaponSlotType::Main)
		return;

	ServerSelectWeapon(EWeaponSlotType::Main);
}

void AASCharacter::SelectSubWeapon()
{
	if (ASInventory == nullptr)
		return;
	if (ASInventory->GetSelectedWeaponSlotType() == EWeaponSlotType::Sub)
		return;

	ServerSelectWeapon(EWeaponSlotType::Sub);
}

void AASCharacter::Shoot()
{
	if (ShootingStance == EShootingStanceType::None)
		return;
	if (ASInventory == nullptr)
		return;

	TWeakObjectPtr<AASWeaponActor> WeaponActor = ASInventory->GetSelectedWeaponActor();
	if (!WeaponActor.IsValid())
		return;
	
	FVector MuzzleLocation;
	FRotator MuzzleRotation;
	WeaponActor->GetMuzzleLocationAndRotation(MuzzleLocation, MuzzleRotation);

	ServerShoot(MuzzleLocation, MuzzleRotation);
}

void AASCharacter::ResetAimKeyState()
{
	bPressedAimButton = false;
	AimKeyHoldTime = 0.0f;
}

void AASCharacter::ServerSprint_Implementation()
{
	if (bSprinted || ShootingStance != EShootingStanceType::None)
		return;

	SetMaxWalkSpeedRate(SprintSpeedRate);
	bSprinted = true;
}

void AASCharacter::ServerSprintEnd_Implementation()
{
	if (!bSprinted)
		return;

	SetMaxWalkSpeedRate(1.0f);
	bSprinted = false;
}

void AASCharacter::OnRep_bSprinted()
{
	SetMaxWalkSpeedRate(bSprinted ? SprintSpeedRate : 1.0f);
}

void AASCharacter::SetMaxWalkSpeedRate(float Rate)
{
	UCharacterMovementComponent* CharMoveComp = GetCharacterMovement();
	auto DefaultCharMoveComp = GetDefault<UCharacterMovementComponent>();
	if (CharMoveComp != nullptr && DefaultCharMoveComp != nullptr)
	{
		CharMoveComp->MaxWalkSpeed = DefaultCharMoveComp->MaxWalkSpeed * Rate;
		CharMoveComp->MaxWalkSpeedCrouched = DefaultCharMoveComp->MaxWalkSpeedCrouched * Rate;
	}
}

void AASCharacter::ServerSetTurnValue_Implementation(float NewTurnValue)
{
	TurnValue = NewTurnValue;
}

void AASCharacter::ServerSetTurnRateValue_Implementation(float NewTurnRateValue)
{
	TurnRateValue = NewTurnRateValue;
}

void AASCharacter::ServerSelectWeapon_Implementation(EWeaponSlotType WeaponSlotType)
{
	if (ASInventory == nullptr)
		return;
	if (ASInventory->GetSelectedWeaponSlotType() == WeaponSlotType)
		return;

	if (ShootingStance != EShootingStanceType::None)
	{
		ServerChangeShootingStance(EShootingStanceType::None);
	}

	ConstItemPtrBoolPair ResultPair = ASInventory->FindItemFromWeaponSlot(WeaponSlotType);
	if (!ResultPair.Value)
		return;

	if (ResultPair.Key != nullptr)
	{
		ASInventory->SelectWeapon(WeaponSlotType);
	}
	else
	{
		FPrimaryAssetId& WeaponAssetId = (WeaponSlotType == EWeaponSlotType::Main) ? TestARAssetId : TestPistolAssetId;

		if (auto WeaponDataAsset = UASAssetManager::Get().GetDataAsset<UASWeaponDataAsset>(WeaponAssetId))
		{
			UASItem* OldWeapon = nullptr;
			if (ASInventory->InsertWeapon(WeaponSlotType, UASWeapon::CreateFromDataAsset(this, WeaponDataAsset), OldWeapon))
			{
				if (OldWeapon != nullptr)
				{
					AS_LOG_S(Error);
				}
			}
		}
	}
}

void AASCharacter::ServerChangeShootingStance_Implementation(EShootingStanceType NewShootingStance)
{
	if (ShootingStance == NewShootingStance)
		return;

	switch (ShootingStance)
	{
	case EShootingStanceType::None:
		break;
	case EShootingStanceType::Aiming:
		Aim(false);
		break;
	case EShootingStanceType::Scoping:
		Scope(false);
		break;
	default:
		AS_LOG_S(Error);
		break;
	}

	switch (NewShootingStance)
	{
	case EShootingStanceType::None:
		break;
	case EShootingStanceType::Aiming:
		Aim(true);
		break;
	case EShootingStanceType::Scoping:
		Scope(true);
		break;
	default:
		AS_LOG_S(Error);
		break;
	}
}

void AASCharacter::OnRep_ShootingStance(EShootingStanceType OldShootingStance)
{
	switch (OldShootingStance)
	{
	case EShootingStanceType::None:
		break;
	case EShootingStanceType::Aiming:
		{
			SetMaxWalkSpeedRate(1.0f);
			if (CameraBoom != nullptr)
			{
				CameraBoom->TargetArmLength = NormalCamArmLength;
				CameraBoom->SocketOffset = NormalCamOffset;
			}
		}		
		break;
	case EShootingStanceType::Scoping:
		{
			SetMaxWalkSpeedRate(1.0f);
			OnUnscopeEvent.Broadcast();
		}
		break;
	default:
		AS_LOG_S(Error);
		break;
	}

	switch (ShootingStance)
	{
	case EShootingStanceType::None:
		break;
	case EShootingStanceType::Aiming:
		{
			SetMaxWalkSpeedRate(AimingSpeedRate);
			if (CameraBoom != nullptr)
			{
				CameraBoom->TargetArmLength = AimingCamArmLength;
				CameraBoom->SocketOffset = AimingCamOffset;
			}
		}
		break;
	case EShootingStanceType::Scoping:
		{
			SetMaxWalkSpeedRate(AimingSpeedRate);

			TWeakObjectPtr<UASWeapon> SelectedWeapon;
			if (ASInventory != nullptr)
			{
				SelectedWeapon = ASInventory->GetSelectedWeapon();
			}
			OnScopeEvent.Broadcast(SelectedWeapon);
		}
		break;
	default:
		AS_LOG_S(Error);
		break;
	}
}

void AASCharacter::Aim(bool bIsAiming)
{
	if (bIsAiming)
	{
		if (!CanAim())
			return;

		ShootingStance = EShootingStanceType::Aiming;
		SetMaxWalkSpeedRate(AimingSpeedRate);
		ServerSprintEnd_Implementation();

		if (CameraBoom != nullptr)
		{
			CameraBoom->TargetArmLength = AimingCamArmLength;
			CameraBoom->SocketOffset = AimingCamOffset;
		}
	}
	else
	{
		ShootingStance = EShootingStanceType::None;
		SetMaxWalkSpeedRate(1.0f);

		if (CameraBoom != nullptr)
		{
			CameraBoom->TargetArmLength = NormalCamArmLength;
			CameraBoom->SocketOffset = NormalCamOffset;
		}
	}
}

void AASCharacter::Scope(bool bIsScoping)
{
	if (bIsScoping)
	{
		if (!CanAim())
			return;

		ServerSprintEnd_Implementation();

		ShootingStance = EShootingStanceType::Scoping;
		SetMaxWalkSpeedRate(AimingSpeedRate);

		TWeakObjectPtr<UASWeapon> SelectedWeapon;
		if (ASInventory != nullptr)
		{
			SelectedWeapon = ASInventory->GetSelectedWeapon();
		}
		OnScopeEvent.Broadcast(SelectedWeapon);
	}
	else
	{
		ShootingStance = EShootingStanceType::None;
		SetMaxWalkSpeedRate(1.0f);

		OnUnscopeEvent.Broadcast();
	}
}

void AASCharacter::ServerShoot_Implementation(const FVector& MuzzleLocation, const FRotator& MuzzleRotation)
{
	if (ShootingStance == EShootingStanceType::None)
		return;
	if (ASInventory == nullptr)
		return;

	const TWeakObjectPtr<UASWeapon>& SelectedWeapon = ASInventory->GetSelectedWeapon();
	if (!SelectedWeapon.IsValid())
		return;

	SelectedWeapon->Fire(ShootingStance, MuzzleLocation, MuzzleRotation);
}
