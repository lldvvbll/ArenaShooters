// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASItemScrollBoxWrapperUserWidget.h"
#include "GUI/ASItemDragDropOperation.h"
#include "GUI/ASItemUserWidget.h"
#include "GUI/ASDragItemUserWidget.h"
#include "Components/ScrollBox.h"
#include "Item/ASItem.h"

void UASItemScrollBoxWrapperUserWidget::AddItemsToScrollBox(const TArray<TWeakObjectPtr<UASItem>>& Items)
{
	if (ItemScrollBox == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return;
	}

	for (auto& Item : Items)
	{
		if (auto ItemWidget = CreateWidget<UASItemUserWidget>(ItemScrollBox, ItemWidgetClass))
		{
			ItemWidget->SetItem(Item);
			ItemScrollBox->AddChild(ItemWidget);
		}
	}
}

void UASItemScrollBoxWrapperUserWidget::RemoveItemsFromScrollBox(const TArray<TWeakObjectPtr<UASItem>>& Items)
{
	if (ItemScrollBox == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return;
	}

	int32 ItemWidgetNum = ItemScrollBox->GetChildrenCount();

	TArray<int32> RemoveItemIndices;
	RemoveItemIndices.Reserve(ItemWidgetNum);
	
	for (auto& Item : Items)
	{
		for (int32 Idx = 0; Idx < ItemWidgetNum; ++Idx)
		{
			auto ItemWidget = Cast<UASItemUserWidget>(ItemScrollBox->GetChildAt(Idx));
			if (ItemWidget == nullptr)
				continue;

			if (ItemWidget->HasItem(Item))
			{
				RemoveItemIndices.Emplace(Idx);
			}
		}
	}

	for (auto& Idx : RemoveItemIndices)
	{
		ItemScrollBox->RemoveChildAt(Idx);
	}
}

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
					DraggedItemWidget->SetSuitableBrush(false);
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
					DraggedItemWidget->SetSuitableBrush(true);
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
