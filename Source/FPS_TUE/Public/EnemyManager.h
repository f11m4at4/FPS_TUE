// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

// �����ð��� �ѹ��� Enemy �� ����� �ʹ�.
// �ʿ�Ӽ� : �����ð�, ������, ����ð�

UCLASS()
class FPS_TUE_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// �ʿ�Ӽ� : �����ð�, ����ð�, ������
	UPROPERTY(EditAnywhere, Category=Setting)
	float minTime = 4;
	UPROPERTY(EditAnywhere, Category=Setting)
	float maxTime = 10;

	UPROPERTY()
	float createTime = 2;

	UPROPERTY(EditAnywhere, Category = Setting)
	float currentTime = 0;

	UPROPERTY(EditAnywhere, Category = Setting)
	TSubclassOf<class AEnemy> enemyFactory;
};
