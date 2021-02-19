// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamRifleAIController.h"

ATeamRifleAIController::ATeamRifleAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BB_TEAM(
		TEXT("/Game/Demo/AI/TeamPlayerBlackBoard"));
	if (BB_TEAM.Succeeded())
		BBAsset = BB_TEAM.Object;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT_TEAM(
		TEXT("/Game/Demo/AI/TeamPlayerBehaviorTree"));
	if (BT_TEAM.Succeeded())
		BTAsset = BT_TEAM.Object;
}