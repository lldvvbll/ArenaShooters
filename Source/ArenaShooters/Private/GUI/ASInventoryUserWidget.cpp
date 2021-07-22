// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASInventoryUserWidget.h"
#include "Character/ASCharacter.h"
#include "ASPlayerController.h"
#include "GUI/ASWeaponSlotUserWidget.h"
#include "GUI/ASArmorSlotUserWidget.h"
#include "GUI/ASItemScrollBoxWrapperUserWidget.h"
#include "Character/ASInventoryComponent.h"
#include "Item/ASWeapon.h"
#include "Item/ASArmor.h"

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

	ASInventoryComp->OnInsertWeapon.AddUObject(this, &UASInventoryUserWidget::OnChangedWeapon);
	ASInventoryComp->OnInsertArmor.AddUObject(this, &UASInventoryUserWidget::OnChangedArmor);
}

void UASInventoryUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MainWeaponSlotWidget = Cast<UASWeaponSlotUserWidget>(GetWidgetFromName(TEXT("MainWeaponSlot")));
	SubWeaponSlotWidget = Cast<UASWeaponSlotUserWidget>(GetWidgetFromName(TEXT("SubWeaponSlot")));
	HelmetSlotWidget = Cast<UASArmorSlotUserWidget>(GetWidgetFromName(TEXT("HelmetSlot")));
	JacketSlotWidget = Cast<UASArmorSlotUserWidget>(GetWidgetFromName(TEXT("JacketSlot")));
	InventoryScrollBoxWrapperWidget = Cast<UASItemScrollBoxWrapperUserWidget>(GetWidgetFromName(TEXT("InventoryScrollBoxWrapper")));
	GroundScrollBoxWrapperWidget = Cast<UASItemScrollBoxWrapperUserWidget>(GetWidgetFromName(TEXT("GroundScrollBoxWrapper")));

	OnChangedWeapon(EWeaponSlotType::Main, nullptr);
	OnChangedWeapon(EWeaponSlotType::Sub, nullptr);
	OnChangedArmor(EArmorSlotType::Helmet, nullptr);
	OnChangedArmor(EArmorSlotType::Jacket, nullptr);


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
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
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
		AS_LOG_SCREEN_S(5.0f, FColor::Red);
		break;
	}
}
