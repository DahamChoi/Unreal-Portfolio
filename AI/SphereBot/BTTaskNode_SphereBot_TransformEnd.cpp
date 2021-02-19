// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_SphereBot_TransformEnd.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../Controller/SphereBotAIController.h"
#include "../../Character/Enemy/SphereBotCharacter.h"

UBTTaskNode_SphereBot_TransformEnd::UBTTaskNode_SphereBot_TransformEnd()
{
	NodeName = TEXT("TransformEnd");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_SphereBot_TransformEnd::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ASphereBotCharacter* SphereBotCharacter = Cast<ASphereBotCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == SphereBotCharacter)	return EBTNodeResult::Failed;

	if (SphereBotCharacter->TransformationEnable && !SphereBotCharacter->GetCharacterMovement()->IsFalling())
	{
		SphereBotCharacter->Transformation();
		IsTransforming = true;
		
		SphereBotCharacter->OnTransformationEnd.AddLambda([this]() -> void {
			IsTransforming = false;
			});

		return EBTNodeResult::InProgress;
	}
	else if (!SphereBotCharacter->TransformationEnable)
	{
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}

void UBTTaskNode_SphereBot_TransformEnd::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsTransforming)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
