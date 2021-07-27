// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASArmorSlotUserWidget.h"
#include "Components/Border.h"
#include "Item/ASArmor.h"
#include "Character/ASInventoryComponent.h"

void UASArmorSlotUserWidget::SetASItem(TWeakObjectPtr<const UASItem>& Item)
{
	Super::SetASItem(Item);
	ArmorPtr = Item;
}

void UASArmorSlotUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

bool UASArmorSlotUserWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

}

bool UASArmorSlotUserWidget::IsSuitableSlot(const TWeakObjectPtr<UASItem>& Item)
{
	if (!Super::IsSuitableSlot(Item))
		return false;

	auto Armor = Cast<UASArmor>(Item);
	if (Armor == nullptr)
		return false;

	if (!UASInventoryComponent::IsSuitableArmorSlot(ArmorSlotType, Armor))
		return false;

	return true;
}
