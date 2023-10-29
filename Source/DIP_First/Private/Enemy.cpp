// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "DIPGameModeBase.h"
#include "Components/WidgetComponent.h"
#include "HPWidget.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "DIPFunctionLibrary.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	capsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	SetRootComponent(capsuleComp);
	capsuleComp->SetCapsuleHalfHeight(110.0f);
	capsuleComp->SetCapsuleRadius(35.0f);
	capsuleComp->SetCollisionProfileName(FName("EnemyPreset"));

	bodyMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body Mesh Component"));
	bodyMeshComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> enemyBody(TEXT("/Game/Animations/Enemy/EnemyMesh"));

	if (enemyBody.Succeeded())
	{
		bodyMeshComp->SetSkeletalMesh(enemyBody.Object);
	}

	bodyMeshComp->SetRelativeLocation(FVector(0, 0, -110.0f));
	bodyMeshComp->SetRelativeRotation(FRotator(0, -90.0f, 0));

	arrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Component"));
	arrowComp->SetupAttachment(RootComponent);
	arrowComp->ArrowColor = FColor::Emerald;

	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	widgetComp->SetupAttachment(RootComponent);
	widgetComp->SetRelativeLocation(FVector(0, 0, 180));
	widgetComp->SetDrawSize(FVector2D(300, 200));
	widgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	pawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// 월드에 있는 플레이어 캐릭터를 찾는다.
	//AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMyCharacter::StaticClass());

	TArray<AActor*> characters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), characters);

	if (characters.Num() > 0)
	{
		for (AActor* playerActor : characters)
		{
			player = Cast<AMyCharacter>(playerActor);
			if (player != nullptr)
			{
				target = player;
				break;
			}
			else
			{
				ACharacter* bp_player = Cast<ACharacter>(playerActor);
				if (bp_player != nullptr)
				{
					target = bp_player;
					break;
				}
			}
		}
	}

	// 자신의 이름을 위젯에 출력하기
	hpWidget = Cast<UHPWidget>(widgetComp->GetWidget());

	if (hpWidget != nullptr)
	{
		hpWidget->SetOwnerName(GetActorNameOrLabel());
	}

	// 최초의 시작 위치를 저장한다.
	startLocation = GetActorLocation();
	startRotation = GetActorRotation();

	// 체력 초기화
	currentHP = maxHP;

	// 현재 설정된 Anim Instance 가져오기
	enemyAnimInstance = bodyMeshComp->GetAnimInstance();

	// 현재 설정된 AI Controller 가져오기
	aiCon = GetController<AAIController>();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Enemy의 상태에 따라 실행될 함수를 선택한다.
	switch (enemyState)
	{
	case EEnemyState::IDLE:
		IdleAction();
		break;
	case EEnemyState::MOVE:
		MoveAction();
		break;
	case EEnemyState::ATTACK:
		AttackAction();
		break;
	case EEnemyState::ATTACKDELAY:
		AttackDelayAction();
		break;
	case EEnemyState::RETURN:
		ReturnAction();
		//MoveAction();
		break;
	case EEnemyState::HIT:
		HitAction();
		break;
	case EEnemyState::DIE:
		//DieAction();
		break;
	}

	// 현재 체력을 위젯에 갱신한다.
	if (hpWidget != nullptr)
	{
		hpWidget->SetHP(currentHP, maxHP);
	}
}

