// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "FPSPlayer.h"
#include <Kismet/GameplayStatics.h>
#include <EngineUtils.h>
#include "Enemy.h"
#include <DrawDebugHelpers.h>
#include "FPS_TUE.h"
#include <TimerManager.h>
#include <AIController.h>
#include "EnemyAnimInstance.h"
#include <NavigationSystem.h>
#include <GameFramework/CharacterMovementComponent.h>

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

	ai = Cast<AAIController>(me->GetController());

	// 사용할 animInstance 할당
	anim = Cast<UEnemyAnimInstance>(me->GetMesh()->GetAnimInstance());

	ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	m_state = EEnemyState::Idle;
	anim->state = m_state;
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
	case EEnemyState::Patrol:
		PatrolState();
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
		// 3. 상태를 Patrol 로 전환하고 싶다.
		m_state = EEnemyState::Patrol;
		// 애니메이션 상태도 패트롤로 전환해라
		anim->state = m_state;

		// 4. 다른 곳에서 사용할 수 있으니 경과시간 초기화 시켜주자
		currentTime = 0;
		me->GetCharacterMovement()->MaxWalkSpeed = 200;
		GetTargetLocation(me, 1000, randomPos);

		// 5. 애니메이션 상태도 Move 로 바꾸자
		//anim->isMoving = true;
	}
}

// 거리는 가까워졌지만 그(Player) 위치로 갈수 없는 환경일 때는  Move 로 상태 전환하지 않는다.
void UEnemyFSM::PatrolState()
{
	// 타겟과의 거리가 invoker 1000 이내로 들어오면 상태를 Move 로 전환해주자
	float distance = FVector::Dist(target->GetActorLocation(), me->GetActorLocation());
	if (distance < 1000 && CanMove())
	{
		m_state = EEnemyState::Move;
		me->GetCharacterMovement()->MaxWalkSpeed = 400;

		anim->state = m_state;

		return;
	}

	// 목적지로 이동해라.
	EPathFollowingRequestResult::Type r = ai->MoveToLocation(randomPos);
	// 목적지에 도착했으면 다른 랜덤 위치 찾도록 하자
	if (r == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		GetTargetLocation(me, 1000, randomPos);
	}
	
	aiDebugActor->SetActorLocation(randomPos);

}

