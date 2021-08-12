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
	
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �����ð��� �ѹ��� Enemy �� ����� �ʹ�.
	// 1. �ð��� �귶���ϱ�
	currentTime += DeltaTime;
	// 2. �ð��� �����ϱ�
	if(currentTime > createTime)
	{
		// 3. ���� ����� �ʹ�.
		GetWorld()->SpawnActor<AEnemy>(enemyFactory, GetActorLocation(), FRotator(0, 0, 0));
		currentTime = 0;
	}
}

