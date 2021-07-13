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
#include "Character/ASStatusComponent.h"
#include "Net/UnrealNetwork.h"
#include "ASAssetManager.h"
#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"
#include "DataAssets/ItemDataAssets/ASArmorDataAsset.h"
#include "Item/ASWeapon.h"
#include "Item/ASArmor.h"
#include "ItemActor/ASWeaponActor.h"
#include "ItemActor/ASArmorActor.h"
#include "ItemActor/ASBullet.h"
#include "GameFramework/PlayerInput.h"
#include "Character/ASAnimInstance.h"
#include "ASGameInstance.h"

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
	bPressedAimButton = false;
	AimKeyHoldTime = 0.0f;
	MaxAimKeyHoldTime = 0.3f;
	ShootingStance = EShootingStanceType::None;
	bPressedShootButton = false;

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
	ASStatus = CreateDefaultSubobject<UASStatusComponent>(TEXT("ASStatus"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->InitCapsuleSize(42.f, 96.0f);
	Capsule->SetCollisionProfileName(TEXT("ASCharacter"));

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

	if (bPressedAimButton && CanAimOrScope() && IsLocallyControlled())
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

	if (bPressedShootButton && IsLocallyControlled())
	{
		Shoot();
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

void AASCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (MyComp == GetMesh())
	{
		if (auto Bullet = Cast<AASBullet>(Other))
		{
			auto OtherChar = Cast<AASCharacter>(Other->GetOwner());
			if (OtherChar != nullptr)
			{
				float Damage = Bullet->GetDamage();
				FVector ShotDir = Bullet->GetActorForwardVector();
				FPointDamageEvent DamageEvent(Damage, Hit, ShotDir, TSubclassOf<UDamageType>(UDamageType::StaticClass()));

				TakeDamage(Damage, DamageEvent, OtherChar->GetController(), Bullet);
			}
		}
	}
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

EShootingStanceType AASCharacter::GetShootingStance() const
{
	return ShootingStance;
}

void AASCharacter::MulticastPlayShootMontage_Implementation()
{
	if (IsNetMode(NM_DedicatedServer))
		return;

	if (USkeletalMeshComponent* SkMesh = GetMesh())
	{
		if (auto AnimInstance = Cast<UASAnimInstance>(SkMesh->GetAnimInstance()))
		{
			AnimInstance->PlayShootMontage(GetUsingWeaponType());
		}
	}

	if (ASInventory != nullptr)
	{
		TWeakObjectPtr<AASWeaponActor> WeaponActor = ASInventory->GetSelectedWeaponActor();
		if (WeaponActor.IsValid())
		{
			WeaponActor->PlayFireAnim();
		}
	}
}

float AASCharacter::InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::InternalTakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);

	if (auto GameInst = GetGameInstance<UASGameInstance>())
	{
		ActualDamage *= GameInst->GetDamageRateByBone(GetMesh(), PointDamageEvent.HitInfo.BoneName);
	}

	if (ASStatus != nullptr)
	{
		ASStatus->ModifyCurrentHealth(-ActualDamage);
	}

	return ActualDamage;
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
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AASCharacter::PressedShootButton);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AASCharacter::ReleasedShootButton);
	PlayerInputComponent->BindAction("ChangeFireMode", IE_Pressed, this, &AASCharacter::ChangeFireMode);

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

void AASCharacter::PressedShootButton()
{
	const TWeakObjectPtr<UASWeapon>& Weapon = ASInventory->GetSelectedWeapon();
	if (!Weapon.IsValid())
		return;

	switch (Weapon->GetFireMode())
	{
	case EFireMode::SemiAuto:
		{
			Shoot();
		}
		break;
	case EFireMode::FullAuto:
		{
			bPressedShootButton = true;
		}
		break;
	default:
		AS_LOG_S(Error);
		break;
	}
}

void AASCharacter::ReleasedShootButton()
{
	bPressedShootButton = false;
}

void AASCharacter::ChangeFireMode()
{
	ServerChangeFireMode();
}

