// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_SphereBot_TurnToTarget.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "../../Controller/SphereBotAIController.h"
#include "../../Character/Enemy/SphereBotCharacter.h"
#include "../Common/BTService_Common_TargetDetecter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTService_SphereBot_Detect.h"

const FName UBTTaskNode_SphereBot_TurnToTarget::TURN_TO_TARGET_TIMER_KEY = FName("TurnToTargetTimer");

UBTTaskNode_SphereBot_TurnToTarget::UBTTaskNode_SphereBot_TurnToTarget()
{
	NodeName = TEXT("TurnToTarget");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_SphereBot_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(TURN_TO_TARGET_TIMER_KEY, 0.f);
	return EBTNodeResult::InProgress;
}

void UBTTaskNode_SphereBot_TurnToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(
		UBTService_Common_TargetDetecter::TARGET_LOCATION_KEY);
	FVector OriginLocation = ControllingPawn->GetActorLocation();

	FVector LookVector = TargetLocation - OriginLocation;
	LookVector.Z = 0.f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();

	float TurnTimer = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TURN_TO_TARGET_TIMER_KEY);
	ControllingPawn->SetActorRotation(
		FMath::Lerp(ControllingPawn->GetActorRotation(), TargetRot, TurnTimer * 5.f));
	
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(TURN_TO_TARGET_TIMER_KEY, TurnTimer + DeltaSeconds);
	if(TurnTimer >= 0.2f)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
