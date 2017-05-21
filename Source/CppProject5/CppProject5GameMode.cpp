// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "CppProject5.h"
#include "CppProject5GameMode.h"
#include "CppProject5HUD.h"
#include "CppProject5Character.h"

ACppProject5GameMode::ACppProject5GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ACppProject5HUD::StaticClass();
}