void AASCharacter::Shoot()
{
	if (ShootingStance == EShootingStanceType::None)
		return;
	if (ASInventory == nullptr)
		return;

	const TWeakObjectPtr<UASWeapon>& Weapon = ASInventory->GetSelectedWeapon();
	if (!Weapon.IsValid())
		return;
	if (!Weapon->CanFire())
		return;

	const TWeakObjectPtr<AASWeaponActor>& WeaponActor = Weapon->GetActor();
	if (!WeaponActor.IsValid())
		return;
	
	switch (ShootingStance)
	{
	case EShootingStanceType::Aiming:
		{
			FVector CamLoc = FollowCamera->GetComponentLocation();
			FVector CamForward = FollowCamera->GetForwardVector().GetSafeNormal();
			FVector MuzzleLoc = WeaponActor->GetMuzzleLocation();

			float LengthToStartLoc = FMath::Abs((CamLoc - MuzzleLoc) | CamForward);
			FVector TraceStartLoc = CamLoc + (CamForward * LengthToStartLoc);
			FVector TraceEndLoc = CamLoc + (CamForward * 15000.0f);

			FVector TargetLoc;

			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			if (GetWorld()->LineTraceSingleByProfile(HitResult, TraceStartLoc, TraceEndLoc, TEXT("Bullet"), QueryParams))
			{
				TargetLoc = HitResult.Location;
			}
			else
			{
				TargetLoc = TraceEndLoc;
			}
			
			FVector FireDir = (TargetLoc - MuzzleLoc).GetSafeNormal();
			FRotator FireRot = FRotationMatrix::MakeFromX(FireDir).Rotator();
			ServerShoot(MuzzleLoc, FireRot);
			
			Weapon->SetLastFireTick();
		}		
		break;
	case EShootingStanceType::Scoping:
		{
			FVector MuzzleLocation;
			FRotator MuzzleRotation;
			WeaponActor->GetMuzzleLocationAndRotation(MuzzleLocation, MuzzleRotation);

			ServerShoot(MuzzleLocation, MuzzleRotation);

			Weapon->SetLastFireTick();
		}
		break;
	default:
		AS_LOG_SA(Error);
		break;
	}
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
		{
			EndAiming();
		}		
		break;
	case EShootingStanceType::Scoping:
		{
			EndScoping();
		}
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
		if (CanAimOrScope())
		{
			StartAiming();
		}		
		break;
	case EShootingStanceType::Scoping:
		if (CanAimOrScope())
		{
			StartScoping();
		}		
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
			EndAiming();
		}		
		break;
	case EShootingStanceType::Scoping:
		{
			EndScoping();
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
			StartAiming();
		}
		break;
	case EShootingStanceType::Scoping:
		{
			StartScoping();
		}
		break;
	default:
		AS_LOG_S(Error);
		break;
	}
}

bool AASCharacter::CanAimOrScope() const
{
	return (ShootingStance == EShootingStanceType::None) && (GetUsingWeaponType() != EWeaponType::None) && !GetCharacterMovement()->IsFalling();
}

void AASCharacter::StartAiming()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		ShootingStance = EShootingStanceType::Aiming;
		ServerSprintEnd_Implementation();
	}	

	SetMaxWalkSpeedRate(AimingSpeedRate);
	if (CameraBoom != nullptr)
	{
		CameraBoom->TargetArmLength = AimingCamArmLength;
		CameraBoom->SocketOffset = AimingCamOffset;
	}
}

void AASCharacter::EndAiming()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		ShootingStance = EShootingStanceType::None;
	}

	SetMaxWalkSpeedRate(1.0f);
	if (CameraBoom != nullptr)
	{
		CameraBoom->TargetArmLength = NormalCamArmLength;
		CameraBoom->SocketOffset = NormalCamOffset;
	}
}

void AASCharacter::StartScoping()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		ShootingStance = EShootingStanceType::Scoping;
		ServerSprintEnd_Implementation();
	}

	SetMaxWalkSpeedRate(AimingSpeedRate);
	if (ASInventory != nullptr)
	{
		OnScopeEvent.Broadcast(ASInventory->GetSelectedWeapon());
	}	
}

void AASCharacter::EndScoping()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		ShootingStance = EShootingStanceType::None;
	}

	SetMaxWalkSpeedRate(1.0f);
	OnUnscopeEvent.Broadcast();
}

//bool AASCharacter::ServerShoot_Validate(const FVector& MuzzleLocation, const FRotator& ShootRotation)
//{
//	constexpr float ValidLocDiff = 200.0f;
//	constexpr float ValidRotDiff = 45.0f;
//
//	TWeakObjectPtr<AASWeaponActor> SelectedWeaponActor = ASInventory->GetSelectedWeaponActor();
//	if (!SelectedWeaponActor.IsValid())
//	{
//		AS_LOG_S(Error);
//		return false;
//	}
//
//	FVector LocDiff = SelectedWeaponActor->GetMuzzleLocation() - MuzzleLocation;
//	if (FMath::Abs(LocDiff.X) > ValidLocDiff || FMath::Abs(LocDiff.Y) > ValidLocDiff || FMath::Abs(LocDiff.Z) > ValidLocDiff)
//	{
//		AS_LOG(Error, TEXT("%s"), *LocDiff.ToString());
//		return false;
//	}
//	
//	FRotator RotDiff = (GetActorRotation() - ShootRotation).GetNormalized();
//	if (FMath::Abs(RotDiff.Pitch) > ValidRotDiff || FMath::Abs(RotDiff.Yaw) > ValidRotDiff)
//	{
//		AS_LOG(Error, TEXT("%s"), *RotDiff.ToString());
//		return false;
//	}
//
//	return true;
//}

void AASCharacter::ServerShoot_Implementation(const FVector& MuzzleLocation, const FRotator& ShootRotation)
{
	if (ShootingStance == EShootingStanceType::None)
		return;
	if (ASInventory == nullptr)
		return;

	const TWeakObjectPtr<UASWeapon>& SelectedWeapon = ASInventory->GetSelectedWeapon();
	if (!SelectedWeapon.IsValid())
		return;

	AASBullet* SpawnedBullet = SelectedWeapon->Fire(this, ShootingStance, MuzzleLocation, ShootRotation);
	if (SpawnedBullet != nullptr)
	{
		MulticastPlayShootMontage();
	}
}

void AASCharacter::ServerChangeFireMode_Implementation()
{
	const TWeakObjectPtr<UASWeapon>& Weapon = ASInventory->GetSelectedWeapon();
	if (!Weapon.IsValid())
		return;

	Weapon->ChangeToNextFireMode();
}
