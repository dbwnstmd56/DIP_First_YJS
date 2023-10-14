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
	
}

void AGrenadeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

