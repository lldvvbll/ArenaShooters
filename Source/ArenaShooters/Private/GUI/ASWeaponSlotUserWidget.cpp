// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASWeaponSlotUserWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Item/ASWeapon.h"
#include "Character/ASCharacter.h"
#include "Character/ASInventoryComponent.h"

void UASWeaponSlotUserWidget::SetASItem(TWeakObjectPtr<UASItem>& NewItem)
{
	Super::SetASItem(NewItem);

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
	if (Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation))
		return true;

	if (GetOperationParentWidget(InOperation) != this)
	{
		TWeakObjectPtr<UASItem> WeaponItem = GetASItemFromDragDropOperation(InOperation);
		if (IsSuitableSlot(WeaponItem))
		{
			auto ASChar = Cast<AASCharacter>(GetOwningPlayerPawn());
			if (ASChar != nullptr)
			{

				ASChar->ServerPickUpWeapon(WeaponSlotType, Cast<UASWeapon>(WeaponItem));
				return true;
			}
			else
			{
				AS_LOG_SCREEN_S(5.0f, FColor::Red);
			}
		}
	}

	return false;
}

bool UASWeaponSlotUserWidget::IsSuitableSlot(const TWeakObjectPtr<UASItem>& InItem)
{
	if (!Super::IsSuitableSlot(InItem))
		return false;

	auto Weapon = Cast<UASWeapon>(InItem);
	if (Weapon == nullptr)
		return false;
	
	if (!UASInventoryComponent::IsSuitableWeaponSlot(WeaponSlotType, Weapon))
		return false;

	return true;
}
