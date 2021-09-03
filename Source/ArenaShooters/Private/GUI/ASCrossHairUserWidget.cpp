// Fill out your copyright notice in the Description page of Project Settings.


#include "GUI/ASCrossHairUserWidget.h"
#include "Components/Image.h"

void UASCrossHairUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TopBar = Cast<UImage>(GetWidgetFromName(TEXT("TopBar")));
	BottomBar = Cast<UImage>(GetWidgetFromName(TEXT("BottomBar")));
	LeftBar = Cast<UImage>(GetWidgetFromName(TEXT("LeftBar")));
	RightBar = Cast<UImage>(GetWidgetFromName(TEXT("RightBar")));
}


