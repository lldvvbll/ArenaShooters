// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Blueprint/UserWidget.h"
#include "Common/ASEnums.h"
#include "ASInventoryUserWidget.generated.h"

class AASCharacter;
class UASWeaponSlotUserWidget;
class UASArmorSlotUserWidget;
class UASItemScrollBoxWrapperUserWidget;
class UASWeapon;
class UASArmor;
class UASInventoryComponent;

UCLASS()
class ARENASHOOTERS_API UASInventoryUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Bind();

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	AASCharacter* GetASCharacter() const;
	void OnChangedWeapon(EWeaponSlotType SlotType, UASWeapon* RemovedWeapon);
	void OnChangedArmor(EArmorSlotType SlotType, UASArmor* RemovedArmor);

private:
	UPROPERTY()
	UASWeaponSlotUserWidget* MainWeaponSlotWidget;

	UPROPERTY()
	UASWeaponSlotUserWidget* SubWeaponSlotWidget;

	UPROPERTY()
	UASArmorSlotUserWidget* HelmetSlotWidget;

	UPROPERTY()
	UASArmorSlotUserWidget* JacketSlotWidget;

	UPROPERTY()
	UASItemScrollBoxWrapperUserWidget* InventoryScrollBoxWrapperWidget;

	UPROPERTY()
	UASItemScrollBoxWrapperUserWidget* GroundScrollBoxWrapperWidget;

	UPROPERTY()
	UASInventoryComponent* ASInventoryComp;
};
