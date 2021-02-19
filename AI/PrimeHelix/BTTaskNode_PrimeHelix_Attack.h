// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_PrimeHelix_Attack.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTTaskNode_PrimeHelix_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_PrimeHelix_Attack();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsEndAttack;
};
