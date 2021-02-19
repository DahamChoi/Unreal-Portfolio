// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAIController.h"
#include "GenericTeamAgentInterface.h"
#include "TeamAIController.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API ATeamAIController : public ABaseAIController
{
	GENERATED_BODY()
	
public:
	ATeamAIController();

	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

protected:
	virtual void BeginPlay() override;
};
