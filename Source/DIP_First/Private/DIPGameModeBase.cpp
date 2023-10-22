// Fill out your copyright notice in the Description page of Project Settings.


#include "DIPGameModeBase.h"
#include "MainUserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "GameOverWidget.h"


void ADIPGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (mainWidget != nullptr)
	{
		// ���� �������Ʈ�� �����Ѵ�.
		mainWidget_inst = CreateWidget<UMainUserWidget>(GetWorld(), mainWidget);

		if (mainWidget_inst != nullptr)
		{
			// ������ ���� �ν��Ͻ��� ����Ʈ�� ǥ���Ѵ�.
			mainWidget_inst->AddToViewport(1);
		}
	}

	// ���콺 Ŀ���� �Ⱥ��̰� ó���ϰ�, �Է� ������ Game �������� �����Ѵ�.
	pc = GetWorld()->GetFirstPlayerController();
	pc->SetInputMode(FInputModeGameOnly());
	pc->SetShowMouseCursor(false);
	
}


// ���� �߰� �Լ�
void ADIPGameModeBase::AddScore(int32 point)
{
	currentScore += point;

	//FString scoreText = FString::Printf(TEXT("Current Score: %d"), currentScore);
	FString scoreText = FString::FormatAsNumber(currentScore);
	
	if (mainWidget_inst != nullptr)
	{
		mainWidget_inst->text_score->SetText(FText::FromString(scoreText));
		mainWidget_inst->PlayAnimation(mainWidget_inst->pointupAnim);
		//mainWidget_inst->PlayAnimationForward(mainWidget_inst->pointupAnim);
	}
}

void ADIPGameModeBase::LoadGameOverUI()
{
	if (gameoverWidget != nullptr)
	{
		gameoverWidget_inst = CreateWidget<UGameOverWidget>(GetWorld(), gameoverWidget);

		if (gameoverWidget_inst != nullptr)
		{
			gameoverWidget_inst->AddToViewport(0);
			gameoverWidget_inst->PlayAnimationForward(gameoverWidget_inst->BackAnim);
			
			// ���콺 Ŀ���� ȭ�鿡 ���̰� �Ѵ�.
			pc->SetShowMouseCursor(true);

			// ���콺 ������ UI �������� ������Ų��.
			pc->SetInputMode(FInputModeUIOnly());

			// ũ�ν���� �̹����� �Ⱥ��̰� ó���Ѵ�.
			mainWidget_inst->img_Crosshair->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
