// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"


void UEnemyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	// ���� �� �ִϸ��̼� �������Ʈ�� �����ϰ� �ִ� ���ʹ� �ν��Ͻ��� ������ ������ �����Ѵ�.
	enemyActor = Cast<AEnemy>(TryGetPawnOwner());

	//UE_LOG(LogTemp, Warning, TEXT("Get Pawn : %s"), enemyActor != nullptr ? *FString("Success!") : *FString("Failed..."));
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// ���ʹ��� ���� ���� �޾Ƽ� enum ������ �����Ѵ�.
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
