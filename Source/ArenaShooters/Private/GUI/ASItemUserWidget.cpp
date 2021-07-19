// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASItemUserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GUI/ASDragItemUserWidget.h"
#include "GUI/ASItemDragDropOperation.h"

void UASItemUserWidget::NativeConstruct()
{
	Super::NativeConstruct();


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
	if (DraggedItemWidget != nullptr)
	{
		DraggedItemWidget->SetItemWidget(this);
	}

	if (auto ItemDragDropOp = Cast<UASItemDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UASItemDragDropOperation::StaticClass())))
	{
		ItemDragDropOp->SetItemWidget(this);
		ItemDragDropOp->Pivot = EDragPivot::CenterCenter;
		ItemDragDropOp->DefaultDragVisual = DraggedItemWidget;

		OutOperation = ItemDragDropOp;
	}
}
