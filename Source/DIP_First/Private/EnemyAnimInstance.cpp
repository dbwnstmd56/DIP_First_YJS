// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"


void UEnemyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	// 현재 이 애니메이션 블루프린트를 소유하고 있는 에너미 인스턴스를 포인터 변수에 저장한다.
	enemyActor = Cast<AEnemy>(TryGetPawnOwner());

	//UE_LOG(LogTemp, Warning, TEXT("Get Pawn : %s"), enemyActor != nullptr ? *FString("Success!") : *FString("Failed..."));
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 에너미의 상태 값을 받아서 enum 변수에 저장한다.
	if (enemyActor != nullptr)
	{
		ownerState = enemyActor->enemyState;
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, UEnum::GetValueAsString<EEnemyState>(ownerState));
	}

}

void UEnemyAnimInstance::AnimNotify_AttackEnd()
{
	if (enemyActor != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackEnd Notify Called!"));
		enemyActor->AttackEnd();
	}
}

void UEnemyAnimInstance::AnimNotify_AttackDamage()
{
	if (enemyActor != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackDamage Notify Called!"));
		enemyActor->AttackTest();
	}
}
