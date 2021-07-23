// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerMove.generated.h"


// ������� �Է��� �޾Ƽ� �̵�ó���ϴ� ������Ʈ
// �ʿ�Ӽ� : �̵��ӵ�, ��������

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_TUE_API UPlayerMove : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerMove();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

	void Horizontal(float value);
	void Vertical(float value);
	// ȸ����Ű��
	void Turn(float value);
	void LookUp(float value);
	// ����
	void Jump();

public:
	// �ʿ�Ӽ� : ��������
	UPROPERTY()
	class AFPSPlayer* me;


};
