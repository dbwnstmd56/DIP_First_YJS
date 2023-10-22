// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class DIP_FIRST_API UMainUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings, meta=(BindWidget))
	class UTextBlock* text_score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings, meta=(BindWidget))
	class UImage* img_Crosshair;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings, meta=(BindWidgetAnim), Transient)
	class UWidgetAnimation* pointupAnim;

};
