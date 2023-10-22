// Fill out your copyright notice in the Description page of Project Settings.


#include "HPWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"


void UHPWidget::SetOwnerName(FString name)
{
	text_name->SetText(FText::FromString(name));
}

void UHPWidget::SetHP(int32 curHP, int32 maxHP)
{
	pb_health->SetPercent((float)curHP / (float) maxHP);
}
