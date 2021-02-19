// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamAIController.h"
#include "../State/DemoPlayerState.h"
#include "../Controller/DemoPlayerController.h"
#include "../UI/MainUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

ATeamAIController::ATeamAIController()
{
	SetGenericTeamId(FGenericTeamId(10));
}

void ATeamAIController::BeginPlay()
{
	Super::BeginPlay();

	ADemoPlayerController* DemoPlayerController = Cast<ADemoPlayerController>(
		UGameplayStatics::GetPlayerController(this->GetWorld(), 0));
	if (DemoPlayerController)
	{
		ADemoPlayerState* DemoPlayerState = GetPlayerState<ADemoPlayerState>();
		if (DemoPlayerState)
		{
			DemoPlayerController->MainUserWidgetBindTeamPlayerState(DemoPlayerState);
		}
	}
}

ETeamAttitude::Type ATeamAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			//Create an alliance with Team with ID 10 and set all the other teams as Hostiles:
			FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();
			if (OtherTeamID == 10) {
				return ETeamAttitude::Friendly;
			}
			else {
				return ETeamAttitude::Hostile;
			}
		}
	}

	return ETeamAttitude::Neutral;
}