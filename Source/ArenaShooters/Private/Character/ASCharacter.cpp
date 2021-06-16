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

AASCharacter::AASCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;	
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(100.0f, 50.0f, 100.0f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	ASAction = CreateDefaultSubobject<UASActionComponent>(TEXT("ASAction"));
	ASInventory = CreateDefaultSubobject<UASInventoryComponent>(TEXT("ASInventory"));

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	SprintSpeedRate = 1.6f;

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

	bReplicates = true;
}

void AASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AASCharacter, bSprinted);
	DOREPLIFETIME_CONDITION(AASCharacter, TurnValue, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AASCharacter, TurnRateValue, COND_SimulatedOnly);

	// Dummy
	DOREPLIFETIME(AASCharacter, WeaponActor);
	DOREPLIFETIME(AASCharacter, HelmetActor);
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

EWeaponType AASCharacter::GetCurrentWeaponType() const
{
	return ASInventory->GetWeaponType();
}

void AASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AASCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AASCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AASCharacter::SprintEnd);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AASCharacter::ToggleCrouch);

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
	ServerSprint();
}

void AASCharacter::SprintEnd()
{
	ServerSpintEnd();
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

void AASCharacter::ServerSprint_Implementation()
{
	SetMaxWalkSpeedRate(SprintSpeedRate);
	bSprinted = true;

	ASInventory->CreateTestItem();
}

void AASCharacter::ServerSpintEnd_Implementation()
{
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
