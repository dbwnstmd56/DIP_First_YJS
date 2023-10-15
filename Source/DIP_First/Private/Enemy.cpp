// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	capsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	SetRootComponent(capsuleComp);
	capsuleComp->SetCapsuleHalfHeight(110.0f);
	capsuleComp->SetCapsuleRadius(35.0f);

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
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// 월드에 있는 플레이어 캐릭터를 찾는다.
	AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMyCharacter::StaticClass());
	player = Cast<AMyCharacter>(playerActor);
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
	case EEnemyState::HIT:
		HitAction();
		break;
	case EEnemyState::DIE:
		DieAction();
		break;
	}

	
}


void AEnemy::IdleAction()
{
	// 만일, 자신의 전방 7미터 지점에 플레이어 캐릭터가 있다면...
	if (player != nullptr)
	{
		FVector directionVec = player->GetActorLocation() - GetActorLocation();
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
	FVector dir = player->GetActorLocation() - GetActorLocation();
	dir.Normalize();
	
	// 이동하려는 방향으로 회전한다.

	// 플레이어의 방향으로 이동한다.
	SetActorLocation(GetActorLocation() + dir * 600.0f * GetWorld()->DeltaTimeSeconds);

	// attack Distance까지 접근했으면 상태를 Attack 상태로 변경한다.

}

void AEnemy::AttackAction()
{

}

void AEnemy::AttackDelayAction()
{

}

void AEnemy::HitAction()
{

}

void AEnemy::DieAction()
{

}

