// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/ASItemFactoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Item/ASItem.h"
#include "DataAssets/ItemDataAssets/ASItemDataAsset.h"
#include "Character/ASCharacter.h"
#include "GameFramework/GameStateBase.h"

UASItem* UASItemFactoryComponent::NewASItem(UWorld* World, AActor* NewOwner, UASItemDataAsset* DataAsset, int32 Count/* = 0*/)
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

	AGameStateBase* GameState = World->GetGameState();
	if (GameState == nullptr)
	{
		AS_LOG_S(Error);
		return nullptr;
	}

	auto ItemFactoryComp = Cast<UASItemFactoryComponent>(GameState->FindComponentByClass(UASItemFactoryComponent::StaticClass()));
	if (ItemFactoryComp == nullptr)
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

	NewItem->SetDataAsset(DataAsset);
	NewItem->SetOwner(NewOwner);
	NewItem->SetCount(Count);

	ItemFactoryComp->ASItems.Emplace(NewItem);

	return NewItem;
}

bool UASItemFactoryComponent::DeleteItem(UWorld* World, UASItem* InItem)
{
	if (!IsValid(InItem))
	{
		AS_LOG_S(Error);
		return false;
	}

	if (World == nullptr)
	{
		AS_LOG_S(Error);
		return false;
	}

	AGameStateBase* GameState = World->GetGameState();
	if (GameState == nullptr)
	{
		AS_LOG_S(Error);
		return nullptr;
	}

	auto ItemFactoryComp = Cast<UASItemFactoryComponent>(GameState->FindComponentByClass(UASItemFactoryComponent::StaticClass()));
	if (ItemFactoryComp == nullptr)
	{
		AS_LOG_S(Error);
		return nullptr;
	}

	int32 Idx = ItemFactoryComp->ASItems.Find(InItem);
	if (Idx == INDEX_NONE)
		return false;

	auto Owner = Cast<AASCharacter>(InItem->GetOwner());
	if (IsValid(Owner))
	{
		if (!Owner->RemoveItem(InItem))
			return false;
	}

	ItemFactoryComp->ASItems[Idx] = nullptr;
	ItemFactoryComp->ASItems.RemoveAtSwap(Idx);

	InItem->MarkPendingKill();

	UObject* Obj = InItem;
	FReferencerInformationList Refs;
	if (IsReferenced(Obj, RF_Public, EInternalObjectFlags::None, true, &Refs))
	{
		for (auto& Info : Refs.InternalReferences)
		{
			if (Info.Referencer != nullptr)
			{
				AS_LOG(Warning, TEXT("Int Ref: %s"), *Info.Referencer->GetName());
			}
		}

		for (auto& Info : Refs.ExternalReferences)
		{
			if (Info.Referencer != nullptr)
			{
				AS_LOG(Warning, TEXT("Ext Ref: %s"), *Info.Referencer->GetName());
			}
		}
	}

	return true;
}

UASItemFactoryComponent::UASItemFactoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

bool UASItemFactoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (auto& ASItem : ASItems)
	{
		if (IsValid(ASItem))
		{
			WroteSomething |= Channel->ReplicateSubobject(ASItem, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UASItemFactoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UASItemFactoryComponent, ASItems);
}