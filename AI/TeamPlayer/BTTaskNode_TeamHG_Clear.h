// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_TeamHG_Clear.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTTaskNode_TeamHG_Clear : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_TeamHG_Clear();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	static const FName HIDING_LOCATION_KEY;
};
