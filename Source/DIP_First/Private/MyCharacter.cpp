// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ������ �� ������Ʈ�� ĸ�� ������Ʈ�� �ڽ� ������Ʈ�� �����Ѵ�.
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->TargetArmLength = 500.0f;

	// ī�޶� ������Ʈ�� ������ �� ������Ʈ�� �ڽ� ������Ʈ�� ���� �� �����Ѵ�.
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Comp"));
	cameraComp->SetupAttachment(springArmComp);

	// ���̷�Ż �޽� ������ �����ؼ� SkeletalMesh Component�� �ֱ�
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> playerMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny"));

	if (playerMesh.Succeeded())
	{
		USkeletalMeshComponent* myMesh = GetMesh();
		myMesh->SetSkeletalMesh(playerMesh.Object);
		myMesh->SetRelativeLocation(FVector(0, 0, -90.0f));
		myMesh->SetRelativeRotation(FRotator(0, -90.0f, 0));
	}
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

