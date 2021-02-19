// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Common_PayloadDetecter.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTService_Common_PayloadDetecter : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Common_PayloadDetecter();

	static const FName PAYLOAD_LOCATION_KEY;
	static const FName PAYLOAD_LENGTH_KEY;

protected:
	virtual void TickNode(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
