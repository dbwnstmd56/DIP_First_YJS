// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enemy.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DIP_FIRST_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=MySettings)
	class AEnemy* enemyActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=MySettings)
	EEnemyState ownerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	class AActor* ikActor;

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void AnimNotify_AttackEnd();
	
	UFUNCTION()
	void AnimNotify_AttackDamage();
};
