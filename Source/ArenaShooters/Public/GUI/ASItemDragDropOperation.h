// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Blueprint/DragDropOperation.h"
#include "ASItemDragDropOperation.generated.h"

class UASItemUserWidget;

UCLASS()
class ARENASHOOTERS_API UASItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	void SetItemWidget(UASItemUserWidget* InItemWidget);
	UASItemUserWidget* GetItemWidget();
	const UASItemUserWidget* GetItemWidget() const;

private:
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	UASItemUserWidget* ItemWidget;

	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	FVector2D DragOffset;
};