// Enemy의 데미지 처리 함수
void AEnemy::OnDamage(int32 damage)
{
	currentHP = FMath::Max(0, currentHP - damage);
	
	// 변경된 체력이 0 이하이면 죽음 상태로 처리한다.
	if (currentHP <= 0)
	{
		enemyState = EEnemyState::DIE;
		UE_LOG(LogTemp, Warning, TEXT("Current Enemy state: %s"), *UEnum::GetValueAsString(enemyState));
		DieAction();
		
		// 게임 모드 베이스에 점수를 추가한다.
		ADIPGameModeBase* gm = Cast<ADIPGameModeBase>(GetWorld()->GetAuthGameMode());
		if (gm != nullptr)
		{
			gm->AddScore(1);
		}
	}
	// 변경된 체력이 0 이상이면 넉백 처리를 한다.
	else
	{
		// 피격 애니메이션 몽타주를 플레이한다.
		if (enemyAnimInstance != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Montage Play!!!"));
			enemyAnimInstance->Montage_Play(hitMontage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Anim Instance..."));
		}
		FVector dir = (player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		SetActorRotation(dir.ToOrientationRotator());
		knockBackLocation = GetActorLocation() + GetActorForwardVector() * -1 * knockBackRange;
		enemyState = EEnemyState::HIT;
	}
	
}

void AEnemy::IdleAction()
{
	// 만일, 자신의 전방 7미터 지점에 플레이어 캐릭터가 있다면...
	if (target != nullptr)
	{
		FVector directionVec = target->GetActorLocation() - GetActorLocation();
		float result = FVector::DotProduct(GetActorForwardVector(), directionVec.GetSafeNormal());

		//float dist = FVector::Distance(player->GetActorLocation(), GetActorLocation());

		if (result >= 0 && directionVec.Length() < 700.0f)
		{
			// 상태를 Move 상태로 전환한다.
			enemyState = EEnemyState::MOVE;
			UE_LOG(LogTemp, Warning, TEXT("Current Enemy state: %s"), *UEnum::GetValueAsString(enemyState));

		}
	}
}

void AEnemy::MoveAction()
{
	// 추적이 가능한 거리를 벗어났다면 복귀 상태로 전환한다.
	if (FVector::Distance(startLocation, GetActorLocation()) > returnDistance && enemyState != EEnemyState::RETURN)
	{
		aiCon->StopMovement();
		enemyState = EEnemyState::RETURN;
		target = startObject;
		UE_LOG(LogTemp, Warning, TEXT("Current Enemy state: %s"), *UEnum::GetValueAsString(enemyState));
		return;
	}

	// 현재 걸려있는 타이머들을 종료한다.
	//GetWorldTimerManager().ClearAllTimersForObject(this);
	//GetWorldTimerManager().ClearTimer(attackTimer);

	// 이동하려는 방향으로 회전한다.
	FRotator rot = UDIPFunctionLibrary::CalculateRotationFromDirection(this, target);
	SetActorRotation(rot);

	// 플레이어의 방향으로 이동한다.
	//SetActorLocation(GetActorLocation() + dir * 600.0f * GetWorld()->DeltaTimeSeconds, true);
	if (aiCon != nullptr)
	{
		UWorld* currentWorld = GetWorld();
		EPathFollowingRequestResult::Type result = aiCon->MoveToActor(target, attackDistance - 50, false);
		UE_LOG(LogTemp, Warning, TEXT("Move Result: %s"), *UEnum::GetValueAsString<EPathFollowingRequestResult::Type>(result));

		// 이동할 루트를 DebugLine을 이용해서 시각화한다.
		UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(currentWorld);
		if (navSys != nullptr)
		{
			UNavigationPath* navPath = navSys->FindPathToLocationSynchronously(currentWorld, GetActorLocation(), target->GetActorLocation());

			if (navPath != nullptr)
			{
				TArray<FVector> points = navPath->PathPoints;

				for (int32 i = 0; i < points.Num() - 1; i++)
				{
					DrawDebugLine(currentWorld, points[i], points[i+1], FColor::Red, false, 0, 0, 2.0f);
					DrawDebugSphere(currentWorld, points[i], 5, 12, FColor::Red, false, 0, 0, 2.0f);
				}
			}
		}
	}
	

	// attack Distance까지 접근했으면 상태를 Attack 상태로 변경한다.
	float distance = FVector::Distance(target->GetActorLocation(), GetActorLocation());
	if (distance <= attackDistance)
	{
		aiCon->StopMovement();
		enemyState = EEnemyState::ATTACKDELAY;
		currentDelay = 1.0f;
		UE_LOG(LogTemp, Warning, TEXT("Current Enemy State: %s"), *UEnum::GetValueAsString(enemyState));

		// 방법 1) Timer Manager를 이용해서 반복하는 방식
		//FTimerHandle attackTimer;
		//GetWorldTimerManager().SetTimer(attackTimer, this, &AEnemy::AttackTest, 2.0f, true);
	}
}

void AEnemy::AttackAction()
{
	// 만일, 공격 대상과의 거리가 공격 가능 범위를 벗어났다면....
	if (FVector::Distance(target->GetActorLocation(), GetActorLocation()) > attackDistance)
	{
		enemyState = EEnemyState::MOVE;
		UE_LOG(LogTemp, Warning, TEXT("Current Enemy State: %s"), *UEnum::GetValueAsString(enemyState));
		return;
	}


	// 대상을 공격하는 함수를 실행한다.
	// 방법 2) 상태 전환을 이용해서 반복하는 방식
	//AttackTest();
	/*if (enemyState == EEnemyState::ATTACK)
	{
		enemyState = EEnemyState::ATTACKDELAY;
	}*/

}

void AEnemy::AttackDelayAction()
{
	// 공격하려는 방향으로 회전한다.
	FRotator rot = UDIPFunctionLibrary::CalculateRotationFromDirection(this, target);
	SetActorRotation(rot);

	currentDelay += GetWorld()->GetDeltaSeconds();

	if (currentDelay >= 2.0f)
	{
		enemyState = EEnemyState::ATTACK;
		currentDelay = 0;
	}
}

void AEnemy::ReturnAction()
{
	UE_LOG(LogTemp, Warning, TEXT("Returning..."));
	FVector dir = startLocation - GetActorLocation();
	dir.Z = 0;
	SetActorRotation(dir.ToOrientationRotator());
	//SetActorLocation(GetActorLocation() + dir.GetSafeNormal() * moveSpeed * GetWorld()->GetDeltaSeconds());
	if (aiCon != nullptr)
	{
		FAIMoveRequest req = FAIMoveRequest();
		req.SetGoalLocation(startLocation);
		req.SetAcceptanceRadius(1.0f);
		req.SetUsePathfinding(true);
		req.SetProjectGoalLocation(false);
		req.SetReachTestIncludesGoalRadius(true);
		req.SetNavigationFilter(aiCon->GetDefaultNavigationFilterClass());
		req.SetCanStrafe(true);
		req.SetAllowPartialPath(true);
		aiCon->MoveTo(req);
		
		//aiCon->MoveToLocation(startLocation);
	}


	if (dir.Length() < 10)
	{
		SetActorLocation(startLocation);
		SetActorRotation(startRotation);

		// 네비게이션 메시 이동을 멈춘다.
		aiCon->StopMovement();
		enemyState = EEnemyState::IDLE;
	}
}

void AEnemy::HitAction()
{
	FVector newLoc = FMath::Lerp(GetActorLocation(), knockBackLocation, 0.05f);

	if (FVector::Distance(newLoc, knockBackLocation) < 5)
	{
		enemyState = EEnemyState::MOVE;
	}
	else
	{
		SetActorLocation(newLoc);
	}
}

void AEnemy::DieAction()
{
	// 애니메이션 몽타주를 플레이한다.
	//if (enemyAnimInstance != nullptr)
	//{
	//	enemyAnimInstance->Montage_Play(dieMontage);
	//	//enemyAnimInstance->Montage_Stop(0.1f, dieMontage); -> 몽타주를 강제로 종료하기
	//}

	// 3초 뒤에 제거한다.
	FTimerHandle dieTimer;
	//GetWorld()->GetTimerManager().SetTimer(...)
	GetWorldTimerManager().SetTimer(dieTimer, this, &AEnemy::DestroyProcess, 5.0f, false);

	// 캡슐 콜리전의 충돌을 Off한다.
	capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
}

void AEnemy::AttackTest()
{
	//UE_LOG(LogTemp, Warning, TEXT("Attack %s"), *player->GetActorNameOrLabel());
	if (player != nullptr)
	{
		player->DamagePlayer(attackPower);
		UE_LOG(LogTemp, Warning, TEXT("Player Current hp : %d"), player->GetCurrentHP());

		if (player->GetCurrentHP() <= 0)
		{
			enemyState = EEnemyState::RETURN;
			UE_LOG(LogTemp, Warning, TEXT("Current Enemy State: %s"), *UEnum::GetValueAsString(enemyState));
		}
	}
}

void AEnemy::AttackEnd()
{
	enemyState = EEnemyState::ATTACKDELAY;
}

void AEnemy::DestroyProcess()
{
	Destroy();
}

