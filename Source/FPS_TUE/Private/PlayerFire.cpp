// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerFire.h"
#include "FPSPlayer.h"
#include <Components/ArrowComponent.h>
#include "Bullet.h"
#include <Kismet/GameplayStatics.h>
#include <Camera/CameraComponent.h>
#include "Enemy.h"
#include "EnemyFSM.h"

// Sets default values for this component's properties
UPlayerFire::UPlayerFire()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bWantsInitializeComponent = true;
}


// Called when the game starts
void UPlayerFire::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UPlayerFire::InitializeComponent()
{
	Super::InitializeComponent();

	me = Cast<AFPSPlayer>(GetOwner());
	if (me)
	{
		// 델리게이트에 내 함수를 등록하고 싶다.
		me->OnInputDelegate.AddUObject(this, &UPlayerFire::SetupPlayerInputComponent);

		// 총구 찾아서 등록 
		firePosition = Cast<UArrowComponent>(me->GetDefaultSubobjectByName(TEXT("FirePosition")));
	}
}

// Called every frame
void UPlayerFire::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerFire::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &UPlayerFire::Fire);
}

// 사용자가 발사버튼을 누르면 호출되는 함수
// 특정 액터는 충돌에서 무시되게 하고 싶다.
void UPlayerFire::Fire()
{
	// 총알을 발사하고 싶다.
	//GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition->GetComponentLocation(), firePosition->GetComponentRotation());

	// Line 쏴서 부딪힌 지점에 효과를 재생하고 싶다.
	// 필요속성 : 효과
	// 부딪힌 정보를 담을 변수가 필요
	FHitResult hitInfo;
	
	// 총구가 아닌 카메라 기준으로 수정
	auto fpsCam = Cast<UCameraComponent>(me->GetDefaultSubobjectByName(TEXT("FPSCamera")));

	FVector start = fpsCam->GetComponentLocation();
	FVector end = fpsCam->GetComponentLocation() + fpsCam->GetForwardVector() * fireDistance;

	FCollisionQueryParams param;
	param.AddIgnoredActor(me);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Pawn, param);

	// 만약 선에 부딪혔다면
	if(bHit)
	{
		// 효과를 재생하고 싶다.
		FTransform trans;
		trans.SetLocation(hitInfo.ImpactPoint);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffect, trans);

		// 부딪힌 물체가 Cube 이면 날려버리고 싶다.
		if (hitInfo.GetActor()->GetName().Contains(TEXT("Cube")))
		{
			auto comp = hitInfo.GetComponent();
			comp->AddForceAtLocation(-hitInfo.ImpactNormal * bulletPower * comp->GetMass(), hitInfo.ImpactPoint);
		}
		// 만약 부딪힌 물체가 Enemy 이면, 
		auto enemy = Cast<AEnemy>(hitInfo.GetActor());
		// Enemy 의 상태를 Damage 로 전환
		if (enemy)
		{
			enemy->enemyFSM->OnDamageProcess();
		}
	}
}

