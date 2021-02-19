// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_TeamRifle_Combat.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTTaskNode_TeamRifle_Combat : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_TeamRifle_Combat();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
