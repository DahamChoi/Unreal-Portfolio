// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_ScifiSoldierHG_Combat.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTTaskNode_ScifiSoldierHG_Combat : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_ScifiSoldierHG_Combat();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	static const FName FIRE_TIMER_KEY;

protected:
	virtual void TickTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
