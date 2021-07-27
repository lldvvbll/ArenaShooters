// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASItemDragDropOperation.h"
#include "GUI/ASDragItemUserWidget.h"

void UASItemDragDropOperation::SetItemData(const TWeakObjectPtr<UASItem>& InItem, UASItemUserWidget* InItemWidget, UASDragItemUserWidget* InDragItemUserWidget)
{
	Item = InItem;
	ItemWidget = InItemWidget;
	DefaultDragVisual = InDragItemUserWidget;
	Pivot = EDragPivot::CenterCenter;
}

const TWeakObjectPtr<UASItem>& UASItemDragDropOperation::GetItem() const
{
	return Item;
}

const UASItemUserWidget* UASItemDragDropOperation::GetItemWidget() const
{
	return ItemWidget;
}
