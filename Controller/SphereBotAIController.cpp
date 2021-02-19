// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereBotAIController.h"
#include "DemoPlayerController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

ASphereBotAIController::ASphereBotAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BB_BOT(
		TEXT("/Game/Demo/AI/SphereBotBlackBoard"));
	if (BB_BOT.Succeeded())
		BBAsset = BB_BOT.Object;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT_BOT(
		TEXT("/Game/Demo/AI/SphereBotBehaviorTree"));
	if (BT_BOT.Succeeded())
		BTAsset = BT_BOT.Object;
}