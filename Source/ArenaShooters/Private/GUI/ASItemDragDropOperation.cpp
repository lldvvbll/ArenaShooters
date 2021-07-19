// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASItemDragDropOperation.h"
#include "GUI/ASDragItemUserWidget.h"

void UASItemDragDropOperation::SetItemWidget(UASItemUserWidget* InItemWidget)
{
	ItemWidget = InItemWidget;
}

UASItemUserWidget* UASItemDragDropOperation::GetItemWidget()
{
	return ItemWidget;
}

const UASItemUserWidget* UASItemDragDropOperation::GetItemWidget() const
{
	return ItemWidget;
}
