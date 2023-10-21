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
	sphereCollision->SetMassOverrideInKg(NAME_None, 0.5f, true);				// ���� ����
	sphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);		// �浹 ó�� off
	sphereCollision->SetSimulatePhysics(false);									// ���� ȿ�� �ѱ�
	sphereCollision->SetEnableGravity(false);									// �߷� ���� off

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGrenadeActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Sphere Collision�� �浹�� ���Ͱ� �ִ� ��� ������ �Լ��� �����ϵ��� �����ϱ�(Binding)
	// Delegate ���
	sphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AGrenadeActor::OnOverlap);
}

void AGrenadeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// �浹 �̺�Ʈ�� �߻����� �� ������ �Լ�
void AGrenadeActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. �浹�� Enemy ���Ϳ��Ը� �������� �� ���
	//AEnemy* enemy = Cast<AEnemy>(OtherActor);

	//if (enemy != nullptr)
	//{
	//	enemy->OnDamage(explosionDamage);
	//}

	// 2. �浹�� ������ �߽����� �ݰ� 8���� ������ ��� Enemy ���Ϳ��� �������� �� ���
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

	// �浹 ���� �ݰ��� ȭ�鿡 �׸���.
	DrawDebugSphere(GetWorld(), GetActorLocation(), 800, 25, FColor::Green, false, 5.0f, 0, 2);

}

