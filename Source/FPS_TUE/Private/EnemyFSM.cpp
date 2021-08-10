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

	// ����� animInstance �Ҵ�
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

	// ����(����)�� ����
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
		// 3. ���¸� Patrol �� ��ȯ�ϰ� �ʹ�.
		m_state = EEnemyState::Patrol;
		// �ִϸ��̼� ���µ� ��Ʈ�ѷ� ��ȯ�ض�
		anim->state = m_state;

		// 4. �ٸ� ������ ����� �� ������ ����ð� �ʱ�ȭ ��������
		currentTime = 0;
		me->GetCharacterMovement()->MaxWalkSpeed = 200;
		GetTargetLocation(me, 1000, randomPos);

		// 5. �ִϸ��̼� ���µ� Move �� �ٲ���
		//anim->isMoving = true;
	}
}

// �Ÿ��� ����������� ��(Player) ��ġ�� ���� ���� ȯ���� ����  Move �� ���� ��ȯ���� �ʴ´�.
void UEnemyFSM::PatrolState()
{
	// Ÿ�ٰ��� �Ÿ��� invoker 1000 �̳��� ������ ���¸� Move �� ��ȯ������
	float distance = FVector::Dist(target->GetActorLocation(), me->GetActorLocation());
	if (distance < 1000 && CanMove())
	{
		m_state = EEnemyState::Move;
		me->GetCharacterMovement()->MaxWalkSpeed = 400;

		anim->state = m_state;

		return;
	}

	// �������� �̵��ض�.
	EPathFollowingRequestResult::Type r = ai->MoveToLocation(randomPos);
	// �������� ���������� �ٸ� ���� ��ġ ã���� ����
	if (r == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		GetTargetLocation(me, 1000, randomPos);
	}
	
	aiDebugActor->SetActorLocation(randomPos);

}

// 1. Ÿ�ٹ������� �̵��ϰ� �ʹ�.
// �ʿ�Ӽ� : Ÿ��, �̵��ӵ�
// 2. Ÿ���� ���ݹ��� �ȿ� ������ ���ݻ��·� ��ȯ�ϰ� �ʹ�.
// �ʿ�Ӽ� : ���ݹ���
void UEnemyFSM::MoveState()
{
	CanMove();

	EPathFollowingRequestResult::Type r = ai->MoveToActor(target);
	// ���� ������ ��ã���� ���¸� Patrol �� �����������
	if (r == EPathFollowingRequestResult::Failed)
	{
		m_state = EEnemyState::Patrol;
		GetTargetLocation(me, 1000, randomPos);
		me->GetCharacterMovement()->MaxWalkSpeed = 200;

		anim->state = m_state;
		return;
	}
	

	// ���ݹ��� �ð�ȭ �غ���
	DrawDebugSphere(GetWorld(), me->GetActorLocation(), attackRange, 10, FColor::Red);

	// Ÿ�ٹ������� �̵��ϰ� �ʹ�.
	// 1. Ÿ�ٰ��� �Ÿ��� �˾ƾ��Ѵ�.
	float distance = FVector::Dist(target->GetActorLocation(), me->GetActorLocation());

	// Ÿ���� ���ݹ��� �ȿ� ������ ���ݻ��·� ��ȯ�ϰ� �ʹ�.
	// 2. Ÿ�ٰ��� �Ÿ��� ���ݹ��� �ȿ� �������ϱ�
	if (distance < attackRange)
	{
		// 3. ���¸� �������� ��ȯ�ϰ� �ʹ�.
		m_state = EEnemyState::Attack;
		currentTime = attackDelayTime;

		ai->StopMovement();
	}
}

// �����ð��� �ѹ��� �����ϰ� �ʹ�.
// �ֿܼ� ���
// �ʿ�Ӽ� : ���ݴ��ð�
void UEnemyFSM::AttackState()
{
	// �����ð��� �ѹ��� �����ϰ� �ʹ�.
	// 1. �ð��� �귶���ϱ�
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. ���ݽð��� �����ϱ�.
	if(currentTime > attackDelayTime)
	{
		// 3. �ֿܼ� ������ ����ϰ� �ʹ�.
		PRINTLOG(TEXT("Attack!!!"));
		// ����ð� �ʱ�ȭ
		currentTime = 0;
	}

	// �������� ���¸� Move �� ��ȯ�ϰ� �ʹ�.
	// -> ���� Ÿ�ٰ��� �Ÿ��� ���ݹ����� �����	
	// 1. Ÿ�ٰ��� �Ÿ��� �˾ƾ��Ѵ�.
	float distance = FVector::Dist(target->GetActorLocation(), me->GetActorLocation());
	if (distance > attackRange)
	{
		m_state = EEnemyState::Move;
	}
}

