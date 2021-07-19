// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASItemScrollBoxWrapperUserWidget.h"
#include "GUI/ASItemDragDropOperation.h"
#include "GUI/ASItemUserWidget.h"
#include "GUI/ASDragItemUserWidget.h"
#include "Components/ScrollBox.h"
#include "Item/ASItem.h"

void UASItemScrollBoxWrapperUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ItemScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ItemScrollBox")));
}

void UASItemScrollBoxWrapperUserWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	if (bInventoryScrollBoxWrapper)
	{
		if (auto Item = GetASItemFromDragDropOperation(InOperation))
		{
			EItemType ItemType = Item->GetItemType();
			if (ItemType == EItemType::Weapon || ItemType == EItemType::Armor)
			{
				if (auto DraggedItemWidget = Cast<UASDragItemUserWidget>(InOperation->DefaultDragVisual))
				{
					DraggedItemWidget->SetIsEnabled(false);
				}
			}
		}
	}
}

void UASItemScrollBoxWrapperUserWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	if (bInventoryScrollBoxWrapper)
	{
		if (auto Item = GetASItemFromDragDropOperation(InOperation))
		{
			EItemType ItemType = Item->GetItemType();
			if (ItemType == EItemType::Weapon || ItemType == EItemType::Armor)
			{
				if (auto DraggedItemWidget = Cast<UASDragItemUserWidget>(InOperation->DefaultDragVisual))
				{
					DraggedItemWidget->SetIsEnabled(true);
				}
			}
		}
	}
}

bool UASItemScrollBoxWrapperUserWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool Result = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (auto Item = GetASItemFromDragDropOperation(InOperation))
	{
		if (bInventoryScrollBoxWrapper)
		{
			EItemType ItemType = Item->GetItemType();
			if (ItemType != EItemType::Weapon && ItemType != EItemType::Armor)
			{

			}
		}
		else
		{

		}
	}

	return Result;
}

UASItem* UASItemScrollBoxWrapperUserWidget::GetASItemFromDragDropOperation(UDragDropOperation* InOperation)
{
	if (InOperation == nullptr)
		return nullptr;

	auto DragDropOp = Cast<UASItemDragDropOperation>(InOperation);
	if (DragDropOp == nullptr)
		return nullptr;

	return Cast<UASItem>(DragDropOp->Payload);
}
