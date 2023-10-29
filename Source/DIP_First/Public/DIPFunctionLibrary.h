// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DIPFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DIP_FIRST_API UDIPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// base 액터가 target 액터를 바라보도록 계산된 회전 값을 반환하는 함수
	UFUNCTION(BlueprintCallable, Category=MyFunction)
	static FRotator CalculateRotationFromDirection(const AActor* base, const AActor* target);

};
