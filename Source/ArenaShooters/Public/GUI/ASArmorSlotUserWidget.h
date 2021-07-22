// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "GUI/ASEquipmentSlotUserWidget.h"
#include "ASArmorSlotUserWidget.generated.h"

UCLASS()
class ARENASHOOTERS_API UASArmorSlotUserWidget : public UASEquipmentSlotUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetASItem(TWeakObjectPtr<const UASItem>& Item) override;

protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual bool IsSuitableSlot(UASItem* Item) override;

protected:
	UPROPERTY(EditAnywhere)
	EArmorSlotType ArmorSlotType;

	UPROPERTY()
	TWeakObjectPtr<const UASItem> ArmorPtr;
};
