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

	widgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	widgetComp->SetupAttachment(RootComponent);
	widgetComp->SetRelativeLocation(FVector(0, 0, 180));
	widgetComp->SetDrawSize(FVector2D(300, 200));
	widgetComp->SetWidgetSpace(EWidgetSpace::Screen);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// ���忡 �ִ� �÷��̾� ĳ���͸� ã�´�.
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

	// �ڽ��� �̸��� ������ ����ϱ�
	hpWidget = Cast<UHPWidget>(widgetComp->GetWidget());

	if (hpWidget != nullptr)
	{
		hpWidget->SetOwnerName(GetActorNameOrLabel());
	}

	// ������ ���� ��ġ�� �����Ѵ�.
	startLocation = GetActorLocation();
	startRotation = GetActorRotation();

	// ü�� �ʱ�ȭ
	currentHP = maxHP;
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

	// ���� ü���� ������ �����Ѵ�.
	if (hpWidget != nullptr)
	{
		hpWidget->SetHP(currentHP, maxHP);
	}
}

// Enemy�� ������ ó�� �Լ�
void AEnemy::OnDamage(int32 damage)
{
	currentHP = FMath::Max(0, currentHP - damage);
	
	// ����� ü���� 0 �����̸� ���� ���·� ó���Ѵ�.
	if (currentHP <= 0)
	{
		enemyState = EEnemyState::DIE;
		UE_LOG(LogTemp, Warning, TEXT("Current Enemy state: %s"), *UEnum::GetValueAsString(enemyState));
		DieAction();
		
		// ���� ��� ���̽��� ������ �߰��Ѵ�.
		ADIPGameModeBase* gm = Cast<ADIPGameModeBase>(GetWorld()->GetAuthGameMode());
		if (gm != nullptr)
		{
			gm->AddScore(1);
		}
	}
	// ����� ü���� 0 �̻��̸� �˹� ó���� �Ѵ�.
	else
	{
		// �ǰ� �ִϸ��̼� ��Ÿ�ָ� �÷����Ѵ�.
		UAnimInstance* enemyAnimInstance = bodyMeshComp->GetAnimInstance();
		if (enemyAnimInstance != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Montage Play!!!"));
			enemyAnimInstance->Montage_Play(hitMontage);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Anim Instance..."));
		}

		knockBackLocation = GetActorLocation() + GetActorForwardVector() * -1 * knockBackRange;
		enemyState = EEnemyState::HIT;
	}
	
}

void AEnemy::IdleAction()
{
	// ����, �ڽ��� ���� 7���� ������ �÷��̾� ĳ���Ͱ� �ִٸ�...
	if (target != nullptr)
	{
		FVector directionVec = target->GetActorLocation() - GetActorLocation();
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
	// ������ ������ �Ÿ��� ����ٸ� ���� ���·� ��ȯ�Ѵ�.
	if (FVector::Distance(startLocation, GetActorLocation()) > returnDistance && enemyState != EEnemyState::RETURN)
	{
		enemyState = EEnemyState::RETURN;
		target = startObject;
		UE_LOG(LogTemp, Warning, TEXT("Current Enemy state: %s"), *UEnum::GetValueAsString(enemyState));
		return;
	}

	// ���� �ɷ��ִ� Ÿ�̸ӵ��� �����Ѵ�.
	//GetWorldTimerManager().ClearAllTimersForObject(this);
	//GetWorldTimerManager().ClearTimer(attackTimer);

	FVector dir = target->GetActorLocation() - GetActorLocation();
	dir.Z = 0;
	dir.Normalize();

	// �̵��Ϸ��� �������� ȸ���Ѵ�.
	FRotator rot = dir.ToOrientationRotator();
	SetActorRotation(rot);

	// �÷��̾��� �������� �̵��Ѵ�.
	SetActorLocation(GetActorLocation() + dir * 600.0f * GetWorld()->DeltaTimeSeconds);

	// attack Distance���� ���������� ���¸� Attack ���·� �����Ѵ�.
	float distance = FVector::Distance(target->GetActorLocation(), GetActorLocation());
	if (distance < attackDistance)
	{
		enemyState = EEnemyState::ATTACKDELAY;
		currentDelay = 1.0f;
		UE_LOG(LogTemp, Warning, TEXT("Current Enemy State: %s"), *UEnum::GetValueAsString(enemyState));

		// ��� 1) Timer Manager�� �̿��ؼ� �ݺ��ϴ� ���
		//FTimerHandle attackTimer;
		//GetWorldTimerManager().SetTimer(attackTimer, this, &AEnemy::AttackTest, 2.0f, true);
	}
}

void AEnemy::AttackAction()
{
	// ����, ���� ������ �Ÿ��� ���� ���� ������ ����ٸ�....
	if (FVector::Distance(target->GetActorLocation(), GetActorLocation()) > attackDistance)
	{
		enemyState = EEnemyState::MOVE;
		UE_LOG(LogTemp, Warning, TEXT("Current Enemy State: %s"), *UEnum::GetValueAsString(enemyState));
		return;
	}


	// ����� �����ϴ� �Լ��� �����Ѵ�.
	// ��� 2) ���� ��ȯ�� �̿��ؼ� �ݺ��ϴ� ���
	//AttackTest();
	/*if (enemyState == EEnemyState::ATTACK)
	{
		enemyState = EEnemyState::ATTACKDELAY;
	}*/

}

void AEnemy::AttackDelayAction()
{
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
	SetActorLocation(GetActorLocation() + dir.GetSafeNormal() * moveSpeed * GetWorld()->GetDeltaSeconds());

	if (dir.Length() < 10)
	{
		SetActorLocation(startLocation);
		SetActorRotation(startRotation);
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
	// 3�� �ڿ� �����Ѵ�.
	FTimerHandle dieTimer;
	//GetWorld()->GetTimerManager().SetTimer(...)
	GetWorldTimerManager().SetTimer(dieTimer, this, &AEnemy::DestroyProcess, 3.0f, false);

	// ĸ�� �ݸ����� �浹�� Off�Ѵ�.
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

