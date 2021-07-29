// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "FPSPlayer.h"
#include <Kismet/GameplayStatics.h>
#include <EngineUtils.h>
#include "Enemy.h"
#include <DrawDebugHelpers.h>

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	me = Cast<AEnemy>(GetOwner());

	target = Cast<AFPSPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AFPSPlayer::StaticClass()));

	//TArray<AActor*> actors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFPSPlayer::StaticClass(), actors);

	////for (int i = 0; i < actors.Num(); i++)
	////{
	////	auto t = actors[i];
	////	target = Cast<AFPSPlayer>(t);
	////	break;
	////}

	//for (auto t : actors)
	//{
	//	target = Cast<AFPSPlayer>(t);
	//	break;
	//}

	/*for (TActorIterator<AFPSPlayer> it( GetWorld() ); it ; ++it)
	{
		target = *it;
	}*/
	
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ����(����)�� ����
	switch (m_state)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}
}

// �����ð� ��ٷȴٰ� ���¸� Move �� ��ȯ�ϰ� �ʹ�.
// �ʿ�Ӽ� : ���ð�, ����ð�
void UEnemyFSM::IdleState()
{
	// �����ð� ��ٷȴٰ� ���¸� Move �� ��ȯ�ϰ� �ʹ�.
	// 1. �ð��� �귯��
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. ���ð��� �����ϱ�
	// -> ���� ����ð��� ���ð��� �ʰ��Ͽ��ٸ�
	if(currentTime > idleDelayTime)
	{
		// 3. ���¸� Move �� ��ȯ�ϰ� �ʹ�.
		m_state = EEnemyState::Move;
		// 4. �ٸ� ������ ����� �� ������ ����ð� �ʱ�ȭ ��������
		currentTime = 0;
	}
}

// 1. Ÿ�ٹ������� �̵��ϰ� �ʹ�.
// �ʿ�Ӽ� : Ÿ��, �̵��ӵ�
// 2. Ÿ���� ���ݹ��� �ȿ� ������ ���ݻ��·� ��ȯ�ϰ� �ʹ�.
// �ʿ�Ӽ� : ���ݹ���
void UEnemyFSM::MoveState()
{
	// Ÿ�ٹ������� �̵��ϰ� �ʹ�.
	// 1. Ÿ���� �־�� �Ѵ�.
	// 	   -> �����Ϳ� �����ؼ� ������ ���
	// 	   -> �������� ã�Ƽ� ������ ���
	

	// 2. ������ �ʿ� direction = target - me
	FVector direction = target->GetActorLocation() - me->GetActorLocation();
	// 1. Ÿ�ٰ��� �Ÿ��� �˾ƾ��Ѵ�.
	float distance = direction.Size();

	direction.Normalize();
	//me->GetCharacterMovement()->bOrientRotationToMovement = true;
	

	// 3. �̵��ϰ� �ʹ�.
	// -> Character Movement �� �̿��Ͽ� �̵�
	me->AddMovementInput(direction, 1);

	// �ٶ󺸰� ���� ����
	FRotator targetRot = direction.ToOrientationRotator();
	FRotator myRot = me->GetActorRotation();

	myRot = FMath::Lerp(myRot, targetRot, 5 * GetWorld()->DeltaTimeSeconds);
	
	// -> �ε巴�� ȸ���ϰ� �ʹ�.
	me->SetActorRotation(myRot);


	// ���ݹ��� �ð�ȭ �غ���
	DrawDebugSphere(GetWorld(), me->GetActorLocation(), attackRange, 10, FColor::Red);

	// Ÿ���� ���ݹ��� �ȿ� ������ ���ݻ��·� ��ȯ�ϰ� �ʹ�.
	// 2. Ÿ�ٰ��� �Ÿ��� ���ݹ��� �ȿ� �������ϱ�
	if(distance < attackRange)
	{
		// 3. ���¸� �������� ��ȯ�ϰ� �ʹ�.
		m_state = EEnemyState::Attack;
	}
	/*FVector P0 = GetOwner()->GetActorLocation();
	FVector P = P0 + direction * 500 * GetWorld()->DeltaTimeSeconds;
	GetOwner()->SetActorLocation(P, true);*/
}

void UEnemyFSM::AttackState()
{
}

void UEnemyFSM::DamageState()
{
}

void UEnemyFSM::DieState()
{
}

