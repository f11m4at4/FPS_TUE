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
		// ��������Ʈ�� �� �Լ��� ����ϰ� �ʹ�.
		me->OnInputDelegate.AddUObject(this, &UPlayerFire::SetupPlayerInputComponent);

		// �ѱ� ã�Ƽ� ��� 
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

// ����ڰ� �߻��ư�� ������ ȣ��Ǵ� �Լ�
// Ư�� ���ʹ� �浹���� ���õǰ� �ϰ� �ʹ�.
void UPlayerFire::Fire()
{
	// �Ѿ��� �߻��ϰ� �ʹ�.
	//GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition->GetComponentLocation(), firePosition->GetComponentRotation());

	// Line ���� �ε��� ������ ȿ���� ����ϰ� �ʹ�.
	// �ʿ�Ӽ� : ȿ��
	// �ε��� ������ ���� ������ �ʿ�
	FHitResult hitInfo;
	
	// �ѱ��� �ƴ� ī�޶� �������� ����
	auto fpsCam = Cast<UCameraComponent>(me->GetDefaultSubobjectByName(TEXT("FPSCamera")));

	FVector start = fpsCam->GetComponentLocation();
	FVector end = fpsCam->GetComponentLocation() + fpsCam->GetForwardVector() * fireDistance;

	FCollisionQueryParams param;
	param.AddIgnoredActor(me);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECC_Pawn, param);

	// ���� ���� �ε����ٸ�
	if(bHit)
	{
		// ȿ���� ����ϰ� �ʹ�.
		FTransform trans;
		trans.SetLocation(hitInfo.ImpactPoint);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffect, trans);

		// �ε��� ��ü�� Cube �̸� ���������� �ʹ�.
		if (hitInfo.GetActor()->GetName().Contains(TEXT("Cube")))
		{
			auto comp = hitInfo.GetComponent();
			comp->AddForceAtLocation(-hitInfo.ImpactNormal * bulletPower * comp->GetMass(), hitInfo.ImpactPoint);
		}
		// ���� �ε��� ��ü�� Enemy �̸�, 
		auto enemy = Cast<AEnemy>(hitInfo.GetActor());
		// Enemy �� ���¸� Damage �� ��ȯ
		if (enemy)
		{
			enemy->enemyFSM->OnDamageProcess();
		}
	}
}

