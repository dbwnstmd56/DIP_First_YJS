// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletActor.h"
#include "Components/SphereComponent.h"


ABulletActor::ABulletActor()
{
	PrimaryActorTick.bCanEverTick = true;

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SetRootComponent(sphereComp);
	sphereComp->SetSphereRadius(6.4f);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetWorldScale3D(FVector(0.04f, 0.04f, 0.04f));

}

void ABulletActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 등속도 공식
	// 속도(Velocity) = 방향(Direction) * 속력(Speed)
	FVector dir = GetActorForwardVector() * moveSpeed;

	// p = p0 + vt
	SetActorLocation(GetActorLocation() + dir * DeltaTime);

}

