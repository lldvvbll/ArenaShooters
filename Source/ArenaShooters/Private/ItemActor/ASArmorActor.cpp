// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor/ASArmorActor.h"

AASArmorActor::AASArmorActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetCanBeDamaged(false);

	ArmorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmorMesh"));
	ArmorMesh->SetCollisionProfileName(TEXT("NoCollision"));

	RootComponent = ArmorMesh;
}
