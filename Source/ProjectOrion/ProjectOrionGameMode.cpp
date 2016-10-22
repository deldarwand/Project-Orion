// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ProjectOrion.h"
#include "ProjectOrionGameMode.h"
#include "ProjectOrionHUD.h"
#include "ProjectOrionCharacter.h"

AProjectOrionGameMode::AProjectOrionGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AProjectOrionHUD::StaticClass();
}
