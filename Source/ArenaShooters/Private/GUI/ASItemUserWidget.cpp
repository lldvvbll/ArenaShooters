// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASItemUserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GUI/ASDragItemUserWidget.h"
#include "GUI/ASItemDragDropOperation.h"
#include "Item/ASItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Common/ASEnums.h"

void UASItemUserWidget::SetItem(const TWeakObjectPtr<UASItem>& NewItem)
{
	Item = NewItem;

	if (!Item.IsValid())
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return;
	}

	if (ItemImage != nullptr)
	{
		ItemImage->SetBrushFromTexture(Item->GetItemImage());
	}

	if (NameTextBlock != nullptr)
	{
		NameTextBlock->SetText(Item->GetItemName());
	}

	if (CountTextBlock != nullptr)
	{
		switch (Item->GetItemType())
		{
		case EItemType::Weapon:	// fallthough
		case EItemType::Armor:
			{
				CountTextBlock->SetVisibility(ESlateVisibility::Hidden);
			}
			break;
		case EItemType::Ammo:	// fallthough
		case EItemType::HealingKit:
			{
				CountTextBlock->SetText(FText::FromString(FString::FromInt(Item->GetCount())));
			}
			break;
		default:
			checkNoEntry();
			break;
		}
	}
}

bool UASItemUserWidget::HasItem(const TWeakObjectPtr<UASItem>& InItem) const
{
	return Item == InItem;
}

void UASItemUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ItemImage = Cast<UImage>(GetWidgetFromName(TEXT("ItemImage")));
	NameTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("NameTextBlock")));
	CountTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("CountTextBlock")));
}

FReply UASItemUserWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UASItemUserWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	DraggedItemWidget = CreateWidget<UASDragItemUserWidget>(this, (DragItemWidgetClass != nullptr ? DragItemWidgetClass : UASDragItemUserWidget::StaticClass()));
	if (DraggedItemWidget != nullptr && Item.IsValid())
	{
		DraggedItemWidget->SetImageAndItemWidget(Item->GetItemImage(), this);
	}

	if (auto ItemDragDropOp = NewObject<UASItemDragDropOperation>(GetTransientPackage(), UASItemDragDropOperation::StaticClass()))
	{
		ItemDragDropOp->SetItemData(Item, this, DraggedItemWidget);
		OutOperation = ItemDragDropOp;
	}
}
