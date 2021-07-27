// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 충돌체 등록
	collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	// 루트컴포넌트로 만들기
	RootComponent = collision;
	// 충돌 옵션
	collision->SetCollisionProfileName(TEXT("BlockAll"));

	// 외관
	bodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	bodyMesh->SetupAttachment(collision);
	bodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// movement
	movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	movement->SetUpdatedComponent(collision);
	// 튕귀는 옵션 
	movement->bShouldBounce = true;
	movement->Bounciness = 0.3f;
	// 속도
	movement->MaxSpeed = speed;
	movement->InitialSpeed = speed;
}

void ABullet::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	
}

// 액터의 특정 속성값을 수정하면 호출되는 함수(이벤트)
// FPropertyChangedEvent -> 수정된 속성의 정보
void ABullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// 만약 speed 가 변경됐으면
	if(PropertyChangedEvent.GetPropertyName() == TEXT("speed"))
	{		
		// movement 에 적용하고 싶다.
		// 속도
		movement->MaxSpeed = speed;
		movement->InitialSpeed = speed;
	}
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	// 생명력
	SetLifeSpan(3);

	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

