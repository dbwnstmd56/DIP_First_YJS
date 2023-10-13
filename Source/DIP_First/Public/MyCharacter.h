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

	UPROPERTY(EditAnywhere, Category = MySettings)
	class UCameraComponent* cameraComp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class USpringArmComponent* springArmComp;

	UPROPERTY(EditAnywhere, Category = MySettings)
	class USkeletalMeshComponent* gunMeshComp;
	
	
	
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
	TSubclassOf<class ABulletActor> bullet_bp;


private:
	FVector moveDir;
	FRotator rotateAxis;
	class APlayerController* pc;

	void OnMoveInput(const struct FInputActionValue& value);
	void OnRotateInput(const struct FInputActionValue& value);
	void OnDashInputStart(const struct FInputActionValue& value);
	void OnDashInputEnd(const struct FInputActionValue& value);
	void OnFireInput(const struct FInputActionValue& value);

	// UPROPERTY 메타 지정자 예시
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = TestCategory)
	int32 testNumber1 = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TestCategory)
	int32 testNumber2 = 20;

	UPROPERTY(EditAnywhere, Category = TestCategory)
	int32 testNumber3 = 30;*/
};
