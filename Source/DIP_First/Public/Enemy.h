// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enemy.generated.h"


// Enemy의 상태 종류
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	IDLE	UMETA(DisplayName="Idle State"),
	MOVE	UMETA(DisplayName = "Move State"),
	ATTACK	UMETA(DisplayName = "Attack State"),
	ATTACKDELAY	UMETA(DisplayName = "Attack Delay State"),
	RETURN	UMETA(DisplayName = "Return State"),
	HIT	UMETA(DisplayName = "Hit State"),
	DIE	UMETA(DisplayName = "Die State"),
};


UCLASS()
class DIP_FIRST_API AEnemy : public APawn
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
	class UWidgetComponent* widgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	class UFloatingPawnMovement* pawnMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	EEnemyState enemyState = EEnemyState::IDLE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	float moveSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	float attackDistance = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	float returnDistance = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	class AActor* startObject;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	int32 maxHP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	float knockBackRange = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	class UAnimMontage* hitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	class UAnimMontage* dieMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	class AActor* boxActor;


	// 에너미의 체력 처리 관련 함수
	UFUNCTION(BlueprintCallable)
	void OnDamage(int32 damage);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurrentHP() { return currentHP; };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MySettings)
	class AMyCharacter* player;

	UFUNCTION(BlueprintCallable)
	void AttackTest();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

private:
	UPROPERTY(VisibleAnywhere, meta=(AllowAccessPrivate))
	class AActor* target;

	UPROPERTY(EditAnywhere, Category=MySettings, meta=(AllowPrivateAccess = true))
	float attackPower = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=MySettings, meta=(AllowPrivateAccess = true))
	int32 currentHP = 0;
	
	FTimerHandle attackTimer;
	float currentDelay = 0;
	FVector startLocation;
	FRotator startRotation;
	FVector knockBackLocation;
	class UHPWidget* hpWidget;
	//bool bCanAttack = true;
	class UAnimInstance* enemyAnimInstance;
	class AAIController* aiCon;

	void IdleAction();
	void MoveAction();
	void AttackAction();
	void AttackDelayAction();
	void ReturnAction();
	void HitAction();
	void DieAction();
	void DestroyProcess();
};
