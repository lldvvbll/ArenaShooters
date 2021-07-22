// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASEquipmentSlotUserWidget.h"
#include "GUI/ASItemDragDropOperation.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Item/ASItem.h"

void UASEquipmentSlotUserWidget::SetASItem(TWeakObjectPtr<const UASItem>& Item)
{
	if (EquipmentImage != nullptr)
	{
		if (Item.IsValid())
		{
			EquipmentImage->SetBrushFromTexture(Item->GetEquipmentSlotImage());
			EquipmentImage->SetOpacity(1.0f);
		}
		else
		{
			EquipmentImage->SetBrushFromTexture(nullptr);
			EquipmentImage->SetOpacity(0.0f);
		}		
	}
}

void UASEquipmentSlotUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HighlightBorder = Cast<UBorder>(GetWidgetFromName(TEXT("HighlightBorder")));
	EquipmentImage = Cast<UImage>(GetWidgetFromName(TEXT("EquipmentImage")));

	Highlight(false);
}

void UASEquipmentSlotUserWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	if (IsSuitableSlot(GetASItemFromDragDropOperation(InOperation)))
	{
		Highlight(true);
	}
}

void UASEquipmentSlotUserWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	Highlight(false);
}

void UASEquipmentSlotUserWidget::Highlight(bool bOn)
{
	if (HighlightBorder != nullptr)
	{
		ESlateVisibility NewVisibility = (bOn ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
		if (HighlightBorder->GetVisibility() != NewVisibility)
		{
			HighlightBorder->SetVisibility(NewVisibility);

		}
	}
}

bool UASEquipmentSlotUserWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Highlight(false);

	if (Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation))
		return true;

	UASItem* EquipItem = GetASItemFromDragDropOperation(InOperation);
	if (!IsSuitableSlot(EquipItem))
		return false;

	return true;
}

bool UASEquipmentSlotUserWidget::IsSuitableSlot(UASItem* Item)
{
	if (Item == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return false;
	}
	if (Item->GetItemType() != ItemType)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return false;
	}

	return true;
}

UASItem* UASEquipmentSlotUserWidget::GetASItemFromDragDropOperation(UDragDropOperation* InOperation)
{
	if (InOperation == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return nullptr;
	}

	auto DragDropOp = Cast<UASItemDragDropOperation>(InOperation);
	if (DragDropOp == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return nullptr;
	}

	return Cast<UASItem>(DragDropOp->Payload);
}
