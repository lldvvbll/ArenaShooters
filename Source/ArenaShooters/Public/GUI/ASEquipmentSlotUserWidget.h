// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Blueprint/UserWidget.h"
#include "Common/ASEnums.h"
#include "ASEquipmentSlotUserWidget.generated.h"

class UBorder;
class UImage;
class UASItem;

UCLASS(Abstract)
class ARENASHOOTERS_API UASEquipmentSlotUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetASItem(TWeakObjectPtr<const UASItem>& Item);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void Highlight(bool bOn);

	virtual bool IsSuitableSlot(UASItem* Item);

	UASItem* GetASItemFromDragDropOperation(UDragDropOperation* InOperation);

protected:
	UPROPERTY()
	UBorder* HighlightBorder;

	UPROPERTY()
	UImage* EquipmentImage;

	UPROPERTY(EditAnywhere)
	EItemType ItemType;
};
