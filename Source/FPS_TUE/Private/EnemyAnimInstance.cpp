// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include <Animation/AnimMontage.h>

void UEnemyAnimInstance::Hit()
{
	Montage_Play(damageMontage);
	state = EEnemyState::Idle;
}

void UEnemyAnimInstance::Die()
{
	Montage_Play(dieMontage);
	
	//dieMontage �ִϸ��̼��� ������ �� Enemy �� ���ְ� �ʹ�.
	/*FOnMontageEnded dieEndDelegate;
	dieEndDelegate.BindUObject(this, &UEnemyAnimInstance::OnDieMontageEnded);
	Montage_SetEndDelegate(dieEndDelegate, dieMontage);*/
}

void UEnemyAnimInstance::OnNotifyTest()
{

}

void UEnemyAnimInstance::OnDieMontageEnded(UAnimMontage* animMontage, bool isEnd)
{
	// �� ����
	TryGetPawnOwner()->Destroy();
}
