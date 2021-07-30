// Fill out your copyright notice in the Description page of Project Settings.


#include "ASItemFactory.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "ASGameState.h"
#include "Item/ASItem.h"
#include "DataAssets/ItemDataAssets/ASItemDataAsset.h"

AASItemFactory::AASItemFactory()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
}

bool AASItemFactory::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
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

void AASItemFactory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AASItemFactory, ASItems);
}

UASItem* AASItemFactory::NewASItem(UWorld* World, AActor* NewOwner, UASItemDataAsset* DataAsset, int32 Count/* = 0*/)
{
	if (DataAsset == nullptr)
	{
		AS_LOG_S(Error);
		return nullptr;
	}
	if (World == nullptr)
	{
		AS_LOG_S(Error);
		return nullptr;
	}

	auto GameState = World->GetGameState<AASGameState>();
	if (GameState == nullptr)
	{
		AS_LOG_S(Error);
		return nullptr;
	}

	AASItemFactory* ItemFactory = GameState->GetASItemFactory();
	if (ItemFactory == nullptr)
	{
		AS_LOG_S(Error);
		return nullptr;
	}

	auto NewItem = NewObject<UASItem>(World->GetCurrentLevel(), DataAsset->ItemClass);
	if (NewItem == nullptr)
	{
		AS_LOG_S(Error);
		return nullptr;
	}

	ItemFactory->AddASItem(NewItem);

	NewItem->SetCount(Count);
	NewItem->SetOwner(NewOwner);
	NewItem->SetDataAsset(DataAsset);

	return NewItem;
}

void AASItemFactory::AddASItem(UASItem* NewItem)
{
	ASItems.Emplace(NewItem);
}

void AASItemFactory::OnRep_ASItems(TArray<UASItem*>& OldASItems)
{
}
