// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	None,
	Grounded,
	InAir,
	Mantling,
	Ragdoll,
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None,
	Pistol,
	Rifle,
};
