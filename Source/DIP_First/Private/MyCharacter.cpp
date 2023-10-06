// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 스프링 암 컴포넌트를 캡슐 컴포넌트에 자식 컴포넌트로 부착한다.
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->TargetArmLength = 500.0f;

	// 카메라 컴포넌트를 스프링 암 컴포넌트의 자식 컴포넌트로 생성 및 부착한다.
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Comp"));
	cameraComp->SetupAttachment(springArmComp);

	// 스켈레탈 메시 파일을 생성해서 SkeletalMesh Component에 넣기
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

	// 현재의 플레이어 컨트롤러를 가져온다.
	APlayerController* pc = GetController<APlayerController>();

	// Enhanced Input Local Player Subsystem 가져오기
	if (pc != nullptr)
	{
		UEnhancedInputLocalPlayerSubsystem* enhancedInputSubsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		// Input Mapping Context 파일을 맵핑하기
		if (enhancedInputSubsys != nullptr)
		{
			enhancedInputSubsys->AddMappingContext(myIMC_File, 0);
		}
	}
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// moveDir 변수의 값에 있는데로 캐릭터를 이동한다.
	// p = p0+vt or AddMovementInput()
	moveDir.Normalize();
	//moveDir = moveDir.GetSafeNormal();

	AddMovementInput(moveDir);
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 기존 입력 이벤트 변수를 EnhancedInput 형태로 캐스팅(변환)한다.
	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	// 입력 키와 실행할 함수를 연결(Bind)하기
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
	// 키 입력 이벤트 발생 시에 받은 value 값을 FVector2D 형태로 변환한다.
	FVector2D moveInput = value.Get<FVector2D>();
	moveDir.X = moveInput.X;
	moveDir.Y = moveInput.Y;
}
