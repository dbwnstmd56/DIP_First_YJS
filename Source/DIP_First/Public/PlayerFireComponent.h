// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerFireComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIP_FIRST_API UPlayerFireComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerFireComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<class ABulletActor> bullet_bp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<class AGrenadeActor> grenade_bp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UParticleSystem* hit_fx;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class USoundBase* fire_sound;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<class UCameraShakeBase> shake_bp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UAnimMontage* fireAnim;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UAnimMontage* fireAnim2;

	UPROPERTY(EditAnywhere, Category = MySettings)
	float throwPower = 1000;

	UPROPERTY(EditAnywhere, Category = MySettings)
	int32 rifleDamage = 30;

	UPROPERTY(EditAnywhere, Category = MySettings)
	float rifleRange = 2000.0f;

	void OnFireInput(const struct FInputActionValue& value);
	void OnThrowInput();

private:
	class AMyCharacter* owningPlayer;

	
};
