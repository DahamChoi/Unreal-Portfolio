// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamHandGunAIController.h"

ATeamHandGunAIController::ATeamHandGunAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BB_TEAM(
		TEXT("/Game/Demo/AI/TeamPlayerHGBlackBoard"));
	if (BB_TEAM.Succeeded())
		BBAsset = BB_TEAM.Object;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT_TEAM(
		TEXT("/Game/Demo/AI/TeamPlayerHGBehaviorTree"));
	if (BT_TEAM.Succeeded())
		BTAsset = BT_TEAM.Object;
}