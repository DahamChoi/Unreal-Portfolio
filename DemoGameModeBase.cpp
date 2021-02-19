// Copyright Epic Games, Inc. All Rights Reserved.


#include "DemoGameModeBase.h"
#include "State/DemoGameStateBase.h"
#include "State/DemoPlayerState.h"
#include "Character/Player/PlayerCharacterRifleForm.h"
#include "Character/Player/PlayerCharacterHandGunForm.h"
#include "Controller/DemoPlayerController.h"

ADemoGameModeBase::ADemoGameModeBase()
{
	DefaultPawnClass = APlayerCharacterRifleForm::StaticClass();
	GameStateClass = ADemoGameStateBase::StaticClass();
	PlayerStateClass = ADemoPlayerState::StaticClass();
	PlayerControllerClass = ADemoPlayerController::StaticClass();
}