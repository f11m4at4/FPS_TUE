// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// �浹ü ���
	collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	// ��Ʈ������Ʈ�� �����
	RootComponent = collision;
	// �浹 �ɼ�
	collision->SetCollisionProfileName(TEXT("BlockAll"));

	// �ܰ�
	bodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	bodyMesh->SetupAttachment(collision);
	bodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// movement
	movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	movement->SetUpdatedComponent(collision);
	// ƨ�ʹ� �ɼ� 
	movement->bShouldBounce = true;
	movement->Bounciness = 0.3f;
	// �ӵ�
	movement->MaxSpeed = 5000;
	movement->InitialSpeed = 5000;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

