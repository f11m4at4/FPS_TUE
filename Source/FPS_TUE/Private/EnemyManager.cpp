// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"
#include "Enemy.h"

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
		
	// 생성시간 랜덤으로 초기화
	createTime = FMath::RandRange(minTime, maxTime);
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 일정시간에 한번씩 Enemy 를 만들고 싶다.
	// 1. 시간이 흘렀으니까
	currentTime += DeltaTime;
	// 2. 시간이 됐으니까
	if(currentTime > createTime)
	{
		// 3. 적을 만들고 싶다.
		GetWorld()->SpawnActor<AEnemy>(enemyFactory, GetActorLocation(), FRotator(0, 0, 0));
		currentTime = 0;
		createTime = FMath::RandRange(minTime, maxTime);
	}
}

