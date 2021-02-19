// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_SphereBot_EndAttack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "../../Controller/SphereBotAIController.h"
#include "../../Character/Enemy/SphereBotCharacter.h"

UBTTaskNode_SphereBot_EndAttack::UBTTaskNode_SphereBot_EndAttack()
{
	NodeName = TEXT("EndAttack");
}

EBTNodeResult::Type UBTTaskNode_SphereBot_EndAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ASphereBotCharacter* SphereBotCharacter = Cast<ASphereBotCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == SphereBotCharacter)	return EBTNodeResult::Failed;

	SphereBotCharacter->StopShooting();

	return Result;
}