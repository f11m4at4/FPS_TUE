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

		FNavLocation loc;
		bool result = ns->GetRandomReachablePointInRadius(me->GetActorLocation(), 1000, loc);
		randomPos = loc.Location;
		// 5. �ִϸ��̼� ���µ� Move �� �ٲ���
		//anim->isMoving = true;
	}
}

// 1. Ÿ�ٹ������� �̵��ϰ� �ʹ�.
// �ʿ�Ӽ� : Ÿ��, �̵��ӵ�
// 2. Ÿ���� ���ݹ��� �ȿ� ������ ���ݻ��·� ��ȯ�ϰ� �ʹ�.
// �ʿ�Ӽ� : ���ݹ���
void UEnemyFSM::MoveState()
{
	// Ÿ�ٹ������� �̵��ϰ� �ʹ�.

	// 2. ������ �ʿ� direction = target - me
	FVector direction = target->GetActorLocation() - me->GetActorLocation();
	// 1. Ÿ�ٰ��� �Ÿ��� �˾ƾ��Ѵ�.
	float distance = direction.Size();

	direction.Normalize();
	//me->GetCharacterMovement()->bOrientRotationToMovement = true;

	// Ÿ�ٰ��� �Ÿ��� ���������ȿ� ������ Ÿ���� ����ٴϰ�
	if (distance < 1000)
	{
		// ���� �ִ� ������ üũ�� �ʿ�
		ai->MoveToActor(target);
	}
	// �׷��� ������ ������ ��ġ �ϳ� ���ؼ� �̵�����.
	else
	{
		// �������� �̵��ض�.
		EPathFollowingRequestResult::Type r = ai->MoveToLocation(randomPos);
		// �������� ���������� �ٸ� ���� ��ġ ã���� ����
		if(r == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			FNavLocation loc;
			bool result = ns->GetRandomReachablePointInRadius(me->GetActorLocation(), 1000, loc);
			randomPos = loc.Location;
		}
	}

	aiDebugActor->SetActorLocation(randomPos);

	// �ӵ��� ������ isMoving �� ��������
	// �ִϸ��̼��� Move �� �ƴҶ� �׸��� �ӵ��� ���� �� 
	if (anim->isMoving == false)
	{
		float velocity = me->GetVelocity().Size();
		if(velocity > 0.1f)
		{
			anim->isMoving = true;
		}
	}
	

	// ���ݹ��� �ð�ȭ �غ���
	DrawDebugSphere(GetWorld(), me->GetActorLocation(), attackRange, 10, FColor::Red);

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

// �ǰ��� �޾��� �� ó���� �Լ�	
// �ǰ� �޾��� �� hp �� ���ҽ�Ű�� 0 ���ϸ� ���¸� Die �� �ٲٰ� ���ֹ�����
// �˹�(Knockback) ó���� �غ���
void UEnemyFSM::OnDamageProcess(FVector shootDirection)
{
	ai->StopMovement();

	// �ǰ� �޾��� �� hp �� ���ҽ�Ű�� 0 ���ϸ� ���¸� Die �� �ٲٰ� ���ֹ�����
	// 1. hp �� ���������ϱ�
	hp--;
	// 2. ���� hp�� 0 �����̸�
	if(hp <= 0)
	{
		// 3. ���¸� Die �� �ٲ۴�. 
		m_state = EEnemyState::Die;
		// 4. ���ֹ�����
		me->Destroy();
		return;
	}

	// �˹�(Knockback) ó���� �غ���
	// ���� �� ��ġ���� ���� ���� �������� �и��� �ǰڴ�.
	// �и� ������ �ʿ�
	//me->SetActorLocation(me->GetActorLocation() + shootDirection * knockback);
	knockbackPos = me->GetActorLocation() + shootDirection * knockback;

	// ���¸� Damage �� ��ȯ�ϰ� �ʹ�.
	m_state = EEnemyState::Damage;
	currentTime = 0;

	// �˶����� ���� �ʹ�.
	// -> �ǰ� ��� �ð���ŭ ��ٸ��� �˶�

	FTimerHandle damagTimer;
	//GetWorld()->GetTimerManager().SetTimer(damagTimer, this, &UEnemyFSM::DamageState, damageDelayTime, false);
}

