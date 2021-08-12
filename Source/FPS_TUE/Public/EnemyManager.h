// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

// 일정시간에 한번씩 Enemy 를 만들고 싶다.
// 필요속성 : 생성시간, 적공장, 경과시간

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
	// 필요속성 : 생성시간, 경과시간, 적공장
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
