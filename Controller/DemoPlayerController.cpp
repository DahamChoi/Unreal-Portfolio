// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoPlayerController.h"
#include "../UI/MainUserWidget.h"
#include "../State/DemoGameStateBase.h"
#include "../State/DemoPlayerState.h"
#include "../Character/Player/PlayerCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

ADemoPlayerController::ADemoPlayerController()
{
	static ConstructorHelpers::FClassFinder<UMainUserWidget> MAIN_USER_WG(
		TEXT("/Game/Demo/UI/MainWidget"));
	if (MAIN_USER_WG.Succeeded())
	{
		MainUserWidgetClass = MAIN_USER_WG.Class;
	}

	TeamId = FGenericTeamId(10);
}

void ADemoPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ADemoPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

}

void ADemoPlayerController::MainUserWidgetBindTeamPlayerState(ADemoPlayerState* State)
{
	if (!MainUserWidget)
	{
		InitializeMainWidget();
	}

	MainUserWidget->AddBindingTeamPlayerState(State);
}

void ADemoPlayerController::InitializeMainWidget()
{
	MainUserWidget = CreateWidget<UMainUserWidget>(this, MainUserWidgetClass);
	MainUserWidget->AddToViewport();

	ADemoGameStateBase* DemoGameState = GetWorld()->GetGameState<ADemoGameStateBase>();
	MainUserWidget->BindGameState(DemoGameState);

	ADemoPlayerState* DemoPlayerState = GetPlayerState<ADemoPlayerState>();
	MainUserWidget->BindPlayerState(DemoPlayerState);

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetCharacter());
	if (PlayerCharacter)
	{
		MainUserWidget->BindCharacterState(PlayerCharacter);
	}
}

void ADemoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!MainUserWidget)
	{
		InitializeMainWidget();
	}	
}

FGenericTeamId ADemoPlayerController::GetGenericTeamId() const
{
	return TeamId;
}
