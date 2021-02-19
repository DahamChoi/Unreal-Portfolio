// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Common_TargetDetecter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../Character/CharacterBase.h"
#include "../../Controller/BaseAIController.h"

const FName UBTService_Common_TargetDetecter::TARGET_KEY = TEXT("Target");
const FName UBTService_Common_TargetDetecter::TARGET_LOCATION_KEY = TEXT("TargetLocation");
const FName UBTService_Common_TargetDetecter::TARGET_LENGTH_KEY = TEXT("TargetLength");

UBTService_Common_TargetDetecter::UBTService_Common_TargetDetecter()
{

}

void UBTService_Common_TargetDetecter::TickNode(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Get Controlling Pawn
	ACharacterBase* ControllingPawn =
		Cast<ACharacterBase>(OwnerComp.GetAIOwner()->GetCharacter());

	UObject* TargetItr = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TARGET_KEY);
	if (TargetItr)
	{
		AActor* Target = Cast<AActor>(TargetItr);

		// Set Location Black Board Value
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(
			TARGET_LOCATION_KEY, Target->GetActorLocation()
		);

		// Set Length Black Board Value
		float length = FMath::Abs<float>(
			FVector::Dist(
				Target->GetActorLocation(),
				ControllingPawn->GetActorLocation())
			);
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(
			TARGET_LENGTH_KEY, length
		);
	}
}