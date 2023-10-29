// Fill out your copyright notice in the Description page of Project Settings.


#include "DIPFunctionLibrary.h"

FRotator UDIPFunctionLibrary::CalculateRotationFromDirection(const AActor* base, const AActor* target)
{
	FVector dir = target->GetActorLocation() - base->GetActorLocation();
	dir.Normalize();
	FRotator calcRot = dir.ToOrientationRotator();
	return calcRot;
}
