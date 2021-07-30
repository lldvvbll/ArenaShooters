// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASInventoryUserWidget.h"
#include "Character/ASCharacter.h"
#include "ASPlayerController.h"
#include "GUI/ASWeaponSlotUserWidget.h"
#include "GUI/ASArmorSlotUserWidget.h"
#include "GUI/ASItemScrollBoxWrapperUserWidget.h"
#include "Character/ASInventoryComponent.h"
#include "Item/ASItem.h"
#include "Item/ASWeapon.h"
#include "Item/ASArmor.h"
#include "ItemActor/ASDroppedItemActor.h"

void UASInventoryUserWidget::Bind()
{
	AASCharacter* ASChar = GetASCharacter();
	if (ASChar == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return;
	}

	ASInventoryComp = ASChar->GetInventoryComponent();
	if (ASInventoryComp == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return;
	}

	OnGroundItemAddEventHandle = ASChar->OnGroundItemAddEvent.AddUObject(this, &UASInventoryUserWidget::AddItemsToGroundScrollBox);
	OnGroundItemRemoveEventHandle = ASChar->OnGroundItemRemoveEvent.AddUObject(this, &UASInventoryUserWidget::RemoveItemsFromGroundScrollBox);

	OnInsertWeaponHandle = ASInventoryComp->OnInsertWeapon.AddUObject(this, &UASInventoryUserWidget::OnChangedWeapon);
	OnInsertArmorHandle = ASInventoryComp->OnInsertArmor.AddUObject(this, &UASInventoryUserWidget::OnChangedArmor);
}

void UASInventoryUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MainWeaponSlotWidget = Cast<UASWeaponSlotUserWidget>(GetWidgetFromName(TEXT("MainWeaponSlot")));
	SubWeaponSlotWidget = Cast<UASWeaponSlotUserWidget>(GetWidgetFromName(TEXT("SubWeaponSlot")));
	HelmetSlotWidget = Cast<UASArmorSlotUserWidget>(GetWidgetFromName(TEXT("HelmetSlot")));
	JacketSlotWidget = Cast<UASArmorSlotUserWidget>(GetWidgetFromName(TEXT("JacketSlot")));
	InventoryItemScrollBoxWrapperWidget = Cast<UASItemScrollBoxWrapperUserWidget>(GetWidgetFromName(TEXT("InventoryItemScrollBoxWrapper")));
	GroundItemScrollBoxWrapperWidget = Cast<UASItemScrollBoxWrapperUserWidget>(GetWidgetFromName(TEXT("GroundItemScrollBoxWrapper")));

	if (AASCharacter* ASChar = GetASCharacter())
	{
		OnChangedWeapon(EWeaponSlotType::Main, nullptr);
		OnChangedWeapon(EWeaponSlotType::Sub, nullptr);
		OnChangedArmor(EArmorSlotType::Helmet, nullptr);
		OnChangedArmor(EArmorSlotType::Jacket, nullptr);

		AddItemsToGroundScrollBox(ASChar->GetGroundItems());
	}
}

void UASInventoryUserWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (AASCharacter* ASChar = GetASCharacter())
	{
		ASChar->OnGroundItemAddEvent.Remove(OnGroundItemAddEventHandle);
		ASChar->OnGroundItemRemoveEvent.Remove(OnGroundItemRemoveEventHandle);
	}
	else
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
	}

	if (ASInventoryComp != nullptr)
	{
		ASInventoryComp->OnInsertWeapon.Remove(OnInsertWeaponHandle);
		ASInventoryComp->OnInsertArmor.Remove(OnInsertArmorHandle);
	}
	else
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
	}
}

FReply UASInventoryUserWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	return Reply;
}

AASCharacter* UASInventoryUserWidget::GetASCharacter() const
{
	auto PlayerController = Cast<AASPlayerController>(GetOwningPlayer());
	if (PlayerController == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return nullptr;
	}

	return Cast<AASCharacter>(PlayerController->GetCharacter());
}

void UASInventoryUserWidget::AddItemsToGroundScrollBox(const TArray<TWeakObjectPtr<UASItem>>& Items)
{
	if (GroundItemScrollBoxWrapperWidget == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return;
	}

	GroundItemScrollBoxWrapperWidget->AddItemsToScrollBox(Items);
}

void UASInventoryUserWidget::RemoveItemsFromGroundScrollBox(const TArray<TWeakObjectPtr<UASItem>>& Items)
{
	if (GroundItemScrollBoxWrapperWidget == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return;
	}

	GroundItemScrollBoxWrapperWidget->RemoveItemsFromScrollBox(Items);
}

void UASInventoryUserWidget::OnChangedWeapon(EWeaponSlotType SlotType, UASWeapon* RemovedWeapon)
{
	if (ASInventoryComp == nullptr)
	{
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		return;
	}

	switch (SlotType)
	{
	case EWeaponSlotType::Main:
		if (MainWeaponSlotWidget != nullptr)
		{
			ConstItemPtrBoolPair ItemPair = ASInventoryComp->FindItemFromWeaponSlot(SlotType);
			if (ItemPair.Value)
			{
				MainWeaponSlotWidget->SetASItem(ItemPair.Key);
			}
			else
			{
				AS_LOG_SCREEN_S(5.0f, FColor::Red);
			}
		}
		else
		{
			AS_LOG_SCREEN_S(5.0f, FColor::Red);
		}
		break;
	case EWeaponSlotType::Sub:
		if (SubWeaponSlotWidget != nullptr)
		{
			ConstItemPtrBoolPair ItemPair = ASInventoryComp->FindItemFromWeaponSlot(SlotType);
			if (ItemPair.Value)
			{
				SubWeaponSlotWidget->SetASItem(ItemPair.Key);
			}
			else
			{
				AS_LOG_SCREEN_S(5.0f, FColor::Red);
			}
		}
		else
		{
			AS_LOG_SCREEN_S(5.0f, FColor::Red);
		}
		break;
	default:
		checkNoEntry();
		break;
	}
}

void UASInventoryUserWidget::OnChangedArmor(EArmorSlotType SlotType, UASArmor* RemovedArmor)
{
	switch (SlotType)
	{
	case EArmorSlotType::Helmet:
		if (HelmetSlotWidget != nullptr)
		{

		}
		break;
	case EArmorSlotType::Jacket:
		if (JacketSlotWidget != nullptr)
		{

		}
		break;
	default:
		checkNoEntry();
		break;
	}
}
