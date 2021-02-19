// Fill out your copyright notice in the Description page of Project Settings.


#include "SuperDuskAIController.h"

ASuperDuskAIController::ASuperDuskAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BB_SD(
		TEXT("/Game/Demo/AI/SuperDuskBlackBoard"));
	if (BB_SD.Succeeded())
		BBAsset = BB_SD.Object;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT_SD(
		TEXT("/Game/Demo/AI/SuperDuskBehaviorTree"));
	if (BT_SD.Succeeded())
		BTAsset = BT_SD.Object;
}