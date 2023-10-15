// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DIP_FIRST_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=MySettings)
	float horizontal;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=MySettings)
	float vertical;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=MySettings)
	bool bIsJump;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=MySettings)
	class AMyCharacter* player;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=MySettings)
	class UCharacterMovementComponent* movementComp;

	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
};
