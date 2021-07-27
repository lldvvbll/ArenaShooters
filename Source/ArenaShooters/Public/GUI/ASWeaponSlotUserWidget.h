// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "GUI/ASEquipmentSlotUserWidget.h"
#include "Common/ASEnums.h"
#include "ASWeaponSlotUserWidget.generated.h"

UCLASS()
class ARENASHOOTERS_API UASWeaponSlotUserWidget : public UASEquipmentSlotUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetASItem(TWeakObjectPtr<const UASItem>& Item) override;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual bool IsSuitableSlot(const TWeakObjectPtr<UASItem>& Item) override;

protected:
	UPROPERTY(EditAnywhere)
	EWeaponSlotType WeaponSlotType;

	UPROPERTY()
	TWeakObjectPtr<const UASItem> WeaponPtr;
};
