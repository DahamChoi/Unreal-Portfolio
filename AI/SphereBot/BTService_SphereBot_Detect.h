// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SphereBot_Detect.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTService_SphereBot_Detect : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SphereBot_Detect();

	static const FName IS_ATTACK_RANGE_KEY;

	static const float ATTACK_RANGE;

protected:
	virtual void TickNode(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
