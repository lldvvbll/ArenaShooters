// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASWeaponSlotUserWidget.h"
#include "Components/Border.h"
#include "Item/ASWeapon.h"
#include "Character/ASInventoryComponent.h"

void UASWeaponSlotUserWidget::SetASItem(TWeakObjectPtr<const UASItem>& Item)
{
	Super::SetASItem(Item);
	WeaponPtr = Item;
}

void UASWeaponSlotUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UASWeaponSlotUserWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	AS_LOG_SCREEN(1.0f, FColor::Yellow, TEXT("NativeOnMouseEnter"));
}

bool UASWeaponSlotUserWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

}

bool UASWeaponSlotUserWidget::IsSuitableSlot(UASItem* Item)
{
	if (!Super::IsSuitableSlot(Item))
		return false;

	auto Weapon = Cast<UASWeapon>(Item);
	if (Weapon == nullptr)
		return false;
	
	if (!UASInventoryComponent::IsSuitableWeaponSlot(WeaponSlotType, Weapon))
		return false;

	return true;
}
