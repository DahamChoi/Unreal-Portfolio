// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_PrimeHelix_Attack.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "../../Character/Enemy/PrimeHelixCharacter.h"
#include "../../Controller/BaseAIController.h"

UBTTaskNode_PrimeHelix_Attack::UBTTaskNode_PrimeHelix_Attack()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_PrimeHelix_Attack::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APrimeHelixCharacter* PrimeHelixCharacter = Cast<APrimeHelixCharacter>(
		OwnerComp.GetAIOwner()->GetCharacter()
		);
	if (PrimeHelixCharacter)
	{
		IsEndAttack = false;

		PrimeHelixCharacter->AttackHeadOn();
		PrimeHelixCharacter->OnEndAttack.AddLambda([this]() -> void {
			IsEndAttack = true;
			});

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTaskNode_PrimeHelix_Attack::TickTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (IsEndAttack)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
