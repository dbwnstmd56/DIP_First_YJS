// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"


void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_restart->OnClicked.AddDynamic(this, &UGameOverWidget::Restart);
	btn_quit->OnClicked.AddDynamic(this, &UGameOverWidget::Quit);
}

void UGameOverWidget::Restart()
{
	//UGameplayStatics::OpenLevel(GetWorld(), FName("MyLevel"));
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), restartLevel);
}

void UGameOverWidget::Quit()
{
	UWorld* world = GetWorld();
	UKismetSystemLibrary::QuitGame(world, world->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
