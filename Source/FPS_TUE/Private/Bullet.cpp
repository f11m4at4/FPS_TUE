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
	movement->MaxSpeed = speed;
	movement->InitialSpeed = speed;
}

void ABullet::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	
}

// ������ Ư�� �Ӽ����� �����ϸ� ȣ��Ǵ� �Լ�(�̺�Ʈ)
// FPropertyChangedEvent -> ������ �Ӽ��� ����
void ABullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// ���� speed �� ���������
	if(PropertyChangedEvent.GetPropertyName() == TEXT("speed"))
	{		
		// movement �� �����ϰ� �ʹ�.
		// �ӵ�
		movement->MaxSpeed = speed;
		movement->InitialSpeed = speed;
	}
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	// �����
	SetLifeSpan(3);

	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

