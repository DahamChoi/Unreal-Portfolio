// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimeHelixAIController.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"

APrimeHelixAIController::APrimeHelixAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BB_SD(
		TEXT("/Game/Demo/AI/PrimeHelixBlackBoard"));
	if (BB_SD.Succeeded())
		BBAsset = BB_SD.Object;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT_SD(
		TEXT("/Game/Demo/AI/PrimeHelixBehaviorTree"));
	if (BT_SD.Succeeded())
		BTAsset = BT_SD.Object;
}