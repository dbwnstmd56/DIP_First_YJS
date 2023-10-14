// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrenadeActor.generated.h"

UCLASS()
class DIP_FIRST_API AGrenadeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGrenadeActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* sphereCollision;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* meshComp;
};
