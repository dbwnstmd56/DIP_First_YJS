// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemy.h"
#include "DrawDebugHelpers.h"


AGrenadeActor::AGrenadeActor()
{
	PrimaryActorTick.bCanEverTick = true;

	sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SetRootComponent(sphereCollision);
	sphereCollision->SetSphereRadius(50.0f);
	sphereCollision->SetWorldScale3D(FVector(0.15f));
	//sphereCollision->SetMassOverrideInKg(NAME_None, 0.5f, true);				// 무게 변경
	sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);		// 충돌 처리 off
	sphereCollision->SetSimulatePhysics(false);									// 물리 효과 켜기
	sphereCollision->SetEnableGravity(false);									// 중력 적용 off

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGrenadeActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Sphere Collision에 충돌한 액터가 있는 경우 지정된 함수를 실행하도록 연결하기(Binding)
	// Delegate 방식
	sphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AGrenadeActor::OnOverlap);
}

void AGrenadeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 충돌 이벤트가 발생했을 때 실행할 함수
void AGrenadeActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. 충돌한 Enemy 액터에게만 데미지를 줄 경우
	//AEnemy* enemy = Cast<AEnemy>(OtherActor);

	//if (enemy != nullptr)
	//{
	//	enemy->OnDamage(explosionDamage);
	//}

	// 2. 충돌한 지점을 중심으로 반경 8미터 주위의 모든 Enemy 액터에게 데미지를 줄 경우
	TArray<FOverlapResult> hitInfos;
	FCollisionObjectQueryParams params;
	params.AddObjectTypesToQuery(ECC_Pawn);
	//params.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	bool bIsOverlap = GetWorld()->OverlapMultiByObjectType(hitInfos, GetActorLocation(), GetActorRotation().Quaternion(), params, FCollisionShape::MakeSphere(800));

	if (bIsOverlap)
	{
		UE_LOG(LogTemp, Warning, TEXT("hit Count: %d"), hitInfos.Num());

		for (FOverlapResult& hitInfo : hitInfos)
		{
		 	AEnemy* enemy = Cast<AEnemy>(hitInfo.GetActor());
			if (enemy != nullptr)
			{
				enemy->OnDamage(explosionDamage);
				UE_LOG(LogTemp, Warning, TEXT("Hit Enemy: %s"), *enemy->GetActorNameOrLabel());
			}
		}
	}

	// 충돌 감지 반경을 화면에 그린다.
	DrawDebugSphere(GetWorld(), GetActorLocation(), 800, 25, FColor::Green, false, 5.0f, 0, 2);

}

