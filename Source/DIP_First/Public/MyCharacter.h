// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"


UCLASS()
class DIP_FIRST_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector moveDir;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	class UCameraComponent* cameraComp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class USpringArmComponent* springArmComp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class USkeletalMeshComponent* gunMeshComp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UWidgetComponent* healthWidget;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UInputMappingContext* myIMC_File;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UInputAction* ia_jump;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UInputAction* ia_move;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UInputAction* ia_rotate;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UInputAction* ia_dash;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UInputAction* ia_fire;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UInputAction* ia_throw;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UInputAction* ia_zoom;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<class ABulletActor> bullet_bp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<class AGrenadeActor> grenade_bp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UParticleSystem* hit_fx;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class USoundBase* fire_sound;

	UPROPERTY(EditAnywhere, Category = MySettings)
	float throwPower = 1000;

	UPROPERTY(EditAnywhere, Category = MySettings)
	TSubclassOf<class UCameraShakeBase> shake_bp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UAnimMontage* fireAnim;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UAnimMontage* fireAnim2;

	UPROPERTY(EditAnywhere, Category = MySettings)
	int32 maxHP = 200;
	
	UPROPERTY(EditAnywhere, Category = MySettings)
	int32 rifleDamage = 30;

	UPROPERTY(EditAnywhere, Category = MySettings)
	float rifleRange = 2000.0f;


	UFUNCTION(BlueprintCallable, DisplayName = "MyLT")
	//UFUNCTION(BlueprintPure, DisplayName = "MyLT")
	bool MyLineTraceMultiByChannel(TArray<struct FHitResult>& hitInfos, const FVector startLoc, const FVector endLoc, enum ECollisionChannel ecc);

	UFUNCTION(BlueprintCallable)
	int32 Add(int32 num1, int32 num2);
	
	FORCEINLINE int32 GetCurrentHP() { return hp; };
	void DamagePlayer(int32 damage);


private:
	int32 hp = 0;
	FRotator rotateAxis;
	class APlayerController* pc;
	float fov = 90;
	class UHPWidget* health_inst;

	void OnMoveInput(const struct FInputActionValue& value);
	void OnRotateInput(const struct FInputActionValue& value);
	void OnDashInputStart(const struct FInputActionValue& value);
	void OnDashInputEnd(const struct FInputActionValue& value);
	void OnFireInput(const struct FInputActionValue& value);
	void OnThrowInput();
	void OnZoomInCamera();
	void OnZoomOutCamera();


	// UPROPERTY 메타 지정자 예시
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = TestCategory)
	int32 testNumber1 = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TestCategory)
	int32 testNumber2 = 20;

	UPROPERTY(EditAnywhere, Category = TestCategory)
	int32 testNumber3 = 30;*/
};
