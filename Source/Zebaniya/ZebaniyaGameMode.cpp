// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ZebaniyaGameMode.h"
#include "ZebaniyaCharacter.h"
#include "UObject/ConstructorHelpers.h"

AZebaniyaGameMode::AZebaniyaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
