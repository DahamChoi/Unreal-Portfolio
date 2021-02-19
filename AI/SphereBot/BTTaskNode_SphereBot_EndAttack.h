// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_SphereBot_EndAttack.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTTaskNode_SphereBot_EndAttack : public UBTTaskNode
{
	GENERATED_BODY()
		
public:
	UBTTaskNode_SphereBot_EndAttack();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
