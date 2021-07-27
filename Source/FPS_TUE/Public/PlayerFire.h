// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerFire.generated.h"

// 사용자가 발사버튼을 누르면 총알을 발사하고 싶다.

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_TUE_API UPlayerFire : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerFire();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

	void Fire();

public:
	// 필요속성 : 소유액터
	UPROPERTY()
	class AFPSPlayer* me;

	// 총알공장
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

	// 총구
	UPROPERTY()
	class UArrowComponent* firePosition;

	// 필요속성 : 효과
	UPROPERTY(EditDefaultsOnly, Category=BulletEffect)
	class UParticleSystem* bulletEffect;

	// 사거리
	UPROPERTY(EditDefaultsOnly, Category = FireDistance)
	float fireDistance = 10000;

	// 날려버릴 힘
	UPROPERTY(EditDefaultsOnly, Category = BulletPower)
	float bulletPower = 10000;

};
