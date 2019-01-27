// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "LostInTimeGameMode.h"
#include "LostInTimeHUD.h"
#include "LostInTimeCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALostInTimeGameMode::ALostInTimeGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ALostInTimeHUD::StaticClass();
}
