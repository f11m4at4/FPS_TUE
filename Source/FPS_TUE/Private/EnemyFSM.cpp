// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "FPSPlayer.h"
#include <Kismet/GameplayStatics.h>
#include <EngineUtils.h>
#include "Enemy.h"
#include <DrawDebugHelpers.h>
#include "FPS_TUE.h"
#include <TimerManager.h>

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

	// 목차(뼈대)를 구성
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
	case EEnemyState::Die:
		DieState();
		break;
	}
}

// 일정시간 기다렸다가 상태를 Move 로 전환하고 싶다.
// 필요속성 : 대기시간, 경과시간
void UEnemyFSM::IdleState()
{
	// 일정시간 기다렸다가 상태를 Move 로 전환하고 싶다.
	// 1. 시간이 흘러서
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. 대기시간이 됐으니까
	// -> 만약 경과시간이 대기시간을 초과하였다면
	if(currentTime > idleDelayTime)
	{
		// 3. 상태를 Move 로 전환하고 싶다.
		m_state = EEnemyState::Move;
		// 4. 다른 곳에서 사용할 수 있으니 경과시간 초기화 시켜주자
		currentTime = 0;
	}
}

// 1. 타겟방향으로 이동하고 싶다.
// 필요속성 : 타겟, 이동속도
// 2. 타겟이 공격범위 안에 들어오면 공격상태로 전환하고 싶다.
// 필요속성 : 공격범위
void UEnemyFSM::MoveState()
{
	// 타겟방향으로 이동하고 싶다.
	// 1. 타겟이 있어야 한다.
	// 	   -> 에디터에 노출해서 얻어오는 방법
	// 	   -> 동적으로 찾아서 얻어오는 방법
	

	// 2. 방향이 필요 direction = target - me
	FVector direction = target->GetActorLocation() - me->GetActorLocation();
	// 1. 타겟과의 거리를 알아야한다.
	float distance = direction.Size();

	direction.Normalize();
	//me->GetCharacterMovement()->bOrientRotationToMovement = true;
	

	// 3. 이동하고 싶다.
	// -> Character Movement 를 이용하여 이동
	me->AddMovementInput(direction, 1);

	// 바라보고 싶은 방향
	FRotator targetRot = direction.ToOrientationRotator();
	FRotator myRot = me->GetActorRotation();

	myRot = FMath::Lerp(myRot, targetRot, 5 * GetWorld()->DeltaTimeSeconds);
	
	// -> 부드럽게 회전하고 싶다.
	me->SetActorRotation(myRot);


	// 공격범위 시각화 해보자
	DrawDebugSphere(GetWorld(), me->GetActorLocation(), attackRange, 10, FColor::Red);

	// 타겟이 공격범위 안에 들어오면 공격상태로 전환하고 싶다.
	// 2. 타겟과의 거리가 공격범위 안에 들어왔으니까
	if(distance < attackRange)
	{
		// 3. 상태를 공격으로 전환하고 싶다.
		m_state = EEnemyState::Attack;
		currentTime = attackDelayTime;
	}
	/*FVector P0 = GetOwner()->GetActorLocation();
	FVector P = P0 + direction * 500 * GetWorld()->DeltaTimeSeconds;
	GetOwner()->SetActorLocation(P, true);*/
}

// 일정시간에 한번씩 공격하고 싶다.
// 콘솔에 출력
// 필요속성 : 공격대기시간
void UEnemyFSM::AttackState()
{
	// 일정시간에 한번씩 공격하고 싶다.
	// 1. 시간이 흘렀으니까
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. 공격시간이 됐으니까.
	if(currentTime > attackDelayTime)
	{
		// 3. 콘솔에 공격을 출력하고 싶다.
		PRINTLOG(TEXT("Attack!!!"));
		// 경과시간 초기화
		currentTime = 0;
	}

	// 도망가면 상태를 Move 로 전환하고 싶다.
	// -> 나와 타겟과의 거리가 공격범위를 벗어나면	
	// 1. 타겟과의 거리를 알아야한다.
	float distance = FVector::Dist(target->GetActorLocation(), me->GetActorLocation());
	if (distance > attackRange)
	{
		m_state = EEnemyState::Move;
	}
}

// 일정시간 기다렸다가 상태를 Idle 로 전환하고 싶다.
// 필요속성 : 피격대기시간
void UEnemyFSM::DamageState()
{
	m_state = EEnemyState::Idle;
	currentTime = 0;
	// 일정시간 기다렸다가 상태를 Idle 로 전환하고 싶다.
	// 1. 시간이 흘렀으니까
	//currentTime += GetWorld()->DeltaTimeSeconds;
	//// 2. 대기시간이 됐으니까
	//if(currentTime > damageDelayTime)
	//{
	//	// 3. 상태를 Idle 로 전환하고 싶다.
	//	m_state = EEnemyState::Idle;
	//	// Idle 일때 경과시간이 0부터 되도록 처리하겠다.
	//	currentTime = 0;
	//}
}

void UEnemyFSM::DieState()
{
}

// 피격을 받았을 때 처리할 함수	
void UEnemyFSM::OnDamageProcess()
{
	// 상태를 Damage 로 전환하고 싶다.
	m_state = EEnemyState::Damage;
	currentTime = 0;

	// 알람맞춰 놓고 싶다.
	// -> 피격 대기 시간만큼 기다리는 알람

	FTimerHandle damagTimer;
	GetWorld()->GetTimerManager().SetTimer(damagTimer, this, &UEnemyFSM::DamageState, damageDelayTime, false);
}

