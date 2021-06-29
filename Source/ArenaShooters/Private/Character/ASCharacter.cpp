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
	AimingCamOffset = FVector(0.0f, 30.0f, 80.0f);
	AimingCamArmLength = 100.0f;
	SprintSpeedRate = 1.6f;
	MaxAimKeyHoldTime = 0.3f;

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
			ServerAiming(true);
		}
	}

	if (bAiming || bScoping)
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
	DOREPLIFETIME(AASCharacter, bAiming);
	DOREPLIFETIME_CONDITION(AASCharacter, AimOffsetRotator, COND_SimulatedOnly); 
	DOREPLIFETIME(AASCharacter, bScoping);
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

		if (bAiming)
		{
			ServerAiming(false);
		}
		if (bScoping)
		{
			ServerScope(false);
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

bool AASCharacter::IsAiming() const
{
	return bAiming;
}

FRotator AASCharacter::GetAimOffsetRotator() const
{
	return AimOffsetRotator;
}

bool AASCharacter::CanAim() const
{
	return !bAiming && !bScoping && (GetUsingWeaponType() != EWeaponType::None) && !GetCharacterMovement()->IsFalling();
}

bool AASCharacter::IsScoping() const
{
	return bScoping;
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
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AASCharacter::ReleasedAmiButton);
	PlayerInputComponent->BindAction("SelectMainWeapon", IE_Pressed, this, &AASCharacter::SelectMainWeapon);
	PlayerInputComponent->BindAction("SelectSubWeapon", IE_Pressed, this, &AASCharacter::SelectSubWeapon);

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

		if ((bAiming || bScoping) && Value > 0.0f)
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
	if (bSprinted || (bAiming || bScoping))
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

void AASCharacter::ReleasedAmiButton()
{
	if (!bAiming && bPressedAimButton)
	{
		ServerScope(!bScoping);
	}
	else
	{
		ServerAiming(false);
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

void AASCharacter::ResetAimKeyState()
{
	bPressedAimButton = false;
	AimKeyHoldTime = 0.0f;
}

void AASCharacter::ServerSprint_Implementation()
{
	if (bSprinted || bAiming)
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

	if (bAiming)
	{
		ServerAiming_Implementation(false);
	}
	if (bScoping)
	{
		ServerScope_Implementation(false);
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

void AASCharacter::ServerAiming_Implementation(bool bIsAiming)
{
	if (bAiming == bIsAiming)
		return;

	if (bIsAiming)
	{
		if (!CanAim())
			return;

		bAiming = true;
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
		bAiming = false;
		SetMaxWalkSpeedRate(1.0f);

		if (CameraBoom != nullptr)
		{
			CameraBoom->TargetArmLength = NormalCamArmLength;
			CameraBoom->SocketOffset = NormalCamOffset;
		}
	}
}

void AASCharacter::OnRep_bAiming()
{
	SetMaxWalkSpeedRate(bAiming ? AimingSpeedRate : 1.0f);

	if (CameraBoom != nullptr)
	{
		CameraBoom->TargetArmLength = bAiming ? AimingCamArmLength : NormalCamArmLength;
		CameraBoom->SocketOffset =  bAiming ? AimingCamOffset : NormalCamOffset;
	}
}

void AASCharacter::ServerScope_Implementation(bool bIsScoping)
{
	if (bScoping == bIsScoping)
		return;

	if (bIsScoping)
	{
		if (!CanAim())
			return;

		ServerSprintEnd_Implementation();

		bScoping = true;
		SetMaxWalkSpeedRate(AimingSpeedRate);
		ChangeViewTargetForScope(true);
	}
	else
	{
		bScoping = false;
		SetMaxWalkSpeedRate(1.0f);
		ChangeViewTargetForScope(false);
	}
}

void AASCharacter::OnRep_bScoping()
{
	if (bScoping)
	{
		SetMaxWalkSpeedRate(AimingSpeedRate);
	}
	else
	{
		SetMaxWalkSpeedRate(1.0f);
	}
}

void AASCharacter::ChangeViewTargetForScope(bool bScope)
{
	auto PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController == nullptr)
		return;

	if (bScope)
	{
		if (ASInventory != nullptr)
		{
			TWeakObjectPtr<UASWeapon> SelectedWeapon = ASInventory->GetSelectedWeapon();
			if (SelectedWeapon.IsValid())
			{
				if (AASWeaponActor* WeaponActor = SelectedWeapon->GetActor().Get())
				{
					PlayerController->SetViewTargetWithBlend(WeaponActor, 0.3f, EViewTargetBlendFunction::VTBlend_EaseOut);
				}
			}
		}
	}
	else
	{
		PlayerController->SetViewTargetWithBlend(this);
	}	
}
