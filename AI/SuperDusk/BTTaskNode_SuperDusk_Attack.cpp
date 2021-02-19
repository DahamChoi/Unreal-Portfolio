// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_SuperDusk_Attack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "../../Character/Enemy/SuperDuskCharacter.h"
#include "../../Controller/BaseAIController.h"

UBTTaskNode_SuperDusk_Attack::UBTTaskNode_SuperDusk_Attack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_SuperDusk_Attack::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	ASuperDuskCharacter* SuperDuskCharacter = Cast<ASuperDuskCharacter>(
			OwnerComp.GetAIOwner()->GetCharacter()
		);
	if (SuperDuskCharacter)
	{
		IsEndAttack = false;

		SuperDuskCharacter->AttackHeadOn();
		SuperDuskCharacter->OnEndAttack.AddLambda([this]() -> void {
			IsEndAttack = true;
			});

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTaskNode_SuperDusk_Attack::TickTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (IsEndAttack)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
