// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Blueprint/UserWidget.h"
#include "ASItemScrollBoxWrapperUserWidget.generated.h"

class UScrollBox;
class UASItem;

UCLASS()
class ARENASHOOTERS_API UASItemScrollBoxWrapperUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UASItem* GetASItemFromDragDropOperation(UDragDropOperation* InOperation);

private:
	UPROPERTY(EditInstanceOnly, Meta = (AllowPrivateAccess = true))
	bool bInventoryScrollBoxWrapper;

	UScrollBox* ItemScrollBox;
};
