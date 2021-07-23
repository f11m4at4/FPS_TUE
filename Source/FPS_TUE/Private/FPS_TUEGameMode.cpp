// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_TUEGameMode.h"
#include "FPS_TUEHUD.h"
#include "FPS_TUECharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPS_TUEGameMode::AFPS_TUEGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPS_TUEHUD::StaticClass();
}
