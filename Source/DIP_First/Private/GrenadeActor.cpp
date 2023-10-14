// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"


AGrenadeActor::AGrenadeActor()
{
	PrimaryActorTick.bCanEverTick = true;

	sphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SetRootComponent(sphereCollision);
	sphereCollision->SetSphereRadius(50.0f);
	sphereCollision->SetWorldScale3D(FVector(0.15f));
	sphereCollision->SetMassOverrideInKg(NAME_None, 0.5f, true);				// 무게 변경
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
	
}

void AGrenadeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

