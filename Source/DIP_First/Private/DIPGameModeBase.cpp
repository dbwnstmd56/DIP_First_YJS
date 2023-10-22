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
		// 위젯 블루프린트를 생성한다.
		mainWidget_inst = CreateWidget<UMainUserWidget>(GetWorld(), mainWidget);

		if (mainWidget_inst != nullptr)
		{
			// 생성된 위젯 인스턴스를 뷰포트에 표시한다.
			mainWidget_inst->AddToViewport(1);
		}
	}

	// 마우스 커서를 안보이게 처리하고, 입력 영역을 Game 영역으로 변경한다.
	pc = GetWorld()->GetFirstPlayerController();
	pc->SetInputMode(FInputModeGameOnly());
	pc->SetShowMouseCursor(false);
	
}


// 점수 추가 함수
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
			
			// 마우스 커서를 화면에 보이게 한다.
			pc->SetShowMouseCursor(true);

			// 마우스 영역을 UI 영역으로 고정시킨다.
			pc->SetInputMode(FInputModeUIOnly());

			// 크로스헤어 이미지를 안보이게 처리한다.
			mainWidget_inst->img_Crosshair->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
