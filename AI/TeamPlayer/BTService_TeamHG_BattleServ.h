// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_TeamHG_BattleServ.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTService_TeamHG_BattleServ : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_TeamHG_BattleServ();

	static const FName IS_ULTIMATE_ON_KEY;
	static const FName IS_DODGE_KEY;
	static const FName BIOTIC_FIELD_LOCATION_KEY;

protected:
	virtual void TickNode(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
