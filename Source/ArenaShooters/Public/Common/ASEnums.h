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
enum class EItemType : uint8
{
	None,
	Weapon,
	Armor,
	Ammo,
	HealingKit,
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None,
	Pistol,
	AssaultRifle,
};

UENUM(BlueprintType)
enum class EWeaponClass : uint8
{
	None,
	Pistol_Silver,
	Pistol_Dark,
	AR_Silver,
	AR_Dark,
};

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	None,
	Helmet,
	Jacket,
};