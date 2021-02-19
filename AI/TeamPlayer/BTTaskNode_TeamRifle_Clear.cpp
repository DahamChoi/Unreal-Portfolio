// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_TeamRifle_Clear.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../Character/Player/PlayerCharacterRifleForm.h"
#include "../../Controller/BaseAIController.h"
#include "BTService_TeamRifle_BattleServ.h"

const FName UBTTaskNode_TeamRifle_Clear::HIDING_LOCATION_KEY = TEXT("HidingLocation");

UBTTaskNode_TeamRifle_Clear::UBTTaskNode_TeamRifle_Clear()
{

}

EBTNodeResult::Type UBTTaskNode_TeamRifle_Clear::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	APlayerCharacterRifleForm* CharacterRifleForm =
		Cast<APlayerCharacterRifleForm>(ControllingPawn);

	// Reload
	if (CharacterRifleForm)
	{
		CharacterRifleForm->PCActionStopFire();
		CharacterRifleForm->PCActionReload();
		CharacterRifleForm->bUseControllerRotationYaw = true;
	}

	// Clear Black Board
	OwnerComp.GetBlackboardComponent()->ClearValue(UBTService_TeamRifle_BattleServ::BIOTIC_FIELD_LOCATION_KEY);
	OwnerComp.GetBlackboardComponent()->ClearValue(HIDING_LOCATION_KEY);

	return Result;
}