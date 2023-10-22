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

	hp = maxHP;

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
	cameraComp->FieldOfView = FMath::Lerp(cameraComp->FieldOfView, fov, DeltaTime*5);

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
		enhancedInputComponent->BindAction(ia_fire, ETriggerEvent::Started, this, &AMyCharacter::OnFireInput);
		enhancedInputComponent->BindAction(ia_throw, ETriggerEvent::Completed, this, &AMyCharacter::OnThrowInput);
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

void AMyCharacter::OnFireInput(const struct FInputActionValue& value)
{
	if (hp <= 0)
	{
		return;
	}

#pragma region 1. 순수한 블루프린트 파일을 월드에 생성하는 방법
	//
	/*FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(bullet_bp, gunMeshComp->GetSocketLocation(FName("Muzzle")), gunMeshComp->GetSocketRotation(FName("Muzzle")), params);*/
#pragma endregion

#pragma region 2. CPP 파일 기반 블루프린트 파일을 월드에 생성하는 방법
	/*FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABulletActor* spawnedBullet = GetWorld()->SpawnActor<ABulletActor>(bullet_bp, gunMeshComp->GetSocketLocation(FName("Muzzle")), gunMeshComp->GetSocketRotation(FName("Muzzle")), params);*/
#pragma endregion	

	//FVector startLoc = gunMeshComp->GetSocketLocation(FName("Muzzle"));
	//FVector endLoc = startLoc + gunMeshComp->GetRightVector() * 1000.0f;
	FVector startLoc = cameraComp->GetComponentLocation() + cameraComp->GetForwardVector() * springArmComp->TargetArmLength;
	FVector endLoc = startLoc + cameraComp->GetForwardVector() * 1000.0f;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

#pragma region 3. 라인 트레이스로 발사하기 - 싱글
	FHitResult hitInfo;
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startLoc, endLoc, ECC_Visibility, params);

	if (bHit)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *hitInfo.GetActor()->GetActorNameOrLabel());
		AEnemy* enemy = Cast<AEnemy>(hitInfo.GetActor());
		if (enemy != nullptr)
		{
			enemy->OnDamage(rifleDamage);
			UE_LOG(LogTemp, Warning, TEXT("Enemy hp: %d"), enemy->GetCurrentHP());

			// 만일, Enemy의 체력이 0 이하인 경우에는 점수를 1점 추가한다.
			/*if (mainWidget_inst != nullptr && enemy->GetCurrentHP() <= 0)
			{
				mainWidget_inst->AddScore(1);
			}*/
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Hit!!"));
	}

	DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, false, 5, 0, 2);

#pragma endregion	

#pragma region 4. 라인 트레이스 발사하기 - 멀티
	//TArray<FHitResult> hitInfos;
	//bool bHit = GetWorld()->LineTraceMultiByChannel(hitInfos, startLoc, endLoc, ECC_Visibility, params);
	//if (bHit)
	//{
	//	for (FHitResult hitInfo : hitInfos)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("%s"), *hitInfo.GetActor()->GetActorNameOrLabel());
	//	}

	//	FVector hitLoc = hitInfos[0].ImpactPoint;

	//	DrawDebugLine(GetWorld(), startLoc, hitLoc, FColor::Red, false, 2.0f, 0, 1.0f);
	//	DrawDebugLine(GetWorld(), hitLoc, endLoc, FColor(0, 255, 0), false, 2.0f, 0, 1.0f);
	//	DrawDebugBox(GetWorld(), hitLoc, FVector(5), FQuat::Identity, FColor(255, 0, 0), false, 2.0f, 0, 2.0f);
	//}
	//else
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("No Hit!!"));
	//	DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, false, 2.0f, 0, 1.0f);
	//}
#pragma endregion	
	
#pragma region 5. 라인 트레이스 오브젝트 타입으로 체크하기
	/*FHitResult hitInfo;
	FCollisionObjectQueryParams objectQueryParams;
	objectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	if (GetWorld()->LineTraceSingleByObjectType(hitInfo, startLoc, endLoc, objectQueryParams, params))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Object Name: %s"), *hitInfo.GetActor()->GetActorNameOrLabel());
	}*/
#pragma endregion

#pragma region 6. 콜리전 프로필을 이용한 라인 트레이스
	//FHitResult hitInfo;
	//if (GetWorld()->LineTraceSingleByProfile(hitInfo, startLoc, endLoc, FName("TestActor"), params))
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Hit Object Name: %s"), *hitInfo.GetActor()->GetActorNameOrLabel());

	//	// 충돌 지점에 특정한 이펙트를 생성한다.
	//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hit_fx, hitInfo.ImpactPoint, FRotator::ZeroRotator, true);
	//}

#pragma endregion
	
	// 총 발사음을 플레이한다.
	UGameplayStatics::PlaySound2D(GetWorld(), fire_sound, 0.3f);

	//// 카메라 셰이킹을 한다.
	//if (pc != nullptr && shake_bp != nullptr)
	//{
	//	pc->ClientStartCameraShake(shake_bp);
	//}

	//// 카메라 페이드 인을 한다.
	//pc->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 1.0f, FLinearColor(1, 1, 1), true);

	// 총 발사 애니메이션을 플레이한다.
	PlayAnimMontage(fireAnim);
}

// 수류탄 투척 함수
void AMyCharacter::OnThrowInput()
{
	if (hp <= 0)
	{
		return;
	}

	// 1. 수류탄을 생성한다.
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AGrenadeActor* grenade_inst = GetWorld()->SpawnActor<AGrenadeActor>(grenade_bp, GetActorLocation() + GetActorForwardVector() * 100, FRotator::ZeroRotator, params);
	
	if (grenade_inst != nullptr)
	{
		// 2. 수류탄 액터의 물리 상태를 던질 수 있는 상태로 변경한다.
		grenade_inst->sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		grenade_inst->sphereCollision->SetSimulatePhysics(true);
		grenade_inst->sphereCollision->SetEnableGravity(true);

		// 3. 수류탄을 대각선 45도로 물리적으로 발사한다.
		FVector throwDir = (GetActorForwardVector() + GetActorUpVector()).GetSafeNormal();
		grenade_inst->sphereCollision->AddImpulse(throwDir * throwPower);
	}

	PlayAnimMontage(fireAnim2);
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

