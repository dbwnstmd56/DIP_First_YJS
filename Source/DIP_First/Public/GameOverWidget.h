// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class DIP_FIRST_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta =(BindWidget), Category=MySettings)
	class UButton* btn_restart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta =(BindWidget), Category=MySettings)
	class UButton* btn_quit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	TSoftObjectPtr<UWorld> restartLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta =(BindWidgetAnim), Transient, Category=MySettings)
	class UWidgetAnimation* BackAnim;

private:
	UFUNCTION()
	void Restart();

	UFUNCTION()
	void Quit();
};