// �����ð� ��ٷȴٰ� ���¸� Idle �� ��ȯ�ϰ� �ʹ�.
// �ʿ�Ӽ� : �ǰݴ��ð�
// -> Lerp �� �̿��ؼ� �ڷ� �и��� �˹� ȿ�� ������
// -> �˹��� ������ ���¸� Idle �� ��ȯ
void UEnemyFSM::DamageState()
{

	// Lerp (from, to, �ӵ�)
	FVector myPos = me->GetActorLocation(); // from
	// to -> knockbackPos
	// 
	// -> Lerp �� �̿��ؼ� �ڷ� �и��� �˹� ȿ�� ������
	myPos = FMath::Lerp(myPos, knockbackPos, knockbackSpeed * GetWorld()->DeltaTimeSeconds);

	// -> �˹��� ������ ���¸� Idle �� ��ȯ
	float distance = FVector::Dist(myPos, knockbackPos);

	// ���� ��ġ�� ������ �Ÿ��� ���� ������ �����ߴٰ� �Ǵ�����
	if (distance < knockbackRange)
	{
		// ������ġ�� �����ϵ��� ����
		myPos = knockbackPos;
		m_state = EEnemyState::Idle;
		currentTime = 0;
	}

	me->SetActorLocation(myPos);

	// �����ð� ��ٷȴٰ� ���¸� Idle �� ��ȯ�ϰ� �ʹ�.
	// 1. �ð��� �귶���ϱ�
	//currentTime += GetWorld()->DeltaTimeSeconds;
	//// 2. ���ð��� �����ϱ�
	//if(currentTime > damageDelayTime)
	//{
	//	// 3. ���¸� Idle �� ��ȯ�ϰ� �ʹ�.
	//	m_state = EEnemyState::Idle;
	//	// Idle �϶� ����ð��� 0���� �ǵ��� ó���ϰڴ�.
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

// AI �� �̵��� �� ���� ������(target) �� ���� �ִ����� Ȯ��
// -> AI �� �̵��� ��� �����Ͱ� �ʿ�
// -> ������ ���� ��ġ�� ǥ�ú���
// -> ������ ��� ��ġ���� target ������ LineTrace ���� �浹üũ
// -> target �ϰ� �浹�� �߻��ϸ� �̵� �����ϴ�
// -> �̵��� ��� �ð�ȭ (����)
bool UEnemyFSM::CanMove()
{
	//1. �̵���� ��������
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
		// -> ������ ���� ��ġ�� ǥ�ú���
		for (int i=0;i< pathActors.Num();i++)
		{
			int32 index = num - (i + 1);
			if (index < 0)
			{
				break;
			}
			// num �� ã�� �� ����
			// �迭�� ������ 0���� �Ѵ�. ũ�Ⱑ n �϶� ������ ���Ҵ� n - 1 �̵ȴ�.
			pathActors[i]->SetActorLocation(points[index].Location);
		}
	}
	
	// ���� �ִ� ��ġ �ð�ȭ(���� �׷�����)
	for (int i=1;i<num;i++)
	{
		// ���� �ʿ��ϴ�.
		FVector point1 = points[i - 1].Location;
		FVector point2 = points[i].Location;

		DrawDebugLine(GetWorld(), point1, point2, FColor::Red, false, 0.1f, 10, 5);
	}


	// 2. Ÿ�� ��ġ�� �̵� �������� üũ
	// -> ������ ��� ��ġ���� target ������ LineTrace ���� �浹üũ
	if (num > 0)
	{
		// -> ������ ��� ��ġ���� target ������ LineTrace ���� �浹üũ
		// 1. ������ �ʿ�
		FVector startPoint = points[num - 1].Location;
		// 2. ���� �ʿ�
		FVector endPoint = target->GetActorLocation();
		// 3. ��(Enemy)�� ����
		FCollisionQueryParams param;
		param.AddIgnoredActor(me);
		// 4. LineTrace ���� �浹üũ�ϰ� �ʹ�.
		FHitResult hitInfo;
		bool r = GetWorld()->LineTraceSingleByChannel(hitInfo, startPoint, endPoint, ECC_WorldStatic, param);

		// Ÿ����ġ�� �̵������ϸ� true �� ��ȯ
		// 5. �浹 ������ �����߰�, �ε��� ��ü�� target �̸� �̵�����
		if (r && hitInfo.GetActor() == target)
		{
			return true;
		}
	}

	// �̵� ������ ��ȯ
	return false;
}

// �ǰ��� �޾��� �� ó���� �Լ�	
// �ǰ� �޾��� �� hp �� ���ҽ�Ű�� 0 ���ϸ� ���¸� Die �� �ٲٰ� ���ֹ�����
// �˹�(Knockback) ó���� �غ���
void UEnemyFSM::OnDamageProcess(FVector shootDirection)
{
	ai->StopMovement();

	// �¾��� �� ��븦 �ٶ󺸵��� ó������
	me->SetActorRotation((-shootDirection).ToOrientationRotator());

	// �ǰ� �޾��� �� hp �� ���ҽ�Ű�� 0 ���ϸ� ���¸� Die �� �ٲٰ� ���ֹ�����
	// 1. hp �� ���������ϱ�
	hp--;
	// 2. ���� hp�� 0 �����̸�
	if(hp <= 0)
	{
		// 3. ���¸� Die �� �ٲ۴�. 
		m_state = EEnemyState::Die;
		// 4. ���ֹ�����
		anim->Die();
		return;
	}

	// �˹�(Knockback) ó���� �غ���
	// ���� �� ��ġ���� ���� ���� �������� �и��� �ǰڴ�.
	// �и� ������ �ʿ�
	//me->SetActorLocation(me->GetActorLocation() + shootDirection * knockback);
	shootDirection.Z = 0;
	knockbackPos = me->GetActorLocation() + shootDirection * knockback;

	// ���¸� Damage �� ��ȯ�ϰ� �ʹ�.
	m_state = EEnemyState::Damage;
	currentTime = 0;

	anim->Hit();
	// �˶����� ���� �ʹ�.
	// -> �ǰ� ��� �ð���ŭ ��ٸ��� �˶�

	FTimerHandle damagTimer;
	//GetWorld()->GetTimerManager().SetTimer(damagTimer, this, &UEnemyFSM::DamageState, damageDelayTime, false);
}

