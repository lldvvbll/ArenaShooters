// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASArmorSlotUserWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Item/ASArmor.h"
#include "Character/ASInventoryComponent.h"
#include "Character/ASCharacter.h"

void UASArmorSlotUserWidget::SetASItem(TWeakObjectPtr<const UASItem>& Item)
{
	Super::SetASItem(Item);
	ArmorPtr = Item;

	if (DurabilityTextBlock != nullptr)
	{
		const UASArmor* Armor = (Item.IsValid() ? Cast<const UASArmor>(Item) : nullptr);

		if (Armor != nullptr)
		{
			DurabilityTextBlock->SetText(FText::AsNumber(Armor->GetCurrentDurability()));
		}
		else
		{
			DurabilityTextBlock->SetText(FText::GetEmpty());
		}		
	}
}

void UASArmorSlotUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DurabilityTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("DurabilityTextBlock")));
}

bool UASArmorSlotUserWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation))
		return true;

	TWeakObjectPtr<UASItem> ArmorItem = GetASItemFromDragDropOperation(InOperation);
	if (IsSuitableSlot(ArmorItem))
	{
		auto ASChar = Cast<AASCharacter>(GetOwningPlayerPawn());
		if (ASChar != nullptr)
		{

			ASChar->ServerPickUpArmor(ArmorSlotType, Cast<UASArmor>(ArmorItem));
			return true;
		}
		else
		{
			AS_LOG_SCREEN_S(5.0f, FColor::Red);
		}
	}

	return false;
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
