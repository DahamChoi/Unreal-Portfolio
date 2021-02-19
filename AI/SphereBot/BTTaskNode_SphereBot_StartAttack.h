// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_SphereBot_StartAttack.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTTaskNode_SphereBot_StartAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_SphereBot_StartAttack();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	static const FName ATTACK_TIMER_KEY;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
