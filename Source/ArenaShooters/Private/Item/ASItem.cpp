// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ASItem.h"
#include "Net/UnrealNetwork.h"

void UASItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

bool UASItem::IsSupportedForNetworking() const
{
	return true;
}

void UASItem::ChangeType()
{
}

EWeaponType UASItem::GetWeaponType() const
{
	return EWeaponType();
}
