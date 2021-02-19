// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DemoPlayerController.h"
#include "Perception/AIPerceptionTypes.h"
#include "GenericTeamAgentInterface.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API ABaseAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ABaseAIController();
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnBaseTargetPerceptionUpdate(AActor* actor, FAIStimulus stimulus);

	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

protected:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AISightRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AISightAge = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AILoseSightRadius = AISightRadius + 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AIFieldOfView = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Damage* DamageConfig;

	static const float AI_SIGHT_RADIUS;
	static const float AI_SIGHT_AGE;
	static const float AI_LOSE_SIGHT_RADIUS;
	static const float AI_FIELD_OF_VIEW;
};
