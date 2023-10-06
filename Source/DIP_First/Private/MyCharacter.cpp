// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


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

	// ������ �÷��̾� ��Ʈ�ѷ��� �����´�.
	APlayerController* pc = GetController<APlayerController>();

	// Enhanced Input Local Player Subsystem ��������
	if (pc != nullptr)
	{
		UEnhancedInputLocalPlayerSubsystem* enhancedInputSubsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		// Input Mapping Context ������ �����ϱ�
		if (enhancedInputSubsys != nullptr)
		{
			enhancedInputSubsys->AddMappingContext(myIMC_File, 0);
		}
	}
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// moveDir ������ ���� �ִµ��� ĳ���͸� �̵��Ѵ�.
	// p = p0+vt or AddMovementInput()
	moveDir.Normalize();
	//moveDir = moveDir.GetSafeNormal();

	AddMovementInput(moveDir);
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ���� �Է� �̺�Ʈ ������ EnhancedInput ���·� ĳ����(��ȯ)�Ѵ�.
	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	// �Է� Ű�� ������ �Լ��� ����(Bind)�ϱ�
	if (enhancedInputComponent != nullptr)
	{
		enhancedInputComponent->BindAction(ia_jump, ETriggerEvent::Started, this, &ACharacter::Jump);
		enhancedInputComponent->BindAction(ia_jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		enhancedInputComponent->BindAction(ia_move, ETriggerEvent::Triggered, this, &AMyCharacter::OnMoveInput);
		enhancedInputComponent->BindAction(ia_move, ETriggerEvent::Completed, this, &AMyCharacter::OnMoveInput);
	}
}


void AMyCharacter::OnMoveInput(const FInputActionValue& value)
{
	// Ű �Է� �̺�Ʈ �߻� �ÿ� ���� value ���� FVector2D ���·� ��ȯ�Ѵ�.
	FVector2D moveInput = value.Get<FVector2D>();
	moveDir.X = moveInput.X;
	moveDir.Y = moveInput.Y;
}
