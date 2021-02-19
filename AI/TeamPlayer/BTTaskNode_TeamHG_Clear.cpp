// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_TeamHG_Clear.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../Character/Player/PlayerCharacterHandGunForm.h"
#include "../../Controller/BaseAIController.h"
#include "BTService_TeamRifle_BattleServ.h"

const FName UBTTaskNode_TeamHG_Clear::HIDING_LOCATION_KEY = TEXT("HidingLocation");

UBTTaskNode_TeamHG_Clear::UBTTaskNode_TeamHG_Clear()
{

}

EBTNodeResult::Type UBTTaskNode_TeamHG_Clear::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	APlayerCharacterHandGunForm* CharacterHandGunForm =
		Cast<APlayerCharacterHandGunForm>(ControllingPawn);

	// Reload
	if (CharacterHandGunForm)
	{
		CharacterHandGunForm->PCActionReload();
		CharacterHandGunForm->bUseControllerRotationYaw = true;
	}

	// Clear Black Board
	OwnerComp.GetBlackboardComponent()->ClearValue(UBTService_TeamRifle_BattleServ::BIOTIC_FIELD_LOCATION_KEY);
	OwnerComp.GetBlackboardComponent()->ClearValue(HIDING_LOCATION_KEY);

	return Result;
}