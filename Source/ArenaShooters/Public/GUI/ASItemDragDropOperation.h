// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Blueprint/DragDropOperation.h"
#include "ASItemDragDropOperation.generated.h"

class UASItem;
class UASItemUserWidget;
class UASDragItemUserWidget;

UCLASS()
class ARENASHOOTERS_API UASItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	void SetItemData(const TWeakObjectPtr<UASItem>& InItem, UASItemUserWidget* InItemWidget, UASDragItemUserWidget* InDragItemUserWidget);

	const TWeakObjectPtr<UASItem>& GetItem() const;
	const UASItemUserWidget* GetItemWidget() const;

private:
	UPROPERTY()
	TWeakObjectPtr<UASItem> Item;

	UPROPERTY()
	UASItemUserWidget* ItemWidget;
};
