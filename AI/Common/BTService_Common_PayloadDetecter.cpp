// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Common_PayloadDetecter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../Actor/PayloadActor.h"
#include "../../Character/CharacterBase.h"
#include "../../Controller/BaseAIController.h"

const FName UBTService_Common_PayloadDetecter::PAYLOAD_LOCATION_KEY = TEXT("PayloadLocation");
const FName UBTService_Common_PayloadDetecter::PAYLOAD_LENGTH_KEY = TEXT("PayloadLength");

UBTService_Common_PayloadDetecter::UBTService_Common_PayloadDetecter()
{
	
}

void UBTService_Common_PayloadDetecter::TickNode(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Get Controlling Pawn
	ACharacterBase* ControllingPawn = 
		Cast<ACharacterBase>(OwnerComp.GetAIOwner()->GetCharacter());

	// Get PayloadActor
	APayloadActor* PayloadActor = nullptr;
	for (TObjectIterator<APayloadActor> Itr; Itr; ++Itr)
	{
		// Is Valid
		if (Itr)
			PayloadActor = *Itr;
	}

	if (PayloadActor)
	{
		// Set Payload Location Black Board Value
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(
			PAYLOAD_LOCATION_KEY, PayloadActor->GetActorLocation()
		);

		// Set Payload Length Black Board Value
		float length = FMath::Abs<float>(
			FVector::Dist(
					PayloadActor->GetActorLocation(),
					ControllingPawn->GetActorLocation())
			);
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(
			PAYLOAD_LENGTH_KEY, length
		);
	}
}