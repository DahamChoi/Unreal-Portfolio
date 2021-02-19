// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_SphereBot_TurnToTarget.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTTaskNode_SphereBot_TurnToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_SphereBot_TurnToTarget();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	static const FName TURN_TO_TARGET_TIMER_KEY;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
