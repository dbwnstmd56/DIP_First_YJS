// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DIPGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DIP_FIRST_API ADIPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	int32 currentScore = 0;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<class UMainUserWidget> mainWidget;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<class UGameOverWidget> gameoverWidget;

	void AddScore(int32 point);
	void LoadGameOverUI();

protected:
	virtual void BeginPlay() override;

private:
	class UMainUserWidget* mainWidget_inst;
	class UGameOverWidget* gameoverWidget_inst;
	class APlayerController* pc;
};
