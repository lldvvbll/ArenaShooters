// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor/ASDroppedItemActor.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Item/ASItem.h"
#include "Item/ASWeapon.h"
#include "ASAssetManager.h"
#include "DataAssets/ItemDataAssets/ASWeaponDataAsset.h"

AASDroppedItemActor::AASDroppedItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetCanBeDamaged(false);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("DroppedItem"));
	Collision->SetGenerateOverlapEvents(true);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshComp->SetIsReplicated(true);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshComp->SetIsReplicated(true);

	RootComponent = Collision;
	SkeletalMeshComp->SetupAttachment(RootComponent);
	StaticMeshComp->SetupAttachment(RootComponent);
}

bool AASDroppedItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (auto& ASItem : ASItems)
	{
		if (ASItem != nullptr && !ASItem->IsPendingKill())
		{
			WroteSomething |= Channel->ReplicateSubobject(ASItem, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void AASDroppedItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AASDroppedItemActor, ASItems);
}

void AASDroppedItemActor::SetSkeletalMesh(USkeletalMesh* InSkelMesh)
{
	if (SkeletalMeshComp != nullptr)
	{
		SkeletalMeshComp->SetSkeletalMesh(InSkelMesh);
	}
}

void AASDroppedItemActor::SetStaticMesh(UStaticMesh* InStaticMesh)
{
	if (StaticMeshComp != nullptr)
	{
		StaticMeshComp->SetStaticMesh(InStaticMesh);
	}
}

TArray<TWeakObjectPtr<UASItem>> AASDroppedItemActor::GetItems() const
{
	TArray<TWeakObjectPtr<UASItem>> Result;

	for (auto& ASItem : ASItems)
	{
		if (ASItem != nullptr && !ASItem->IsPendingKill())
		{
			Result.Emplace(ASItem);
		}
	}

	return Result;
}

void AASDroppedItemActor::AddItem(UASItem* InItem)
{
	if (InItem == nullptr)
	{
		AS_LOG_S(Error);
		return;
	}

	InItem->SetOwner(this);

	if (GetLifeSpan() > 0.0f)
	{
		SetSelfDestroy(0.0f);		// Cancel self destory
	}

	ASItems.Emplace(InItem);
}

bool AASDroppedItemActor::RemoveItem(UASItem* InItem)
{
	if (InItem == nullptr)
	{
		AS_LOG_S(Error);
		return false;
	}

	if (ASItems.Remove(InItem) == 0)
	{
		AS_LOG_S(Error);
		return false;
	}

	InItem->SetOwner(nullptr);

	if (ASItems.Num() == 0)
	{
		SetSelfDestroy(3.0f);
	}

	return true;
}

void AASDroppedItemActor::SetSelfDestroy(float InLifeSpan)
{
	SetActorHiddenInGame(InLifeSpan > 0.0f);
	SetLifeSpan(InLifeSpan);
}

void AASDroppedItemActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		if (TestARAssetId.IsValid() && ASItems.Num() == 0)
		{
			if (auto WeaponDataAsset = UASAssetManager::Get().GetDataAsset<UASWeaponDataAsset>(TestARAssetId))
			{
				ASItems.Emplace(UASWeapon::CreateFromDataAsset(GetWorld(), this, WeaponDataAsset));
			}
		}
	}	
}

void AASDroppedItemActor::OnRep_ASItems(TArray<UASItem*>& OldItems)
{
	if (OnRemoveItemEvent.IsBound())
	{
		for (auto& Item : OldItems)
		{
			if (!ASItems.Contains(Item))
			{
				TWeakObjectPtr<UASItem> ItemPtr = MakeWeakObjectPtr(Item);
				OnRemoveItemEvent.Broadcast(ItemPtr);
			}
		}
	}
}
