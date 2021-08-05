// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

// Enemy 상태정의
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Patrol,
	Move,
	Attack,
	Damage,
	Die
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_TUE_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	UPROPERTY(EditAnywhere, Category=FSM, BlueprintReadWrite)
	EEnemyState m_state;

	// 필요속성 : 대기시간, 경과시간
	UPROPERTY(EditAnywhere, Category=FSM)
	float idleDelayTime = 2;

	UPROPERTY()
	float currentTime = 0;

	// 필요속성 : 타겟, 이동속도
	UPROPERTY(EditAnywhere, Category=FSM)
	class AFPSPlayer* target;

	UPROPERTY()
	class AEnemy* me;

	// 필요속성 : 공격범위
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 200;

	// 필요속성 : 공격대기시간
	UPROPERTY(EditAnywhere, Category=FSM)
	float attackDelayTime = 2;

	// 필요속성 : 피격대기시간
	UPROPERTY(EditAnywhere, Category = FSM)
	float damageDelayTime = 2;

	// hp
	UPROPERTY(EditAnywhere, Category=FSM)
	int hp = 3;

	// Damage 받을 때 뒤로 밀릴 힘
	UPROPERTY(EditAnywhere, Category = KNOCKBACK)
	float knockback = 500;

	// Damage 받을 때 넉백 종료 위치
	UPROPERTY()
	FVector knockbackPos;

	UPROPERTY(EditAnywhere, Category = KNOCKBACK)
	float knockbackSpeed = 20;

	UPROPERTY(EditAnywhere, Category = KNOCKBACK)
	float knockbackRange = 20;

	// 자동 길찾기 하기위한 Enemy 의 AIController 를 기억
	UPROPERTY()
	class AAIController* ai;
	
	// Enemy 가 사용하고 있는 AnimInstance 기억
	UPROPERTY()
	class UEnemyAnimInstance* anim;

	// 랜덤으로 길찾기 할 목저기
	UPROPERTY()
	FVector randomPos;

	// 사용할 NavgationSystem 객체 기억
	UPROPERTY()
	class UNavigationSystemV1* ns;

	// AI 가 랜덤으로 갈 위치확인 물체
	UPROPERTY(EditAnywhere, Category=AI)
	class AActor* aiDebugActor;

private:
	void IdleState();
	void PatrolState();
	void MoveState();
	void AttackState();
	void DamageState();
	void DieState();

	// 파라미터로 액터를 받아서 그 액터를 기준으로 갈수 있는 랜덤 위치 알려주는 함수
	bool GetTargetLocation(const AActor* targetActor, float radius, FVector &dest);
public:
	// 피격을 받았을 때 처리할 함수	
	void OnDamageProcess(FVector shootDirection);
};
