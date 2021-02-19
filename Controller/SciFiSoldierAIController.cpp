// Fill out your copyright notice in the Description page of Project Settings.


#include "SciFiSoldierAIController.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"

ASciFiSoldierAIController::ASciFiSoldierAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BB_SD(
		TEXT("/Game/Demo/AI/SciFiSoldierBlackBoard"));
	if (BB_SD.Succeeded())
		BBAsset = BB_SD.Object;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT_SD(
		TEXT("/Game/Demo/AI/SciFiSoldierBehaviorTree"));
	if (BT_SD.Succeeded())
		BTAsset = BT_SD.Object;
}