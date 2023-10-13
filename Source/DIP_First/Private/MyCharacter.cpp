// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BulletActor.h"



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

	// �� ������ �����ֱ� ���� ���̷�Ż �޽� ������Ʈ�� �ٵ� �޽��� �ڽ� ������Ʈ�� ���� �� �����Ѵ�.
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Mesh Comp"));
	gunMeshComp->SetupAttachment(GetMesh(), FName("WeaponSocket"));


	// ���̷�Ż �޽� ������ �����ؼ� SkeletalMesh Component�� �ֱ�
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> playerMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny"));

	if (playerMesh.Succeeded())
	{
		USkeletalMeshComponent* myMesh = GetMesh();
		myMesh->SetSkeletalMesh(playerMesh.Object);
		myMesh->SetRelativeLocation(FVector(0, 0, -90.0f));
		myMesh->SetRelativeRotation(FRotator(0, -90.0f, 0));
	}

	// �⺻ �̵� �ӵ��� 600cm/s �� �����Ѵ�.
	GetCharacterMovement()->MaxWalkSpeed = 600;
	
	// �ִ� ���� Ƚ���� 3ȸ�� �����Ѵ�.
	JumpMaxCount = 3;

	// ��ũ���⸦ �����ϰ� �����Ѵ�.
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ������ �÷��̾� ��Ʈ�ѷ��� �����´�.
	pc = GetController<APlayerController>();

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

	// �α׸� ����ϱ�
	UE_LOG(LogTemp, Log, TEXT("MyLog1"));
	UE_LOG(LogTemp, Warning, TEXT("MyLog2"));
	UE_LOG(LogTemp, Error, TEXT("MyLog3"));
	
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// moveDir ������ ���� �ִµ��� ĳ���͸� �̵��Ѵ�.
	// p = p0+vt or AddMovementInput()
	moveDir.Normalize();
	//moveDir = moveDir.GetSafeNormal();

	// �÷��̾� ��Ʈ�ѷ��� ������ �������� �Է� ���� �ٽ� ����Ѵ�.
	//moveDir = pc->GetControlRotation() * moveDir.X + pc->GetControlRotation() * moveDir.Y;
	FVector forwardVector = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::X);
	FVector rightVector = FRotationMatrix(pc->GetControlRotation()).GetUnitAxis(EAxis::Y);
	moveDir = (forwardVector * moveDir.X + rightVector * moveDir.Y).GetSafeNormal();

	AddMovementInput(moveDir);

	//UE_LOG(LogTemp, Warning, TEXT("Walk Speed : %.3f"), GetCharacterMovement()->MaxWalkSpeed);

	// ĳ���� �¿� ȸ�� - �÷��̾� ��Ʈ�ѷ� ȸ�� ���
	AddControllerYawInput(rotateAxis.Yaw);

	// ī�޶� ���� ȸ�� - ������ �� ȸ�� ���
	FRotator currentCamRot = springArmComp->GetComponentRotation();
	float modifyPitch = currentCamRot.Pitch + rotateAxis.Pitch;
	
	// ������ ���� ȸ�� ������ -60�� ~ 60�� ���̷� �����Ѵ�.
	modifyPitch = FMath::Clamp(modifyPitch, -60, 60);
	FRotator modifiedRot = FRotator(modifyPitch, currentCamRot.Yaw, currentCamRot.Roll);
	springArmComp->SetWorldRotation(modifiedRot);
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
		enhancedInputComponent->BindAction(ia_rotate, ETriggerEvent::Triggered, this, &AMyCharacter::OnRotateInput);
		enhancedInputComponent->BindAction(ia_rotate, ETriggerEvent::Completed, this, &AMyCharacter::OnRotateInput);
		enhancedInputComponent->BindAction(ia_dash, ETriggerEvent::Started, this, &AMyCharacter::OnDashInputStart);
		enhancedInputComponent->BindAction(ia_dash, ETriggerEvent::Completed, this, &AMyCharacter::OnDashInputEnd);
		enhancedInputComponent->BindAction(ia_fire, ETriggerEvent::Started, this, &AMyCharacter::OnFireInput);
	}
}


void AMyCharacter::OnMoveInput(const FInputActionValue& value)
{
	// Ű �Է� �̺�Ʈ �߻� �ÿ� ���� value ���� FVector2D ���·� ��ȯ�Ѵ�.
	FVector2D moveInput = value.Get<FVector2D>();
	moveDir.X = moveInput.X;
	moveDir.Y = moveInput.Y;
}

void AMyCharacter::OnRotateInput(const FInputActionValue& value)
{
	// ���콺 �Է� ���� RotateAxis ������ Yaw��� Pitch�࿡ �����Ѵ�.
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

void AMyCharacter::OnFireInput(const struct FInputActionValue& value)
{
	// 1. ������ �������Ʈ ������ ���忡 �����ϴ� ���
	/*FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(bullet_bp, gunMeshComp->GetSocketLocation(FName("Muzzle")), gunMeshComp->GetSocketRotation(FName("Muzzle")), params);*/

	// 2. CPP ���� ��� �������Ʈ ������ ���忡 �����ϴ� ���
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABulletActor* spawnedBullet = GetWorld()->SpawnActor<ABulletActor>(bullet_bp, gunMeshComp->GetSocketLocation(FName("Muzzle")), gunMeshComp->GetSocketRotation(FName("Muzzle")), params);
	


}