// 1. 타겟방향으로 이동하고 싶다.
// 필요속성 : 타겟, 이동속도
// 2. 타겟이 공격범위 안에 들어오면 공격상태로 전환하고 싶다.
// 필요속성 : 공격범위
void UEnemyFSM::MoveState()
{
	CanMove();

	EPathFollowingRequestResult::Type r = ai->MoveToActor(target);
	// 만약 갈곳을 못찾으면 상태를 Patrol 로 변경시켜주자
	if (r == EPathFollowingRequestResult::Failed)
	{
		m_state = EEnemyState::Patrol;
		GetTargetLocation(me, 1000, randomPos);
		me->GetCharacterMovement()->MaxWalkSpeed = 200;

		anim->state = m_state;
		return;
	}
	

	// 공격범위 시각화 해보자
	DrawDebugSphere(GetWorld(), me->GetActorLocation(), attackRange, 10, FColor::Red);

	// 타겟방향으로 이동하고 싶다.
	// 1. 타겟과의 거리를 알아야한다.
	float distance = FVector::Dist(target->GetActorLocation(), me->GetActorLocation());

	// 타겟이 공격범위 안에 들어오면 공격상태로 전환하고 싶다.
	// 2. 타겟과의 거리가 공격범위 안에 들어왔으니까
	if (distance < attackRange)
	{
		// 3. 상태를 공격으로 전환하고 싶다.
		m_state = EEnemyState::Attack;
		currentTime = attackDelayTime;

		ai->StopMovement();
	}
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
// -> Lerp 를 이용해서 뒤로 밀리는 넉백 효과 만들어보자
// -> 넉백이 끝나면 상태를 Idle 로 전환
void UEnemyFSM::DamageState()
{

	// Lerp (from, to, 속도)
	FVector myPos = me->GetActorLocation(); // from
	// to -> knockbackPos
	// 
	// -> Lerp 를 이용해서 뒤로 밀리는 넉백 효과 만들어보자
	myPos = FMath::Lerp(myPos, knockbackPos, knockbackSpeed * GetWorld()->DeltaTimeSeconds);

	// -> 넉백이 끝나면 상태를 Idle 로 전환
	float distance = FVector::Dist(myPos, knockbackPos);

	// 최종 위치와 나와의 거리가 아주 작으면 도착했다고 판단하자
	if (distance < knockbackRange)
	{
		// 최종위치에 도착하도록 설정
		myPos = knockbackPos;
		m_state = EEnemyState::Idle;
		currentTime = 0;
	}

	me->SetActorLocation(myPos);

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

bool UEnemyFSM::GetTargetLocation(const AActor* targetActor, float radius, FVector& dest)
{
	FNavLocation loc;
	bool result = ns->GetRandomReachablePointInRadius(targetActor->GetActorLocation(), radius, loc);
	dest = loc.Location;
	
	return result;
}

// AI 가 이동할 때 최종 목적지(target) 에 갈수 있는지를 확인
// -> AI 가 이동할 경로 데이터가 필요
// -> 마지막 두점 위치를 표시보자
// -> 마지막 경로 위치에서 target 쪽으로 LineTrace 쏴서 충돌체크
// -> target 하고 충돌이 발생하면 이동 가능하다
// -> 이동할 경로 시각화 (서비스)
bool UEnemyFSM::CanMove()
{
	//1. 이동경로 가져오기
	FPathFindingQuery query;
	FAIMoveRequest req;
	req.SetAcceptanceRadius(3);
	req.SetGoalActor(target);
	ai->BuildPathfindingQuery(req, query);
	FPathFindingResult result = ns->FindPathSync(query);

	TArray<FNavPathPoint> points = result.Path->GetPathPoints();

	int32 num = points.Num();

	PRINTLOG(TEXT("Path Num : %d"), num);

	if(num > 1 && pathActors.Num() > 1)
	{
		// -> 마지막 두점 위치를 표시보자
		for (int i=0;i< pathActors.Num();i++)
		{
			int32 index = num - (i + 1);
			if (index < 0)
			{
				break;
			}
			// num 은 찾을 길 갯수
			// 배열의 시작은 0부터 한다. 크기가 n 일때 마지막 원소는 n - 1 이된다.
			pathActors[i]->SetActorLocation(points[index].Location);
		}
	}
	
	// 갈수 있는 위치 시각화(선을 그려보자)
	for (int i=1;i<num;i++)
	{
		// 두점 필요하다.
		FVector point1 = points[i - 1].Location;
		FVector point2 = points[i].Location;

		DrawDebugLine(GetWorld(), point1, point2, FColor::Red, false, 0.1f, 10, 5);
	}


	// 2. 타겟 위치로 이동 가능한지 체크
	// -> 마지막 경로 위치에서 target 쪽으로 LineTrace 쏴서 충돌체크
	if (num > 0)
	{
		// -> 마지막 경로 위치에서 target 쪽으로 LineTrace 쏴서 충돌체크
		// 1. 시작점 필요
		FVector startPoint = points[num - 1].Location;
		// 2. 끝점 필요
		FVector endPoint = target->GetActorLocation();
		// 3. 나(Enemy)는 제외
		FCollisionQueryParams param;
		param.AddIgnoredActor(me);
		// 4. LineTrace 쏴서 충돌체크하고 싶다.
		FHitResult hitInfo;
		bool r = GetWorld()->LineTraceSingleByChannel(hitInfo, startPoint, endPoint, ECC_WorldStatic, param);

		// 타겟위치로 이동가능하면 true 를 반환
		// 5. 충돌 검출이 성공했고, 부딪힌 물체가 target 이면 이동가능
		if (r && hitInfo.GetActor() == target)
		{
			return true;
		}
	}

	// 이동 못함을 반환
	return false;
}

// 피격을 받았을 때 처리할 함수	
// 피격 받았을 때 hp 를 감소시키고 0 이하면 상태를 Die 로 바꾸고 없애버리자
// 넉백(Knockback) 처리를 해보자
void UEnemyFSM::OnDamageProcess(FVector shootDirection)
{
	ai->StopMovement();

	// 맞았을 때 상대를 바라보도록 처리하자
	me->SetActorRotation((-shootDirection).ToOrientationRotator());

	// 피격 받았을 때 hp 를 감소시키고 0 이하면 상태를 Die 로 바꾸고 없애버리자
	// 1. hp 가 감소했으니까
	hp--;
	// 2. 만약 hp가 0 이하이면
	if(hp <= 0)
	{
		// 3. 상태를 Die 로 바꾼다. 
		m_state = EEnemyState::Die;
		// 4. 없애버리자
		anim->Die();
		return;
	}

	// 넉백(Knockback) 처리를 해보자
	// 현재 내 위치에서 내가 맞은 방향으로 밀리면 되겠다.
	// 밀릴 방향이 필요
	//me->SetActorLocation(me->GetActorLocation() + shootDirection * knockback);
	shootDirection.Z = 0;
	knockbackPos = me->GetActorLocation() + shootDirection * knockback;

	// 상태를 Damage 로 전환하고 싶다.
	m_state = EEnemyState::Damage;
	currentTime = 0;

	anim->Hit();
	// 알람맞춰 놓고 싶다.
	// -> 피격 대기 시간만큼 기다리는 알람

	FTimerHandle damagTimer;
	//GetWorld()->GetTimerManager().SetTimer(damagTimer, this, &UEnemyFSM::DamageState, damageDelayTime, false);
}

