// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASEquipmentSlotUserWidget.h"
#include "GUI/ASItemDragDropOperation.h"
#include "GUI/ASDragItemUserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
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

	if (NameTextBlock != nullptr)
	{
		NameTextBlock->SetText((Item.IsValid() ?  Item->GetItemName() : FText::GetEmpty()));
	}
}

void UASEquipmentSlotUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HighlightBorder = Cast<UBorder>(GetWidgetFromName(TEXT("HighlightBorder")));
	EquipmentImage = Cast<UImage>(GetWidgetFromName(TEXT("EquipmentImage")));
	NameTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("NameTextBlock")));

	Highlight(false);
}

void UASEquipmentSlotUserWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	if (IsSuitableSlot(GetASItemFromDragDropOperation(InOperation)))
	{
		Highlight(true);
	}
	else
	{
		if (auto DraggedItemWidget = Cast<UASDragItemUserWidget>(InOperation->DefaultDragVisual))
		{
			DraggedItemWidget->SetSuitableBrush(false);
		}
	}
}

void UASEquipmentSlotUserWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	Highlight(false);

	if (auto DraggedItemWidget = Cast<UASDragItemUserWidget>(InOperation->DefaultDragVisual))
	{
		DraggedItemWidget->SetSuitableBrush(true);
	}
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

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

bool UASEquipmentSlotUserWidget::IsSuitableSlot(const TWeakObjectPtr<UASItem>& Item)
{
	if (!Item.IsValid())
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return false;
	}

	if (Item->GetItemType() != ItemType)
		return false;

	return true;
}

TWeakObjectPtr<UASItem> UASEquipmentSlotUserWidget::GetASItemFromDragDropOperation(UDragDropOperation* InOperation)
{
	if (InOperation == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return TWeakObjectPtr<UASItem>();
	}

	auto DragDropOp = Cast<UASItemDragDropOperation>(InOperation);
	if (DragDropOp == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return TWeakObjectPtr<UASItem>();
	}

	return DragDropOp->GetItem();
}
