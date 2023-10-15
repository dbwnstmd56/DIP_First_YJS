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
	
	// ���忡 �ִ� �÷��̾� ĳ���͸� ã�´�.
	AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMyCharacter::StaticClass());
	player = Cast<AMyCharacter>(playerActor);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Enemy�� ���¿� ���� ����� �Լ��� �����Ѵ�.
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
	// ����, �ڽ��� ���� 7���� ������ �÷��̾� ĳ���Ͱ� �ִٸ�...
	if (player != nullptr)
	{
		FVector directionVec = player->GetActorLocation() - GetActorLocation();
		float result = FVector::DotProduct(GetActorForwardVector(), directionVec.GetSafeNormal());
		
		//float dist = FVector::Distance(player->GetActorLocation(), GetActorLocation());

		if (result >= 0 && directionVec.Length() < 700.0f)
		{
			// ���¸� Move ���·� ��ȯ�Ѵ�.
			enemyState = EEnemyState::MOVE;
			UE_LOG(LogTemp, Warning, TEXT("Current Enemy state: %s"), *UEnum::GetValueAsString(enemyState));
			
		}
	}
}

void AEnemy::MoveAction()
{
	FVector dir = player->GetActorLocation() - GetActorLocation();
	dir.Normalize();
	
	// �̵��Ϸ��� �������� ȸ���Ѵ�.

	// �÷��̾��� �������� �̵��Ѵ�.
	SetActorLocation(GetActorLocation() + dir * 600.0f * GetWorld()->DeltaTimeSeconds);

	// attack Distance���� ���������� ���¸� Attack ���·� �����Ѵ�.

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

