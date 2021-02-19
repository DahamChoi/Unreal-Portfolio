// Fill out your copyright notice in the Description page of Project Settings.


#include "SciFiSoldierHGAIController.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"

ASciFiSoldierHGAIController::ASciFiSoldierHGAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BB_SD(
		TEXT("/Game/Demo/AI/SciFiSoldierHandGunBlackBoard"));
	if (BB_SD.Succeeded())
		BBAsset = BB_SD.Object;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT_SD(
		TEXT("/Game/Demo/AI/SciFiSoldierHandGunBehaviorTree"));
	if (BT_SD.Succeeded())
		BTAsset = BT_SD.Object;
}