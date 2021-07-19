// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Blueprint/UserWidget.h"
#include "ASDragItemUserWidget.generated.h"

class UASItemUserWidget;

UCLASS()
class ARENASHOOTERS_API UASDragItemUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetItemWidget(UASItemUserWidget* InItemWidget);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(EditDefaultsOnly, Meta = (AllowPrivateAccess = true))
	UASItemUserWidget* ItemWidget;
};
