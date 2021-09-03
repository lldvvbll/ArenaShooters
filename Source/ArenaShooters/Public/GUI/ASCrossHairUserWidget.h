// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaShooters.h"
#include "Blueprint/UserWidget.h"
#include "ASCrossHairUserWidget.generated.h"

class UImage;

UCLASS()
class ARENASHOOTERS_API UASCrossHairUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	UImage* TopBar;

	UPROPERTY()
	UImage* BottomBar;

	UPROPERTY()
	UImage* LeftBar;

	UPROPERTY()
	UImage* RightBar;
};
