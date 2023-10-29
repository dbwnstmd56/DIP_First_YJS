// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BulletActor.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GrenadeActor.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"
#include "DIPGameModeBase.h"
#include "Components/WidgetComponent.h"
#include "HPWidget.h"
#include "PlayerFireComponent.h"


#define PrintMsg(msg) UE_LOG(LogTemp, Warning, TEXT("%s(%d): %s"), *FString(__FUNCTION__), __LINE__, msg)


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

	// 총 외형을 보여주기 위한 스켈레탈 메시 컴포넌트를 바디 메시의 자식 컴포넌트로 생성 및 부착한다.
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Mesh Comp"));
	gunMeshComp->SetupAttachment(GetMesh(), FName("WeaponSocket"));

	// 총 발사 기능 컴포넌트를 추가한다.
	playerFireComp = CreateDefaultSubobject<UPlayerFireComponent>(TEXT("Player Fire Component"));

	// 캡슐 콜리전의 프리셋을 적용하기
	GetCapsuleComponent()->SetCollisionProfileName(FName("PlayerPreset"));


	// 스켈레탈 메시 파일을 생성해서 SkeletalMesh Component에 넣기
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> playerMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny"));

	if (playerMesh.Succeeded())
	{
		USkeletalMeshComponent* myMesh = GetMesh();
		myMesh->SetSkeletalMesh(playerMesh.Object);
		myMesh->SetRelativeLocation(FVector(0, 0, -90.0f));
		myMesh->SetRelativeRotation(FRotator(0, -90.0f, 0));
	}

	// 기본 이동 속도를 600cm/s 로 설정한다.
	GetCharacterMovement()->MaxWalkSpeed = 600;

	// 최대 점프 횟수를 3회로 설정한다.
	JumpMaxCount = 3;

	// 웅크리기를 가능하게 설정한다.
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// 헤드업 디스플레이 위젯 컴포넌트를 생성 및 루트에 부착한다.
	healthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health UI"));
	healthWidget->SetupAttachment(RootComponent);

	static ConstructorHelpers::FClassFinder<UHPWidget> ui_obj(TEXT("/Game/UI/CPP/WBP_HealthWidget"));
	if (ui_obj.Succeeded() && healthWidget != nullptr)
	{
		healthWidget->SetWidgetClass(ui_obj.Class);
		healthWidget->SetDrawSize(FVector2D(300.0f, 200.0f));
		healthWidget->SetRelativeLocation(FVector(0, 0, 150));
	}
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 현재의 플레이어 컨트롤러를 가져온다.
	pc = GetController<APlayerController>();

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

	// 생성된 health UI 인스턴스를 전역 변수에 저장해놓는다.
	health_inst = Cast<UHPWidget>(healthWidget->GetUserWidgetObject());
	UE_LOG(LogTemp, Warning, TEXT("Result: %s"), health_inst == nullptr ? *FString("Null") : *FString("Success"));

	hp = maxHP;
	if (health_inst != nullptr)
	{
		health_inst->SetOwnerName(GetActorNameOrLabel());
	}


	// 로그를 출력하기
	//UE_LOG(LogTemp, Log, TEXT("%s(%d): MyLog1"), *FString(__FUNCTION__), __LINE__);
	/*PrintMsg(*FString("My Log !"));
	UE_LOG(LogTemp, Warning, TEXT("MyLog2"));
	UE_LOG(LogTemp, Error, TEXT("MyLog3"));*/

	// Pawn에 대한 Collision Response를 Block -> Ignore로 변경
	/*GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 체력을 위젯에 반영한다.
	if (health_inst != nullptr)
	{
		health_inst->SetHP(hp, maxHP);
	}

	if (hp <= 0)
	{
		return;
	}

	// moveDir 변수의 값에 있는데로 캐릭터를 이동한다.
	// p = p0+vt or AddMovementInput()
	moveDir.Normalize();
	//moveDir = moveDir.GetSafeNormal();

	// 플레이어 콘트롤러의 방향을 기준으로 입력 값을 다시 계산한다.
	//moveDir = pc->GetControlRotation() * moveDir.X + pc->GetControlRotation() * moveDir.Y;
	FVector forwardVector = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::X);
	FVector rightVector = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::Y);
	moveDir = (forwardVector * moveDir.X + rightVector * moveDir.Y).GetSafeNormal();

	AddMovementInput(moveDir);

	//UE_LOG(LogTemp, Warning, TEXT("Walk Speed : %.3f"), GetCharacterMovement()->MaxWalkSpeed);

	// 캐릭터 좌우 회전 - 플레이어 컨트롤러 회전 기반
	AddControllerYawInput(rotateAxis.Yaw);

	// 카메라 상하 회전 - 스프링 암 회전 기반
	FRotator currentCamRot = springArmComp->GetComponentRotation();
	float modifyPitch = currentCamRot.Pitch + rotateAxis.Pitch;

	// 스프링 암의 회전 범위는 -60도 ~ 60도 사이로 제한한다.
	modifyPitch = FMath::Clamp(modifyPitch, -60, 60);
	FRotator modifiedRot = FRotator(modifyPitch, currentCamRot.Yaw, currentCamRot.Roll);
	springArmComp->SetWorldRotation(modifiedRot);

	// 카메라의 화각을 fov의 값으로 서서히 변경한다.
	cameraComp->FieldOfView = FMath::Lerp(cameraComp->FieldOfView, fov, DeltaTime * 5);

	

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
		enhancedInputComponent->BindAction(ia_rotate, ETriggerEvent::Triggered, this, &AMyCharacter::OnRotateInput);
		enhancedInputComponent->BindAction(ia_rotate, ETriggerEvent::Completed, this, &AMyCharacter::OnRotateInput);
		enhancedInputComponent->BindAction(ia_dash, ETriggerEvent::Started, this, &AMyCharacter::OnDashInputStart);
		enhancedInputComponent->BindAction(ia_dash, ETriggerEvent::Completed, this, &AMyCharacter::OnDashInputEnd);
		enhancedInputComponent->BindAction(ia_fire, ETriggerEvent::Started, playerFireComp, &UPlayerFireComponent::OnFireInput);
		enhancedInputComponent->BindAction(ia_throw, ETriggerEvent::Completed, playerFireComp, &UPlayerFireComponent::OnThrowInput);
		enhancedInputComponent->BindAction(ia_zoom, ETriggerEvent::Started, this, &AMyCharacter::OnZoomInCamera);
		enhancedInputComponent->BindAction(ia_zoom, ETriggerEvent::Completed, this, &AMyCharacter::OnZoomOutCamera);
	}
}


void AMyCharacter::OnMoveInput(const FInputActionValue& value)
{
	// 키 입력 이벤트 발생 시에 받은 value 값을 FVector2D 형태로 변환한다.
	FVector2D moveInput = value.Get<FVector2D>();
	moveDir.X = moveInput.X;
	moveDir.Y = moveInput.Y;
}

void AMyCharacter::OnRotateInput(const FInputActionValue& value)
{
	// 마우스 입력 값을 RotateAxis 변수의 Yaw축과 Pitch축에 저장한다.
	FVector2D rotateInput = value.Get<FVector2D>();
	rotateAxis.Yaw = rotateInput.X;
	rotateAxis.Pitch = rotateInput.Y;
}

void AMyCharacter::OnDashInputStart(const FInputActionValue& value)
{
	//GetCharacterMovement()->MaxWalkSpeed = 2400.0f;
	Crouch();
}

void AMyCharacter::OnDashInputEnd(const FInputActionValue& value)
{
	//GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	UnCrouch();
}

void AMyCharacter::OnZoomInCamera()
{
	springArmComp->TargetArmLength = -50;
	fov = 45.0f;
}

void AMyCharacter::OnZoomOutCamera()
{
	springArmComp->TargetArmLength = 500;
	fov = 90.0f;
}

// 블루프린트 노드로 사용할 함수
bool AMyCharacter::MyLineTraceMultiByChannel(TArray<FHitResult>& hitInfos, const FVector startLoc, const FVector endLoc, ECollisionChannel ecc)
{
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceMultiByChannel(hitInfos, startLoc, endLoc, ecc, params);

	if (bHit)
	{
		FVector hitLoc = hitInfos[0].ImpactPoint;

		DrawDebugLine(GetWorld(), startLoc, hitLoc, FColor::Red, false, 2.0f, 0, 1.0f);
		DrawDebugLine(GetWorld(), hitLoc, endLoc, FColor(0, 255, 0), false, 2.0f, 0, 1.0f);
		DrawDebugBox(GetWorld(), hitLoc, FVector(5), FQuat::Identity, FColor(255, 0, 0), false, 2.0f, 0, 2.0f);
	}
	else
	{
		DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, false, 2.0f, 0, 1.0f);
	}

	return bHit;
}

int32 AMyCharacter::Add(int32 num1, int32 num2)
{
	return num1 + num2;
}

void AMyCharacter::DamagePlayer(int32 damage)
{
	hp = FMath::Max(0, hp - damage);

	if (hp <= 0)
	{
		ADIPGameModeBase* gm = Cast<ADIPGameModeBase>(GetWorld()->GetAuthGameMode());
		if (gm != nullptr)
		{
			// 게임 오버 위젯을 화면에 출력한다.
			gm->LoadGameOverUI();
		}

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

