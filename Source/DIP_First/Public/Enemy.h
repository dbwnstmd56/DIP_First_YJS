// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.generated.h"


// Enemy의 상태 종류
UENUM(BlueprintType)
enum class EEnemyState
{
	IDLE	UMETA(DisplayName="Idle State"),
	MOVE	UMETA(DisplayName = "Move State"),
	ATTACK	UMETA(DisplayName = "Attack State"),
	ATTACKDELAY	UMETA(DisplayName = "Attack Delay State"),
	HIT	UMETA(DisplayName = "Hit State"),
	DIE	UMETA(DisplayName = "Die State"),
};


UCLASS()
class DIP_FIRST_API AEnemy : public AActor
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	class UCapsuleComponent* capsuleComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	class USkeletalMeshComponent* bodyMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	class UArrowComponent* arrowComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	EEnemyState enemyState = EEnemyState::IDLE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	float moveSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	float attackDistance = 150.0f;

private:
	UPROPERTY()
	class AMyCharacter* player;
	
	
	
	void IdleAction();
	void MoveAction();
	void AttackAction();
	void AttackDelayAction();
	void HitAction();
	void DieAction();

};
