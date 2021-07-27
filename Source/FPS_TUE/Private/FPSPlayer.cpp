// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayer.h"
#include "FPS_TUE.h"
#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "PlayerMove.h"
#include "PlayerFire.h"

// Sets default values
AFPSPlayer::AFPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	// 카메라컴포넌트 등록
	fpsCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	fpsCamera->SetupAttachment(GetCapsuleComponent());
	fpsCamera->SetRelativeLocation(FVector(-40, 0, 50));

	bodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	bodyMesh->SetupAttachment(fpsCamera);

	playerMove = CreateDefaultSubobject<UPlayerMove>(TEXT("PlayerMove"));

	playerFire = CreateDefaultSubobject<UPlayerFire>(TEXT("PlayerFire"));


	// 만들어질 때 동적으로 skeletalMesh 로드하여 할당하기
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/FirstPerson/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms'"));

	if (tempMesh.Succeeded())
	{
		bodyMesh->SetSkeletalMesh(tempMesh.Object);
	}

}

// Called when the game starts or when spawned
void AFPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	// 등록된 델리게이트 함수 모조리 실행해 주기
	OnInputDelegate.Broadcast(PlayerInputComponent);
}

