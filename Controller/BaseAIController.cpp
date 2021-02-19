// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"
#include "DemoPlayerController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/CharacterBase.h"

const float ABaseAIController::AI_SIGHT_RADIUS = 2500.f;
const float ABaseAIController::AI_SIGHT_AGE = 5.f;
const float ABaseAIController::AI_LOSE_SIGHT_RADIUS = AI_SIGHT_RADIUS + 100.f;
const float ABaseAIController::AI_FIELD_OF_VIEW = 90.f;

ABaseAIController::ABaseAIController()
{
	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));
	SightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	SightConfig->SightRadius = AI_SIGHT_RADIUS;
	SightConfig->LoseSightRadius = AI_LOSE_SIGHT_RADIUS;
	SightConfig->PeripheralVisionAngleDegrees = AI_FIELD_OF_VIEW;
	SightConfig->SetMaxAge(AI_SIGHT_AGE);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);

	DamageConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));
	GetPerceptionComponent()->ConfigureSense(*DamageConfig);

	FScriptDelegate Delegate;
	Delegate.BindUFunction(this, "OnBaseTargetPerceptionUpdate");
	GetPerceptionComponent()->OnTargetPerceptionUpdated.Add(Delegate);

	SetGenericTeamId(FGenericTeamId(5));
}

void ABaseAIController::PostInitializeComponents()
{
	bWantsPlayerState = true;
	Super::PostInitializeComponents();
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BBAsset, Blackboard))
	{
		RunBehaviorTree(BTAsset);
	}
}

void ABaseAIController::OnBaseTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	ACharacterBase* CharacterBase = Cast<ACharacterBase>(Actor);
	if (CharacterBase)
	{
		if (!CharacterBase->IsDead)
		{
			GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Actor);
		}
	}
}

ETeamAttitude::Type ABaseAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* OtherPawn = Cast<APawn>(&Other))
	{
		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			//Create an alliance with Team with ID 10 and set all the other teams as Hostiles:
			FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();
			if (OtherTeamID == 5) {
				return ETeamAttitude::Friendly;
			}
			else {
				return ETeamAttitude::Hostile;
			}
		}
	}

	return ETeamAttitude::Neutral;
}
