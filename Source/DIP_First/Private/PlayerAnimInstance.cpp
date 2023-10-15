// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	player = Cast<AMyCharacter>(TryGetPawnOwner());

	if (player != nullptr)
	{
		movementComp = player->GetCharacterMovement();
	}

	//UE_LOG(LogTemp, Warning, TEXT("%s"), player == nullptr ? *FString("Player is Null!") : *FString("Player Set!"));
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (player != nullptr)
	{
		vertical = player->moveDir.X * movementComp->MaxWalkSpeed;
		horizontal = player->moveDir.Y * movementComp->MaxWalkSpeed;
		bIsJump = movementComp->IsFalling();

		//UE_LOG(LogTemp, Warning, TEXT("V: %f, H: %f"), vertical, horizontal);
	}
}
