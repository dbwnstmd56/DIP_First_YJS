// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUserWidget.h"
#include "Components/TextBlock.h"


void UMainUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	text_score->SetText(FText::FromString("Current Score: 0"));

}
