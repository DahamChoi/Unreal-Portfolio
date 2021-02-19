// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_SphereBot_StartAttack.h"
#include "../../Character/Enemy/SphereBotCharacter.h"
#include "../../Controller/SphereBotAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName UBTTaskNode_SphereBot_StartAttack::ATTACK_TIMER_KEY = FName("AttackTimer");

UBTTaskNode_SphereBot_StartAttack::UBTTaskNode_SphereBot_StartAttack()
{
	NodeName = TEXT("StartAttack");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_SphereBot_StartAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(ATTACK_TIMER_KEY, 0.f);

	ASphereBotCharacter* SphereBotCharacter = Cast<ASphereBotCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (SphereBotCharacter)
	{
		if (SphereBotCharacter->TransformationEnable)
			return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UBTTaskNode_SphereBot_StartAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ASphereBotCharacter* SphereBotCharacter = Cast<ASphereBotCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (SphereBotCharacter)
		SphereBotCharacter->StartShooting();

	float AttackTimer = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(ATTACK_TIMER_KEY);
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(ATTACK_TIMER_KEY, AttackTimer + DeltaSeconds);
	if (AttackTimer >= 3.f)
	{
		if (SphereBotCharacter)
		{
			SphereBotCharacter->StopShooting();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}
