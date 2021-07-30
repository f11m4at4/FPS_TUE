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

private:
	void IdleState();
	void MoveState();
	void AttackState();
	void DamageState();
	void DieState();

public:
	// 피격을 받았을 때 처리할 함수	
	void OnDamageProcess();
};
