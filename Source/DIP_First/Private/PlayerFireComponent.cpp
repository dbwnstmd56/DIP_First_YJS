// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerFireComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "GrenadeActor.h"
#include "Components/SphereComponent.h"
#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


UPlayerFireComponent::UPlayerFireComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UPlayerFireComponent::BeginPlay()
{
	Super::BeginPlay();

	// 이 액터 컴포넌트가 부착된 주인 액터를 포인터 변수에 저장한다.
	owningPlayer = Cast<AMyCharacter>(GetOwner());
}


void UPlayerFireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPlayerFireComponent::OnFireInput(const struct FInputActionValue& value)
{
	if(owningPlayer == nullptr) return;


	if (owningPlayer->GetCurrentHP() <= 0)
	{
		return;
	}

#pragma region 1. 순수한 블루프린트 파일을 월드에 생성하는 방법
	//
	/*FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(bullet_bp, gunMeshComp->GetSocketLocation(FName("Muzzle")), gunMeshComp->GetSocketRotation(FName("Muzzle")), params);*/
#pragma endregion

#pragma region 2. CPP 파일 기반 블루프린트 파일을 월드에 생성하는 방법
	/*FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABulletActor* spawnedBullet = GetWorld()->SpawnActor<ABulletActor>(bullet_bp, gunMeshComp->GetSocketLocation(FName("Muzzle")), gunMeshComp->GetSocketRotation(FName("Muzzle")), params);*/
#pragma endregion	

	//FVector startLoc = gunMeshComp->GetSocketLocation(FName("Muzzle"));
	//FVector endLoc = startLoc + gunMeshComp->GetRightVector() * 1000.0f;
	FVector startLoc = owningPlayer->cameraComp->GetComponentLocation() + owningPlayer->cameraComp->GetForwardVector() * owningPlayer->springArmComp->TargetArmLength;
	FVector endLoc = startLoc + owningPlayer->cameraComp->GetForwardVector() * rifleRange;
	FCollisionQueryParams params;
	params.AddIgnoredActor(owningPlayer);

#pragma region 3. 라인 트레이스로 발사하기 - 싱글
	FHitResult hitInfo;
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startLoc, endLoc, ECC_Visibility, params);

	if (bHit)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *hitInfo.GetActor()->GetActorNameOrLabel());
		AEnemy* enemy = Cast<AEnemy>(hitInfo.GetActor());
		if (enemy != nullptr)
		{
			enemy->OnDamage(rifleDamage);
			UE_LOG(LogTemp, Warning, TEXT("Enemy hp: %d"), enemy->GetCurrentHP());

			// 만일, Enemy의 체력이 0 이하인 경우에는 점수를 1점 추가한다.
			/*if (mainWidget_inst != nullptr && enemy->GetCurrentHP() <= 0)
			{
				mainWidget_inst->AddScore(1);
			}*/
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Hit!!"));
	}

	//DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, false, 5, 0, 2);

#pragma endregion	

#pragma region 4. 라인 트레이스 발사하기 - 멀티
	//TArray<FHitResult> hitInfos;
	//bool bHit = GetWorld()->LineTraceMultiByChannel(hitInfos, startLoc, endLoc, ECC_Visibility, params);
	//if (bHit)
	//{
	//	for (FHitResult hitInfo : hitInfos)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("%s"), *hitInfo.GetActor()->GetActorNameOrLabel());
	//	}

	//	FVector hitLoc = hitInfos[0].ImpactPoint;

	//	DrawDebugLine(GetWorld(), startLoc, hitLoc, FColor::Red, false, 2.0f, 0, 1.0f);
	//	DrawDebugLine(GetWorld(), hitLoc, endLoc, FColor(0, 255, 0), false, 2.0f, 0, 1.0f);
	//	DrawDebugBox(GetWorld(), hitLoc, FVector(5), FQuat::Identity, FColor(255, 0, 0), false, 2.0f, 0, 2.0f);
	//}
	//else
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("No Hit!!"));
	//	DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, false, 2.0f, 0, 1.0f);
	//}
#pragma endregion	

#pragma region 5. 라인 트레이스 오브젝트 타입으로 체크하기
	/*FHitResult hitInfo;
	FCollisionObjectQueryParams objectQueryParams;
	objectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	if (GetWorld()->LineTraceSingleByObjectType(hitInfo, startLoc, endLoc, objectQueryParams, params))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Object Name: %s"), *hitInfo.GetActor()->GetActorNameOrLabel());
	}*/
#pragma endregion

#pragma region 6. 콜리전 프로필을 이용한 라인 트레이스
	//FHitResult hitInfo;
	//if (GetWorld()->LineTraceSingleByProfile(hitInfo, startLoc, endLoc, FName("TestActor"), params))
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Hit Object Name: %s"), *hitInfo.GetActor()->GetActorNameOrLabel());

	//	// 충돌 지점에 특정한 이펙트를 생성한다.
	//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hit_fx, hitInfo.ImpactPoint, FRotator::ZeroRotator, true);
	//}

#pragma endregion

	// 총 발사음을 플레이한다.
	UGameplayStatics::PlaySound2D(GetWorld(), fire_sound, 0.3f);

	//// 카메라 셰이킹을 한다.
	//if (pc != nullptr && shake_bp != nullptr)
	//{
	//	pc->ClientStartCameraShake(shake_bp);
	//}

	//// 카메라 페이드 인을 한다.
	//pc->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 1.0f, FLinearColor(1, 1, 1), true);

	// 총 발사 애니메이션을 플레이한다.
	owningPlayer->PlayAnimMontage(fireAnim);
}


// 수류탄 투척 함수
void UPlayerFireComponent::OnThrowInput()
{
	if (owningPlayer == nullptr) return;

	if (owningPlayer->GetCurrentHP() <= 0)
	{
		return;
	}

	// 1. 수류탄을 생성한다.
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AGrenadeActor* grenade_inst = GetWorld()->SpawnActor<AGrenadeActor>(grenade_bp, owningPlayer->GetActorLocation() + owningPlayer->GetActorForwardVector() * 100, FRotator::ZeroRotator, params);

	if (grenade_inst != nullptr)
	{
		// 2. 수류탄 액터의 물리 상태를 던질 수 있는 상태로 변경한다.
		grenade_inst->sphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		grenade_inst->sphereCollision->SetSimulatePhysics(true);
		grenade_inst->sphereCollision->SetEnableGravity(true);

		// 3. 수류탄을 대각선 45도로 물리적으로 발사한다.
		FVector throwDir = (owningPlayer->GetActorForwardVector() + owningPlayer->GetActorUpVector()).GetSafeNormal();
		grenade_inst->sphereCollision->AddImpulse(throwDir * throwPower);
	}

	owningPlayer->PlayAnimMontage(fireAnim2);
}