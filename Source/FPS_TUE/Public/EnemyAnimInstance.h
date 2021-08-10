// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyFSM.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPS_TUE_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category=FSM, BlueprintReadOnly)
	bool isMoving = false;

	UPROPERTY(EditAnywhere, Category = FSM, BlueprintReadOnly)
	bool isPatrol = false;

	UPROPERTY(EditAnywhere, Category = FSM, BlueprintReadOnly)
	EEnemyState state;

	// 재생할 피격 몽타주
	UPROPERTY(EditAnywhere, Category = FSM, BlueprintReadOnly)
	class UAnimMontage* damageMontage;

	UPROPERTY(EditAnywhere, Category = FSM, BlueprintReadOnly)
	class UAnimMontage* dieMontage;

	// 피격 당했을 때 호출될 함수(Animation 재생)
	void Hit();

	// 죽었을 때 호출될 함수
	void Die();

	// 노티파이 이벤트 호출될때 처리할 함수
	UFUNCTION(BlueprintCallable, Category=FSM)
	void OnNotifyTest();

	// 몽타주 끝났을 때 호출될 이벤트 함수
	UFUNCTION()
	void OnDieMontageEnded(UAnimMontage* animMontage, bool isEnd);
};
